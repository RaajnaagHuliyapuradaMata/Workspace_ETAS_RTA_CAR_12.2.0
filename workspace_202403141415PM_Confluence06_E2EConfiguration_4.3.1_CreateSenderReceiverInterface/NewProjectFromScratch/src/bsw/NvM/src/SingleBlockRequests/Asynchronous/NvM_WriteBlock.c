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
#include "NvM_Prv_HideRteApi.h"

#include "NvM_Prv_Service.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#ifdef NVM_CFG_API_WRITE_BLOCK
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here, not in a header file
/* MR12 RULE 8.5, 8.6 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_WRITE_BLOCK(NvM_BlockIdType BlockId, NvM_Rb_ConstVoidPtr NvM_SrcPtr);
#endif

#ifdef NVM_CFG_API_WRITEPRAM_BLOCK
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here, not in a header file
/* MR12 RULE 8.5, 8.6 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_WRITEPRAM_BLOCK(NvM_BlockIdType BlockId);
#endif

LOCAL_INLINE Std_ReturnType NvM_Prv_WriteBlock(NvM_Prv_idService_tuo idService_uo,
                                               NvM_BlockIdType idBlock_uo,
                                               NvM_Rb_ConstVoidPtr NvM_SrcPtr);
static NvM_Prv_idQueue_tuo NvM_Prv_Write_UpdateData(NvM_Prv_BlockData_tst* BlockData_pst);
static boolean NvM_Prv_Write_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);
static boolean NvM_Prv_Write_CheckBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                            NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);
static void NvM_Prv_Write_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This public function initiates a single-block request to write a NVRAM-block from the RAM block
 * into the non-volatile memory.
 *
 * \details
 * A write request from the RAM block will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled including preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_Write_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - all validity checks for the block data are fulfilled (see NvM_Prv_Write_CheckBlockData)
 *   - no other operation on the block to be written is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *   - Required queue for single-block requests is not full
 *     (see NvM_Prv_RequestQueue_Enqueue called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of the write request from the RAM block this function executes
 * following actions:
 * - request result is set to pending state (i.e. NVM_GetErrorStatus() will return NVM_REQ_PENDING
 *   for the requested block)
 * - bit which globally represents the request in administrative-block is set
 * - status of the configured permanent RAM block is set to VALID if used for writing
 * - dependent on the block configuration the write request is enqueued into the standard priority or
 *   into the immediate priority queue
 *
 * \note
 * Actually the initiation of a single-block request to write a NVRAM-block from the corresponding RAM block is
 * implemented in the private function NvM_Prv_WriteBlock().
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_WriteBlockBase()
 * which invokes the private function NvM_Prv_WriteBlock().
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_WriteBlock() directly to get the data written from the corresponding RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be written from the RAM block
 * \param[in] NvM_SrcPtr
 * Pointer to the RAM block for data to write
 *
 * \return
 * Initiation status:
 * - E_OK = write request from the RAM block has been initiated successfully
 * - E_NO_OK = initiation of the write request from the RAM block has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_WriteBlock(NvM_BlockIdType BlockId, NvM_Rb_ConstVoidPtr NvM_SrcPtr)
{
    // TRACE[SWS_NvM_00455] Service to copy the data of the RAM block to its corresponding NV block

#ifdef NVM_CFG_API_WRITE_BLOCK
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_WRITE_BLOCK(BlockId, NvM_SrcPtr);
#else
    return NvM_Prv_WriteBlock(NVM_SERVICE_ID_WRITE_BLOCK, BlockId, NvM_SrcPtr);
#endif
}

#ifdef NVM_CFG_API_WRITE_BLOCK
/**
 * \brief
 * This public function provides the possibility to write a NVRAM-block from the corresponding RAM block
 * into the non-volatile memory within the overloaded API for NvM_WriteBlock().
 *
 * \details
 * To write a NVRAM-block this function invokes the private function NvM_Prv_WriteBlock().
 *
 * \attention
 * This function is only available if user has configured an overloaded API for the request to write the corresponding
 * RAM block into the non-volatile memory.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that NvM writes data
 * from the corresponding RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be written from the corresponding RAM block
 * \param[in] NvM_SrcPtr
 * Pointer to the RAM block for data to write
 *
 * \return
 * Initiation status:
 * - E_OK = write request from the corresponding RAM block has been initiated successfully
 * - E_NO_OK = initiation of the write request from the corresponding RAM block has failed
 */
Std_ReturnType NvM_Rb_WriteBlockBase(NvM_BlockIdType BlockId, NvM_Rb_ConstVoidPtr NvM_SrcPtr)
{
    return NvM_Prv_WriteBlock(NVM_SERVICE_ID_WRITE_BLOCK, BlockId, NvM_SrcPtr);
}
#endif

/**
 * \brief
 * This public function initiates a single-block request to write a NVRAM-block from the configured permanent RAM block
 * into the non-volatile memory.
 *
 * \details
 * A write request from the configured permanent RAM block will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled including preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_Write_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - all validity checks for the block data are fulfilled (see NvM_Prv_Write_CheckBlockData)
 *   - no other operation on the block to be written is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *   - Required queue for single-block requests is not full
 *     (see NvM_Prv_RequestQueue_Enqueue called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of the wrte request from the configured permanent RAM block this function executes
 * following actions:
 * - status of the configured permanent RAM block is set to VALID
 * - request result is set to pending state (i.e. NVM_GetErrorStatus() will return NVM_REQ_PENDING
 *   for the requested block)
 * - bit which globally represents the request in administrative-block is set
 * - dependent on the block configuration the write request is enqueued into the standard priority or
 *   into the immediate priority queue
 *
 * \note
 * Actually the initiation of a single-block request to write a NVRAM-block from the configured permanent RAM block is
 * implemented in the private function NvM_Prv_WriteBlock().
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_WritePRAMBlockBase()
 * which invokes the private function NvM_Prv_WriteBlock().
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_WriteBlock() directly to get the data written from the configured permanent RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be written from the configured permanent RAM block
 *
 * \return
 * Initiation status:
 * - E_OK = write request from the configured permanent RAM block has been initiated successfully
 * - E_NO_OK = initiation of the write request from the configured permanent RAM block has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_WritePRAMBlock(NvM_BlockIdType BlockId)
{
    // TRACE[SWS_NvM_00793] Service to copy the data of the RAM block to its corresponding permanent RAM block
    // TRACE[SWS_NvM_00794] The job of this service shall copy the data of the permanent RAM block to the NV block
    // TRACE[SWS_NvM_00812] The job of this service shall write the compiled NVRAM configuration ID
    //                      to the stored NVRAM configuration ID (block 1)

#ifdef NVM_CFG_API_WRITEPRAM_BLOCK
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_WRITEPRAM_BLOCK(BlockId);
#else
    return NvM_Prv_WriteBlock(NVM_SERVICE_ID_WRITE_PRAM_BLOCK, BlockId, NULL_PTR);
#endif
}

#ifdef NVM_CFG_API_WRITEPRAM_BLOCK
/**
 * \brief
 * This public function provides the possibility to write a NVRAM-block from the configured permanent RAM block
 * into the non-volatile memory within the overloaded API for NvM_WritePRAMBlock().
 *
 * \details
 * To write a NVRAM-block this function invokes the private function NvM_Prv_WriteBlock().
 *
 * \attention
 * This function is only available if user has configured an overloaded API for the request to write
 * the configured permanent RAM block into the non-volatile memory.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that NvM writes data
 * from the configured permanent RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be written from the configured permanent RAM block
 * \param[in] NvM_DstPtr
 * Pointer to the RAM block for data to write
 *
 * \return
 * Initiation status:
 * - E_OK = write request from the configured permanent RAM block has been initiated successfully
 * - E_NO_OK = initiation of the write request from the configured permanent RAM block has failed
 */
Std_ReturnType NvM_Rb_WritePRAMBlockBase(NvM_BlockIdType BlockId)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3281] Support API overloading for NvM_WritePRAMBlock
    return NvM_Prv_WriteBlock(NVM_SERVICE_ID_WRITE_PRAM_BLOCK, BlockId, NULL_PTR);
}
#endif

/**
 * \brief
 * This local private function implements the actual initiation of a single-block request to write the data
 * from the RAM block to the corresponding NVRAM-block in the non-volatile memory
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] idService_uo
 * ID of the service to be initiated
 * \param[in] BlockId
 * ID of the block to be written into the non-volatile memory
 * \param[in] NvM_SrcPtr
 * Pointer to the RAM block with data to write
 *
 * \return
 * Initiation status:
 * - E_OK = write request from the RAM block has been initiated successfully
 * - E_NOT_OK = initiation of the write request from the RAM block has failed
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_WriteBlock(NvM_Prv_idService_tuo idService_uo,
                                               NvM_BlockIdType idBlock_uo,
                                               NvM_Rb_ConstVoidPtr NvM_SrcPtr)
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_idQueue_tuo idQueue_uo;
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // The following union is necessary to remove an otherwise unavoidable compiler warning
    // due to a discarded const qualifier
    union
    {
        const void *pcv;
        void *pv;
    } adrRamBlock_un;
    adrRamBlock_un.pcv = NvM_SrcPtr;

    // put all service parameters in a structure for new block data
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = adrRamBlock_un.pv;
    BlockData_st.QueueEntry_st.idService_uo                 = idService_uo;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Write_e;
    BlockData_st.Result_uo                                  = NVM_REQ_PENDING;
    BlockData_st.errorInfoDetail_en                         = NvM_Rb_ErrorInfoDetail_NotAvailable_e;
    BlockData_st.idxDataset_u8                              = 0u;
    BlockData_st.maskBitsToChange_uo                        = 0u;
    BlockData_st.maskBitsNewValue_uo                        = 0u;

    idQueue_uo = NvM_Prv_idQueue_Standard_e;

    ServiceConfiguration_st.CheckPendingBlock_b     = TRUE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = idBlock_uo;
    ServiceConfiguration_st.UpdateData_pfct         = &NvM_Prv_Write_UpdateData;
    ServiceConfiguration_st.CheckParameter_pfct     = &NvM_Prv_Write_CheckParameter;
    ServiceConfiguration_st.CheckBlockData_pfct     = &NvM_Prv_Write_CheckBlockData;
    ServiceConfiguration_st.SetBlockData_pfct       = &NvM_Prv_Write_SetBlockData;

    // TRACE[SWS_NvM_00614] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    // TRACE[SWS_NvM_00827] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    // TRACE[SWS_NvM_00828] Report the DET error NVM_E_BLOCK_PENDING when another request is pending for the passed block
    // TRACE[SWS_NvM_00618] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
    // TRACE[SWS_NvM_00829] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
    // TRACE[SWS_NvM_00798] Take over given parameters, queue the read request in the job queue and return
    stReturn_uo = NvM_Prv_Service_InitiateSingle(idQueue_uo,
                                                 &BlockData_st,
                                                 &ServiceConfiguration_st);

    // Return whether this operation was successful
    return stReturn_uo;
}

/**
 * \brief
 * This local private function defines updates for all data before initiating the request to write a NVRAM-block
 *
 * \details
 * This function updates following data:
 * - ID of the internal user request queue to be used for current request
 *   (see NvM_Prv_BlkDesc_GetJobPriorityForModifyingServices)
 * - RAM block to be used for current request,
 *   if pointer to the RAM block passed by the user is a NULL pointer then NvM will write the data from the configured
 *   permanent RAM block to the required NVRAM-block.
 * - masks to set block status to VALID after successful initiation of the request, if user uses configured permanent
 *   RAM block.
 *
 * \param[inout] BlockData_pst
 * Pointer to the structure with data to be updated
 *
 * \return
 * ID of the internal user request queue to be used
 */
static NvM_Prv_idQueue_tuo NvM_Prv_Write_UpdateData(NvM_Prv_BlockData_tst* BlockData_pst)
{
    NvM_Prv_idQueue_tuo idQueue_uo;
    void* ptrPermanentRamBlock_pv = NvM_Prv_BlkDesc_GetPRamBlockAddress(BlockData_pst->QueueEntry_st.idBlock_uo);

    // determine which queue is required
    // TRACE[SWS_NvM_00378] Only single block write requests for immediate blocks are queued in the immediate queue
    //                      All other single block requests are queued in the standard queue.
    idQueue_uo = NvM_Prv_BlkDesc_GetIdQueueForModifyingServices(BlockData_pst->QueueEntry_st.idBlock_uo);


    // determine whether permanent RAM block shall be used
    if (NULL_PTR == BlockData_pst->QueueEntry_st.BlockData_un.ptrRamBlock_pv)
    {
        // TRACE[SWS_NvM_00280] Take the permanent RAM block only if no temporary RAM block has been specified
        BlockData_pst->QueueEntry_st.BlockData_un.ptrRamBlock_pv = ptrPermanentRamBlock_pv;
    }

    // if configured permanent RAM block is used then its status will be set to VALID
    if (BlockData_pst->QueueEntry_st.BlockData_un.ptrRamBlock_pv == ptrPermanentRamBlock_pv)
    {
        // TRACE[SWS_NVM_00303] Set the status of the permanent RAM block to VALID before processing starts
        // TRACE[SWS_NvM_00803] Set the status of the permanent RAM block to VALID before processing starts
        BlockData_pst->maskBitsToChange_uo = NVM_BLOCK_STATE_RAM_VALID;
        BlockData_pst->maskBitsNewValue_uo = NVM_BLOCK_STATE_RAM_VALID;
    }

    return idQueue_uo;
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to write a NVRAM-block safely.
 *
 * \details
 * This function implements following special parameter checks:
 * - check if used RAM block (includes also explicit synchronization) is valid,
 *   if this check fails then this function sets the corresponding DET error
 *   (see NvM_Prv_ErrorDetection_IsRamBlockAddressValid())
 *
 * \note
 * The NvM implements all parameter checks which are common for all NvM requests (services) within the unit
 * NvM_Service.c, e.g. check for uninitialized NvM or check for invalid ID of the requested block.
 *
 *
 * \param[in] BlockData_pcst
 * Pointer to the constant structure with data to be checked for validity
 *
 * \return
 * Status of the parameter check:
 * TRUE = all checks passed -> initiation of the write request is possible
 * FALSE = at least one check fails -> initiation of the write request is NOT possible
 */
static boolean NvM_Prv_Write_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // TRACE[SWS_NvM_00622] Report the DET error NVM_E_PARAM_ADDRESS when no permanent RAM block is configured and
    //                      a NULL pointer is passed via the parameter NvM_SrcPtr
    // TRACE[SWS_NvM_00210] Same as NVM622. It includes the case where explicit synchronization is configured
    //                      for this block and no NvM mirror exists
    // TRACE[SWS_NvM_00893] Report the DET error NVM_E_PARAM_ADDRESS when no permanent RAM block is configured
    //                      (includes also explicit configuration)
    return NvM_Prv_ErrorDetection_IsRamBlockAddressValid(BlockData_pcst->QueueEntry_st.idService_uo,
                                                         BlockData_pcst->QueueEntry_st.idBlock_uo,
                                                         BlockData_pcst->QueueEntry_st.BlockData_un.ptrRamBlock_pv);
}

/**
 * \brief
 * This local private function defines validity checks for the block data to ensure that the NvM can initiate
 * the request to write a NVRAM-block safely.
 *
 * \details
 * This function implements following special block data checks:
 * - check if used block is writeable, if this check fails then parameter Errors_puo contains occured error
 *   (see NvM_Prv_Block_IsWriteable())
 *
 * \note
 * The NvM implements all block data checks which are common for all NvM requests (services) within the unit
 * NvM_Service.c, e.g. check if another request is pending for the requested block.
 *
 * \attention
 * The unit NvM_Service.c invokes this function with active scheduler lock, that means that this function shall NOT
 * invoke any external functions, e.g. DET functions.
 *
 * \param[in] BlockData_pcst
 * Pointer to the constant structure with data to be checked for validity
 * \param[inout] Errors_puo
 * Pointer to the errors which can occure during block data check
 *
 * \return
 * Status of the block data check:
 * TRUE = all checks passed -> initiation of the write request is possible
 * FALSE = at least one check fails -> initiation of the write request is NOT possible
 */
static boolean NvM_Prv_Write_CheckBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                            NvM_Prv_Service_InitiationErrors_tuo *Errors_puo)
{
    // TRACE[SWS_NvM_00209] Check write protection status of this block before enqueuing this request
    // TRACE[SWS_NvM_00377] Check that the dataset index does not point to a ROM block if the NvM block is of type DATASET
    // TRACE[SWS_NvM_00799] Check write protection status of this block before enqueuing this request
    // TRACE[SWS_NvM_00377] Do the same if a writing to a ROM block of an NvM block of type DATASET is attempted
    // TRACE[SWS_NvM_00217] If a write protected block is attempted to be written, E_NOT_OK is returned
    // TRACE[SWS_NvM_00377] If the dataset index points to a ROM block (in case the NvM block is of type DATASET) and
    //                      a write is attempted, E_NOT_OK is returned
    // TRACE[SWS_NvM_00375] Writing to an NV block of a block of type DATASET is not possible
    //                      if the block is write protected
    // TRACE[SWS_NvM_00796] If a write protected block is attempted to be written, E_NOT_OK is returned
    return NvM_Prv_Block_IsWriteable(BlockData_pcst->QueueEntry_st.idBlock_uo,
                                     NvM_Prv_Block_GetIdxDataset(BlockData_pcst->QueueEntry_st.idBlock_uo),
                                     Errors_puo);
}

/**
 * \brief
 * This local private function defines which block data will be set in case of a successful initiation of the request
 * to write a NVRAM-block.
 *
 * \details
 * This function sets following block data:
 * - request result is set to pending state (i.e. NVM_GetErrorStatus() will return NVM_REQ_PENDING
 *   for the requested block)
 * - the error info detail is set to not available (i.e. NvM_Rb_GetErrorInfoDetail will return
 *   NvM_Rb_ErrorInfoDetail_NotAvailable_e for the requested block)
 * - bit which globally represents the request in administrative block is set to signal internally
 *   which request is pending
 * - block status information as required
 *
 * \note
 * The NvM sets block data which are common for all NvM requests (services) within the unit NvM_Service.c,
 * e.g. enqueuing of the block into the required queue.
 *
 * \attention
 * The unit NvM_Service.c invokes this function with active scheduler lock, that means that this function shall NOT
 * invoke any external functions, e.g. DET functions.
 *
 * \param[in] BlockData_pcst
 * Pointer to the constant structure with data to be set in case of a successful initiation of the write request
 */
static void NvM_Prv_Write_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    NvM_BlockIdType idBlock_uo = BlockData_pcst->QueueEntry_st.idBlock_uo;

    // TRACE[SWS_NVM_00185] Set single block request result to NVM_REQ_PENDING on successful enqueuing
    NvM_Prv_Block_SetRequestResult(idBlock_uo, BlockData_pcst->Result_uo);
    NvM_Prv_Block_SetErrorInfoDetail(idBlock_uo, BlockData_pcst->errorInfoDetail_en);
    NvM_Prv_Block_SetRequest(idBlock_uo, BlockData_pcst->QueueEntry_st.ServiceBit_uo);
    NvM_Prv_Block_SetState(idBlock_uo, BlockData_pcst->maskBitsToChange_uo, BlockData_pcst->maskBitsNewValue_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

