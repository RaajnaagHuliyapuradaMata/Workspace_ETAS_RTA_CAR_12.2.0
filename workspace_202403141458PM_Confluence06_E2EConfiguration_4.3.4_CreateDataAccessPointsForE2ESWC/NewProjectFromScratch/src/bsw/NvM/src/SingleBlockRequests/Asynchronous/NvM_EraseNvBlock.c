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

#ifdef NVM_CFG_API_ERASE_NV_BLOCK
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5, 8.6 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_ERASE_NV_BLOCK(NvM_BlockIdType BlockId);
#endif

LOCAL_INLINE Std_ReturnType NvM_Prv_EraseNvBlock(NvM_BlockIdType idBlock_uo);
static boolean NvM_Prv_Erase_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);
static boolean NvM_Prv_Erase_CheckBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                            NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);
static void NvM_Prv_Erase_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst);

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
 * This public function initiates a single-block request to erase a NVRAM-block from the non-volatile memory.
 *
 * \details
 * An erasing request will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled including preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_Erase_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - all validity checks for the block data are fulfilled (see NvM_Prv_Erase_CheckBlockData)
 *   - no other operation on the block to be erased is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *   - Required queue for single-block requests is not full
 *     (see NvM_Prv_RequestQueue_Enqueue called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of the erasing request from the RAM block this function executes
 * following actions:
 * - request result is set to pending state (i.e. NVM_GetErrorStatus() will return NVM_REQ_PENDING
 *   for the requested block)
 * - bit which globally represents the request in administrative-block is set
 * - the erasing request is enqueued into the standard queue for single-block requests
 *
 * \note
 * Actually the initiation of a single-block request to erase a NVRAM-block is implemented in the private function
 * NvM_Prv_EraseNvBlock().
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_EraseNvBlockBase()
 * which invokes the private function NvM_Prv_EraseNvBlock().
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_EraseNvBlock() directly to erase the NVRAM-block.
 *
 * \param[in] BlockId
 * ID of the block to be erased from the non-volatile memory
 *
 * \return
 * Initiation status:
 * - E_OK = erasing request from the RAM block has been initiated successfully
 * - E_NO_OK = initiation of the erasing request from the RAM block has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_EraseNvBlock(NvM_BlockIdType BlockId)
{
    // TRACE[NVM457] Service to erase a NV block

#ifdef NVM_CFG_API_ERASE_NV_BLOCK
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_ERASE_NV_BLOCK(BlockId);
#else
    return NvM_Prv_EraseNvBlock(BlockId);
#endif
}

#ifdef NVM_CFG_API_ERASE_NV_BLOCK
/**
 * \brief
 * This public function provides the possibility to erase a NVRAM-block from the non-volatile memory
 * within the overloaded API for NvM_EraseNvBlock.
 *
 * \details
 * To erase a NVRAM-block this function invokes the private function NvM_Prv_EraseNvBlock.
 *
 * \attention
 * This function is only available if user has configured an overloaded API for the request to erase the corresponding
 * RAM block from the non-volatile memory.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that NvM erases data
 * from the non-volatile memory.
 *
 * \param[in] BlockId
 * ID of the block to be erased from the non-volatile memory
 *
 * \return
 * Initiation status:
 * - E_OK = erasing request has been initiated successfully
 * - E_NO_OK = initiation of the erasing request has failed
 */
Std_ReturnType NvM_Rb_EraseNvBlockBase(NvM_BlockIdType BlockId)
{
    return NvM_Prv_EraseNvBlock(BlockId);
}
#endif

/**
 * \brief
 * This local private function implements the actual initiation of a single-block request to erase the data
 * from the non-volatile memory
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] BlockId
 * ID of the block to be erased from the non-volatile memory
 *
 * \return
 * Initiation status:
 * - E_OK = erasing request has been initiated successfully
 * - E_NOT_OK = initiation of the erasing request has failed
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_EraseNvBlock(NvM_BlockIdType idBlock_uo)
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // put all service parameters in a structure for a queue entry
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = NULL_PTR;
    BlockData_st.QueueEntry_st.idService_uo                 = NVM_SERVICE_ID_ERASE_NV_BLOCK;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Erase_e;
    // TRACE[NVM185] Set single block request result to NVM_REQ_PENDING on successful enqueuing
    BlockData_st.Result_uo                                  = NVM_REQ_PENDING;
    BlockData_st.errorInfoDetail_en                         = NvM_Rb_ErrorInfoDetail_NotAvailable_e;
    BlockData_st.idxDataset_u8                              = 0u;
    BlockData_st.maskBitsToChange_uo                        = 0u;
    BlockData_st.maskBitsNewValue_uo                        = 0u;

    ServiceConfiguration_st.CheckPendingBlock_b     = TRUE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = idBlock_uo;
    // No data update required for erase block request
    ServiceConfiguration_st.UpdateData_pfct         = NULL_PTR;
    ServiceConfiguration_st.CheckParameter_pfct     = &NvM_Prv_Erase_CheckParameter;
    ServiceConfiguration_st.CheckBlockData_pfct     = &NvM_Prv_Erase_CheckBlockData;
    ServiceConfiguration_st.SetBlockData_pfct       = &NvM_Prv_Erase_SetBlockData;

    // TRACE[NVM631] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    // TRACE[NVM635] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
    stReturn_uo = NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_Standard_e,
                                                 &BlockData_st,
                                                 &ServiceConfiguration_st);

    // Return whether this operation was successful
    return stReturn_uo;
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to erase a NVRAM-block safely.
 *
 * \details
 * This function implements following special parameter checks:
 * - check if used NVRAM-block has immediate priority,
 *   if this check fails then this function sets the corresponding DET error
 *   (see NvM_Prv_ErrorDetection_IsBlockPriorityImmediate())
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
 * TRUE = all checks passed -> initiation of the erasing request is possible
 * FALSE = at least one check fails -> initiation of the erasing request is NOT possible
 */
static boolean NvM_Prv_Erase_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // TRACE[NVM636] Report the DET error NVM_E_BLOCK_CONFIG when the NVRAM block has not immediate priority
    return NvM_Prv_ErrorDetection_IsBlockPriorityImmediate(NVM_SERVICE_ID_ERASE_NV_BLOCK,
                                                           BlockData_pcst->QueueEntry_st.idBlock_uo);
}

/**
 * \brief
 * This local private function defines validity checks for the block data to ensure that the NvM can initiate
 * the request to erase a NVRAM-block safely.
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
 * TRUE = all checks passed -> initiation of the erasing request is possible
 * FALSE = at least one check fails -> initiation of the erasing request is NOT possible
 */
static boolean NvM_Prv_Erase_CheckBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                            NvM_Prv_Service_InitiationErrors_tuo *Errors_puo)
{
    // TRACE[NVM418] Queuing this request is done in case write-protection is disabled
    // TRACE[NVM273] Check the write protection before queuing the request and report an error
    //               if the block is write-protected
    // Check that the dataset index does not point to a ROM block if the NvM block is of type DATASET
    // To be compliant with NVM377, do the same if an erase of a ROM block of an NvM-bloc
    // of type DATASET is attempted
    // TRACE[NVM262] If a write protected block is attempted to be erased, E_NOT_OK is returned
    // TRACE[NVM661] If the dataset-index points to a ROM-block (in case the NvM-block is of type DATASET)
    //               and an erase is attempted, E_NOT_OK is returned
    return NvM_Prv_Block_IsWriteable(BlockData_pcst->QueueEntry_st.idBlock_uo,
                                     NvM_Prv_Block_GetIdxDataset(BlockData_pcst->QueueEntry_st.idBlock_uo),
                                     Errors_puo);
}

/**
 * \brief
 * This local private function defines which block data will be set in case of a successful initiation of the request
 * to erase a NVRAM-block.
 *
 * \details
 * This function sets following block data:
 * - request result is set to pending state (i.e. NVM_GetErrorStatus() will return NVM_REQ_PENDING
 *   for the requested block)
 * - the error info detail is set to not available (i.e. NvM_Rb_GetErrorInfoDetail will return
 *   NvM_Rb_ErrorInfoDetail_NotAvailable_e for the requested block)
 * - bit which globally represents the request in administrative block is set to signal internally
 *   which request is pending
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
 * Pointer to the constant structure with data to be set in case of a successful initiation of the erasing request
 */
static void NvM_Prv_Erase_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    NvM_Prv_Block_SetRequestResult(BlockData_pcst->QueueEntry_st.idBlock_uo, BlockData_pcst->Result_uo);
    NvM_Prv_Block_SetErrorInfoDetail(BlockData_pcst->QueueEntry_st.idBlock_uo, BlockData_pcst->errorInfoDetail_en);
    NvM_Prv_Block_SetRequest(BlockData_pcst->QueueEntry_st.idBlock_uo, BlockData_pcst->QueueEntry_st.ServiceBit_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

