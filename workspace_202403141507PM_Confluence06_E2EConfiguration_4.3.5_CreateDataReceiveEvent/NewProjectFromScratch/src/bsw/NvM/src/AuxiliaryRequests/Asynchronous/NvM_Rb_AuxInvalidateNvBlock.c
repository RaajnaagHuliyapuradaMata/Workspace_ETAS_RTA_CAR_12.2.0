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

#include "NvM_Prv_Service.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"

#if (NVM_RB_AUX_INTERFACE == STD_ON)
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

# ifdef NVM_CFG_API_RB_AUX_INVALIDATE_NV_BLOCK
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_RB_AUX_INVALIDATE_NV_BLOCK(NvM_BlockIdType BlockId);
# endif

LOCAL_INLINE Std_ReturnType NvM_Prv_AuxInvalidateNvBlock(NvM_BlockIdType idBlock_uo);
static boolean NvM_Prv_AuxInvalidate_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);
static boolean NvM_Prv_AuxInvalidate_CheckBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                                    NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);
static void NvM_Prv_AuxInvalidate_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst);

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
 * This public function initiates a single-block request to invalidate a NVRAM-block for auxiliary user.
 *
 * \details
 * An invalidation request will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled including preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_AuxInvalidate_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - all validity checks for the block data are fulfilled (see NvM_Prv_AuxInvalidate_CheckBlockData)
 *   - no other operation on the block to be invalidated is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *   - Required queue for auxiliary requests is not full
 *     (see NvM_Prv_RequestQueue_Enqueue called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of the invalidation request this function executes following actions:
 * - request result is set to pending state (i.e. NVM_GetErrorStatus() will return NVM_REQ_PENDING
 *   for the requested block)
 * - bit which globally represents the request in administrative-block is set
 * - the invalidation request is enqueued into the queue for auxiliary requests
 *
 * \note
 * Actually the initiation of a single-block request to invalidate a NVRAM-block is implemented in the private function
 * NvM_Prv_AuxInvalidateNvBlock.
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_AuxInvalidateNvBlockBase
 * which invokes the private function NvM_Prv_AuxInvalidateNvBlock.
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_AuxInvalidateNvBlock() directly to get the data invalidated.
 *
 * \param[in] BlockId
 * ID of the block to be invalidated
 *
 * \return
 * Initiation status:
 * - E_OK = invalidation request has been initiated successfully
 * - E_NO_OK = initiation of the invalidation request has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_Rb_AuxInvalidateNvBlock(NvM_BlockIdType BlockId)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2942] Service used from an auxiliary user to invalidate a NV block

# ifdef NVM_CFG_API_RB_AUX_INVALIDATE_NV_BLOCK
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_RB_AUX_INVALIDATE_NV_BLOCK(BlockId);
# else
    return NvM_Prv_AuxInvalidateNvBlock(BlockId);
# endif
}

# ifdef NVM_CFG_API_RB_AUX_INVALIDATE_NV_BLOCK
/**
 * \brief
 * This public function provides the possibility to invalidate a NVRAM-block within the overloaded API
 * for NvM_Rb_AuxInvalidateNvBlock().
 *
 * \details
 * To invalidate a NVRAM-block this function invokes the private function NvM_Prv_AuxInvalidateNvBlock().
 *
 * \attention
 * This function is only available if user has configured an overloaded API for the request to invalidate
 * the corresponding NVRAM-block.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that NvM invalidates data
 * for the corresponding NVRAM-block.
 *
 * \param[in] BlockId
 * ID of the block to be invalidated
 *
 * \return
 * Initiation status:
 * - E_OK = invalidation request for a NVRAM-block has been initiated successfully
 * - E_NO_OK = initiation of the invalidation request for a NVRAM-block has failed
 */
Std_ReturnType NvM_Rb_AuxInvalidateNvBlockBase(NvM_BlockIdType BlockId)
{
    return NvM_Prv_AuxInvalidateNvBlock(BlockId);
}
# endif


/**
 * \brief
 * This local private function implements the actual initiation of a single-block request to invalidate a NVRAM-block
 * for auxiliary user.
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] idBlock_uo
 * ID of the block to be invalidated
 *
 * \return
 * Initiation status:
 * - E_OK = invalidation request for a NVRAM-block has been initiated successfully
 * - E_NOT_OK = initiation of the invalidation request for a NVRAM-block has failed
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_AuxInvalidateNvBlock(NvM_BlockIdType idBlock_uo)
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // put all service parameters in a structure for new block data
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = NULL_PTR;
    BlockData_st.QueueEntry_st.idService_uo                 = NVM_SERVICE_ID_RB_AUX_INVALIDATE_NV_BLOCK;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Invalidate_e;
    BlockData_st.Result_uo                                  = NVM_REQ_PENDING;
    BlockData_st.errorInfoDetail_en                         = NvM_Rb_ErrorInfoDetail_NotAvailable_e;
    BlockData_st.idxDataset_u8                              = 0u;
    BlockData_st.maskBitsToChange_uo                        = 0u;
    BlockData_st.maskBitsNewValue_uo                        = 0u;

    ServiceConfiguration_st.CheckPendingBlock_b     = TRUE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = NVM_PRV_AUX_ADMIN_BLOCK;
    // No data update required for auxiliary invalidation block request
    ServiceConfiguration_st.UpdateData_pfct         = NULL_PTR;
    ServiceConfiguration_st.CheckParameter_pfct     = NvM_Prv_AuxInvalidate_CheckParameter;
    ServiceConfiguration_st.CheckBlockData_pfct     = NvM_Prv_AuxInvalidate_CheckBlockData;
    ServiceConfiguration_st.SetBlockData_pfct       = NvM_Prv_AuxInvalidate_SetBlockData;

    // TRACE[NVM638] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    // TRACE[NVM642] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
    stReturn_uo = NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_Aux_e,
                                                 &BlockData_st,
                                                 &ServiceConfiguration_st);

    // Return whether this operation was successful
    return stReturn_uo;
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to invalidate a NVRAM-block safely.
 *
 * \details
 * This function implements following special parameter checks:
 * - check if dataset index set for used NVRAM block is valid (the check is done only for blocks of type DATASET),
 *   if this check fails then this function sets the corresponding DET error
 *   (see NvM_Prv_ErrorDetection_IsBlockIdxValid())
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
 * TRUE = all checks passed -> initiation of the invalidation request is possible
 * FALSE = at least one check fails -> initiation of the invalidation request is NOT possible
 */
static boolean NvM_Prv_AuxInvalidate_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    boolean isParameterValid_b = FALSE;

    // If currently set dataset index is out of range for this block
    // -> reject this request with E_NOT_OK and report this situation to Det if Det reporting is enabled
    // Note: Since the set index will be used only for dataset blocks
    //       it is not necessary to check whether the set index is greater than 0 for non-dataset blocks
    // Note: in the standard API, dataset index limits are already checked in the SetDataIndex service,
    //       but this is not possible in the auxiliary interface because its SetDataIndex service has
    //       no BlockId parameter)
    if (NVM_BLOCK_DATASET == NvM_Prv_BlkDesc_GetType(BlockData_pcst->QueueEntry_st.idBlock_uo))
    {
        if (NvM_Prv_ErrorDetection_IsBlockIdxValid(BlockData_pcst->QueueEntry_st.idService_uo,
                                                   BlockData_pcst->QueueEntry_st.idBlock_uo,
                                                   NvM_Prv_Block_GetIdxDataset(NVM_PRV_AUX_ADMIN_BLOCK)))
        {
            isParameterValid_b = TRUE;
        }
    }
    else
    {
        isParameterValid_b = TRUE;
    }
    return isParameterValid_b;
}

/**
 * \brief
 * This local private function defines validity checks for the block data to ensure that the NvM can initiate
 * the request to invalidate a NVRAM-block safely.
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
 * TRUE = all checks passed -> initiation of the invalidation request is possible
 * FALSE = at least one check fails -> initiation of the invalidation request is NOT possible
 */
static boolean NvM_Prv_AuxInvalidate_CheckBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                                    NvM_Prv_Service_InitiationErrors_tuo *Errors_puo)
{
    // TRACE[NVM424] Check write protection status of this block before enqueuing this request
    // Check that the dataset index does not point to a ROM block if the NvM block is of type DATASET
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2937] Write protection shall be respected by an auxiliary user
    // TRACE[NVM377] Do the same if a writing to a ROM block of an NvM block of type DATASET is attempted
    // TRACE[NVM217] If a write protected block is attempted to be written, E_NOT_OK is returned
    // TRACE[NVM377] If the dataset index points to a ROM block (in case the NvM block is of type DATASET) and
    //               a write is attempted, E_NOT_OK is returned
    // TRACE[NVM375] Writing to an NV block of a block of type DATASET is not possible
    //               if the block is write protected
    return NvM_Prv_Block_IsWriteable(BlockData_pcst->QueueEntry_st.idBlock_uo,
                                     NvM_Prv_Block_GetIdxDataset(NVM_PRV_AUX_ADMIN_BLOCK),
                                     Errors_puo);
}

/**
 * \brief
 * This local private function defines which block data will be set in case of a successful initiation of the request
 * to invalidate a NVRAM-block.
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
 * Pointer to the constant structure with data to be set in case of a successful initiation of the invalidation request
 */
static void NvM_Prv_AuxInvalidate_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // TRACE[NVM185] Set single block request result to NVM_REQ_PENDING on successful enqueuing
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2929] Use special auxiliary admin block for the request result update
    NvM_Prv_Block_SetRequestResult(NVM_PRV_AUX_ADMIN_BLOCK, BlockData_pcst->Result_uo);
    NvM_Prv_Block_SetErrorInfoDetail(NVM_PRV_AUX_ADMIN_BLOCK, BlockData_pcst->errorInfoDetail_en);
    NvM_Prv_Block_SetRequest(NVM_PRV_AUX_ADMIN_BLOCK, BlockData_pcst->QueueEntry_st.ServiceBit_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif
