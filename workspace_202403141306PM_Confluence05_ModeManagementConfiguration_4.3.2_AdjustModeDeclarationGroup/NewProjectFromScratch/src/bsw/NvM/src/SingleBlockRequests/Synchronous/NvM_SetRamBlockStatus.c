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
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#ifdef NVM_CFG_API_SET_RAM_BLOCK_STATUS
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_SET_RAM_BLOCK_STATUS(NvM_BlockIdType BlockId, boolean BlockChanged);
#endif

LOCAL_INLINE Std_ReturnType NvM_Prv_SetRamBlockStatus(NvM_BlockIdType idBlock_uo, boolean BlockChanged);
static NvM_Prv_idQueue_tuo NvM_Prv_SetRamBlockStatus_UpdateData(NvM_Prv_BlockData_tst* BlockData_pst);
static boolean NvM_Prv_SetRamBlockStatus_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);
static void NvM_Prv_SetRamBlockStatus_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst);

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
 * This public function initiates a single-block request to set permanent RAM block status for a NVRAM-block.
 *
 * \details
 * A request to set permanent RAM block status will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled excluding preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_SetRamBlockStatus_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - no other operation on required block is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of this request this function executes following actions:
 * - permanent RAM block status is set as required
 *
 * \note
 * Actually the initiation of a single-block request to set permanent RAM block status is implemented
 * in the private function NvM_Prv_SetRamBlockStatus.
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_SetRamBlockStatusBase
 * which invokes the private function NvM_Prv_SetRamBlockStatus.
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_SetRamBlockStatus directly to get the data invalidated.
 *
 * \param[in] BlockId
 * ID of the block for which permanent RAM block status will be set
 * \param[in] BlockChanged
 * Required state for permanent RAM block status:
 * - TRUE = permanent RAM block is VALID/CHANGED
 * - FALSE = permanent RAM block is INVALID/UNCHANGED
 *
 * \return
 * Initiation status:
 * - E_OK = request to set permanent RAM block status has been initiated successfully
 * - E_NO_OK = initiation of the request to set permanent RAM block status has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_SetRamBlockStatus(NvM_BlockIdType BlockId, boolean BlockChanged)
{
    // TRACE[NVM453] Service for setting the permanent RAM block status of an NVRAM block

#ifdef NVM_CFG_API_SET_RAM_BLOCK_STATUS
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_SET_RAM_BLOCK_STATUS(BlockId, BlockChanged);
#else
    return NvM_Prv_SetRamBlockStatus(BlockId, BlockChanged);
#endif
}

#ifdef NVM_CFG_API_SET_RAM_BLOCK_STATUS
/**
 * \brief
 * This public function provides the possibility to set permanent RAM block status within the overloaded API
 * for NvM_SetRamBlockStatus.
 *
 * \details
 * To set block protection this function invokes the private function NvM_Prv_SetRamBlockStatus.
 *
 * \attention
 * This function is only available if user has configured an overloaded API for current request.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that permanent RAM block status
 * is set as required.
 *
 * \param[in] BlockId
 * ID of the block for which permanent RAM block status will be set
 * \param[in] BlockChanged
 * Required state for permanent RAM block status:
 * - TRUE = permanent RAM block is VALID/CHANGED
 * - FALSE = permanent RAM block is INVALID/UNCHANGED
 *
 * \return
 * Initiation status:
 * - E_OK = request to set permanent RAM block status has been initiated successfully
 * - E_NO_OK = initiation of the request to set permanent RAM block status has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_Rb_SetRamBlockStatusBase(NvM_BlockIdType BlockId, boolean BlockChanged)
{
    return NvM_Prv_SetRamBlockStatus(BlockId, BlockChanged);
}
#endif

/**
 * \brief
 * This local private function implements the actual initiation of a single-block request
 * to set permanent RAM block status.
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] idBlock_uo
 * ID of the block for which permanent RAM block status will be set
 * \param[in] BlockChanged
 * Required state for permanent RAM block status:
 * - TRUE = permanent RAM block is VALID/CHANGED
 * - FALSE = permanent RAM block is INVALID/UNCHANGED
 *
 * \return
 * Initiation status:
 * - E_OK = request to set permanent RAM block status has been initiated successfully
 * - E_NO_OK = initiation of the request to set permanent RAM block status has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_SetRamBlockStatus(NvM_BlockIdType idBlock_uo, boolean BlockChanged)
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // put all service parameters in a structure for a queue entry
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = NULL_PTR;
    BlockData_st.QueueEntry_st.idService_uo                 = NVM_SERVICE_ID_SET_RAM_BLOCK_STATUS;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Unspecified_e;
    BlockData_st.Result_uo                                  = NVM_REQ_OK;
    BlockData_st.idxDataset_u8                              = 0u;
    // TRACE[NVM405] BlockChanged FALSE -> set RAM block status to INVALID-UNCHANGED
    // TRACE[NVM453] Invalidate the RAM block and mark block as unchanged
    // TRACE[SWS_NvM_00121] Recalculation of the permanent RAM block CRC is required
    // TRACE[SWS_NvM_00121] Recalculation of the permanent RAM block CRC is not finished
    // TRACE[SWS_NvM_00121] Abort a pending recalculation of the permanent RAM block CRC
    BlockData_st.maskBitsToChange_uo =    NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_ONGOING;

    if (BlockChanged)
    {
        // TRACE[NVM406] BlockChanged TRUE -> set RAM block status to VALID-CHANGED
        // TRACE[NVM453] Validate the RAM block and mark block as changed
        // TRACE[SWS_NvM_00121] Recalculation of the permanent RAM block CRC is not finished for blocks with RAM CRC
        // TRACE[SWS_NvM_00121] Recalculation of the permanent RAM block CRC is finished for blocks without RAM CRC
        // TRACE[SWS_NvM_00121] Abort a pending recalculation of the permanent RAM block CRC
        BlockData_st.maskBitsNewValue_uo =   NVM_BLOCK_STATE_RAM_VALID
                                           | NVM_BLOCK_STATE_RAM_CHANGED;
    }
    else
    {
        // Invalid, unchanged, no CRC recalc required/ongoing
        BlockData_st.maskBitsNewValue_uo = 0u;
    }

    ServiceConfiguration_st.CheckPendingBlock_b     = TRUE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = idBlock_uo;
    ServiceConfiguration_st.UpdateData_pfct         = &NvM_Prv_SetRamBlockStatus_UpdateData;
    ServiceConfiguration_st.CheckParameter_pfct     = &NvM_Prv_SetRamBlockStatus_CheckParameter;
    // No check for block data required for set RAM block status request
    ServiceConfiguration_st.CheckBlockData_pfct     = NULL_PTR;
    ServiceConfiguration_st.SetBlockData_pfct       = &NvM_Prv_SetRamBlockStatus_SetBlockData;

    stReturn_uo = NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_nrQueues_e,
                                                 &BlockData_st,
                                                 &ServiceConfiguration_st);

    // Return whether this operation was successful
    return stReturn_uo;
}

/**
 * \brief
 * This local private function defines updates for all data before initiating the request to set permanent RAM block
 * status.
 *
 * \details
 * This function updates following data:
 * - masks to set block status for requiring CRC recalculation after successful initiation of the request,
 *   if current block is configured for RAM block CRC and block status shall be changed to VALID-CHANGED.
 *
 * \param[inout] BlockData_pst
 * Pointer to the structure with data to be updated
 *
 * \return
 * User request to set RAM block status is synchronous and is not enqueued (s. NvM_Prv_idQueue_nrQueues_e)
 */
static NvM_Prv_idQueue_tuo NvM_Prv_SetRamBlockStatus_UpdateData(NvM_Prv_BlockData_tst* BlockData_pst)
{
    if (   NvM_Prv_BlkDesc_IsBlockSelected(BlockData_pst->QueueEntry_st.idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC)
        && ((NVM_BLOCK_STATE_RAM_VALID | NVM_BLOCK_STATE_RAM_CHANGED) == BlockData_pst->maskBitsNewValue_uo))
    {
        BlockData_pst->maskBitsNewValue_uo |= NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ;
    }

    return NvM_Prv_idQueue_nrQueues_e;
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to set permanent RAM block status.
 *
 * \details
 * This function implements following special parameter checks:
 * - check if NVRAM-block has configured a permanent RAM block
 *   (see NvM_Prv_BlkDesc_GetPRamBlockAddress())
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
 * TRUE = all checks passed -> initiation of the request is possible
 * FALSE = at least one check fails -> initiation of the request is NOT possible
 */
static boolean NvM_Prv_SetRamBlockStatus_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // -> check whether the selected block has an associated RAM block
    // TRACE[NVM240] If no permanent RAM block has been configured for the block, this function has no effect
    return (NULL_PTR != NvM_Prv_BlkDesc_GetPRamBlockAddress(BlockData_pcst->QueueEntry_st.idBlock_uo));
}

/**
 * \brief
 * This local private function defines which block data will be set in case of a successful initiation of the request
 * to set permanent RAM block status.
 *
 * \details
 * This function sets following block data:
 * - permanent RAM block status is set as required
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
 * Pointer to the constant structure with data to be set in case of a successful initiation of the request
 */
static void NvM_Prv_SetRamBlockStatus_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    NvM_Prv_Block_SetState(BlockData_pcst->QueueEntry_st.idBlock_uo,
                           BlockData_pcst->maskBitsToChange_uo,
                           BlockData_pcst->maskBitsNewValue_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

