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

#ifdef NVM_CFG_API_READ_BLOCK
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5, 8.6 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_READ_BLOCK(NvM_BlockIdType BlockId, NvM_Rb_VoidPtr NvM_DstPtr);
#endif

#ifdef NVM_CFG_API_READPRAM_BLOCK
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5, 8.6 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_READPRAM_BLOCK(NvM_BlockIdType BlockId);
#endif

LOCAL_INLINE Std_ReturnType NvM_Prv_ReadBlock(NvM_Prv_idService_tuo idService_uo,
                                              NvM_BlockIdType idBlock_uo,
                                              NvM_Rb_VoidPtr RamBlock_po);
static NvM_Prv_idQueue_tuo NvM_Prv_Read_UpdateData(NvM_Prv_BlockData_tst* BlockData_pst);
static boolean NvM_Prv_Read_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);
static void NvM_Prv_Read_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst);

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
 * This public function initiates a single-block request to read a NVRAM-block from the non-volatile memory
 * into the corresponding RAM block.
 *
 * \details
 * A read request into the corresponding RAM block will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled including preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_Read_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - no other operation on the block to be invalidated is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *   - Required queue for single-block requests is not full
 *     (see NvM_Prv_RequestQueue_Enqueue called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of the read request into the configured permanent RAM block this function executes
 * following actions:
 * - if configured permanent RAM block is used then its status is set to INVALID
 * - request result is set to pending state (i.e. NVM_GetErrorStatus will return NVM_REQ_PENDING
 *   for the requested block)
 * - bit which globally represents the request in administrative-block is set
 * - status of the configured permanent RAM block is set to INVALID if used for reading
 * - read request is enqueued into the standard priority queue
 *
 * \note
 * Actually the initiation of a single-block request to read a NVRAM-block into the corresponding RAM block is
 * implemented in the private function NvM_Prv_ReadBlock().
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_ReadBlockBase()
 * which invokes the private function NvM_Prv_ReadBlock().
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_ReadBlock() directly to get the data written from the corresponding RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be read into the corresponding RAM block
 * \param[in] NvM_DstPtr
 * Pointer to the RAM block for read data
 *
 * \return
 * Initiation status:
 * - E_OK = read request into the configured permanent RAM block has been initiated successfully
 * - E_NO_OK = initiation of the read request into the configured permanent RAM block has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_ReadBlock(NvM_BlockIdType BlockId, NvM_Rb_VoidPtr NvM_DstPtr)
{
    // TRACE[SWS_NvM_00454] Service to copy the data of the NV block to its corresponding RAM block

#ifdef NVM_CFG_API_READ_BLOCK
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_READ_BLOCK(BlockId, NvM_DstPtr);
#else
    return NvM_Prv_ReadBlock(NVM_SERVICE_ID_READ_BLOCK, BlockId, NvM_DstPtr);
#endif
}

#ifdef NVM_CFG_API_READ_BLOCK
/**
 * \brief
 * This public function provides the possibility to read a NVRAM-block into the corresponding RAM block
 * from the non-volatile memory within the overloaded API for NvM_ReadBlock().
 *
 * \details
 * To read a NVRAM-block this function invokes the private function NvM_Prv_ReadBlock().
 *
 * \attention
 * This function is only available if user has configured an overloaded API for the request to read the corresponding
 * RAM block from the non-volatile memory.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that NvM reads data
 * into the corresponding RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be read from the corresponding RAM block
 * \param[in] NvM_DstPtr
 * Pointer to the corresponding RAM block for read data
 *
 * \return
 * Initiation status:
 * - E_OK = read request into the corresponding RAM block has been initiated successfully
 * - E_NO_OK = initiation of the read request into the corresponding RAM block has failed
 */
Std_ReturnType NvM_Rb_ReadBlockBase(NvM_BlockIdType BlockId, NvM_Rb_VoidPtr NvM_DstPtr)
{
    return NvM_Prv_ReadBlock(NVM_SERVICE_ID_READ_BLOCK, BlockId, NvM_DstPtr);
}
#endif

/**
 * \brief
 * This public function initiates a single-block request to read a NVRAM-block from the non-volatile memory
 * into the configured permanent RAM block.
 *
 * \details
 * A read request into the configured permanent RAM block will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled including preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_Read_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - no other operation on the block to be invalidated is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *   - Required queue for single-block requests is not full
 *     (see NvM_Prv_RequestQueue_Enqueue called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of the read request into the configured permanent RAM block this function executes
 * following actions:
 * - request result is set to pending state (i.e. NVM_GetErrorStatus will return NVM_REQ_PENDING
 *   for the requested block)
 * - bit which globally represents the request in administrative-block is set
 * - status of the configured permanent RAM block is set to INVALID
 * - read request is enqueued into the standard priority queue
 *
 * \note
 * Actually the initiation of a single-block request to read a NVRAM-block into the configured permanent RAM block is
 * implemented in the private function NvM_Prv_ReadBlock().
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_ReadPRAMBlockBase()
 * which invokes the private function NvM_Prv_ReadBlock().
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_ReadBlock() directly to get the data read into the configured permanent RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be read into the configured permanent RAM block
 *
 * \return
 * Initiation status:
 * - E_OK = read request into the configured permanent RAM block has been initiated successfully
 * - E_NO_OK = initiation of the read request into the configured permanent RAM block has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_ReadPRAMBlock(NvM_BlockIdType BlockId)
{
    // TRACE[SWS_NvM_00764] Service to copy the data of the NV block to its corresponding permanent RAM block
    // TRACE[SWS_NvM_00765] The job of this service shall copy the data of the NV block to the permanent RAM block

#ifdef NVM_CFG_API_READPRAM_BLOCK
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_READPRAM_BLOCK(BlockId);
#else
    return NvM_Prv_ReadBlock(NVM_SERVICE_ID_READ_PRAM_BLOCK, BlockId, NULL_PTR);
#endif
}

#ifdef NVM_CFG_API_READPRAM_BLOCK
/**
 * \brief
 * This public function provides the possibility to read a NVRAM-block into the configured permanent RAM block
 * from the non-volatile memory within the overloaded API for NvM_ReadBlock().
 *
 * \details
 * To read a NVRAM-block this function invokes the private function NvM_Prv_ReadBlock().
 *
 * \attention
 * This function is only available if user has configured an overloaded API for the request
 * to read the configured permanent RAM block from the non-volatile memory.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that NvM reads data
 * into the configured permanent RAM block.
 *
 * \param[in] BlockId
 * ID of the block to be read from the corresponding RAM block
 * \param[in] NvM_DstPtr
 * Pointer to the configured permanent RAM block for read data
 *
 * \return
 * Initiation status:
 * - E_OK = read request into the configured permanent RAM block has been initiated successfully
 * - E_NO_OK = initiation of the read request into the configured permanent RAM block has failed
 */
Std_ReturnType NvM_Rb_ReadPRAMBlockBase(NvM_BlockIdType BlockId)
{
    return NvM_Prv_ReadBlock(NVM_SERVICE_ID_READ_PRAM_BLOCK, BlockId, NULL_PTR);
}
#endif

/**
 * \brief
 * This local private function implements the actual initiation of a single-block request to read a NVRAM-block
 * from the non-volatile memory into the required RAM block.
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] idService_uo
 * ID of the service to be initiated
 * \param[in] BlockId
 * ID of the block to be read into the RAM block
 * \param[in] RamBlock_po
 * Pointer to the RAM block for read data
 *
 * \return
 * Initiation status:
 * - E_OK = read request into the RAM block has been initiated successfully
 * - E_NOT_OK = initiation of the read request into the RAM block has failed
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_ReadBlock(NvM_Prv_idService_tuo idService_uo,
                                              NvM_BlockIdType idBlock_uo,
                                              NvM_Rb_VoidPtr RamBlock_po)
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // put all service parameters in a structure for new block data
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = RamBlock_po;
    BlockData_st.QueueEntry_st.idService_uo                 = idService_uo;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Read_e;
    BlockData_st.Result_uo                                  = NVM_REQ_PENDING;
    BlockData_st.errorInfoDetail_en                         = NvM_Rb_ErrorInfoDetail_NotAvailable_e;
    BlockData_st.idxDataset_u8                              = 0u;
    BlockData_st.maskBitsToChange_uo                        = 0u;
    BlockData_st.maskBitsNewValue_uo                        = 0u;

    ServiceConfiguration_st.CheckPendingBlock_b     = TRUE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = idBlock_uo;
    ServiceConfiguration_st.UpdateData_pfct         = &NvM_Prv_Read_UpdateData;
    ServiceConfiguration_st.CheckParameter_pfct     = &NvM_Prv_Read_CheckParameter;
    // No check for block data required for read block request
    ServiceConfiguration_st.CheckBlockData_pfct     = NULL_PTR;
    ServiceConfiguration_st.SetBlockData_pfct       = &NvM_Prv_Read_SetBlockData;

    // TRACE[SWS_NvM_00614] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    // TRACE[SWS_NvM_00618] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
    // TRACE[SWS_NvM_00823] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    // TRACE[SWS_NvM_00824] Report the DET error NVM_E_BLOCK_PENDING when block is already queued or in progress
    // TRACE[SWS_NvM_00826] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
    // TRACE[SWS_NvM_00766] Take over given parameters, queue the read request in the job queue and return
    // TRACE[SWS_NvM_00792] Read job for NvM_ReadPRAMBlock can be accepted only if NvM is initialized
    stReturn_uo = NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_Standard_e,
                                                 &BlockData_st,
                                                 &ServiceConfiguration_st);

    // Return whether this operation was successful
    return stReturn_uo;
}

/**
 * \brief
 * This local private function defines updates for all data before initiating the request to read a NVRAM-block
 *
 * \details
 * This function updates following data:
 * - RAM block to be used for current request,
 *   if pointer to the RAM block passed by the user is a NULL pointer then NvM will read required NVRAM-block
 *   into the configured permanent RAM block.
 * - Instead of reading default data will be restored if block to be read is located in ROM (s. NvM_Prv_Block_IsInRom)
 * - masks to set block status to INVALID after successful initiation of the request, if user uses configured permanent
 *   RAM block.
 *
 * \param[inout] BlockData_pst
 * Pointer to the structure with data to be updated
 *
 * \return
 * For user request to read a NVRAM-block standard internal queue is used (s. NvM_Prv_idQueue_Standard_e)
 */
static NvM_Prv_idQueue_tuo NvM_Prv_Read_UpdateData(NvM_Prv_BlockData_tst* BlockData_pst)
{
    void* ptrPermanentRamBlock_pv = NvM_Prv_BlkDesc_GetPRamBlockAddress(BlockData_pst->QueueEntry_st.idBlock_uo);

    // determine whether permanent RAM block shall be used
    if (NULL_PTR == BlockData_pst->QueueEntry_st.BlockData_un.ptrRamBlock_pv)
    {
        // TRACE[SWS_NvM_00278] Only take the permanent RAM block if no temporary RAM block has been specified
        BlockData_pst->QueueEntry_st.BlockData_un.ptrRamBlock_pv = ptrPermanentRamBlock_pv;
    }

    // TRACE[SWS_NvM_00374] Any assigned NV block is allowed to be read when the block is of type DATASET
    // TRACE[SWS_NvM_00769] If block is of type DATASET then set data index will be used
    if (NvM_Prv_Block_IsInRom(BlockData_pst->QueueEntry_st.idBlock_uo,
                              NvM_Prv_Block_GetIdxDataset(BlockData_pst->QueueEntry_st.idBlock_uo)))
    {
        // TRACE[SWS_NvM_00354] The block is of type DATASET and the index is pointing to an ROM block ->
        //                      prepare to queue a RESTORE request
        // TRACE[SWS_NvM_00772] The block is of type DATASET and the index is pointing to an ROM block ->
        //                      prepare to queue a RESTORE request
        // TRACE[SWS_NvM_00376] Reading ROM blocks for blocks of type DATASET is possible
        BlockData_pst->QueueEntry_st.ServiceBit_uo = NvM_Prv_ServiceBit_Restore_e;
    }

    // if configured permanent RAM block is used then its status will be set to INVALID
    if (BlockData_pst->QueueEntry_st.BlockData_un.ptrRamBlock_pv == ptrPermanentRamBlock_pv)
    {
        // TRACE[SWS_NvM_00198] Set the status of the permanent RAM block to INVALID before processing starts
        //                      in case of a READ request
        // TRACE[SWS_NvM_00227] Set the status of the permanent RAM block to INVALID before processing starts
        //                      in case of a block of type DATASET and a restore from ROM is to be performed
        // TRACE[SWS_NvM_00767] Set the status of the permanent RAM block to INVALID before processing starts
        //                      in case of a READ request
        // TRACE[SWS_NvM_00819] Set the status of the permanent RAM block to INVALID before processing starts
        //                      in case of a block of type DATASET and a restore from ROM is to be performed
        BlockData_pst->maskBitsToChange_uo = NVM_BLOCK_STATE_RAM_VALID;
        BlockData_pst->maskBitsNewValue_uo = 0u;
    }

    return NvM_Prv_idQueue_Standard_e;
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to read a NVRAM-block safely.
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
 * TRUE = all checks passed -> initiation of the read is possible
 * FALSE = at least one check fails -> initiation of the read is NOT possible
 */
static boolean NvM_Prv_Read_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // TRACE[SWS_NvM_00616] Report the DET error NVM_E_PARAM_ADDRESS when no permanent RAM block is configured and
    //                      a NULL pointer is passed via the parameter NvM_DstPtr
    // TRACE[SWS_NvM_00196] Same as NVM616. It includes the case where explicit synchronization is configured
    //                      for this block and no NvM mirror exists
    // TRACE[SWS_NvM_00825] Report the DET error NVM_E_PARAM_ADDRESS when no permanent RAM block is configured
    //                      (includes also explicit synchronization)
    return NvM_Prv_ErrorDetection_IsRamBlockAddressValid(BlockData_pcst->QueueEntry_st.idService_uo,
                                                         BlockData_pcst->QueueEntry_st.idBlock_uo,
                                                         BlockData_pcst->QueueEntry_st.BlockData_un.ptrRamBlock_pv);
}

/**
 * \brief
 * This local private function defines which block data will be set in case of a successful initiation of the request
 * to read a NVRAM-block.
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
 * Pointer to the constant structure with data to be set in case of a successful initiation of the read request
 */
static void NvM_Prv_Read_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    NvM_BlockIdType idBlock_uo = BlockData_pcst->QueueEntry_st.idBlock_uo;

    // TRACE[SWS_NvM_00185] Set single block request result to NVM_REQ_PENDING on successful enqueuing
    NvM_Prv_Block_SetRequestResult(idBlock_uo, BlockData_pcst->Result_uo);
    NvM_Prv_Block_SetErrorInfoDetail(idBlock_uo, BlockData_pcst->errorInfoDetail_en);
    NvM_Prv_Block_SetRequest(idBlock_uo, BlockData_pcst->QueueEntry_st.ServiceBit_uo);
    NvM_Prv_Block_SetState(idBlock_uo, BlockData_pcst->maskBitsToChange_uo, BlockData_pcst->maskBitsNewValue_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

