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
#include "NvM_Prv_ErrorDetection.h"

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#ifdef NVM_CFG_API_SET_BLOCK_LOCK_STATUS
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern void NVM_CFG_API_SET_BLOCK_LOCK_STATUS(NvM_BlockIdType BlockId, boolean BlockLocked);
#endif

LOCAL_INLINE void NvM_Prv_SetBlockLockStatus(NvM_BlockIdType idBlock_uo, boolean BlockLocked);
static boolean NvM_Prv_SetBlockLockStatus_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst);

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
 * This public function initiates a single-block request to set the lock status for a permanent RAM block
 * of an NVRAM block.
 *
 * \details
 * NvM currently does not support block locking via the NvM_SetBlockLockStatus service.
 * This function only provides a stub without any functionality.
 *
 * This request will be initiated only if following conditions are fulfilled:
 * - all common pre-conditions are fulfilled excluding preconditions for asynchronous single-block services
 *   (see NvM_Prv_Service_CheckPreconditions)
 * - all validity checks for all significant parameters provided by the user are fulfilled
 *   (see NvM_Prv_SetBlockLockStatus_CheckParameter)
 * - all conditions required to finalize the request initiation are fulfilled,
 *   within the function NvM_Prv_Service_CheckBlockData following checks will be done:
 *   - no other operation on required block is requested or in progress
 *     (see NvM_Prv_Block_IsPending called in NvM_Prv_Service_CheckBlockData)
 *
 * In case of a successful initiation of the request this function no actions will be executed.
 *
 *
 * \note
 * Actually the initiation of a single-block request to set the lock status is implemented in the private function
 * NvM_Prv_SetBlockLockStatus.
 * If user has configured an overloaded API for this request then this function invokes the overloaded API and
 * then within the overloaded API the user is responsible to invoke the function NvM_Rb_SetBlockLockStatusBase
 * which invokes the private function NvM_Prv_SetBlockLockStatus.
 * And if user has NOT configured an overloaded API for this request then this function invokes the function
 * NvM_Prv_SetBlockLockStatus directly to get the data invalidated.
 *
 * \param[in] BlockId
 * ID of the block for which lock status will be set
 * \param[in] BlockLocked
 * Required state for lock status
 */
void NvM_SetBlockLockStatus(NvM_BlockIdType BlockId, boolean BlockLocked)
{
    // TRACE[NVM548] Service for setting the lock status of a permanent RAM block of an NVRAM block

#ifdef NVM_CFG_API_SET_BLOCK_LOCK_STATUS
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    NVM_CFG_API_SET_BLOCK_LOCK_STATUS(BlockId, BlockLocked);
#else
    NvM_Prv_SetBlockLockStatus(BlockId, BlockLocked);
#endif
}

#ifdef NVM_CFG_API_SET_BLOCK_LOCK_STATUS
/**
 * \brief
 * This public function provides the possibility to set the lock status within the overloaded API
 * for NvM_Rb_SetWriteAllTrigger.
 *
 * \details
 * To set the lock status this function invokes the private function NvM_Prv_SetBlockLockStatus.
 *
 * \attention
 * This function is only available if user has configured an overloaded API for current request.
 * \attention
 * The user is responsible to invoke this function wihin the overloaded API to make sure that lock status is set
 * as required.
 *
 * \param[in] BlockId
 * ID of the block for which lock status will be set
 * \param[in] BlockLocked
 * Required state for lock status
 */
void NvM_Rb_SetBlockLockStatusBase(NvM_BlockIdType BlockId, boolean BlockLocked)
{
    NvM_Prv_SetBlockLockStatus(BlockId, BlockLocked);
}
#endif

/**
 * \brief
 * This local private function implements the actual initiation of a single-block request to set the lock status.
 *
 * \details
 * This function prepares block data and service configuration and initiates the requested service by calling
 * the NvM internal function NvM_Prv_Service_Initiate() with prepared data.
 *
 * \param[in] idBlock_uo
 * ID of the block for which lock status will be set
 * \param[in] BlockLocked
 * Required state for lock status
 */
LOCAL_INLINE void NvM_Prv_SetBlockLockStatus(NvM_BlockIdType idBlock_uo, boolean BlockLocked)
{
    NvM_Prv_Service_Configuration_tst ServiceConfiguration_st;
    NvM_Prv_BlockData_tst BlockData_st;

    // put all service parameters in a structure for a queue entry
    BlockData_st.QueueEntry_st.idBlock_uo                   = idBlock_uo;
    BlockData_st.QueueEntry_st.BlockData_un.ptrRamBlock_pv  = NULL_PTR;
    BlockData_st.QueueEntry_st.idService_uo                 = NVM_SERVICE_ID_SET_BLOCK_LOCK_STATUS;
    BlockData_st.QueueEntry_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Unspecified_e;
    BlockData_st.Result_uo                                  = NVM_REQ_OK;
    BlockData_st.idxDataset_u8                              = 0u;
    BlockData_st.maskBitsToChange_uo                        = 0u;
    BlockData_st.maskBitsNewValue_uo                        = 0u;

    // Avoid "unused parameter" warning
    (void)BlockLocked;

    ServiceConfiguration_st.CheckPendingBlock_b     = TRUE;
    ServiceConfiguration_st.idBlockPendingCheck_uo  = idBlock_uo;
    // No data update required for set block lock status request
    ServiceConfiguration_st.UpdateData_pfct         = NULL_PTR;
    ServiceConfiguration_st.CheckParameter_pfct     = &NvM_Prv_SetBlockLockStatus_CheckParameter;
    // No check for block data required for set block lock status request
    ServiceConfiguration_st.CheckBlockData_pfct     = NULL_PTR;
    // no block data shall be set
    ServiceConfiguration_st.SetBlockData_pfct       = NULL_PTR;

    (void)NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_nrQueues_e,
                                         &BlockData_st,
                                         &ServiceConfiguration_st);
}

/**
 * \brief
 * This local private function defines validity checks for all significant parameters provided by the user to ensure
 * that the NvM can initiate the request to set the lock status.
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
static boolean NvM_Prv_SetBlockLockStatus_CheckParameter(NvM_Prv_BlockData_tst const* BlockData_pcst)
{
    // TRACE[NVM730] Report the DET error NVM_E_BLOCK_CONFIG when the NVRAM block is configured
    //               with NvMWriteBlockOnce = TRUE
    return NvM_Prv_ErrorDetection_IsBlockWriteProtectionChangeable(BlockData_pcst->QueueEntry_st.idService_uo,
                                                                   BlockData_pcst->QueueEntry_st.idBlock_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

