/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Ea.h"

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#include "Ea_Prv.h"

#define EA_START_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"
MemIf_StatusType          Ea_GlobModuleState_st = MEMIF_UNINIT; /* Global module state */
rba_EaFs2_Prv_DriverState_ten    rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_UNINIT_E;

#define EA_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
MemIf_JobResultType         Ea_JobResult;
#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"
/**
 *********************************************************************
 * Ea_Init(): Init function of the EA
 *
 * This function initializes the EEPROM abstraction module. It must be called
 * once before the module is used.
 *
 * \param    None
 * \return   None
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
void Ea_Init(void)
{
    uint16 idxBlk_qu16;

#if(FALSE != EA_PRV_DEBUGGING)
    rba_EaFs2_Prv_DebugTimeStart(&rba_EaFs2_Debug_st.initTime_st);
#endif

    for(idxBlk_qu16 = 0u; idxBlk_qu16 < EA_NUM_BLOCKS; idxBlk_qu16++)
    {
        rba_EaFs2_Prv_ReadJobResultDetailed_aen[idxBlk_qu16] = MEMIF_RB_DETAILED_BLK_INFO_INIT_E;
    }
    /* Set Ea initial state */
    rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_INIT_E;
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
    rba_EaFs2_Prv_FatRepair_st.isFatRepair = (boolean)FALSE;
    rba_EaFs2_Prv_FatRepair_st.state_en = RBA_EAFS2_PRV_FATREPAIR_INIT_E;
#endif
    rba_EaFs2_Prv_Eeprom_st.sync_b = (boolean)TRUE;
    rba_EaFs2_Prv_FsInfo_st.isPreAllocReq_b = (boolean)TRUE;

    /* default mode for background operation */
    rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;

    rba_EaFs2_Prv_ParseReservedArea();

    rba_EaFs2_Prv_FatInit();

    if((boolean)FALSE != rba_EaFs2_Prv_FsInfo_st.isPreAllocReq_b)
    {
        /* First time allocation of survival blocks */
        rba_EaFs2_Prv_PreAllocation();
    }
#if (EA_WRITE_SPREAD != FALSE)
    rba_EaFs2_Prv_WriteSpreadRamTableInit();
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    /* [EA128]:
     * The function Ea_Init shall set the module state to MEMIF_IDLE and the job result to MEMIF_JOB_OK after
     * successful module initialization and before it returns to the caller. */

    if((boolean)FALSE == rba_EaFs2_Prv_FsInfo_st.isInitFailed_b)
    {
        rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_IDLE_E;
        Ea_JobResult = MEMIF_JOB_OK;
        Ea_GlobModuleState_st = MEMIF_IDLE;

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
        /* Setting the migration status is init is success */
        if(     ((boolean)FALSE != rba_EaFs2_Prv_TempMarker_st.wrfinish_b) ||
                (0 < rba_EaFs2_Prv_FsInfo_st.totalGaps_u16) ||
                (rba_EaFs2_Prv_FsInfo_st.allotedblocks_u16 < EA_NUM_BLOCKS) ||
                (rba_EaFs2_Prv_FsInfo_st.updateMigragationMarker_b != (boolean)FALSE)/* to write migration marker when only survival block property is changed*/
        )
        {
            rba_EaFs2_Prv_FsInfo_st.updateMigragationMarker_b = (boolean)FALSE;
            Ea_Rb_MigrationStaus_u8 = EA_RB_MIGRATION_RUNNING;
        }
        else
        {
            Ea_Rb_MigrationStaus_u8 = EA_RB_MIGRATION_NOTNEEDED;
        }
#endif
    }
    else
    {
        /* Report error to DET */
        RBA_EAFS2_DET_REPORTERROR(EA_SID_INIT, EA_E_INIT_FAILED);
    }

    rba_EaFs2_Prv_Eeprom_st.sync_b = (boolean)FALSE;
    rba_EaFs2_Prv_FsInfo_st.allocIter_u16 = 0;
    rba_EaFs2_Prv_Bgo_st.state_en = RBA_EAFS2_PRV_BGO_IDLE_E;


#if(FALSE != EA_PRV_DEBUGGING)
    rba_MemLib_UpdateTimerUs();
    rba_EaFs2_Prv_DebugTimeStop(&rba_EaFs2_Debug_st.initTime_st);
#endif
}

/**
 *********************************************************************
 * Ea_Rb_EndInit : Function call Inidactes that ReadAll is completed
 *
 * Functions which initiates the background operations in a power cycle
 *
 * \param    (void)
 * \return   (void)
 *********************************************************************
 */
void Ea_Rb_EndInit(void)
{
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
    /* Background operations are enabled in application mode */
    rba_EaFs2_Prv_FsInfo_st.mode_en =RBA_EAFS2_PRV_BGOTASK_IDLE_E;
#else
    /* Boot mode, no background operations */
    rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;
#endif
}


#if(FALSE != EA_PRV_DEBUGGING)
/**
 *********************************************************************
 * rba_EaFs2_Prv_Initialize_Init():
 *
 * This function used as initialzation for global variable
 *
 * \param    None
 *
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_EaFs2_Prv_Initialize_Init(void)
{
    Ea_JobResult = MEMIF_JOB_OK;
}
#endif
#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
