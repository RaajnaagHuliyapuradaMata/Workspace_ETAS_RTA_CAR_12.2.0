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

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#if (NVM_DYNAMIC_CONFIGURATION == STD_ON)
static void NvM_Prv_DynConfig(void);
static boolean NvM_Prv_DynConfig_IsBlockParticipating(NvM_BlockIdType idBlock_uo,
                                                      NvM_Prv_Service_InitiationErrors_tuo* Errors_puo);
static void NvM_Prv_DynConfig_UpdateBlockData(void);
static void NvM_Prv_DynConfig_FindBlocks(void);
static void NvM_Prv_DynConfig_Finalize(void);
#endif

#if (NVM_RB_INIT_AT_LAYOUT_CHANGE == STD_ON)
static void NvM_Prv_InitAtlayoutChange(void);
static boolean NvM_Prv_InitAtLayoutChange_IsBlockParticipating(NvM_BlockIdType idBlock_uo,
                                                               NvM_Prv_Service_InitiationErrors_tuo* Errors_puo);
static void NvM_Prv_InitAtLayoutChange_UpdateBlockData(void);
static void NvM_Prv_InitAtLayoutChange_FindBlocks(void);
static void NvM_Prv_InitAtLayoutChange_Finalize(void);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#if ((NVM_DYNAMIC_CONFIGURATION == STD_ON) || (NVM_RB_INIT_AT_LAYOUT_CHANGE == STD_ON))

/**
 * \brief
 * This function initiates the internal multi-block service for block initialization after a ReadAll service.

 * \details
 * Initialization after a ReadAll service is required for following features:
 *
 * - Initialization at layout change:
 *   - initialize all blocks configured for this feature in following cases:
 *     -> reading configuration ID has succeeded AND configuration ID has changed
 *     -> invalidated configuration ID has been read (someone has intentionally invalidated it)
 *     -> reading configuration ID has failed
 *     -> inconsistent configuration ID has been read
 *        (configuration ID has got corrupted in NV memory or was never written into the NV memory)
 *
 * - Dynamic configuration:
 *   - Dynamic configuration as defined in AR:
 *     - skip reading all blocks which are non-resistant to SW change in following cases:
 *       -> reading configuration ID has succeeded AND configuration ID has changed
 *       -> reading configuration ID has failed
 *   - Enhanced dynamic configuration (RTA-BSW AR extension)
 *     - remove all blocks which are non-resistant to SW change in following cases:
 *       -> reading configuration ID has succeeded AND configuration ID has changed
 *       -> reading configuration ID has failed
 *
 * Additionally if changed configuration ID is detected then this service writes it as last block
 * if one following features is enabled by configuration:
 * initialization at layout change OR enhanced dynamic configuration.
 * \note
 * For dynamic configuration as defined by AUTOSAR the NvM writes changed configuration ID at the end of next WriteAll.
 *
 * NvM invokes this function only after reading configuration ID during ReadAll service.
 *
 * \param JobResult_en
 * Read configuration ID job result
 */
void NvM_Prv_InitAfterReadAll(NvM_Prv_JobResult_ten JobResult_en)
{
# if (NVM_DYNAMIC_CONFIGURATION == STD_ON)
    if (   ((NvM_Prv_JobResult_Succeeded_e == JobResult_en) && NvM_Prv_Block_HasIdConfigChanged())
        || ((NvM_Prv_JobResult_Succeeded_MemIfSkipped_e == JobResult_en) && NvM_Prv_Block_HasIdConfigChanged())
        || (NvM_Prv_JobResult_Failed_e == JobResult_en)
       )
    {
        NvM_Prv_DynConfig();
    }
#  if (NVM_RB_REMOVE_NON_RESISTANT_BLOCKS == STD_ON)
    if (NvM_Prv_Block_HasIdConfigChanged())
    {
        // If no removing is required but configuration ID must be written
        // then service RemoveNonResistant must be set also for multi-block
        // If this bit was set just before then it will be set here again
        NvM_Prv_Block_SetRequest(NVM_PRV_MULTI_BLOCK, NvM_Prv_ServiceBit_RemoveNonResistant_e);
        NvM_Prv_Block_SetRequest(NVM_RB_CONFIG_ID_BLOCK, NvM_Prv_ServiceBit_RemoveNonResistant_e);
        NvM_Prv_Block_SetRequestResult(NVM_RB_CONFIG_ID_BLOCK, NVM_REQ_PENDING);
        NvM_Prv_Block_SetErrorInfoDetail(NVM_RB_CONFIG_ID_BLOCK, NvM_Rb_ErrorInfoDetail_NotAvailable_e);
    }
#  endif
# endif

# if (NVM_RB_INIT_AT_LAYOUT_CHANGE == STD_ON)
    if (   ((NvM_Prv_JobResult_Succeeded_e == JobResult_en) && NvM_Prv_Block_HasIdConfigChanged())
        || ((NvM_Prv_JobResult_Succeeded_MemIfSkipped_e == JobResult_en) && NvM_Prv_Block_HasIdConfigChanged())
        || (NvM_Prv_JobResult_BlockInvalidated_e == JobResult_en)
        || (NvM_Prv_JobResult_BlockInconsistent_e == JobResult_en)
        || (NvM_Prv_JobResult_Failed_e == JobResult_en)
       )
    {
        NvM_Prv_InitAtlayoutChange();
    }
    if (NvM_Prv_Block_HasIdConfigChanged())
    {
        // If no initialization is required but configuration ID must be written
        // then service InitAtLayoutChange must be set also for multi-block
        // If this bit was set just before then it will be set here again
        NvM_Prv_Block_SetRequest(NVM_PRV_MULTI_BLOCK, NvM_Prv_ServiceBit_InitAtLayoutChange_e);
        NvM_Prv_Block_SetRequest(NVM_RB_CONFIG_ID_BLOCK, NvM_Prv_ServiceBit_InitAtLayoutChange_e);
        NvM_Prv_Block_SetRequestResult(NVM_RB_CONFIG_ID_BLOCK, NVM_REQ_PENDING);
        NvM_Prv_Block_SetErrorInfoDetail(NVM_RB_CONFIG_ID_BLOCK, NvM_Rb_ErrorInfoDetail_NotAvailable_e);
    }
# endif
}

#endif


#if (NVM_DYNAMIC_CONFIGURATION == STD_ON)

/**
 * \brief
 * This function initiates the internal multi-block service to handle all blocks which are non-resistant to SW change
 * if dynamic configuration is enabled by configuration.

 * \details
 * This function uses the service pattern to initiate the multi-block service (s. NvM_Prv_Service_InitiateMulti)
 */
static void NvM_Prv_DynConfig(void)
{
    NvM_Prv_QueueEntry_tst QueueEntry_st;
    NvM_Prv_Service_ConfigurationMulti_tst Configuration_st;

    // put all service parameters in the queue entry structure
    QueueEntry_st.idService_uo                  = NVM_SERVICE_ID_RB_INIT_AFTER_READ_ALL;
    QueueEntry_st.idQueueOriginal_uo            = NvM_Prv_idQueue_Multi_e;
    QueueEntry_st.idBlock_uo                    = NVM_RB_FIRST_USER_BLOCK;
    QueueEntry_st.ServiceBit_uo                 = NvM_Prv_ServiceBit_RemoveNonResistant_e;
    QueueEntry_st.BlockData_un.ptrRamBlock_pv   = NULL_PTR;

    // put all required functions in the multi-block service configuration structure
    Configuration_st.UpdateBlockDataForMulti_pfct = &NvM_Prv_DynConfig_UpdateBlockData;
    Configuration_st.FinalizeMulti_pfct = &NvM_Prv_DynConfig_Finalize;

    NvM_Prv_Service_InitiateMulti(NvM_Prv_idQueue_nrQueues_e, &QueueEntry_st, &Configuration_st);
}

/**
 * \brief
 * This function checks whether a block shall participate in dynamic configuration and
 * stores occured errors in the given variable for later evaluation.

 * \details
 * The NvM provides 2 kinds of dynamic configuration and blocks participate in multi-block service
 * if following conditions are fulfilled:
 * - Dynamic configuration as defined in AR:
 *   -> configuration ID has changed AND block is configured to be non-resistant to changed SW
 * - Enhanced dynamic configuration (RTA-BSW AR extension):
 *   -> Block is configured to be non-resistant to changed SW
 *
 * Due to the design concept of participation functions (see description for NvM_Prv_Block_IsNvmEnqueuingForMulti_pfct)
 * this function must have parameter Errors_puo. Since no error can occure in this function the parameter Errors_puo
 * is actually not used here.
 *
 * \param[in] idBlock_uo
 * Id of the block to check participation status
 * \param[out] Errors_puo
 * Pointer to the variable to store occured errors for later evaluation
 *
 * \return
 * - true = passed block participates in dynamic configuration
 * - false = passed block does not participate in dynamic configuration
 */
/* MR12 RULE 8.13 VIOLATION: All IsBlockParticipating functions shall have the same signature and
 * in IsBlockParticipating functions where an error can occur Errors_puo has to be changeable */
static boolean NvM_Prv_DynConfig_IsBlockParticipating(NvM_BlockIdType idBlock_uo,
                                                      NvM_Prv_Service_InitiationErrors_tuo* Errors_puo)
{
    boolean IsBlockPartOfRemoveNonResistant_b = FALSE;
    (void)Errors_puo;
    IsBlockPartOfRemoveNonResistant_b = !NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo,
                                                                         NVM_PRV_BLOCK_FLAG_RESISTANT_TO_CHANGED_SW);

    return IsBlockPartOfRemoveNonResistant_b;
}

/**
 * \brief
 * This function updates all block data for dynamic configuration as required by the service pattern
 * to initiate the multi-block service (s. NvM_Prv_Service_InitiateMulti).
 *
 * \details
 * The NvM provides 2 kinds of dynamic configuration and following block data shall be updated
 * with active scheduler lock:
 * - Dynamic configuration as defined in AR:
 *   -> function to check whether the NvM is currently enqueuing a multi-block service for a block
 * - Enhanced dynamic configuration (RTA-BSW AR extension):
 *   -> Set request to remove non-resistant blocks as pending for multi-block
 *   -> function to check whether the NvM is currently enqueuing a multi-block service for a block
 */
static void NvM_Prv_DynConfig_UpdateBlockData(void)
{
# if (NVM_RB_REMOVE_NON_RESISTANT_BLOCKS == STD_ON)
    NvM_Prv_Block_SetRequest(NVM_PRV_MULTI_BLOCK, NvM_Prv_ServiceBit_RemoveNonResistant_e);
# endif

    NvM_Prv_Block_SetIsNvmEnqueuingForMulti(&NvM_Prv_DynConfig_IsBlockParticipating);
}

/**
 * \brief
 * This function finds all blocks which shall participate in dynamic configuration and updates block data as required.
 *
 * \details
 * The NvM provides 2 kinds of dynamic configuration which require following block data updates:
 *
 * - Dynamic configuration as defined in AR:
 *   - If a block shall participate in dynamic configuration (s. NvM_Prv_DynConfig_IsBlockParticipating)
 *     then required block data updates depends on common configuration parameter NvMRbDynConfigLoadDefaults:
 *     - NvMRbDynConfigLoadDefaults = false:
 *       -> clear pending ReadAll service
 *       -> set result of the ReadAll service to NVM_REQ_BLOCK_SKIPPED
 *       -> set error info detail to NvM_Rb_ErrorInfoDetail_NotAvailable_e
 *     - NvMRbDynConfigLoadDefaults = true:
 *       -> set request for extended run-time preparation as defined by AR
 *
 * - Enhanced dynamic configuration (RTA-BSW AR extension):
 *   - If a block shall participate in dynamic configuration (s. NvM_Prv_DynConfig_IsBlockParticipating)
 *     then following block data updates are required:
 *     -> clear pending ReadAll service
 *     -> set request to remove non-resistant blocks as pending
 *     -> set result of the request remove non-resistant blocks to NVM_REQ_PENDING
 *     -> set error info detail to NvM_Rb_ErrorInfoDetail_NotAvailable_e
 *
 * \Attention
 * This function updates block data and it is safe to invoke it with disabled scheduler lock.
 */
static void NvM_Prv_DynConfig_FindBlocks(void)
{
    NvM_BlockIdType idBlock_uo;
    for (idBlock_uo = NVM_RB_FIRST_USER_BLOCK; idBlock_uo < NVM_CFG_NR_BLOCKS; ++idBlock_uo)
    {
        NvM_Prv_Service_InitiationErrors_tuo Errors_uo = 0u;

        if (NvM_Prv_DynConfig_IsBlockParticipating(idBlock_uo, &Errors_uo))
        {
# if (NVM_RB_REMOVE_NON_RESISTANT_BLOCKS == STD_OFF)
#  if (NVM_RB_DYN_CONFIG_LOAD_DEFAULTS == STD_OFF)
            NvM_Prv_Block_ClearRequests(idBlock_uo, (1u << NvM_Prv_ServiceBit_ReadAll_e));
            NvM_Prv_Block_SetRequestResult(idBlock_uo, NVM_REQ_BLOCK_SKIPPED);
            NvM_Prv_Block_SetErrorInfoDetail(idBlock_uo, NvM_Rb_ErrorInfoDetail_NotAvailable_e);
#  else
            if (NvM_Prv_Block_IsRequestPending(idBlock_uo, (1u << NvM_Prv_ServiceBit_ReadAll_e)))
            {
                NvM_Prv_Block_SetRequest(idBlock_uo, NvM_Prv_ServiceBit_ReadAllExtPreparation_e);
            }
#  endif
# else
            NvM_Prv_Block_ClearRequests(idBlock_uo, (1u << NvM_Prv_ServiceBit_ReadAll_e));
            NvM_Prv_Block_SetRequest(idBlock_uo, NvM_Prv_ServiceBit_RemoveNonResistant_e);
            NvM_Prv_Block_SetRequestResult(idBlock_uo, NVM_REQ_PENDING);
            NvM_Prv_Block_SetErrorInfoDetail(idBlock_uo, NvM_Rb_ErrorInfoDetail_NotAvailable_e);
# endif
        }
    }
}

/**
 * \brief
 * This function finalizes the initiation of the multi-block request for dynamic configuration
 * as required by the service pattern to initiate the multi-block service (s. NvM_Prv_Service_InitiateMulti).
 *
 * \details
 * The NvM provides 2 kinds of dynamic configuration which require the same finalization procedure:
 * - find all blocks which participate in dynamic configuration (s. NvM_Prv_DynConfig_FindBlocks)
 * - reset function to check whether the NvM is currently enqueuing a multi-block service for a block
 *   (s. NvM_Prv_Block_SetIsNvmEnqueuingForMulti)
 */
static void NvM_Prv_DynConfig_Finalize(void)
{
    NvM_Prv_DynConfig_FindBlocks();
    NvM_Prv_Block_SetIsNvmEnqueuingForMulti(NULL_PTR);
}

#endif

#if (NVM_RB_INIT_AT_LAYOUT_CHANGE == STD_ON)

/**
 * \brief
 * This function initiates the internal multi-block service to initialize all configured blocks at layout change.

 * \details
 * This function uses the service pattern to initiate the multi-block service (s. NvM_Prv_Service_InitiateMulti)
 */
static void NvM_Prv_InitAtlayoutChange(void)
{
    NvM_Prv_QueueEntry_tst QueueEntry_st;
    NvM_Prv_Service_ConfigurationMulti_tst Configuration_st;

    // put all service parameters in the queue entry structure
    QueueEntry_st.idService_uo                  = NVM_SERVICE_ID_RB_INIT_AFTER_READ_ALL;
    QueueEntry_st.idQueueOriginal_uo            = NvM_Prv_idQueue_Multi_e;
    QueueEntry_st.idBlock_uo                    = NVM_RB_FIRST_USER_BLOCK;
    QueueEntry_st.ServiceBit_uo                 = NvM_Prv_ServiceBit_InitAtLayoutChange_e;
    QueueEntry_st.BlockData_un.ptrRamBlock_pv   = NULL_PTR;

    // put all required functions in the multi-block service configuration structure
    Configuration_st.UpdateBlockDataForMulti_pfct = &NvM_Prv_InitAtLayoutChange_UpdateBlockData;
    Configuration_st.FinalizeMulti_pfct = &NvM_Prv_InitAtLayoutChange_Finalize;

    NvM_Prv_Service_InitiateMulti(NvM_Prv_idQueue_nrQueues_e, &QueueEntry_st, &Configuration_st);
}

/**
 * \brief
 * This function checks whether a block shall participate in initialization at layout change and
 * stores occured errors in the given variable for later evaluation.

 * \details
 * A block participates in initialization at layout change if following conditions are fulfilled:
 * - Block is configured to be initialized at layout change
 *
 * Due to the design concept of participation functions (see description for NvM_Prv_Block_IsNvmEnqueuingForMulti_pfct)
 * this function must have parameter Errors_puo. Since no error can occure in this function the parameter Errors_puo
 * is actually not used here.
 *
 * \param idBlock_uo
 * Id of the block to check participation status
 * \param[out] Errors_puo
 * Pointer to the variable to store occured errors for later evaluation
 *
 * \return
 * - true = passed block participates in initialization at layout change
 * - false = passed block does not participate in initialization at layout change
 */
/* MR12 RULE 8.13 VIOLATION: All IsBlockParticipating functions shall have the same signature and
 * in IsBlockParticipating functions where an error can occur Errors_puo has to be changeable */
static boolean NvM_Prv_InitAtLayoutChange_IsBlockParticipating(NvM_BlockIdType idBlock_uo,
                                                               NvM_Prv_Service_InitiationErrors_tuo* Errors_puo)
{
    (void)Errors_puo;
    return NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo, NVM_PRV_BLOCK_FLAG_SELECT_FOR_INIT_AT_LAYOUT_CHANGE);
}

/**
* \brief
* This function updates all block data for initialization at layout change as required by the service pattern
* to initiate the multi-block service (s. NvM_Prv_Service_InitiateMulti).
*
* \details
* Following block data shall be updated with active scheduler lock:
* - Set request to initialize blocks at layout change as pending for multi-block
* - function to check whether the NvM is currently enqueuing a multi-block service for a block
*/
static void NvM_Prv_InitAtLayoutChange_UpdateBlockData(void)
{
    NvM_Prv_Block_SetRequest(NVM_PRV_MULTI_BLOCK, NvM_Prv_ServiceBit_InitAtLayoutChange_e);
    NvM_Prv_Block_SetIsNvmEnqueuingForMulti(&NvM_Prv_InitAtLayoutChange_IsBlockParticipating);
}

/**
 * \brief
 * This function finds all blocks which shall participate in initialization at layout change and
 * updates block data as required.
 *
 * \details
 * - If a block shall participate in initialization at layout change (s. NvM_Prv_InitAtLayoutChange_IsBlockParticipating)
 *   then following block data updates are required:
 *   -> set request to initialize blocks at layout change as pending
 *   -> set result of the request to initialize blocks at layout change to NVM_REQ_PENDING
 *   -> set error info detail to NvM_Rb_ErrorInfoDetail_NotAvailable_e
 *
 * \Attention
 * This function updates block data and it is safe to invoke it with disabled scheduler lock.
 */
static void NvM_Prv_InitAtLayoutChange_FindBlocks(void)
{
    NvM_BlockIdType idBlock_uo;
    for (idBlock_uo = NVM_RB_FIRST_USER_BLOCK; idBlock_uo < NVM_CFG_NR_BLOCKS; ++idBlock_uo)
    {
        NvM_Prv_Service_InitiationErrors_tuo Errors_uo = 0u;

        if (NvM_Prv_InitAtLayoutChange_IsBlockParticipating(idBlock_uo, &Errors_uo))
        {
            NvM_Prv_Block_SetRequest(idBlock_uo, NvM_Prv_ServiceBit_InitAtLayoutChange_e);
            NvM_Prv_Block_SetRequestResult(idBlock_uo, NVM_REQ_PENDING);
            NvM_Prv_Block_SetErrorInfoDetail(idBlock_uo, NvM_Rb_ErrorInfoDetail_NotAvailable_e);
        }
    }
}

/**
 * \brief
 * This function finalizes the initiation of the multi-block request for initialization at layout change
 * as required by the service pattern to initiate the multi-block service (s. NvM_Prv_Service_InitiateMulti).
 *
 * \details
 * Initialization at layout change requires following finalization procedure:
 * - find all blocks which participate in initialization at layout change (s. NvM_Prv_InitAtLayoutChange_FindBlocks)
 * - reset function to check whether the NvM is currently enqueuing a multi-block service for a block
 *   (s. NvM_Prv_Block_SetIsNvmEnqueuingForMulti)
 */
static void NvM_Prv_InitAtLayoutChange_Finalize(void)
{
    NvM_Prv_InitAtLayoutChange_FindBlocks();
    NvM_Prv_Block_SetIsNvmEnqueuingForMulti(NULL_PTR);
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

