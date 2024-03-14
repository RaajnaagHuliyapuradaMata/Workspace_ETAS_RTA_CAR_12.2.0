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
#include "NvM_Prv_BlockData_WriteCntr_Inl.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#if (NVM_RB_SET_WRITEALL_TRIGGER_API == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

# ifdef NVM_CFG_API_RB_SET_WRITE_ALL_TRIGGER
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_RB_SET_WRITE_ALL_TRIGGER(NvM_BlockIdType BlockId, boolean WriteAllTrigger);
# endif

LOCAL_INLINE Std_ReturnType NvM_Prv_SetWriteAllTrigger(NvM_BlockIdType idBlock_uo, boolean WriteAllTrigger);
static boolean NvM_Prv_SetWriteAllTrg_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);
static void NvM_Prv_SetWriteAllTrg_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst);

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
 * This public function initiates a single-block request to trigger participation in multi-block write
 * for a NVRAM-block.
 *
 * \details
 * If NvM initiates this service successfully then given NVRAM-block will participate in NvM_WriteAll unconditionally.
 *
 * \attention
 * Normal conditions for participation in multi-block write are not affected by this trigger.
 *
 * This request will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled excluding preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_AuxInvalidate_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData no checks will be done.
 *
 * In case of a successful initiation of the request this function executes following actions:
 * - status bit for WriteAll-trigger is set as required (see parameter WriteAllTrigger)
 *
 *
 * \note
 * Actually the initiation of a single-block request to trigger participation in multi-block write is implemented
 * in the private function NvM_Prv_SetWriteAllTrigger.
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_SetWriteAllTriggerBase
 * which invokes the private function NvM_Prv_SetWriteAllTrigger.
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_SetWriteAllTrigger() directly to get the data invalidated.
 *
 * \param[in] BlockId
 * ID of the block for which participation in multi-block write will be triggered
 * \param[in] WriteAllTrigger
 * Required state for WriteAll-Trigger:
 * - TRUE = Block will participate in multi-block write unconditionally
 * - FALSE = Block will not participate in multi-block write unconditionally
 *
 * \return
 * Initiation status:
 * - E_OK = request for participation in multi-block write has been initiated successfully
 * - E_NO_OK = initiation of the participation in multi-block write has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_Rb_SetWriteAllTrigger(NvM_BlockIdType BlockId, boolean WriteAllTrigger)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2977] Service for triggering a multi-block write job

# ifdef NVM_CFG_API_RB_SET_WRITE_ALL_TRIGGER
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_RB_SET_WRITE_ALL_TRIGGER(BlockId, WriteAllTrigger);
# else
    return NvM_Prv_SetWriteAllTrigger(BlockId, WriteAllTrigger);
# endif
}

# ifdef NVM_CFG_API_RB_SET_WRITE_ALL_TRIGGER
/**
 * \brief
 * This public function provides the possibility to trigger participation in multi-block write within the overloaded API
 * for NvM_Rb_SetWriteAllTrigger.
 *
 * \details
 * To trigger participation in multi-block write this function invokes the private function NvM_Prv_SetWriteAllTrigger.
 *
 * \attention
 * This function is only available if user has configured an overloaded API for current request.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that participation
 * in multi-block write is set as required.
 *
 * \param[in] BlockId
 * ID of the block for which participation in multi-block write will be triggered
 * \param[in] WriteAllTrigger
 * Required state for WriteAll-Trigger:
 * - TRUE = Block will participate in multi-block write unconditionally
 * - FALSE = Block will not participate in multi-block write unconditionally
 *
 * \return
 * Initiation status:
 * - E_OK = request for participation in multi-block write has been initiated successfully
 * - E_NO_OK = initiation of the participation in multi-block write has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_Rb_SetWriteAllTriggerBase(NvM_BlockIdType BlockId, boolean WriteAllTrigger)
{
    return NvM_Prv_SetWriteAllTrigger(BlockId, WriteAllTrigger);
}
# endif

/**
 * \brief
 * This local private function implements the actual initiation of a single-block request to trigger participation
 * in multi-block write.
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] idBlock_uo
 * ID of the block for which participation in multi-block write will be triggered
 * \param[in] WriteAllTrigger
 * Required state for WriteAll-Trigger:
 * - TRUE = Block will participate in multi-block write unconditionally
 * - FALSE = Block will not participate in multi-block write unconditionally
 *
 * \return
 * Initiation status:
 * - E_OK = request for participation in multi-block write has been initiated successfully
 * - E_NO_OK = initiation of the participation in multi-block write has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_SetWriteAllTrigger(NvM_BlockIdType idBlock_uo, boolean WriteAllTrigger)
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // put all service parameters in a structure for a queue entry
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = NULL_PTR;
    BlockData_st.QueueEntry_st.idService_uo                 = NVM_SERVICE_ID_RB_SET_WRITEALL_TRIGGER;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Unspecified_e;
    BlockData_st.Result_uo                                  = NVM_REQ_OK;
    BlockData_st.idxDataset_u8                              = 0u;
    BlockData_st.maskBitsToChange_uo                        = NVM_RB_BLOCK_STATE_MASK_TRG_WRITEALL;

    if (WriteAllTrigger)
    {
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2977] Set the flag for the storage of NvM Data during writeall
        BlockData_st.maskBitsNewValue_uo = NVM_RB_BLOCK_STATE_MASK_TRG_WRITEALL;
    }
    else
    {
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2977] Reset the flag for the storage of NvM Data during writeall
        BlockData_st.maskBitsNewValue_uo = 0u;
    }

    ServiceConfiguration_st.CheckPendingBlock_b     = FALSE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = idBlock_uo;
    // No data update required for block request to set WriteAll trigger
    ServiceConfiguration_st.UpdateData_pfct         = NULL_PTR;
    ServiceConfiguration_st.CheckParameter_pfct     = &NvM_Prv_SetWriteAllTrg_CheckParameter;
    // No check for block data required for block request to set WriteAll trigger
    ServiceConfiguration_st.CheckBlockData_pfct     = NULL_PTR;
    ServiceConfiguration_st.SetBlockData_pfct       = &NvM_Prv_SetWriteAllTrg_SetBlockData;

    stReturn_uo = NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_nrQueues_e,
                                                 &BlockData_st,
                                                 &ServiceConfiguration_st);

    // Return whether this operation was successful
    return stReturn_uo;
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to trigger participation in multi-block write.
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
static boolean NvM_Prv_SetWriteAllTrg_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // -> check whether the selected block has an associated RAM block
    return (NULL_PTR != NvM_Prv_BlkDesc_GetPRamBlockAddress(BlockData_pcst->QueueEntry_st.idBlock_uo));
}

/**
 * \brief
 * This local private function defines which block data will be set in case of a successful initiation of the request
 * to trigger participation in multi-block write.
 *
 * \details
 * This function sets following block data:
 * - status bit for WriteAll-trigger is set as required
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
static void NvM_Prv_SetWriteAllTrg_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    NvM_Prv_Block_SetState(BlockData_pcst->QueueEntry_st.idBlock_uo,
                           BlockData_pcst->maskBitsToChange_uo,
                           BlockData_pcst->maskBitsNewValue_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif
