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

#include "NvM_Prv.h"
#include "NvM_Prv_Crc.h"
#include "NvM_Prv_JobQueue.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_RequestQueue.h"
#include "NvM_Prv_ErrorDetection.h"
#include "NvM_Prv_ProcessMultiBlock.h"
#include "NvM_Prv_ExplicitSynchronization.h"

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
LOCAL_INLINE void NvM_Prv_InitializeDataInClearedRam(void);

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * Public NvM interface for initializing all NvM internal data.
 */
void NvM_Init(void)
{
    // TRACE[NVM447] Service for resetting all internal variables
    // TRACE[NVM399] Reset all internal variables to their initial values

    // Per default all data in the saved zone is lost
    boolean    isSavedZoneDataLost_ab[NVM_RB_RAM_INIT_CHECK_NR_SAVEDZONES];
    uint8      idxSavedZone_u8;

    for (idxSavedZone_u8 = 0u; idxSavedZone_u8 < (NVM_RB_RAM_INIT_CHECK_NR_SAVEDZONES); idxSavedZone_u8++)
    {
        isSavedZoneDataLost_ab[idxSavedZone_u8] = TRUE;
    }

    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2785] Initialize saved-zone only if it is lost after a reset
#if (NVM_RB_RAM_INIT_CHECK == NVM_RB_RAM_INIT_CHECK_QUICK)
    for (idxSavedZone_u8 = 0u; idxSavedZone_u8 < (NVM_RB_RAM_INIT_CHECK_NR_SAVEDZONES); idxSavedZone_u8++)
    {
        isSavedZoneDataLost_ab[idxSavedZone_u8] = (NvM_Prv_GetMarkerSavedZoneId(idxSavedZone_u8) != NVM_PRV_SAVED_ZONE_MAGIC_VALUE);
        NvM_Prv_SetMarkerSavedZoneId(idxSavedZone_u8, NVM_PRV_SAVED_ZONE_MAGIC_VALUE);
    }
#endif
    NvM_Prv_InitializeDataInClearedRam();
    NvM_Prv_Block_InitializeData(isSavedZoneDataLost_ab);
#if (NVM_RB_AUX_INTERFACE == STD_ON)
    NvM_Prv_Block_InitializeDataAux();
    NvM_Prv_InitializeAuxMigrationResult();
#endif

    NvM_Prv_JobQueue_Initialize();
    NvM_Prv_RequestQueue_Initialize(isSavedZoneDataLost_ab[NVM_RB_DEFAULT_SAVEDZONE_IDX]);
    NvM_Prv_ErrorDetection_InitializeDetError(isSavedZoneDataLost_ab[NVM_RB_DEFAULT_SAVEDZONE_IDX]);

#if (NVM_RB_REQ_MONITOR == NVM_RB_REQ_MON_WRITE)
    // Write request monitoring
    NvM_Prv_InitializeWriteMonCntrs(isSavedZoneDataLost_ab[NVM_RB_DEFAULT_SAVEDZONE_IDX]);
#endif

    NvM_Prv_Crc_Initialize();

    // TRACE[NVM157] Initialization of the underlying memory drivers is not done here
    // TRACE[NVM193] Also don't initialize any other modules here
    // TRACE[NVM091] We also don't do an automatic loading of permanent RAM blocks here
    // TRACE[NVM158] The loading of permanent RAM blocks has to be triggered by the user via NvM_ReadAll
    // TRACE[NVM400] The permanent RAM block contents are not modified here
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
 * Local inline function to initialize internal NvM data which is located in the cleared RAM.
 * \details
 * This function initializes production errors for all blocks and all RAM block properties(block length,
 * permanent RAM block addresses and RAM mirror properties for explicit synchronization).
 * Afterwards this function checks if all blocks selected for the explicit synchronization can be copied
 * into the RAM mirror, i.e. RAM mirror size >= block size.
 * If this check succeeds this function initializes NvM main states.
 * And if this check fails this function uninitializes NvM main states to make sure that NvM will not accept
 * any requests due to incorrect configuration.
 */
LOCAL_INLINE void NvM_Prv_InitializeDataInClearedRam(void)
{
    NvM_Prv_ErrorDetection_InitializeProductionErrors();

    //TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2999] Initialize the variable to show the potentially incompatible
    //     blocks after layout changes are not sanitized by ReadAll function
#if ((NVM_DYNAMIC_CONFIGURATION == STD_ON) && (NVM_RB_REMOVE_NON_RESISTANT_BLOCKS == STD_ON))
    NvM_Prv_InitializeStateReadAll();
#endif

    // TRACE [BSW_SWCS_AR_NVRAMManager_Ext-3031] Runtime calculation of NV block lengths
    // TRACE [BSW_SWCS_AR_NVRAMManager_Ext-3038] Runtime calculation of RAM block addresses
    // TRACE [BSW_SWCS_AR_NVRAMManager_Ext-3047] Runtime calculation of explicit synchronization buffer
    NvM_Prv_InitRamBlockProperties();

    NvM_Prv_Multi_Initialize();

    NvM_Prv_JobResource_Init();

    if (NvM_Prv_ExplicitSync_CheckBlockSizes())
    {
        // TRACE[NVM399] Indicate finished initialization by setting NvM state
        NvM_Prv_InitializeMainStates();
    }
    else
    {
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3100] Indicate bad initialization if at least one block configured
        //                                               for explicit synchronization is too big for the RAM mirror
        NvM_Prv_UninitializeMainStates();
    }
}
