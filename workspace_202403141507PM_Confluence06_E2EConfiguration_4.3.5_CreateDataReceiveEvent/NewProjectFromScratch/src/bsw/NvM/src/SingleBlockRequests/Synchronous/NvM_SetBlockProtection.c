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

#ifdef NVM_CFG_API_SET_BLOCK_PROTECTION
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_SET_BLOCK_PROTECTION(NvM_BlockIdType BlockId, boolean ProtectionEnabled);
#endif

LOCAL_INLINE Std_ReturnType NvM_Prv_SetBlockProtection(NvM_BlockIdType idBlock_uo, boolean ProtectionEnabled);
static boolean NvM_Prv_SetBlockProtection_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);
static void NvM_Prv_SetBlockProtection_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst);

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
 * This public function initiates a single-block request to set block protection for a NVRAM-block.
 *
 * \details
 * A request to set block protection will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled excluding preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_SetBlockProtection_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - no other operation on required block is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of this request this function executes following actions:
 * - block protection status is set as required
 *
 * \note
 * Actually the initiation of a single-block request to set block protection is implemented in the private function
 * NvM_Prv_SetBlockProtection.
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_SetBlockProtectionBase
 * which invokes the private function NvM_Prv_SetBlockProtection.
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_SetBlockProtection directly to get the data invalidated.
 *
 * \param[in] BlockId
 * ID of the block for which block protection will be set
 * \param[in] ProtectionEnabled
 * Required state for block protection:
 * - TRUE = enable block protection
 * - FALSE = disable block protection
 *
 * \return
 * Initiation status:
 * - E_OK = request to set block protection has been initiated successfully
 * - E_NO_OK = initiation of the request to set block protection has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_SetBlockProtection(NvM_BlockIdType BlockId, boolean ProtectionEnabled)
{
    // TRACE[NVM450] Service for setting/resetting the write protection for a NV block

#ifdef NVM_CFG_API_SET_BLOCK_PROTECTION
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_SET_BLOCK_PROTECTION(BlockId, ProtectionEnabled);
#else
    return NvM_Prv_SetBlockProtection(BlockId, ProtectionEnabled);
#endif
}

#ifdef NVM_CFG_API_SET_BLOCK_PROTECTION
/**
 * \brief
 * This public function provides the possibility to set block protection within the overloaded API
 * for NvM_SetBlockProtection.
 *
 * \details
 * To set block protection this function invokes the private function NvM_Prv_SetBlockProtection.
 *
 * \attention
 * This function is only available if user has configured an overloaded API for current request.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that block protection is set
 * as required.
 *
 * \param[in] BlockId
 * ID of the block for which block protection will be set
 * \param[in] ProtectionEnabled
 * Required state for block protection:
 * - TRUE = enable block protection
 * - FALSE = disable block protection
 *
 * \return
 * Initiation status:
 * - E_OK = request to set block protection has been initiated successfully
 * - E_NO_OK = initiation of the request to set block protectionhas failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
Std_ReturnType NvM_Rb_SetBlockProtectionBase(NvM_BlockIdType BlockId, boolean ProtectionEnabled)
{
    return NvM_Prv_SetBlockProtection(BlockId, ProtectionEnabled);
}
#endif

/**
 * \brief
 * This local private function implements the actual initiation of a single-block request to set block protection.
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] idBlock_uo
 * ID of the block for which block protection will be set
 * \param[in] ProtectionEnabled
 * Required state for block protection:
 * - TRUE = enable block protection
 * - FALSE = disable block protection
 *
 * \return
 * Initiation status:
 * - E_OK = request to set block protection has been initiated successfully
 * - E_NO_OK = initiation of the request to set block protectionhas failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 */
LOCAL_INLINE Std_ReturnType NvM_Prv_SetBlockProtection(NvM_BlockIdType idBlock_uo, boolean ProtectionEnabled)
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // put all service parameters in a structure for a queue entry
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = NULL_PTR;
    BlockData_st.QueueEntry_st.idService_uo                 = NVM_SERVICE_ID_SET_BLOCK_PROTECTION;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Unspecified_e;
    BlockData_st.Result_uo                                  = NVM_REQ_OK;
    BlockData_st.idxDataset_u8                              = 0u;
    // TRACE[NVM578] Allow to disable the write protection state
    BlockData_st.maskBitsToChange_uo                        = NVM_BLOCK_STATE_WRITE_PROT;
    BlockData_st.maskBitsNewValue_uo                        = 0u;

    if (ProtectionEnabled)
    {
        // TRACE[NVM016] Set the write protection flag in the administrative block according
        //               to the ProtectionEnabled parameter
        BlockData_st.maskBitsToChange_uo = NVM_BLOCK_STATE_WRITE_PROT;
        BlockData_st.maskBitsNewValue_uo = NVM_BLOCK_STATE_WRITE_PROT;
    }

    ServiceConfiguration_st.CheckPendingBlock_b     = TRUE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = idBlock_uo;
    // No data update required to set block protection
    ServiceConfiguration_st.UpdateData_pfct         = NULL_PTR;
    ServiceConfiguration_st.CheckParameter_pfct     = &NvM_Prv_SetBlockProtection_CheckParameter;
    // No check for block data required to set block protection
    ServiceConfiguration_st.CheckBlockData_pfct     = NULL_PTR;
    ServiceConfiguration_st.SetBlockData_pfct       = &NvM_Prv_SetBlockProtection_SetBlockData;

    stReturn_uo = NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_nrQueues_e,
                                                 &BlockData_st,
                                                 &ServiceConfiguration_st);

    // Return whether this operation was successful
    return stReturn_uo;
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to set block protection.
 *
 * \details
 * This function implements following special parameter checks:
 * - check if write protection is changeable for given NVRAM-block,
 *   if this check fails then this function sets the corresponding DET error
 *   (see NvM_Prv_ErrorDetection_IsBlockWriteProtectionChangeable())
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
static boolean NvM_Prv_SetBlockProtection_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // TRACE[NVM608] Report the DET error NVM_E_BLOCK_CONFIG when the NVRAM block is configured
    //               with NvMWriteBlockOnce = TRUE
    return NvM_Prv_ErrorDetection_IsBlockWriteProtectionChangeable(BlockData_pcst->QueueEntry_st.idService_uo,
                                                                   BlockData_pcst->QueueEntry_st.idBlock_uo);
}

/**
 * \brief
 * This local private function defines which block data will be set in case of a successful initiation of the request
 * to set block protection.
 *
 * \details
 * This function sets following block data:
 * - block protection status is set as required
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
static void NvM_Prv_SetBlockProtection_SetBlockData(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    NvM_Prv_Block_SetState(BlockData_pcst->QueueEntry_st.idBlock_uo,
                           BlockData_pcst->maskBitsToChange_uo,
                           BlockData_pcst->maskBitsNewValue_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

