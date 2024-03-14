/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM.h"
#include "NvM_Cfg_SchM.h"

#include "NvM_Prv.h"
#include "NvM_Prv_Service.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_RequestQueue.h"
#include "NvM_Prv_ErrorDetection.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
LOCAL_INLINE  boolean NvM_Prv_Service_CheckPreconditions(NvM_Prv_idQueue_tuo idQueue_uo,
                                                         NvM_BlockIdType idBlock_uo,
                                                         NvM_Prv_idService_tuo idService_uo);
LOCAL_INLINE Std_ReturnType NvM_Prv_Service_FinalizeSingle(NvM_Prv_idQueue_tuo idQueue_uo,
                                                           NvM_Prv_BlockData_tst const* BlockData_pcst,
                                                           NvM_Prv_Service_Configuration_tst const* ServiceConfiguration_pcst);
LOCAL_INLINE boolean NvM_Prv_Service_CheckBlockDataSingle(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                                          NvM_Prv_Service_CheckBlockData_tpfct CheckBlockData_pfct,
                                                          NvM_Prv_Service_InitiationErrors_tuo* Errors_puo);
LOCAL_INLINE boolean NvM_Prv_Service_CheckPendingSingle(boolean isCheckPendingRequired_b,
                                                        NvM_BlockIdType idBlockPendingCheck_uo,
                                                        NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);
LOCAL_INLINE boolean NvM_Prv_Service_Enqueue(NvM_Prv_idQueue_tuo idQueue_uo,
                                             NvM_Prv_Queue_idx_tuo* idxPosition_puo,
                                             NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This function initiates a multi-block service.
 *
 * \details
 * The NvM initiates a multi-block service only if it has been properly initialized before.
 *
 * To initiate a multi-block service the NvM executes following steps with enabled scheduler lock:
 * - enqueue multi-block service into the corresponding queue,
 *   some multi-block services must not be enqueued and in this case it is possible to avoid enqueuing by passing
 *   the special queue ID NvM_Prv_idQueue_nrQueues_e to this function (s. parameter idQueue_uo)
 * - update block data by calling the corresponding function via function pointer passed in the configuration structure
 *   (s. parameter ConfigurationMulti_pcst).
 *
 * Afterwards the NvM executes all remaining actions required to finalize multi-block service initiation with disabled
 * scheduler lock. These actions are defined in the function passed in the configuration structure
 * (s. parameter ConfigurationMulti_pcst).
 *
 * \attention
 * This function uses scheduler lock. The duration of the scheduler lock corresponds to 18 copy-operations:
 * - NvM_Prv_Service_Enqueue = 12 (see description of NvM_Prv_Service_Enqueue)
 * - UpdateBlockDataForMulti_pfct = 6 (call of the request-specific function, in worst case WriteAll)
 *
 * \param idQueue_uo
 * Queue ID to enqueue the multi-block service. A multi-block service may be enqueued only into the queue
 * for multi-block services (s. NvM_Prv_idQueue_Multi_e). If a multi-block service requires no enqueueing
 * then queue ID NvM_Prv_idQueue_nrQueues_e shall be used.
 *
 * \param QueueEntry_pcst
 * Pointer to the structure with data required to enqueue a service.
 * The caller has to make sure that this pointer is valid and points to plausible data.
 *
 * \param ConfigurationMulti_pcst
 * Pointer to the structure with configuration data required to initiate a multi-block service
 * The caller has to make sure that this pointer is valid and points to plausible data.
 */
void NvM_Prv_Service_InitiateMulti(NvM_Prv_idQueue_tuo idQueue_uo,
                                   NvM_Prv_QueueEntry_tst const* QueueEntry_pcst,
                                   NvM_Prv_Service_ConfigurationMulti_tst const* ConfigurationMulti_pcst)
{
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(QueueEntry_pcst->idService_uo, NVM_PRV_MULTI_BLOCK))
    {
        boolean isMultiInitiationStarted_b = FALSE;
        uint8 idxPosition_uo = 0u;
        NvM_Prv_Service_InitiationErrors_tuo Errors_uo = 0u;

        SchM_Enter_NvM_Main();

        if (NvM_Prv_Service_Enqueue(idQueue_uo, &idxPosition_uo, &Errors_uo))
        {
            ConfigurationMulti_pcst->UpdateBlockDataForMulti_pfct();
            isMultiInitiationStarted_b = TRUE;
        }

        // run-time with enabled scheduler lock = 18 (see detailed description)
        SchM_Exit_NvM_Main();

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
        TestCd_NvM_CheckSchedulerLock();
#endif

        if (isMultiInitiationStarted_b)
        {
            ConfigurationMulti_pcst->FinalizeMulti_pfct();

            if (NvM_Prv_idQueue_Multi_e == idQueue_uo)
            {
                /* Note: Queue-ID is valid here */
                NvM_Prv_RequestQueue_Insert(NvM_Prv_idQueue_Multi_e, QueueEntry_pcst, idxPosition_uo);
            }

        }
        else
        {
            NvM_Prv_ErrorDetection_ReportServiceInitiationErrors(QueueEntry_pcst->idService_uo,
                                                                 QueueEntry_pcst->idBlock_uo,
                                                                 Errors_uo);
        }
    }
}

/**
 * \brief
 * This function initiates a single-block service (user request).
 *
 * \details
 * This function initiates a single-block service only if all required pre-conditions are fulfilled
 * (see NvM_Prv_Service_CheckPreconditions).
 * If all required pre-conditions are fulfilled this function updates used block data via function defined
 * in service-specific configuration if required (see UpdateData_pfct in ServiceConfiguration_pcst).
 * Afterwards this function checks all significant parameters provided by the user to ensure that the NvM can initiate
 * current service safely (see CheckParameter_pfct in ServiceConfiguration_pcst).
 * If this check is also fulfilled then this function finalizes the initiation of the single-block service
 * (see NvM_Prv_Service_FinalizeSingle).
 *
 * \param[in] idQueue_uo
 * Queue ID to enqueue the current service. If current service requires no enqueueing then queue ID
 * NvM_Prv_idQueue_nrQueues_e shall be used.
 * \param[inout] BlockData_pst
 * Pointer to the structure with block data used in current service.
 * The caller has to make sure that this pointer is valid and points to plausible data.
 * \param[in] ServiceConfiguration_pcst
 * Pointer to the structure with configuration data required to initiate a single-block service
 * The caller has to make sure that this pointer is valid and points to plausible data.
 *
 * \return
 * - E_OK = initiation of the single-block service has succeeded
 * - E_NOT_OK = initiation of the single-block service has failed
 */
Std_ReturnType NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_tuo idQueue_uo,
                                              NvM_Prv_BlockData_tst* BlockData_pst,
                                              NvM_Prv_Service_Configuration_tst const* ServiceConfiguration_pcst)
{
    Std_ReturnType stInitiation_uo = E_NOT_OK;

    if (NvM_Prv_Service_CheckPreconditions(idQueue_uo,
                                           BlockData_pst->QueueEntry_st.idBlock_uo,
                                           BlockData_pst->QueueEntry_st.idService_uo))
    {
        boolean isCheckParameterSuccessful_b = TRUE;

        if (NULL_PTR != ServiceConfiguration_pcst->UpdateData_pfct)
        {
            idQueue_uo = ServiceConfiguration_pcst->UpdateData_pfct(BlockData_pst);
        }

        if (NULL_PTR != ServiceConfiguration_pcst->CheckParameter_pfct)
        {
            isCheckParameterSuccessful_b = ServiceConfiguration_pcst->CheckParameter_pfct(BlockData_pst);
        }

        if (isCheckParameterSuccessful_b)
        {
            // set original queue before finalizing service initiation
            BlockData_pst->QueueEntry_st.idQueueOriginal_uo = idQueue_uo;

            stInitiation_uo = NvM_Prv_Service_FinalizeSingle(idQueue_uo,
                                                             BlockData_pst,
                                                             ServiceConfiguration_pcst);
        }
    }

    return stInitiation_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */
/**
 * \brief
 * This local private function checks all preconditions before a single-block service can be initiated.
 * \details
 * Following preconditions must be fulfilled for all single-block services:
 * - NvM is initialized (see NvM_Prv_ErrorDetection_IsNvmInitialized)
 * - ID of the block used in current single-block service is valid (see NvM_Prv_ErrorDetection_IsBlockIdValid)
 *
 * Following preconditions must be fulfilled only for asynchronous single-block services
 * (NvM enqueues only asynchronous single-block services into an internal queue, if a single-block service does not use
 * a valid queue ID (valid queue IDs are always < NvM_Prv_idQueue_nrQueues_e) then it is a synchronous service):
 * - Length of the block used in current single-block service is valid (see NvM_Prv_BlkDesc_IsLengthValid)
 * - Block used in current single-block service is sanitized by ReadAll (see NvM_Prv_IsSanitizedByReadAll)
 * - Block processing is not blocked by ReadAll (see NvM_Prv_IsSingleBlockProcessingIndependentOnReadAll)
 *
 * \param[in] idQueue_uo
 * Id of the NvM internal user request queue to be used for current single-block service,
 * this parameter defines whether current service is synchronous (idQueue_uo >= NvM_Prv_idQueue_nrQueues_e) or
 * asynchronous (idQueue_uo < NvM_Prv_idQueue_nrQueues_e)
 * \param[in] idBlock_uo
 * ID of the block used in current single-block service
 * \param[in] idService_uo
 * ID of the current single-block service
 *
 * \return
 * - true = preconditions fulfilled -> NvM can initiate current single-block service
 * - false = preconditions NOT fulfilled -> NvM cannot initiate current single-block service
 */
LOCAL_INLINE boolean NvM_Prv_Service_CheckPreconditions(NvM_Prv_idQueue_tuo idQueue_uo,
                                                        NvM_BlockIdType idBlock_uo,
                                                        NvM_Prv_idService_tuo idService_uo)
{
    boolean stReturn_b = FALSE;
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(idService_uo, idBlock_uo))
    {
        if (NvM_Prv_ErrorDetection_IsBlockIdValid(idService_uo, idBlock_uo, FALSE))
        {
            if (idQueue_uo < NvM_Prv_idQueue_nrQueues_e)
            {
                // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3032] Ignore blocks with length null
                boolean isBlockLengthValid_b = NvM_Prv_BlkDesc_IsLengthValid(idBlock_uo);
                // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2999] Potentially incompatible blocks after layout changes shall
                //                                          be sanitized by ReadAll at first, then the current single
                //                                          block request is allowed to be accepted and executed
                boolean isSanitizedByReadAll_b = NvM_Prv_IsSanitizedByReadAll(idBlock_uo);

                // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3328] If the configuration parameter NvMRbNoSingleBlockBeforeReadAll
                //                                          is set to "true", the NvM shall not accept single block request
                //                                          before a ReadAll request is executed.
                boolean isSingleBlockIndependentOnReadAll_b = NvM_Prv_IsSingleBlockProcessingIndependentOnReadAll();

                if (isBlockLengthValid_b && isSanitizedByReadAll_b && isSingleBlockIndependentOnReadAll_b)
                {
                    stReturn_b = TRUE;
                }
            }
            else
            {
                stReturn_b = TRUE;
            }
        }
    }
    return stReturn_b;
}

/**
 * \brief
 * This local private function checks all remaining conditions and finalizes the initiation of a single-block
 * service (user request).
 * \details
 * The finalization of a service initiation is done with active scheduler lock and consists of following checks and
 * actions if required:
 * - check all block data used for current service (see NvM_Prv_Service_CheckBlockDataSingle)
 * - check whether any asynchronous service is already pending (see NvM_Prv_Service_CheckPendingSingle)
 * - enqueue current service into the given queue if required (see NvM_Prv_Service_Enqueue)
 * - set all block data as required (see SetBlockData_pfct in passed service configuration ServiceConfiguration_pcst)
 *
 * Afterwards this function inserts current service into the corresponding queue (s. NvM_Prv_RequestQueue_Insert)
 * if required and service initiation has succeeded.
 * If service initiation has failed this function reports all errors occured while scheduler lock was enabled
 * (s. NvM_Prv_ErrorDetection_ReportServiceInitiationErrors).
 *
 * \attention
 * This function uses scheduler lock. Normally this function has the run-time of 28
 * (38 in worst case, but this is rather improbable):
 * - 1 for internal copy-operations
 * - NvM_Prv_Service_CheckBlockDataSingle = 7 (see description of NvM_Prv_Service_CheckBlockDataSingle)
 * - NvM_Prv_Service_CheckPendingSingle = normally 4 (14 in worst case, but this is rather improbable,
 *                                        see description of NvM_Prv_Service_CheckPendingSingle)
 * - NvM_Prv_Service_Enqueue = 12 (see description of NvM_Prv_Service_Enqueue)
 * - SetBlockData_pfct = 4 (contains call of the service-specific function, in worst case NvM_Prv_Write_SetBlockData)
 *
 * \param[in] idQueue_uo
 * Queue ID to enqueue the current service. If current service requires no enqueueing then queue ID
 * NvM_Prv_idQueue_nrQueues_e shall be used.
 * \param[in] BlockData_pcst
 * Pointer to the structure with block data used in current service.
 * The caller has to make sure that this pointer is valid and points to plausible data.
 * \param[in] ServiceConfiguration_pcst
 * Pointer to the structure with configuration data required to initiate a single-block service
 * The caller has to make sure that this pointer is valid and points to plausible data.
 *
 * \return
 * - E_OK = initiation of the single-block service has succeeded
 * - E_NOT_OK = initiation of the single-block service has failed
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_Service_FinalizeSingle(NvM_Prv_idQueue_tuo idQueue_uo,
                                                           NvM_Prv_BlockData_tst const* BlockData_pcst,
                                                           NvM_Prv_Service_Configuration_tst const* ServiceConfiguration_pcst)
{
    Std_ReturnType stInitiation_uo = E_NOT_OK;
    uint8 idxPosition_uo = 0u;
    // Variable used to report DET errors out of the scheduler lock
    NvM_Prv_Service_InitiationErrors_tuo Errors_uo = 0u;

    SchM_Enter_NvM_Main();

    if (NvM_Prv_Service_CheckBlockDataSingle(BlockData_pcst,
                                             ServiceConfiguration_pcst->CheckBlockData_pfct,
                                             &Errors_uo))
    {
        if (NvM_Prv_Service_CheckPendingSingle(ServiceConfiguration_pcst->CheckPendingBlock_b,
                                               ServiceConfiguration_pcst->idBlockPendingCheck_uo,
                                               &Errors_uo))
        {
            if (NvM_Prv_Service_Enqueue(idQueue_uo, &idxPosition_uo, &Errors_uo))
            {
                if (NULL_PTR != ServiceConfiguration_pcst->SetBlockData_pfct)
                {
                    ServiceConfiguration_pcst->SetBlockData_pfct(BlockData_pcst);
                }

                stInitiation_uo = E_OK;
            }
        }
    }

    // run-time with enabled scheduler lock = normally 28 and 38 in worst case which is rather improbable
    // (see detailed description)
    SchM_Exit_NvM_Main();

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
        TestCd_NvM_CheckSchedulerLock();
#endif

    if (E_OK == stInitiation_uo)
    {
        // check whether service shall be enqueued and has been enqueued
        if ((idQueue_uo < NvM_Prv_idQueue_nrQueues_e))
        {
            /* Note: idQueue_uo is valid here */
            NvM_Prv_RequestQueue_Insert(idQueue_uo, &BlockData_pcst->QueueEntry_st, idxPosition_uo);
        }
    }
    else
    {
        // Calling foreign DET function is done out of the scheduler lock
        NvM_Prv_ErrorDetection_ReportServiceInitiationErrors(BlockData_pcst->QueueEntry_st.idService_uo,
                                                             BlockData_pcst->QueueEntry_st.idBlock_uo,
                                                             Errors_uo);
    }

    return stInitiation_uo;
}

/**
 * \brief
 * This local private function checks whether all block data used for current service (user request) is valid
 * so that current service can be initiated safely.
 * \details
 * Each service defines a function to check used block data and passes it via service configuration.
 * If no check is required (normally if service initiation is independent on any block data which can be changed
 * by the user) then the function passed via service configuration is a NULL_PTR.
 * If check function fails then an error to be reported to the DET is returned, see parameter Errors_puo.
 *
 * \attention
 * The user shall make sure that the call of this function is done only with active scheduler lock.
 * If called with enabled scheduler lock the run-time is defined essentially by the passed check function,
 * in worst case 7:
 * - 1 for internal copy-operations
 * - check function passed via CheckBlockData_pfct = call of the service-specific function,
 *                                                   in worst case NvM_Prv_Write_CheckBlockData with run-time of 6
 *
 * \param[in] BlockData_pcst
 * Pointer to the block data to be checked
 * \param[in] CheckBlockData_pfct
 * Pointer to the block data check fuction to be used
 * \param[inout] Errors_puo
 * Errors which can occur while checking used block data
 *
 * \return
 * - TRUE = check for used block data is NOT required or has succeeded
 * - FALSE = check for used block data has failed
 */
LOCAL_INLINE boolean NvM_Prv_Service_CheckBlockDataSingle(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                                          NvM_Prv_Service_CheckBlockData_tpfct CheckBlockData_pfct,
                                                          NvM_Prv_Service_InitiationErrors_tuo* Errors_puo)
{
    boolean isCheckBlockDataSuccessful_b = TRUE;

    if (NULL_PTR != CheckBlockData_pfct)
    {
        isCheckBlockDataSuccessful_b = CheckBlockData_pfct(BlockData_pcst, Errors_puo);
    }

    return isCheckBlockDataSuccessful_b;
}

/**
 * \brief
 * This local private function checks whether any asynchronous service (user request) is already pending for the given
 * block.
 * \details
 * The check is done only for asynchronous services, see parameter isCheckPendingRequired_b.
 * If no check is required (current service is synchronous) then this function returns TRUE.
 * If any asynchronous service is already pending then this function returns the error NvM_Prv_Service_InitiationError_MaskBlkPEnding_e
 * to be reported to the DET, see parameter Errors_puo and NvM_Prv_Block_IsPending.
 *
 * \attention
 * The user shall make sure that the call of this function is done only with active scheduler lock.
 * If called with enabled scheduler lock the run-time of 4 can be used (14 in worst case, but this is rather improbable):
 * - NvM_Prv_Block_IsPending = normally run-time of this function is 4;
 *                             in worst case run-time of this function is 14 (see description of NvM_Prv_Block_IsPending)
 *
 * \param[in] isCheckPendingRequired_b
 * Information whether the check for pending user request is required or not
 * \param[in] idBlockPendingCheck_uo
 * ID of the block for which the check for pending user request will be done
 * \param[inout] Errors_puo
 * Errors which can occur while checking for pending user request
 *
 * \return
 * - TRUE = check for pending user request is NOT required or has succeeded
 * - FALSE = check for pending user request has failed
 */
LOCAL_INLINE boolean NvM_Prv_Service_CheckPendingSingle(boolean isCheckPendingRequired_b,
                                                        NvM_BlockIdType idBlockPendingCheck_uo,
                                                        NvM_Prv_Service_InitiationErrors_tuo *Errors_puo)
{
    boolean isBlockPending_b = FALSE;

    if (isCheckPendingRequired_b)
    {
        // TRACE[NVM385] Only queue this request if no operation on this block is requested or in progress
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2866] Exception to NVM385: A service can be queued for a block
        //                                               whose the only pending operation is a maintenance operation.
        isBlockPending_b = NvM_Prv_Block_IsPending(idBlockPendingCheck_uo, Errors_puo);
    }

    return (!isCheckPendingRequired_b || !isBlockPending_b);
}

/**
 * \brief
 * This local private function enqueues a service (user request) into the given queue if required and returns
 * whether enqueuing has succeeded or not.
 * \details
 * Before enqueuing a service (user request) this function checks whether enqueuing is required or not.
 * If no enqueuing is required (parameter idQueue_uo = NvM_Prv_idQueue_nrQueues_e) then this function returns TRUE.
 * Otherwise this function enqueues the user request and returns position in the queue where user request has been
 * inserted for further usage, see parameter idxPosition_puo.
 * Enqueuing can fail only if corresponding queue is full. In this case  enqueuing has failed then this function
 * returns the error NvM_Prv_Service_InitiationError_MaskQueueFull_e to be reported to the DET, see parameter Errors_puo.
 *
 * \attention
 * The user shall make sure that the call of this function is done only with active scheduler lock.
 * If called with enabled scheduler lock the run-time of 12 can be used:
 * - NvM_Prv_RequestQueue_Enqueue = 12 (see description of NvM_Prv_RequestQueue_Enqueue)
 *
 * \attention
 * This function can be used for all services, single- and multi-block services.
 *
 * \param[in] idQueue_uo
 * Queue ID to enqueue a service. If a service requires no enqueueing then queue ID NvM_Prv_idQueue_nrQueues_e shall
 * be used.
 * \param[inout] idxPosition_puo
 * Index in the queue where user request has been enqueued.
 * This position shall be used for insertion of user request.
 * \param[inout] Errors_puo
 * Errors which can occur while enqueuing user request
 *
 * \return
 * - TRUE = enqueuing of user request is NOT required or has succeeded
 * - FALSE = enqueuing of user request has failed
 */
LOCAL_INLINE boolean NvM_Prv_Service_Enqueue(NvM_Prv_idQueue_tuo idQueue_uo,
                                             NvM_Prv_Queue_idx_tuo* idxPosition_puo,
                                             NvM_Prv_Service_InitiationErrors_tuo* Errors_puo)
{
    boolean isServiceEnqueued_b = FALSE;
    boolean isEnqueuingRequired_b = (idQueue_uo < NvM_Prv_idQueue_nrQueues_e);

    if (isEnqueuingRequired_b)
    {
        // TRACE[NVM181] Queue this request to allow multiple concurrent single block requests
        // TRACE[NVM184] In case of a queue overflow, the return value is E_NOT_OK
        /* Note: idQueue_uo is valid here as checked above in isEnqueuingRequired_b */
        isServiceEnqueued_b = NvM_Prv_RequestQueue_Enqueue(idQueue_uo, idxPosition_puo);

        if (!isServiceEnqueued_b)
        {
            *Errors_puo |= (NvM_Prv_Service_InitiationErrors_tuo)NvM_Prv_Service_InitiationError_MaskQueueFull_e;
        }
    }

    return (!isEnqueuingRequired_b || isServiceEnqueued_b);
}

