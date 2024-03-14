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
#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

#if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv.h"
#include "rba_FeeFs1x_Prv_Cfg.h"

#include "rba_FeeFs1x_Prv_SectorReorg.h"
#include "rba_FeeFs1x_Prv_SectorReorgTypes.h"

#include "Fee_Prv_Lib.h"

#include "rba_FeeFs1x_Prv_Sector.h"
#include "rba_FeeFs1x_Prv_PAMap.h"
#include "rba_FeeFs1x_Prv_PAWrite.h"

#include "rba_FeeFs1x_Prv_Searcher.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"
#include "rba_FeeFs1x_Prv_BC.h"

#include "rba_FeeFs1x_Prv_CacheKwn.h"
#include "rba_FeeFs1x_Prv_CacheUnkwn.h"



/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#define RBA_FEEFS1X_REORG_PRV_LOGSECTOR_TO_BE_REORGED   0u  // Currently, only the oldest sector is always reorgd
#define RBA_FEEFS1X_REORG_PRV_CLONE_SRC_START_PAGE_NR   (RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR * (RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 2u))
#define RBA_FEEFS1X_REORG_PRV_CLONE_DEST_START_PAGE_NR  (RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR * (RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u))

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_Reorg_stm_tst         rba_FeeFs1x_Reorg_stm_st)
RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_Reorg_Known_data_tst  rba_FeeFs1x_Reorg_Known_data_st)

static rba_FeeFs1x_Reorg_transferCopies_data_tst        rba_FeeFs1x_Reorg_transfer1Copy_data_st;
static rba_FeeFs1x_Reorg_transferCopies_data_tst        rba_FeeFs1x_Reorg_transfer2Copies_data_st;

RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst    rba_FeeFs1x_Reorg_repairSectOverflow_data_st)
static rba_FeeFs1x_Reorg_reorgUnknown_data_tst          rba_FeeFs1x_Reorg_reorgUnknown_data_st;
static rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndFillSect0End(rba_FeeFs1x_Reorg_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_known(rba_FeeFs1x_Reorg_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_unknown(rba_FeeFs1x_Reorg_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_sectorOverflow(rba_FeeFs1x_Reorg_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndInitSect0OverlapBlk(rba_FeeFs1x_Reorg_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_transferSect0OverlapBlk(rba_FeeFs1x_Reorg_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_end(rba_FeeFs1x_Reorg_stm_tst * fsm_pst);

static uint8 rba_FeeFs1x_Reorg_calculateNrOfCopiesToBeTransferred(boolean isKnown_b, boolean isRedundantBlock_b, uint16 feeIdx_u16, rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
LOCAL_INLINE boolean rba_FeeFs1x_Reorg_shouldRedCopyBeTransferred(boolean isRedundantBlock_b, rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);

LOCAL_INLINE void                     rba_FeeFs1x_Reorg_reorgKnown(boolean isSoftReorg_b);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgKnownDo(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgKnownDo_selectBlk(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgKnownDo_searchBlk(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgKnownDo_checkReq(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgKnownDo_copy1(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgKnownDo_copy2(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst);
LOCAL_INLINE boolean                  rba_FeeFs1x_Reorg_reorgKnown_isSoftOngoing(void);

LOCAL_INLINE void                     rba_FeeFs1x_Reorg_reorgUnknown(boolean isSoftReorg_b);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo_checkCacheRebuildReq  (rba_FeeFs1x_Reorg_reorgUnknown_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo_cacheRebuild          (rba_FeeFs1x_Reorg_reorgUnknown_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo_reorgCachedBlocks     (rba_FeeFs1x_Reorg_reorgUnknown_data_tst * fsm_pst);
LOCAL_INLINE boolean                  rba_FeeFs1x_Reorg_reorgUnknown_isSoftOngoing(void);

LOCAL_INLINE void                     rba_FeeFs1x_Reorg_reorgUnknownCached(boolean isSoftReorg_b);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownCachedDo(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownCachedDo_selectBlk      (rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownCachedDo_searchBlk      (rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownCachedDo_checkReq       (rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy1          (rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy2          (rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst);
LOCAL_INLINE boolean                  rba_FeeFs1x_Reorg_reorgUnknownCached_isSoftOngoing(void);

static void rba_FeeFs1x_Reorg_transferCopyDo_updCache(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en, uint16 feeIndex_u16 , boolean isKnownBlock_b);

LOCAL_INLINE boolean                rba_FeeFs1x_Reorg_transfer1Copy_wasThereRetry(void);
static void                         rba_FeeFs1x_Reorg_transfer1Copy(boolean isKnownBlock_b, uint16 feeIdx_u16, boolean isCloneWrite_b);
static rba_FeeFs1x_RetVal_ten       rba_FeeFs1x_Reorg_transfer1CopyDo(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer1CopyDo_init1 (rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer1CopyDo_copy1 (rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer1CopyDo_error1(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst);

static void                         rba_FeeFs1x_Reorg_transfer2Copies(boolean isKnownBlock_b, uint16 feeIdx_u16);
static rba_FeeFs1x_RetVal_ten       rba_FeeFs1x_Reorg_transfer2CopiesDo(void);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer2CopiesDo_init(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst,rba_FeeFs1x_Reorg_transferCopy_ten copy_en);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer2CopiesDo_copy(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst,rba_FeeFs1x_Reorg_transferCopy_ten copy_en);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer2CopiesDo_error(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst,rba_FeeFs1x_Reorg_transferCopy_ten copy_en);

LOCAL_INLINE Std_ReturnType         rba_FeeFs1x_Reorg_updatePages(uint32 amountOfPages_u32);

LOCAL_INLINE void                     rba_FeeFs1x_Reorg_repairSectOverflow(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_erase(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);

# if (FALSE != RBA_FEEFS1X_PRV_IS_MULTI_SECTOR_EMULATION)
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_wrClone(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE boolean                  rba_FeeFs1x_Reorg_repairSectOverflowDo_shouldOverlapDataBeTransferred(uint32 firstValidHdrPgNr_u32, uint32 startMarkerPgNr_u32, rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_searchFirstBlk(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_copyOverlapData(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_checkIfBlkOverlapsToLastSect(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_transferValidBlk(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_searchNextValidBlk(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_wrUsed(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_wrStart(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_wrR2E(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
# else
// for 2 sector emulation, cloning is not needed. Re-erase and retry the reorg. All below states are dummy and should never be entered -> so return rba_FeeFs1x_ErrorInternal_e
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    /* To avoid violation of MISRA rule 8.13 (QAC_3673: The object addressed by the pointer parameter 'fsm_pst' is not
     * modified and so the pointer could be of type 'pointer to const'), a member of fsm_pst needs be be modified. Use
     * same logic like in default branch of rba_FeeFs1x_Reorg_repairSectOverflowDo. */
    fsm_pst->state_en = rba_FeeFs1x_Reorg_repairSectOverflow_stm_idle_e;
    return rba_FeeFs1x_ErrorInternal_e;
}

#define rba_FeeFs1x_Reorg_repairSectOverflowDo_wrClone                         rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_searchFirstBlk                  rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_copyOverlapData                 rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_checkIfBlkOverlapsToLastSect    rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_transferValidBlk                rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_searchNextValidBlk              rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_wrUsed                          rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_wrStart                         rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy
#define rba_FeeFs1x_Reorg_repairSectOverflowDo_wrR2E                           rba_FeeFs1x_Reorg_repairSectOverflowDo_dummy

# endif

LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_repairSectOverflowDo_cacheBuildup(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst);

static void                           rba_FeeFs1x_Reorg_switchToHardReorg(void);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_cancel
 * initializes the unit internal variables
 * \seealso
 * \usedresources
 *********************************************************************
 */

void rba_FeeFs1x_Reorg_cancel(void)
{
    rba_FeeFs1x_Reorg_stm_st.state_en = rba_FeeFs1x_Reorg_stm_idle_e;
    rba_FeeFs1x_Reorg_stm_st.isSoftReorgOngoing_b = FALSE;

    rba_FeeFs1x_Reorg_Known_data_st.state_en = rba_FeeFs1x_Reorg_Known_stm_idle_e;
    rba_FeeFs1x_Reorg_Known_data_st.isSoftReorgOngoing_b    = FALSE;

    rba_FeeFs1x_Reorg_transfer1Copy_data_st.state_en = rba_FeeFs1x_Reorg_transferCopies_stm_idle_e;

    rba_FeeFs1x_Reorg_transfer2Copies_data_st.state_en = rba_FeeFs1x_Reorg_transferCopies_stm_idle_e;

    rba_FeeFs1x_Reorg_repairSectOverflow_data_st.state_en = rba_FeeFs1x_Reorg_repairSectOverflow_stm_idle_e;

    rba_FeeFs1x_Reorg_reorgUnknown_data_st.state_en = rba_FeeFs1x_Reorg_reorgUnknown_stm_idle_e;
    rba_FeeFs1x_Reorg_reorgUnknown_data_st.isSoftReorgOngoing_b = FALSE;

    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.state_en = rba_FeeFs1x_Reorg_reorgUnknownCached_stm_idle_e;
    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.isSoftReorgOngoing_b   = FALSE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSector
 * job init function for trigggering a reorganisation of log. sector 0
 * \param   isSoftReorg_b : is it a request for a soft or a hard sector reorg
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Reorg_reorganiseSector(boolean isSoftReorg_b)
{
    if(rba_FeeFs1x_Reorg_stm_st.isSoftReorgOngoing_b)
    {
        // Soft Reorg Calls should not cause any action while the state should be changed
        // for a hard reorg call
        if(isSoftReorg_b)
        {
            // The job init function is called during an already ongoing soft reorg; do not reset the state machine
            // continue the previous state
        }
        else
        {
            // A external request during a soft reorg caused a hard reorg
            rba_FeeFs1x_Reorg_switchToHardReorg();

            // Attention: If a soft reorg is ongoing, the reorg state is not allowed to be idle.
            // Otherwise, the hard reorg is going to exit with an error internal
        }
    }
    else
    {
        // No reorg ongoing, trigger the reorg and set the soft reorg flag according to the parameter
        rba_FeeFs1x_Reorg_stm_st.state_en = rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e;
        rba_FeeFs1x_Reorg_stm_st.entry_b  = TRUE;

        rba_FeeFs1x_Reorg_stm_st.isSoftReorgOngoing_b = isSoftReorg_b;
        rba_FeeFs1x_Reorg_stm_st.sectorOverflowAttemptCtr_u8 = 0u;
        rba_FeeFs1x_Reorg_stm_st.persIdOfSector0OverlapBlk_u16 = RBA_FEEFS1X_MAXUINT16;
    }
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo
 * jobDo function for asynchronously processing the sector reorganisation
 * Some errors will be never reported from known, unknown and sector overflow states
 * This will always cause a restart of the reorg from the beginning
 * If reorg can never be finished this may cause an endless loop of reorgs
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_JobFailed_e : reorg finished and can be restarted
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch (rba_FeeFs1x_Reorg_stm_st.state_en)
    {
        case rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndFillSect0End(&rba_FeeFs1x_Reorg_stm_st);
        }break;
        case rba_FeeFs1x_Reorg_stm_known_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo_known(&rba_FeeFs1x_Reorg_stm_st);
        } break;
        case rba_FeeFs1x_Reorg_stm_unknown_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo_unknown(&rba_FeeFs1x_Reorg_stm_st);
        } break;
        case rba_FeeFs1x_Reorg_stm_sectorOverFlowHandler_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo_sectorOverflow(&rba_FeeFs1x_Reorg_stm_st);
        }break;
        case rba_FeeFs1x_Reorg_stm_checkAndInitSector0OverlapBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndInitSect0OverlapBlk(&rba_FeeFs1x_Reorg_stm_st);
        }break;
        case rba_FeeFs1x_Reorg_stm_transferSector0OverlapBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo_transferSect0OverlapBlk(&rba_FeeFs1x_Reorg_stm_st);
        }break;
        case rba_FeeFs1x_Reorg_stm_end_e:
        {
            // trigger the sector marking R2E, reordering and reduceOwnPages trigger
            retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo_end(&rba_FeeFs1x_Reorg_stm_st);
        } break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        } break;
    }

    if( (retVal_en != rba_FeeFs1x_Pending_e) && !(rba_FeeFs1x_Reorg_stm_st.isSoftReorgOngoing_b))
    {
        // reset on job exit if no soft reorg is ongoing
        rba_FeeFs1x_Reorg_stm_st.state_en = rba_FeeFs1x_Reorg_stm_idle_e;
        // isSoftReorgOngoing_b explicitly not reset to prevent soft reorgs getting reset
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndFillSect0End
 * state function for filling up sector 0 in case the actual write page is in sector 0
 *
 * \param   fsm_pst : pointer to the statemachine data
 * \return  is the job accepted?
 * \retval  rba_FeeFs1x_JobOK_e: job finished successful
 * \retval  rba_FeeFs1x_Pending_e: job ongoing, call the function again
 * \retval  rba_FeeFs1x_ErrorInternal_e: sector reorg job failed, internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: sector reorg job failed, external error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndFillSect0End(rba_FeeFs1x_Reorg_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValFill_en;

    /* Check if logical sector 0 has to be filled up before starting with transfer of blocks.
     * Enter the state in case the sector 0 filling is ongoing (indicated when fsm_pst->entry_b = FALSE),
     * since write page gets incremented even before the write is completed (to support cancel feature). */
    if((rba_FeeFs1x_PAMap_getCurrWrPage() < RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR) ||
       (!fsm_pst->entry_b))
    {
        // current write page is in sector 0 => fill up sector 0 to assure that the write page is above sector 0
        if(FEE_PRV_LIBENTRY)
        {
            rba_FeeFs1x_PAMap_fillSectorEnd();
        }

        // Call the do-function of the job
        retValFill_en =  rba_FeeFs1x_PAMap_fillSectorEndDo();

        // evaluate the job result
        switch (retValFill_en)
        {
            case rba_FeeFs1x_JobOK_e:
            {
                // as soon as the state's job is finished, transit to next state
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_known_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }break;
            case rba_FeeFs1x_Pending_e:
            case rba_FeeFs1x_ErrorInternal_e:
            case rba_FeeFs1x_ErrorExternal_e:
            {
                retVal_en = retValFill_en;
            }break;
            default:
            {
                // on error or unexpected return value, return error to the caller and stop the state machine
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }break;
        }
    }
    else
    {
        // current write page is above sector 0 => reorganize sector without filling up sector 0
        FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_known_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo_known
 * statefunction of reorganiseSectorDo executing the known block reorg
 * Errors will be never reported and will always cause a restart of the reorg
 * If reorg can never be finished this may cause an endless loop of reorgs
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_known(rba_FeeFs1x_Reorg_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValKnown_en;
    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Reorg_reorgKnown(fsm_pst->isSoftReorgOngoing_b);
    }
    retValKnown_en = rba_FeeFs1x_Reorg_reorgKnownDo();

    switch(retValKnown_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {

            if(rba_FeeFs1x_Reorg_reorgKnown_isSoftOngoing())
            {
                // soft reorg is pending but interruptible; repeat this state during the next background cycle
                // no state transition --> don't call the reorgKnown init function again
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // known reorg is finished successfully, switch to unknown reorg
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_unknown_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }

        }break;
        case rba_FeeFs1x_JobFailed_e:
        {
            // sector overflow occured, switch to overflow handler state
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_sectorOverFlowHandler_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            // error during reorg --> restart the whole reorg
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;

        default:
        {
            //unexpected return value
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo_unknown
 * statefunction of reorganiseSectorDo executing the unknown block reorg
 * Errors will be never reported and will always cause a restart of the reorg
 * If reorg can never be finished this may cause an endless loop of reorgs
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_unknown(rba_FeeFs1x_Reorg_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(rba_FeeFs1x_Prv_isReorgUnknown() || rba_FeeFs1x_Prv_isSurvivalActive())
    {
        rba_FeeFs1x_RetVal_ten retValInner_en;
        if(FEE_PRV_LIBENTRY)
        {
            rba_FeeFs1x_Reorg_reorgUnknown(fsm_pst->isSoftReorgOngoing_b);
        }
        retValInner_en = rba_FeeFs1x_Reorg_reorgUnknownDo();

        switch(retValInner_en)
        {
            case rba_FeeFs1x_JobOK_e:
            {
                if(rba_FeeFs1x_Reorg_reorgUnknown_isSoftOngoing())
                {
                    // soft reorg is pending but unknown reorg is interruptible
                    // repeat this state during the next background cycle
                    // no state transition --> don't call the reorgUnknown init function again
                    retVal_en = rba_FeeFs1x_JobOK_e;
                }
                else
                {
                    // unknown reorg is finished successfully, switch to check+ transfer of the skipped overlap block
                    FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndInitSector0OverlapBlk_e);
                    retVal_en = rba_FeeFs1x_Pending_e;
                }
            }break;
            case rba_FeeFs1x_JobFailed_e:
            {
                // sector overflow occured, switch to overflow handler state
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_sectorOverFlowHandler_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }break;
            case rba_FeeFs1x_Pending_e:
            {
                retVal_en = rba_FeeFs1x_Pending_e;
            }break;
            case rba_FeeFs1x_ErrorExternal_e:
            case rba_FeeFs1x_ErrorInternal_e:
            {
                // error during reorg --> restart the whole reorg
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }break;

            default:
            {
                //unexpected return value
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }break;
        }
    }
    else
    {
        FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndInitSector0OverlapBlk_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo_sectorOverflow
 * statefunction of reorganiseSectorDo handling sector overflows.
 * External errors will be never reported and will always cause a restart of the sector overflow handling internally.
 *
 * \param   fsm_pst : reference to stm data
 * \return  job result of the sector overflow handling
 * \retval  rba_FeeFs1x_Pending_e       : job needs more cyclic calls
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_sectorOverflow(rba_FeeFs1x_Reorg_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValSectOverflow_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Reorg_repairSectOverflow();
    }

    retValSectOverflow_en = rba_FeeFs1x_Reorg_repairSectOverflowDo();

    switch(retValSectOverflow_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // This is first attempt to come out of memory full, restart the reorg
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e);
            fsm_pst->sectorOverflowAttemptCtr_u8++;
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;

        // external error should never be returned --> do not continue further
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            //unexpected return value
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndInitSect0OverlapBlk
 * statefunction of reorgDo: check if the overlap block has to be transferred and if yes, initialize the corresponding
 * block copy to transfer the block.
 *
 * \param   fsm_pst : reference to stm data
 *
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e       : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e         : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_checkAndInitSect0OverlapBlk(rba_FeeFs1x_Reorg_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten          retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en;

    if((rba_FeeFs1x_Prv_isMultiSectorEmulation()) &&
       (0u != fsm_pst->sectorOverflowAttemptCtr_u8) &&
       (RBA_FEEFS1X_MAXUINT16 != fsm_pst->persIdOfSector0OverlapBlk_u16))
    {
        if(FEE_PRV_LIBENTRY)
        {
            // Initialize the block copy by triggering the search. It is enough to do only header validation
            rba_FeeFs1x_Searcher_SR_findNextValidBlockHdr(&fsm_pst->pageNrOfSector0OverlapBlk_u32);
        }

        retValSearch_en = rba_FeeFs1x_Searcher_SR_findNextValidBlockHdrDo();

        switch (retValSearch_en)
        {
            case rba_FeeFs1x_Searcher_RetVal_Pending_e:
            {
                // do nothing
            }break;

            /* Block was found and intialized. */
            case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
            {
                // For safety recheck that the expected block was found
                if(rba_FeeFs1x_BC_getPersID(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e) == fsm_pst->persIdOfSector0OverlapBlk_u16)
                {
                    FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_transferSector0OverlapBlk_e);
                }
                else
                {
                    // error during search --> restart the whole reorg
                    FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e);
                }
            }break;

            // error during search --> restart the whole reorg
            case rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e:
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e);
            }break;

            case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
            case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
            case rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e:
            default:
            {
                //unexpected return value
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }break;
        }

    }
    else
    {
        // Either 2 sector emulation or there is no need to transfer overlap block. go to next step.
        FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_end_e);
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo_transferSect0OverlapBlk
 * statefunction of reorgDo: transfer the overlap block in sector 0. This state will be reached only after sector overflow.
 * In case memory is full, block transfer is skipped.
 *
 * \param   fsm_pst : reference to stm data
 *
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e       : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e         : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_transferSect0OverlapBlk(rba_FeeFs1x_Reorg_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten retValCopy_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* Generic transfer function reused. It is important that the cache should be updated during this write (for both known and unknown).
         * To achieve this the parameter isCloneWrite_b is set to FALSE.
         * Also, cache update for unknown block will also work, since the block which will be written now will always
         * be present in the unknown cache -> as this is the last block of the sector 0 and in the last run of the
         * unknown cache handling, this block will be cached. */
        rba_FeeFs1x_Reorg_transfer1Copy(fsm_pst->sector0OverlapBlkIsKnownBlk_b, fsm_pst->feeIndexOfSect0OverlapBlk_u16, FALSE);
    }

    retValCopy_en = rba_FeeFs1x_Reorg_transfer1CopyDo();

    switch(retValCopy_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            // do nothing
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // copying finished successfully -> reset the variables and go to next step
            rba_FeeFs1x_Reorg_stm_st.persIdOfSector0OverlapBlk_u16 = RBA_FEEFS1X_MAXUINT16;
            rba_FeeFs1x_Reorg_stm_st.feeIndexOfSect0OverlapBlk_u16 = RBA_FEEFS1X_MAXUINT16;
            rba_FeeFs1x_Reorg_stm_st.pageNrOfSector0OverlapBlk_u32 = RBA_FEEFS1X_MAXUINT32;

            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_end_e);
        }break;

        case rba_FeeFs1x_JobFailed_e:
        {
            rba_FeeFs1x_Reorg_stm_st.persIdOfSector0OverlapBlk_u16 = RBA_FEEFS1X_MAXUINT16;
            rba_FeeFs1x_Reorg_stm_st.feeIndexOfSect0OverlapBlk_u16 = RBA_FEEFS1X_MAXUINT16;
            rba_FeeFs1x_Reorg_stm_st.pageNrOfSector0OverlapBlk_u32 = RBA_FEEFS1X_MAXUINT32;

            if(!rba_FeeFs1x_Reorg_transfer1Copy_wasThereRetry())
            {
                // sector overflow detected. Block could not be transfered. Continue with next step. Known weakness of losing native block.
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_end_e);
            }
            else
            {
                /* May be the memory got full in one retry itself. To have same level of robustness with other block
                 * transfers performed during reorg, retry the operation once again. */
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e);
            }
        }break;

        // error during write --> restart the whole reorg
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_stm_checkAndFillSector0End_e);
        }break;

        // error during reorg --> restart the whole reorg
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            //unexpected return value
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseSectorDo_end
 * statefunction of reorganiseSectorDo executing the final steps of the sector reorganisation
 *
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e       : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e         : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo_end(rba_FeeFs1x_Reorg_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValMark_en;
    Std_ReturnType         retValUpdatePages_en;

    if(FEE_PRV_LIBENTRY)
    {
        // trigger the marker write
        rba_FeeFs1x_Sector_markReady4Erase(RBA_FEEFS1X_REORG_PRV_LOGSECTOR_TO_BE_REORGED);
    }

    // Cyclic Do function Call
    retValMark_en = rba_FeeFs1x_Sector_markSectorDo();

    //evaluate the result
    switch(retValMark_en)
    {
        /* case JobFailed: writing of the R2E marker in flash failed, but state in RAM is set
           if in this driving cycle an erase is triggered, sector will be erased, due to the correct state in RAM
           if a reset occurs, complete reorg will be reexecuted, and sector will be finally detected again as R2E
           -> treat this case as if it was JobOK */
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_JobOK_e:
        {
            // reset the soft reorg flag
            fsm_pst->isSoftReorgOngoing_b = FALSE;

            // marker write finished successfully; update pages
            retValUpdatePages_en = rba_FeeFs1x_Reorg_updatePages(RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR);

            // on sector overflow, there could be one block not transferred and leading to block loss
            // this is a known limitation and so continue with the reordering of the sector and finishing the reorganisation successfully
            if ((E_OK == retValUpdatePages_en) || (0u < fsm_pst->sectorOverflowAttemptCtr_u8))
            {
                // reorder sector states
                rba_FeeFs1x_Sector_reorderLogSect0();
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // error during update of pages. return error, to restart complete reorganisation.
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }
        }break;

        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValMark_en;
        }break;

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_calculateNrOfCopiesToBeTransferred
 * Function calculates the number of copies to be transferred. It considers both native and redundant copies +
 * known/unknown blocks + if reorg is reattempt or not.
 *
 * \param   isKnown_b           : = TRUE, is known block
 * \param   isRedundantBlock_b  : = TRUE, is redundant block
 * \param   feeIdx_u16          : fee index of the block
 * \param   retValSearch_en     : result of the search operation
 *
 * \return  number of copies to be transferred
 * \seealso
 * \usedresources
 *********************************************************************
 */
static uint8 rba_FeeFs1x_Reorg_calculateNrOfCopiesToBeTransferred(boolean isKnown_b, boolean isRedundantBlock_b, uint16 feeIdx_u16, rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    uint8 ctrCopiesRequired_u8 = 0u;
    uint32 pageNrLatest_u32;

    pageNrLatest_u32 = rba_FeeFs1x_BC_getPageNo(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);

    if(pageNrLatest_u32 < RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR)
    {
        // native copy is oldest sector -> should be transferred
        ctrCopiesRequired_u8++;
    }

    if(0u == rba_FeeFs1x_Reorg_stm_st.sectorOverflowAttemptCtr_u8)
    {
        // This is first attempt for reorg -> check if redundant copy has to be also transferred or not
        if(rba_FeeFs1x_Reorg_shouldRedCopyBeTransferred(isRedundantBlock_b, retValSearch_en))
        {
            ctrCopiesRequired_u8++;
        }

        /* Check if the block is an overlap block. It is enough to check this for native copy only (latest copy of the block).
         * If redundant copy is overlapping, it will anyway be not transferred during reorg reattempt. */
        if(rba_FeeFs1x_BC_isBlkOverlappingToNextSector(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e))
        {
            // remember the block details, which might be needed during sector overflow reattempt
            rba_FeeFs1x_Reorg_stm_st.persIdOfSector0OverlapBlk_u16 = rba_FeeFs1x_BC_getPersID(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
            rba_FeeFs1x_Reorg_stm_st.feeIndexOfSect0OverlapBlk_u16 = feeIdx_u16;
            rba_FeeFs1x_Reorg_stm_st.sector0OverlapBlkIsKnownBlk_b = isKnown_b;
            rba_FeeFs1x_Reorg_stm_st.pageNrOfSector0OverlapBlk_u32 = pageNrLatest_u32;
        }
    }
    else
    {
        // On retry, redundant copy transfer is skipped + the overlap block is transferred last
        if(rba_FeeFs1x_Reorg_stm_st.persIdOfSector0OverlapBlk_u16 == rba_FeeFs1x_BC_getPersID(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e))
        {
            ctrCopiesRequired_u8 = 0;
        }
    }

    return(ctrCopiesRequired_u8);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_shouldRedCopyBeTransferred
 * subfunction of rba_FeeFs1x_Reorg_calculateNrOfCopiesToBeTransferred: checks if redundant copy has to be transferred
 * during reorganisation or not.
 *
 * \param   isRedundantBlock_b  : = TRUE, is redundant block
 * \param   retValSearch_en     : result of the search operation
 *
 * \return  should redundant copy be transferred?
 * \retval  TRUE                : redundant copy should be transferred
 *          FALSE               : no need to transfer redundant copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Reorg_shouldRedCopyBeTransferred(boolean isRedundantBlock_b, rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    uint32  pageNrRed_u32;
    boolean retVal_b;

    if(isRedundantBlock_b)
    {
        if(retValSearch_en == rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e)
        {
            // two copies were found, get the pageNr of the redundant copy
            pageNrRed_u32 = rba_FeeFs1x_BC_getPageNo(rba_FeeFs1x_BC_Cpy_SectorReorg_Redundant_e);

            // Check where the redundant copy is present
            if(pageNrRed_u32 < RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR)
            {
                // redundant copy is present in oldest sector -> should also be transferred
                retVal_b = TRUE;
            }
            else
            {
                // redundant copy is outside the oldest sector -> no need to be transferred
                retVal_b = FALSE;
            }
        }
        else
        {
            // redundant copy is missing -> heal it and transfer one copy
            retVal_b = TRUE;
        }
    }
    else
    {
        // is not a redundant block
        retVal_b = FALSE;
    }

    return (retVal_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseKnown
 * job init function for executing the known block reorg
 * \return  is the job accepted?
 * \retval  E_OK     : job accepted, do function allowed to be called
 * \retval  E_NOT_OK : job not accepted, do not call the do function
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Reorg_reorgKnown(boolean isSoftReorg_b)
{
    // no soft reorg is ongoing; reset the statemachine
    rba_FeeFs1x_Reorg_Known_data_st.nextBlockIndex_u16 = 0u;

    rba_FeeFs1x_Reorg_Known_data_st.state_en = rba_FeeFs1x_Reorg_Known_stm_selectBlk_e;
    rba_FeeFs1x_Reorg_Known_data_st.entry_b = TRUE;

    rba_FeeFs1x_Reorg_Known_data_st.isSoftReorgOngoing_b = isSoftReorg_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseKnownDo
 * job do function for executing the known block reorg
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgKnownDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch(rba_FeeFs1x_Reorg_Known_data_st.state_en)
    {
        case rba_FeeFs1x_Reorg_Known_stm_selectBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgKnownDo_selectBlk(&rba_FeeFs1x_Reorg_Known_data_st);
        }break;
        case rba_FeeFs1x_Reorg_Known_stm_searchBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgKnownDo_searchBlk(&rba_FeeFs1x_Reorg_Known_data_st);
        }break;
        case rba_FeeFs1x_Reorg_Known_stm_checkReq_e :
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgKnownDo_checkReq(&rba_FeeFs1x_Reorg_Known_data_st);
        }break;
        case rba_FeeFs1x_Reorg_Known_stm_copy1_e     :
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgKnownDo_copy1(&rba_FeeFs1x_Reorg_Known_data_st);
        }break;
        case rba_FeeFs1x_Reorg_Known_stm_copy2_e     :
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgKnownDo_copy2(&rba_FeeFs1x_Reorg_Known_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if((retVal_en != rba_FeeFs1x_Pending_e) && !(rba_FeeFs1x_Reorg_Known_data_st.isSoftReorgOngoing_b))
    {
        rba_FeeFs1x_Reorg_Known_data_st.state_en = rba_FeeFs1x_Reorg_Known_stm_idle_e;
    }

    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgKnownDo_selectBlk
 * statefunction of reorgKnownDo
 * selects the next known block for checking the reorg necessity
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgKnownDo_selectBlk(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    fsm_pst->currentBlockIndex_u16 = fsm_pst->nextBlockIndex_u16;
    fsm_pst->nextBlockIndex_u16++;

    if(fsm_pst->currentBlockIndex_u16 < RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS)
    {
        boolean cpyHasRedBitActive_b;

        // block is to be checked for reorg necessity

        // if the block is configured as redundant block, search for two copies instead of one
        if(E_OK == rba_FeeFs1x_CacheKwn_isCachedCopyRedundantBitActive(fsm_pst->currentBlockIndex_u16, &cpyHasRedBitActive_b))
        {
            // cache element redundancy extraction successful
            fsm_pst->isRedundantBlock_b = cpyHasRedBitActive_b;

            // switch to search that block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_searchBlk_e);

            if(fsm_pst->isSoftReorgOngoing_b)
            {
                // on soft reorg, pause for the possibility of accepting jobs
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // on hard reorg, directly continue with the search during the next cycle
                retVal_en = rba_FeeFs1x_Pending_e;
            }

        }
        else
        {
            // cache element is not allowed to be accessed.
            // Either an OOB against the RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS occured( catched at upper position)
            // or the current block doesn't have a valid cache entry.

            // no cache entry found --> reenter the state to select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
    }
    else
    {
        // end of known block reorg reached
        // reset the soft reorg status
        fsm_pst->isSoftReorgOngoing_b = FALSE;

        retVal_en = rba_FeeFs1x_JobOK_e;
    }
    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgKnownDo_searchBlk
 * statefunction of reorgKnownDo
 * searches block copies of the current block
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgKnownDo_searchBlk(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en;

    if(FEE_PRV_LIBENTRY)
    {
        if(fsm_pst->isRedundantBlock_b)
        {
            // for blocks the latest known copy is redundant, search for the 2 newest copies
            rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known(fsm_pst->currentBlockIndex_u16);
        }
        else
        {
            // for blocks the latest known copy isn't redundant, search only the latest consistent copy
            rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known(fsm_pst->currentBlockIndex_u16);
        }
    }

    // Cyclic Do function Call
    retValSearch_en = rba_FeeFs1x_Searcher_findCopiesDo();

    switch(retValSearch_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_Pending_e:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        {
            // no copy found --> no reorg necessity. Switch state to select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
        {
            // copie(s) were found --> check reorg necessity for this block
            fsm_pst->retValSearch_en = retValSearch_en;
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_checkReq_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e:
        {
            // unexpected search return or error
            // --> exit the known reorg, restart the whole reorg
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e:
        default:
        {
            // unexpected search return or error
            // --> exit the known reorg, restart the whole reorg
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgKnownDo_checkReq
 * statefunction of reorgKnownDo
 * checks based on the search result whether the block needs to be reorged (0/1/2 times)
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgKnownDo_checkReq(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst)
{
    uint8                   ctrCopiesRequired_u8;
    rba_FeeFs1x_RetVal_ten  retVal_en;

    ctrCopiesRequired_u8 = rba_FeeFs1x_Reorg_calculateNrOfCopiesToBeTransferred(TRUE, fsm_pst->isRedundantBlock_b, fsm_pst->currentBlockIndex_u16, fsm_pst->retValSearch_en);

    // select the required action based on the amount of copies to be transferred
    switch(ctrCopiesRequired_u8)
    {
        case 0u:
        {
            // no latest copy of this block is located within the sector 0
            // continue with the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case 1u:
        {
            // one transfer of the latest consistent copy required
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_copy1_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case 2u:
        {
            // two transfers of the latest consistent copy required
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_copy2_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        default:
        {
            // unexpected amount of writes
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgKnownDo_copy1
 * statefunction of reorgKnownDo
 * 1-time transfer for the current block
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgKnownDo_copy1(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCopy_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Reorg_transfer1Copy(TRUE, fsm_pst->currentBlockIndex_u16, FALSE);
    }

    retValCopy_en = rba_FeeFs1x_Reorg_transfer1CopyDo();

    switch(retValCopy_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // copying finished successfully, select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_JobFailed_e:
            // sector overflow detected, stop the known reorg
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValCopy_en;
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;

}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgKnownDo_copy2
 * statefunction of reorgKnownDo
 * 2-time transfer for the current block
 * \param   fsm_pst : reference to stm data
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgKnownDo_copy2(rba_FeeFs1x_Reorg_Known_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCopy_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Reorg_transfer2Copies(TRUE, fsm_pst->currentBlockIndex_u16);
    }

    retValCopy_en = rba_FeeFs1x_Reorg_transfer2CopiesDo();


    switch(retValCopy_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // copying finished successfully, select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_Known_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_JobFailed_e:
            // sector overflow detected, stop the known reorg
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValCopy_en;
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;

}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgKnown_isSoftOngoing
 * function returing whether the known reorg is ongoing during a soft reorg
 * \return  is the soft reorg ongoing?
 * \retval  TRUE  : call the function again during the next background cycle
 * \retval  FALSE : job exited completely
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean  rba_FeeFs1x_Reorg_reorgKnown_isSoftOngoing(void)
{
    return rba_FeeFs1x_Reorg_Known_data_st.isSoftReorgOngoing_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer1Copy_wasThereRetry
 * function returing whether there was retry during the last rba_FeeFs1x_Reorg_transfer1Copy operation
 * \return
 * \retval  TRUE  : there was retry
 * \retval  FALSE : there was no retry
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean  rba_FeeFs1x_Reorg_transfer1Copy_wasThereRetry(void)
{
    boolean retVal_b;

    if(!rba_FeeFs1x_Reorg_transfer1Copy_data_st.isCloneWrite_b)
    {
        retVal_b = (RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES != rba_FeeFs1x_Reorg_transfer1Copy_data_st.allowedRetries_u8);
    }
    else
    {
        // there is no retries when write is performed by clone operation
        retVal_b = FALSE;
    }

    return (retVal_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer1Copy
 * job init function for transferring one block copy during reorg
 * \param   isKnownBlock_b : is the block a known or unknown block
 * \param   feeIdx_u16     : fee index of the block
 * \param   isCloneWrite_b : writing is requested from clone function -> skip cache update + retry
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_Reorg_transfer1Copy(boolean isKnownBlock_b, uint16 feeIdx_u16, boolean isCloneWrite_b)
{
    rba_FeeFs1x_Reorg_transfer1Copy_data_st.state_en = rba_FeeFs1x_Reorg_transferCopies_stm_init1_e;
    rba_FeeFs1x_Reorg_transfer1Copy_data_st.entry_b = TRUE;

    rba_FeeFs1x_Reorg_transfer1Copy_data_st.feeIdx_u16 = feeIdx_u16;
    if(!isCloneWrite_b)
    {
        rba_FeeFs1x_Reorg_transfer1Copy_data_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;
    }
    else
    {
        // For writing during clone operation, there should not be retries. Instead error will be handled by the cloning algorithm
        rba_FeeFs1x_Reorg_transfer1Copy_data_st.allowedRetries_u8 = 0u;
    }
    rba_FeeFs1x_Reorg_transfer1Copy_data_st.isKnownBlock_b = isKnownBlock_b;
    rba_FeeFs1x_Reorg_transfer1Copy_data_st.isCloneWrite_b = isCloneWrite_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer1CopyDo
 *  job do function for transferring one block copy during reorg
 * \return  job result of the copy operation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : copy created
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
static rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer1CopyDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_Reorg_transfer1Copy_data_st.state_en)
    {
        case rba_FeeFs1x_Reorg_transferCopies_stm_init1_e :
        {
            retVal_en = rba_FeeFs1x_Reorg_transfer1CopyDo_init1(&rba_FeeFs1x_Reorg_transfer1Copy_data_st);
        }break;
        case rba_FeeFs1x_Reorg_transferCopies_stm_copy1_e :
        {
            retVal_en = rba_FeeFs1x_Reorg_transfer1CopyDo_copy1(&rba_FeeFs1x_Reorg_transfer1Copy_data_st);
        }break;
        case rba_FeeFs1x_Reorg_transferCopies_stm_error1_e :
        {
            retVal_en = rba_FeeFs1x_Reorg_transfer1CopyDo_error1(&rba_FeeFs1x_Reorg_transfer1Copy_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_Reorg_transfer1Copy_data_st.state_en = rba_FeeFs1x_Reorg_transferCopies_stm_idle_e;

        // finalize the copies before exit
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_SectorReorg_Redundant_e);
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_SectorReorg_NewLatest_e);
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer1CopyDo_init1
 *  statefunction of transfer1Copy initing the new block copy
 * \return  job result of the copy operation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : copy created
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer1CopyDo_init1(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValInit_en;
    if(FEE_PRV_LIBENTRY)
    {
        uint16 length_u16;
        uint16 persID_u16;
        uint8 DFLASHStatusByte_u8;

        length_u16 =          rba_FeeFs1x_BC_getBlkLength (rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
        persID_u16 =          rba_FeeFs1x_BC_getPersID   (rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
        DFLASHStatusByte_u8 = rba_FeeFs1x_BC_getBlkDFLASHStatusByte (rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);

        rba_FeeFs1x_BC_initCopySectReorgWr
                (
                        rba_FeeFs1x_BC_Cpy_SectorReorg_NewLatest_e,
                        length_u16,
                        persID_u16,
                        (uint16)DFLASHStatusByte_u8
                );
    }

    /* Cyclic Do function Call */
    retValInit_en = rba_FeeFs1x_BC_initCopySectReorgWrDo();

    switch(retValInit_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // init done, start the transfer
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_copy1_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_JobFailed_e:
            // sector overflow detected, stop the known reorg
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValInit_en;
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;

}
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer1CopyDo_copy1
 *  statefunction of transfer1Copy copying Fls2Fls
 * \return  job result of the copy operation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : copy created
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer1CopyDo_copy1(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValWr_en;

    if(FEE_PRV_LIBENTRY)
    {
        // always execute in fast mode -> in Fee_Medium only the blank check before writing will be skipped
        rba_FeeFs1x_BC_wrFls2Fls(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e,
                                 rba_FeeFs1x_BC_Cpy_SectorReorg_NewLatest_e,
                                 FALSE, 0x0,
                                 FEE_RB_WRITEJOB_WRITE_VERIFY_E);
    }

    /* Cyclic Call for Do Function */
    retValWr_en = rba_FeeFs1x_BC_wrData2FlsDo();

    switch(retValWr_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // write done, update chache only when the write is not triggered from clone operation
            if(!fsm_pst->isCloneWrite_b)
            {
                rba_FeeFs1x_Reorg_transferCopyDo_updCache
                (
                        rba_FeeFs1x_BC_Cpy_SectorReorg_NewLatest_e,
                        fsm_pst->feeIdx_u16,
                        fsm_pst->isKnownBlock_b
                );
            }
            retVal_en = rba_FeeFs1x_JobOK_e;
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_error1_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValWr_en;
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer1CopyDo_error1
 * statefunction of rba_FeeFs1x_Reorg_transfer1CopyDo
 * \param   fsm_pst : statemachine data pointer
 * \return  job result
 * \retval  rba_FeeFs1x_Pending_e: ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: transfer of the copy failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer1CopyDo_error1(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // check whether a retry is allowed
    if(fsm_pst->allowedRetries_u8 > 0u)
    {
        fsm_pst->allowedRetries_u8--;
        // retry counter doesn't forbid another retry --> restart the write
        retVal_en = rba_FeeFs1x_Pending_e;
        FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_init1_e);
    }
    else
    {
        // retry maximum reached, exit the job with an error
        retVal_en = rba_FeeFs1x_ErrorExternal_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transferCopyDo_updCache
 * Function for updating the cache after the transfer of the copy to new sector. Same function could be used for both
 * first copy and second copy transfer.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   copy_en     : Block copy object which was just transferred for cache update is required
 * \param   feeIndex_u16: Fee index of the block was just transferred
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_Reorg_transferCopyDo_updCache(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en,
        uint16 feeIndex_u16 ,
        boolean isKnownBlock_b)
{
    uint32 pageNr_u32;
    boolean isRed_b;

    pageNr_u32 = rba_FeeFs1x_BC_getPageNo(copy_en);
    isRed_b    = rba_FeeFs1x_BC_getCpyRedBit(copy_en);

    // Job finished successfully, update the cache to finalize the operation
    if(isKnownBlock_b)
    {
        // known cache update
        rba_FeeFs1x_CacheKwn_addNewerCopy(
                feeIndex_u16,
                TRUE,
                pageNr_u32,
                isRed_b);
    }
    else
    {
        // unknown cache update
        rba_FeeFs1x_CacheUnkwn_addNewerCopy(feeIndex_u16, TRUE, pageNr_u32, isRed_b);
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer2Copies
 *  job init function for copying a block copy twice during reorg
 * \param   isKnownBlock_b : is the block a known or unknown block
 * \param   feeIdx_u16     : fee index of the block
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_Reorg_transfer2Copies(boolean isKnownBlock_b, uint16 feeIdx_u16)
{
    rba_FeeFs1x_Reorg_transfer2Copies_data_st.state_en = rba_FeeFs1x_Reorg_transferCopies_stm_init1_e;
    rba_FeeFs1x_Reorg_transfer2Copies_data_st.entry_b = TRUE;

    rba_FeeFs1x_Reorg_transfer2Copies_data_st.feeIdx_u16 = feeIdx_u16;
    /* Since two new copies are written, increment the number of retry attempts. In total each copy is tried =
     * one attempt + RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES reattempts. So +1 is added below */
    rba_FeeFs1x_Reorg_transfer2Copies_data_st.allowedRetries_u8 = (RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES * 2u) + 1u;
    rba_FeeFs1x_Reorg_transfer2Copies_data_st.isKnownBlock_b = isKnownBlock_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer2CopiesDo
 *  job do function for copying a block copy twice during reorg
 * \return  job result of the copy operation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : copy created
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
static rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer2CopiesDo(void)
{
    rba_FeeFs1x_Reorg_transferCopies_stm_ten state_en = rba_FeeFs1x_Reorg_transfer2Copies_data_st.state_en;
    rba_FeeFs1x_Reorg_transferCopy_ten copy_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(state_en)
    {
        case rba_FeeFs1x_Reorg_transferCopies_stm_init1_e :
        case rba_FeeFs1x_Reorg_transferCopies_stm_copy1_e :
        case rba_FeeFs1x_Reorg_transferCopies_stm_error1_e :
        {
            copy_en = rba_FeeFs1x_Reorg_transferCopy_1_e;
        } break;
        case rba_FeeFs1x_Reorg_transferCopies_stm_init2_e :
        case rba_FeeFs1x_Reorg_transferCopies_stm_copy2_e :
        case rba_FeeFs1x_Reorg_transferCopies_stm_error2_e :
        {
            copy_en = rba_FeeFs1x_Reorg_transferCopy_2_e;
        } break;
        default:
        {
            state_en = rba_FeeFs1x_Reorg_transferCopies_stm_idle_e;
        } break;

    }

    switch(state_en)
    {
        case rba_FeeFs1x_Reorg_transferCopies_stm_init1_e :
        case rba_FeeFs1x_Reorg_transferCopies_stm_init2_e :
        {
            retVal_en = rba_FeeFs1x_Reorg_transfer2CopiesDo_init(&rba_FeeFs1x_Reorg_transfer2Copies_data_st,copy_en);
        }break;
        case rba_FeeFs1x_Reorg_transferCopies_stm_copy1_e :
        case rba_FeeFs1x_Reorg_transferCopies_stm_copy2_e :
        {
            retVal_en = rba_FeeFs1x_Reorg_transfer2CopiesDo_copy(&rba_FeeFs1x_Reorg_transfer2Copies_data_st,copy_en);
        }break;
        case rba_FeeFs1x_Reorg_transferCopies_stm_error1_e :
        case rba_FeeFs1x_Reorg_transferCopies_stm_error2_e :
        {
            retVal_en = rba_FeeFs1x_Reorg_transfer2CopiesDo_error(&rba_FeeFs1x_Reorg_transfer2Copies_data_st,copy_en);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;

    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_Reorg_transfer2Copies_data_st.state_en = rba_FeeFs1x_Reorg_transferCopies_stm_idle_e;

        // finalize the copies before exit
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_SectorReorg_Redundant_e);
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_SectorReorg_NewLatest_e);
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_SectorReorg_NewRedundant_e);
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer2CopiesDo_init
 *  statefunction of transfer2Copies initing the earlier of both new copies
 * \param   type_en : Block copy object type to be handeled
 * \return  job result of the copy operation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : copy created
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer2CopiesDo_init(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst, rba_FeeFs1x_Reorg_transferCopy_ten copy_en)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValInit_en;
    if(FEE_PRV_LIBENTRY)
    {
        uint16 length_u16;
        uint16 persID_u16;
        uint8 DFLASHStatusByte_u8;
        rba_FeeFs1x_BC_Cpy_blockCopyObject_ten BlockCopyObj;

        length_u16          = rba_FeeFs1x_BC_getBlkLength           (rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
        persID_u16          = rba_FeeFs1x_BC_getPersID              (rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
        DFLASHStatusByte_u8 = rba_FeeFs1x_BC_getBlkDFLASHStatusByte (rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);

        if(rba_FeeFs1x_Reorg_transferCopy_1_e == copy_en)
        {
            BlockCopyObj = rba_FeeFs1x_BC_Cpy_SectorReorg_NewRedundant_e;
        }
        else
        {
            BlockCopyObj = rba_FeeFs1x_BC_Cpy_SectorReorg_NewLatest_e;
        }

        rba_FeeFs1x_BC_initCopySectReorgWr
                (
                        BlockCopyObj,
                        length_u16,
                        persID_u16,
                        (uint16)DFLASHStatusByte_u8
                );
    }

    /* Cyclic call for Do function */
    retValInit_en = rba_FeeFs1x_BC_initCopySectReorgWrDo();

    switch(retValInit_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // init done, start the transfer
            if(rba_FeeFs1x_Reorg_transferCopy_1_e == copy_en)
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_copy1_e);
            }
            else
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_copy2_e);
            }
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = retValInit_en;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;

}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer2CopiesDo_copy
 *  statefunction of transfer2Copies writing the earlier of both new copies
 * \param   type_en : Block copy object type to be handeled
 * \return  job result of the copy operation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : copy created
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer2CopiesDo_copy(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst,rba_FeeFs1x_Reorg_transferCopy_ten copy_en)
{
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten BlockCopyObj;
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValWr_en;

    if(rba_FeeFs1x_Reorg_transferCopy_1_e == copy_en)
    {
        BlockCopyObj = rba_FeeFs1x_BC_Cpy_SectorReorg_NewRedundant_e;
    }
    else
    {
        BlockCopyObj = rba_FeeFs1x_BC_Cpy_SectorReorg_NewLatest_e;
    }

    if(FEE_PRV_LIBENTRY)
    {
        // always execute in fast mode -> in Fee_Medium only the blank check before writing will be skipped
        rba_FeeFs1x_BC_wrFls2Fls(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e,
                                            BlockCopyObj,
                                            FALSE, 0x0,
                                            FEE_RB_WRITEJOB_WRITE_VERIFY_E);
    }

    /* Cyclic call for Do function */
    retValWr_en = rba_FeeFs1x_BC_wrData2FlsDo();

    switch(retValWr_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // write done, update chache
            rba_FeeFs1x_Reorg_transferCopyDo_updCache
            (
                    BlockCopyObj,
                    fsm_pst->feeIdx_u16,
                    fsm_pst->isKnownBlock_b
            );
            if(rba_FeeFs1x_Reorg_transferCopy_1_e == copy_en)
            {
                // go to next copy transfer
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_init2_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // switch to the error handler
            if(rba_FeeFs1x_Reorg_transferCopy_1_e == copy_en)
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_error1_e);
            }
            else
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_error2_e);
            }
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_transfer2CopiesDo_error
 * statefunction of rba_FeeFs1x_Reorg_transfer2CopiesDo
 * \param   type_en : Block copy object type to be handeled
 * \param   fsm_pst : statemachine data pointer
 * \return  job result
 * \retval  rba_FeeFs1x_Pending_e: ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: transfer of the copy failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_transfer2CopiesDo_error(rba_FeeFs1x_Reorg_transferCopies_data_tst * fsm_pst, rba_FeeFs1x_Reorg_transferCopy_ten copy_en)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // check whether a retry is allowed
    if(fsm_pst->allowedRetries_u8 > 0u)
    {
        fsm_pst->allowedRetries_u8--;
        // retry counter doesn't forbid another retry --> restart the write
        retVal_en = rba_FeeFs1x_Pending_e;

        if(rba_FeeFs1x_Reorg_transferCopy_1_e == copy_en)
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_init1_e);
        }
        else
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_transferCopies_stm_init2_e);
        }
    }
    else
    {
        // retry maximum reached, exit the job with an error
        retVal_en = rba_FeeFs1x_ErrorExternal_e;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_checkHSRNecessity
 * \param   lastPageOfNewBlock_u32 : last page number the block would occupy
 * \return  returns whether an HSR execution is required due to HSR threshold crossing
 * \retval  TRUE  : HSR needs to be executed
 * \retval  FALSE : HSR is not required
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_Reorg_checkHSRNecessity(uint32 lastPageOfNewBlock_u32)
{
    boolean retVal_b;

    if(lastPageOfNewBlock_u32 < RBA_FEEFS1X_PRV_CFG_HSR_THRESHOLD_PAGE)
    {
        // The last page is not crossing the HSR
        retVal_b = FALSE;
    }
    else
    {
        // the last page is crossing the HSR
        retVal_b = TRUE;
    }

    return retVal_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_isSoftReorgRequired
 *
 * \return returns whether a soft reorg should be executed in background
 * \retval  TRUE : soft reorg should be executed
 * \retval  FALSE: soft reorg doesn't need to be executed
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_Reorg_isSoftReorgRequired(void)
{
    boolean retVal_b;
    uint32 currWrPage_u32;

    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();

    // if the current write page is succeeding the threshold, return TRUE
    if(currWrPage_u32 >= RBA_FEEFS1X_PRV_CFG_SSR_THRESHOLD_PAGE)
    {
        retVal_b = TRUE;
    }
    else
    {
        retVal_b = FALSE;
    }

    return retVal_b;
}

/**
 *********************************************************************
 * call subscriber functions
 * \param   amountOfPages_u32: reduction number of pages after sector reorganisation
 * \return  return of all subscriber functions
 * \retval  E_OK: all subscriber functions returned without an error
 * \retval  E_NOT_OK: at least one subscriber function returned with an error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Reorg_updatePages(uint32 amountOfPages_u32)
{
    Std_ReturnType PAMap_retVal_en;
    Std_ReturnType retValSearcher_en;
    Std_ReturnType retValCacheKwn_en;
    Std_ReturnType retValCacheUnkwn_en;
    Std_ReturnType retVal_en;

    PAMap_retVal_en = rba_FeeFs1x_PAMap_reduceOwnPages(amountOfPages_u32);
    rba_FeeFs1x_BC_reduceOwnPages(amountOfPages_u32);
    retValSearcher_en = rba_FeeFs1x_Searcher_reduceOwnPages(amountOfPages_u32);
    retValCacheKwn_en = rba_FeeFs1x_CacheKwn_reduceOwnPages(amountOfPages_u32);
    retValCacheUnkwn_en = rba_FeeFs1x_CacheUnkwn_reduceOwnPages(amountOfPages_u32);

    if((E_OK == PAMap_retVal_en) && (E_OK == retValSearcher_en) && (E_OK == retValCacheKwn_en) && (E_OK == retValCacheUnkwn_en))
    {
        retVal_en = E_OK;
    }
    else
    {
        retVal_en = E_NOT_OK;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflow
 *  job init function for handling robust sector overflows
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Reorg_repairSectOverflow(void)
{
    rba_FeeFs1x_Reorg_repairSectOverflow_data_st.state_en = rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e;
    rba_FeeFs1x_Reorg_repairSectOverflow_data_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo
 *  job do function for handling robust sector overflows. On any external error (read, write, erase) the control remains
 *  in the state machine and does an infinite reattempt starting from first step.
 *
 * \return  job result to be returned by the job do function
 * \retval  rba_FeeFs1x_Pending_e       : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e         : overflow cleaned up, reorg can be restarted
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch(rba_FeeFs1x_Reorg_repairSectOverflow_data_st.state_en)
    {
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_erase(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrClone_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_wrClone(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_searchFirstBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_searchFirstBlk(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_copyOverlapData_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_copyOverlapData(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_checkIfBlkOverlapsToLastSect_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_checkIfBlkOverlapsToLastSect(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_transferValidBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_transferValidBlk(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_searchValidBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_searchNextValidBlk(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrUsed_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_wrUsed(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrStart_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_wrStart(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrR2E_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_wrR2E(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        case rba_FeeFs1x_Reorg_repairSectOverflow_stm_cacheBuildup_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_repairSectOverflowDo_cacheBuildup(&rba_FeeFs1x_Reorg_repairSectOverflow_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_Reorg_repairSectOverflow_data_st.state_en = rba_FeeFs1x_Reorg_repairSectOverflow_stm_idle_e;
    }
    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_erase
 * statefunction of repairSectOverflow: erases the latest sector
 *
 * \retval  rba_FeeFs1x_Pending_e       : either erase is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_erase(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten retValErase_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Sector_erase(RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u, TRUE);
    }

    /* Cyclic call the Do function */
    retValErase_en = rba_FeeFs1x_Sector_eraseDo();

    switch (retValErase_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            //do nothing
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // finished. set write address to beginning of the sector and switch to next state
            rba_FeeFs1x_PAMap_setCurrWrPage(RBA_FEEFS1X_REORG_PRV_CLONE_DEST_START_PAGE_NR);

            if(rba_FeeFs1x_Prv_isMultiSectorEmulation())
            {
                // for multi sector emulation, cloning algorithm should be run to recover from mem full scenario
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrClone_e);
            }
            else
            {
                // for 2 sector emulation, cloning is not needed. Re-erase and retry the reorg.
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_cacheBuildup_e);
            }
        }break;

        /* Ideally control should never reach here. On erase error, control will remain in rba_FeeFs1x_Sector_eraseDo()
         * where the erase will be reattempted infinetly. */
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

# if (FALSE != RBA_FEEFS1X_PRV_IS_MULTI_SECTOR_EMULATION)
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_wrClone
 * statefunction of repairSectOverflow: writes a clone marker into the latest sector
 *
 * \retval  rba_FeeFs1x_Pending_e       : either writing of marker is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_wrClone(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten retValInner_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Sector_markClone(RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u);
    }

    retValInner_en = rba_FeeFs1x_Sector_markSectorDo();

    switch (retValInner_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            //do nothing
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // finished, switch to next state
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_searchFirstBlk_e);
        }break;

        // something went wrong. restart complete operation.
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e);
        }break;

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_shouldOverlapDataBeTransferred
 * Helper function of repairSectOverflow: The function checks if the overlap data at the beginning of the last but one
 * logical sector has to be transferred or not.
 *
 * \return Should the block be transferred?
 * \retval TRUE : Overlap data has to be transferrred
 * \retval FALSE: Overlap data to be ignored -> no transfer needed
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Reorg_repairSectOverflowDo_shouldOverlapDataBeTransferred(uint32 firstValidHdrPgNr_u32, uint32 startMarkerPgNr_u32, rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    boolean retVal_b;

    /* Overlap data needs to be taken over under following conditions:
     * 1.  There is valid start marker -> if start marker is lost, the overlap block is also lost -> known weakness and
     * 2.  There is overlap block -> when address in start marker does not matche with first logical page of the sector and
     * 3.1 There is complete data data pages -> when first valid block in the sector is found at higher address when compared to start marker content or
     * 3.2 No valid block was found, so its not possible to know if the overlap data is complete or not. In this case, just take over the data as it is. */
    if((RBA_FEEFS1X_MAXUINT32 != startMarkerPgNr_u32) &&
       (RBA_FEEFS1X_REORG_PRV_CLONE_SRC_START_PAGE_NR != startMarkerPgNr_u32) &&
       ((startMarkerPgNr_u32 <= firstValidHdrPgNr_u32) ||
        (rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e == retValSearch_en)))
    {
        /* This could be a valid data. Transfer the block */
        retVal_b = TRUE;
    }
    else
    {
        // Only partial or not data is available. Skip the transfer of the block.
        retVal_b = FALSE;
    }

    return (retVal_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_searchFirstBlk
 * statefunction of repairSectOverflow: search for first valid block in last but one logical sector
 *
 * \retval  rba_FeeFs1x_Pending_e       : either search is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_searchFirstBlk(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten          retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en;
    uint32                          oldStartMarkerPgNr_u32 = RBA_FEEFS1X_MAXUINT32;

    if(FEE_PRV_LIBENTRY)
    {
        // start the search from the top of the last but one logical sector
        fsm_pst->searchPageNr_u32 = RBA_FEEFS1X_REORG_PRV_CLONE_SRC_START_PAGE_NR;

        rba_FeeFs1x_Searcher_SR_findNextValidBlockHdr(&fsm_pst->searchPageNr_u32);
    }

    retValSearch_en = rba_FeeFs1x_Searcher_SR_findNextValidBlockHdrDo();

    switch (retValSearch_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_Pending_e:
        {
            // do nothing
        }break;

        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        {
            fsm_pst->retValSearch_en = retValSearch_en;

            if(E_OK == rba_FeeFs1x_Sector_getStartMarkerContentAsPageNo((RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 2u), &oldStartMarkerPgNr_u32))
            {
                // There is a valid start marker present. Adapt the value to the new sector
                // Calculate the offset relative to start of the corresponding logical sector
                fsm_pst->startMarkerOffsetPageNr_u32 = oldStartMarkerPgNr_u32 - RBA_FEEFS1X_REORG_PRV_CLONE_SRC_START_PAGE_NR;
            }

            // Check if overlap data has to be transferred or not
            if(rba_FeeFs1x_Reorg_repairSectOverflowDo_shouldOverlapDataBeTransferred(fsm_pst->searchPageNr_u32, oldStartMarkerPgNr_u32, retValSearch_en))
            {
                // Overlap data has to be transferred
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_copyOverlapData_e);
            }
            else
            {
                /* Either there is no overlap data or it could be ignored
                 * -> New block will start at the first logical address of the sector. */
                fsm_pst->startMarkerOffsetPageNr_u32 = 0u;

                if(rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e != retValSearch_en)
                {
                    // transfer the found valid block
                    FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_checkIfBlkOverlapsToLastSect_e);
                }
                else
                {
                    /* Software reaching here means that there is no valid overlap block + no other valid er valid blocks
                     * the sector. Cloning is completed. directly go the next step to write UM. */
                    FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrUsed_e);
                }
            }
        }break;

        // something went wrong. restart complete operation.
        case rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e);
        }break;

        // function rba_FeeFs1x_Searcher_SR_findNextValidBlockHdr() should search only one copy
        case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
        case rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_copyOverlapData
 * statefunction of repairSectOverflow: clones the overlap data at beginning of the last but one logical sector to the
 * last logical sector.
 *
 * \retval  rba_FeeFs1x_Pending_e       : either copying is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_copyOverlapData(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten retValInner_en;

    if(FEE_PRV_LIBENTRY)
    {
        // Standard FLS2FLS function could be used + update the write address during the writing
        rba_FeeFs1x_PAWrite_copyFls2Fls(RBA_FEEFS1X_REORG_PRV_CLONE_SRC_START_PAGE_NR,
                                        fsm_pst->startMarkerOffsetPageNr_u32,
                                        RBA_FEEFS1X_REORG_PRV_CLONE_DEST_START_PAGE_NR,
                                        TRUE);
    }

    retValInner_en = rba_FeeFs1x_PAWrite_copyFls2FlsDo();

    switch (retValInner_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            //do nothing
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // finished, switch to next state
            if(rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e == fsm_pst->retValSearch_en)
            {
                /* There is no valid block present in the complete sector -> cloning of valid blocks is completed.
                 * So go to next step to write UM */
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrUsed_e);
            }
            else
            {
                // Transfer the found valid block
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_checkIfBlkOverlapsToLastSect_e);
            }
        }break;

        // something went wrong. restart complete operation.
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e);
        }break;

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_checkIfBlkOverlapsToLastSect
 * statefunction of repairSectOverflow: checks if the block overlaps into the last sector
 *
 * \return  always pending (rba_FeeFs1x_Pending_e)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_checkIfBlkOverlapsToLastSect(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    if(!rba_FeeFs1x_BC_isBlkOverlappingToNextSector(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e))
    {
        //Is not overlapping. transfer the block to latest sector.
        FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_transferValidBlk_e);
    }
    else
    {
        /* The block overlaps into logical sector RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u, which is not present.
         * Skip this block transfer and go to next step. For safety it is better to continue to search for the blocks
         * till the end of the sector.
         * (Either this is a corrupt block with valid crc or header is valid but not all data pages are present). */
        FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_searchValidBlk_e);
    }

    return rba_FeeFs1x_Pending_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_transferValidBlk
 * statefunction of repairSectOverflow: transfers a block from last but one logical sector to last logical sector
 *
 * \retval  rba_FeeFs1x_Pending_e       : either transfer is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_transferValidBlk(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten retValCopy_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* Generic transfer copy function reused. First two parameter values does not matter as they are not used when
         * isCloneWrite_b = TRUE. */
        rba_FeeFs1x_Reorg_transfer1Copy(FALSE, RBA_FEEFS1X_MAXUINT16, TRUE);
    }

    retValCopy_en = rba_FeeFs1x_Reorg_transfer1CopyDo();

    switch(retValCopy_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            // do nothing
        }break;

        // Memory got filled. Continue to search state (control will exit from the search state)
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_JobOK_e:
        {
            // copying finished successfully, search the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_searchValidBlk_e);
        }break;

        // something went wrong. restart complete operation.
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e);
        }break;

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_searchNextValidBlk
 * statefunction of repairSectOverflow: search for first valid block in last but one logical sector
 *
 * \retval  rba_FeeFs1x_Pending_e       : either search is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_searchNextValidBlk(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten          retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* Continue the search immediately after the last address. The search will continue to next logical sector as
         * well (search only stops when the last address of the sector is reached). This is ok, since at end of the search
         * it is checked that only the copies in last but one sector are considered. */
        fsm_pst->searchPageNr_u32++;

        rba_FeeFs1x_Searcher_SR_findNextValidBlockHdr(&fsm_pst->searchPageNr_u32);
    }

    retValSearch_en = rba_FeeFs1x_Searcher_SR_findNextValidBlockHdrDo();

    switch (retValSearch_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_Pending_e:
        {
            // do nothing
        }break;

        /* A valid block header was found. Go to next state to check if this block has to be transferred or not. */
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        {
            if(RBA_FEEFS1X_REORG_PRV_CLONE_DEST_START_PAGE_NR > fsm_pst->searchPageNr_u32)
            {
                // valid block found. transfer the block
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_checkIfBlkOverlapsToLastSect_e);
            }
            else
            {
                // Block was found in the clone sector. this means all blocks have been transferred. Go to write UM.
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrUsed_e);
            }
        }break;

        /* End of the dflash is reached and no valid copy was found. This means that the copy transferred during
         * cloning is also lost (may be due to hardware error). For safety, better to retry starting from first step. */
        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        // something went wrong. restart complete operation.
        case rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e);
        }break;

        // function rba_FeeFs1x_Searcher_SR_findNextValidBlockHdr() should search only one copy
        case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
        case rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_wrUsed
 *  statefunction of repairSectOverflow to write used marker
 *
 * \retval  rba_FeeFs1x_Pending_e       : either writing of marker is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_wrUsed(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten retValInner_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Sector_markUsed(RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u);
    }

    retValInner_en = rba_FeeFs1x_Sector_markSectorDo();

    switch (retValInner_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            //do nothing
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // finished, switch to next state
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrStart_e);
        }break;

        // if writing of the used marker fails, restart overflow handling from first step
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e);
        }break;

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_wrStart
 *  statefunction of repairSectOverflow: writes the start marker into the latest sector
 *
 * \retval  rba_FeeFs1x_Pending_e       : either writing of marker is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_wrStart(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten  retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten  retValInner_en;
    uint32                  startAdr_u32;

    if(FEE_PRV_LIBENTRY)
    {
        startAdr_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->startMarkerOffsetPageNr_u32 + RBA_FEEFS1X_REORG_PRV_CLONE_DEST_START_PAGE_NR);
        rba_FeeFs1x_Sector_markStart((RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u), startAdr_u32);
    }

    retValInner_en = rba_FeeFs1x_Sector_markSectorDo();

    switch (retValInner_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            // do nothing
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // finished, switch to next state
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_wrR2E_e);
        }break;

        /* Ideally it is ok to continue when writing of start marker fails. However, to keep the design consistent
         * with the implementation in rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrStart(), on error complete operation
         * is retried starting with erase. */
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_erase_e);
        }break;

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_wrR2E
 *  statefunction of repairSectOverflow: writes the R2E marker into the previous sector and exchanges the logical sector order
 *
 * \retval  rba_FeeFs1x_Pending_e       : either writing of marker is ongoing or was successful and control is transferred to next state
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_wrR2E(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten  retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten  retValInner_en;
    uint32                  adr_newWr_u32;

    if(FEE_PRV_LIBENTRY)
    {
        // Erase the now cloned sector
        rba_FeeFs1x_Sector_markReady4Erase(RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 2u);
    }

    retValInner_en = rba_FeeFs1x_Sector_markSectorDo();

    switch (retValInner_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            // do nothing
        }break;

        /* case JobFailed: writing of the R2E marker in flash failed, but state in RAM is set
           if in this driving cycle an erase is triggered, sector will be erased, due to the correct state in RAM
           if a reset occurs, complete reorg will be reexecuted, and sector will be finally detected again as R2E
           -> treat this case as if it was JobOK */
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_JobOK_e:
        {
            // exchange the cloned with the clone sector
            // done after the last marker write to make sure the correct sector gets marked
            rba_FeeFs1x_Sector_switchLogSectors
            (
                    (RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u) ,
                    (RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 2u)
            );

            // adapt the write address to new logical sector
            adr_newWr_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();
            adr_newWr_u32 -= RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR;
            rba_FeeFs1x_PAMap_setCurrWrPage(adr_newWr_u32);

            // finished, switch to next state
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_cacheBuildup_e);
        }break;

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return(retVal_en);
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_repairSectOverflowDo_cacheBuildup
 *  statefunction of repairSectOverflow: rebuilds the known cache
 *
 * \retval  rba_FeeFs1x_Pending_e       : cache build up ongoing
 * \retval  rba_FeeFs1x_JobOK_e         : cache build up is completed -> overflow handling is also completed successfuly, reorg can be restarted
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_repairSectOverflowDo_cacheBuildup(rba_FeeFs1x_Reorg_repairSectOverflow_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten  retVal_en = rba_FeeFs1x_Pending_e;
    rba_FeeFs1x_RetVal_ten  retValInner_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* During cloning either all blocks or only some blocks or no blocks were transferred. Safest would be to mark
         * all entries pointing to latest sector - 1 as invalid and search for such blocks again.
         * Invalidate cache entry with highest page no left, because to avoid a data CRC fail/block search when this entry was
         * pointing to an overlapping block, which got destroyed partially */
        rba_FeeFs1x_CacheKwn_invalidateCacheEntriesFromPageNoAndHighestPageNoLeft(RBA_FEEFS1X_REORG_PRV_CLONE_SRC_START_PAGE_NR);

        // invalidate complete unknown cache
        rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache();

        // after cloning, all already cached unknown blocks (starting from sector 0) are deleted,
        // so unknown cache build up has to be completely restarted from sector 0
        rba_FeeFs1x_Searcher_resetSearchForUnknownCacheBuildup();

        // rebuild the cache for the recently invalidated copies
        (void) rba_FeeFs1x_Searcher_buildupCache(FALSE);
    }

    /* Cyclic call the Do-function */
    retValInner_en = rba_FeeFs1x_Searcher_buildupCacheDo();

    switch (retValInner_en)
    {
        case rba_FeeFs1x_JobOK_e:
        case rba_FeeFs1x_Pending_e:
        {
            // map possible return values
            retVal_en = retValInner_en;
        }break;

        /* May be there was some read error -> despite sectors being swapped and a sector being marked for request to erase,
         * it is enough to retry directly with cache build up. */
        case rba_FeeFs1x_ErrorExternal_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_repairSectOverflow_stm_cacheBuildup_e);
        }break;

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected error -> do not continue further
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknown
 * job init function for executing the unknown block reorg
 * \return  is the job accepted?
 * \retval  E_OK     : job accepted, do function allowed to be called
 * \retval  E_NOT_OK : job not accepted, do not call the do function
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Reorg_reorgUnknown(boolean isSoftReorg_b)
{
    // no soft reorg is ongoing
    // reset the statemachine to the first state
    rba_FeeFs1x_Reorg_reorgUnknown_data_st.state_en = rba_FeeFs1x_Reorg_reorgUnknown_stm_reorgCachedBlocks_e;
    rba_FeeFs1x_Reorg_reorgUnknown_data_st.entry_b = TRUE;
    rba_FeeFs1x_Reorg_reorgUnknown_data_st.isSoftReorgOngoing_b = isSoftReorg_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorganiseKnownDo
 * job do function for executing the unknown block reorg
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch(rba_FeeFs1x_Reorg_reorgUnknown_data_st.state_en)
    {
        case rba_FeeFs1x_Reorg_reorgUnknown_stm_checkCacheRebuildReq_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownDo_checkCacheRebuildReq (&rba_FeeFs1x_Reorg_reorgUnknown_data_st);
        }break;
        case rba_FeeFs1x_Reorg_reorgUnknown_stm_cacheRebuild_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownDo_cacheRebuild(&rba_FeeFs1x_Reorg_reorgUnknown_data_st);
        }break;
        case rba_FeeFs1x_Reorg_reorgUnknown_stm_reorgCachedBlocks_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownDo_reorgCachedBlocks(&rba_FeeFs1x_Reorg_reorgUnknown_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    // on job exit without running soft reorg, reset the statemachine
    if( (retVal_en != rba_FeeFs1x_Pending_e) && !(rba_FeeFs1x_Reorg_reorgUnknown_data_st.isSoftReorgOngoing_b))
    {
        rba_FeeFs1x_Reorg_reorgUnknown_data_st.state_en = rba_FeeFs1x_Reorg_reorgUnknown_stm_idle_e;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownDo_checkCacheRebuildReq
 * statefunction of reorgUnknownDo
 * Checks the current unknown cache buildup page and state for necessity to execute another buildup in this reorg
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo_checkCacheRebuildReq  (rba_FeeFs1x_Reorg_reorgUnknown_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(rba_FeeFs1x_Searcher_isUnknownBlkCacheBuildupRequired())
    {
        // The cache buildup is required
        uint32 pageLimitLastUnknownReorg_u32;
        pageLimitLastUnknownReorg_u32 = rba_FeeFs1x_Searcher_getUnknownBlkReorgLimit();

        // Is the cache built up
        if(pageLimitLastUnknownReorg_u32 < RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR)
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknown_stm_cacheRebuild_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            // further rebuild is not required during this reorg, because all blocks with copies in the 0th sector are covered
            fsm_pst->isSoftReorgOngoing_b = FALSE;
            retVal_en = rba_FeeFs1x_JobOK_e;
        }
    }
    else
    {
        // further cache buildup is not required and the cached unknown blocks already are checked before this state
        // reset the soft reorg state
        fsm_pst->isSoftReorgOngoing_b = FALSE;
        retVal_en = rba_FeeFs1x_JobOK_e;
    }
    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownDo_cacheRebuild
 * statefunction of reorgUnknownDo
 * executes the unknown cache buildup
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo_cacheRebuild          (rba_FeeFs1x_Reorg_reorgUnknown_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValInner_en;
    if(FEE_PRV_LIBENTRY)
    {
        // free the unknown cache
        rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache();

        // buildup the unknown cache starting at the previous page
        rba_FeeFs1x_Searcher_buildUpUnknownBlkCache();
    }

    // Cyclic call the Do-function
    retValInner_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo();

    // evaluate the job result
    switch (retValInner_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // after finishing the cache buildup, reorg all cached copies
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknown_stm_reorgCachedBlocks_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorExternal_e:

        {
            retVal_en = retValInner_en;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownDo_reorgCachedBlocks
 * statefunction of reorgUnknownDo
 * executes the blockwise check for all cache elements
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownDo_reorgCachedBlocks     (rba_FeeFs1x_Reorg_reorgUnknown_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValInner_en;
    if(FEE_PRV_LIBENTRY)
    {
        // trigger the reorg of all cached elements
        rba_FeeFs1x_Reorg_reorgUnknownCached(fsm_pst->isSoftReorgOngoing_b);
    }
    // state-do: execute the reorg of cached elements
    retValInner_en = rba_FeeFs1x_Reorg_reorgUnknownCachedDo();

    // evaluate the job result
    switch (retValInner_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            if(rba_FeeFs1x_Reorg_reorgUnknownCached_isSoftOngoing())
            {
                // cache check only interrupted due to soft reorg, repeat this state in next background cycle
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // cache check finished, check the necessity of further cache buildups
                FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknown_stm_checkCacheRebuildReq_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
        }break;
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            // propagate the given return value
            retVal_en = retValInner_en;
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;

        }break;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknown_isSoftOngoing
 * function returing whether the unknown reorg is ongoing during a soft reorg
 * \return  is the soft reorg ongoing?
 * \retval  TRUE  : call the function again during the next background cycle
 * \retval  FALSE : job exited completely
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Reorg_reorgUnknown_isSoftOngoing(void)
{
    return rba_FeeFs1x_Reorg_reorgUnknown_data_st.isSoftReorgOngoing_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCached
 * job init function for checking and if required transferring the cached unknown blocks
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Reorg_reorgUnknownCached(boolean isSoftReorg_b)
{

    // no soft reorg is ongoing
    // reset the counters to the current cache state
    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.nextCacheIdx_u16 = 0u;
    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.cntCachedElements_u16 = rba_FeeFs1x_CacheUnkwn_getUnknownBlkCacheLevel();

    // init the statemachine
    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.state_en = rba_FeeFs1x_Reorg_reorgUnknownCached_stm_selectBlk_e;
    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.entry_b  = TRUE;
    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.isSoftReorgOngoing_b = isSoftReorg_b;


}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCachedDo
 * job do function to reorg all cached unknown blocks
 * executes the blockwise check for all cache elements
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Reorg_reorgUnknownCachedDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch(rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.state_en)
    {
        case rba_FeeFs1x_Reorg_reorgUnknownCached_stm_selectBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownCachedDo_selectBlk(&rba_FeeFs1x_Reorg_reorgUnknownCached_data_st);
        }break;
        case rba_FeeFs1x_Reorg_reorgUnknownCached_stm_searchBlk_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownCachedDo_searchBlk(&rba_FeeFs1x_Reorg_reorgUnknownCached_data_st);
        }break;
        case rba_FeeFs1x_Reorg_reorgUnknownCached_stm_checkReq_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownCachedDo_checkReq(&rba_FeeFs1x_Reorg_reorgUnknownCached_data_st);
        }break;
        case rba_FeeFs1x_Reorg_reorgUnknownCached_stm_copy1_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy1(&rba_FeeFs1x_Reorg_reorgUnknownCached_data_st);
        }break;
        case rba_FeeFs1x_Reorg_reorgUnknownCached_stm_copy2_e:
        {
            retVal_en = rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy2(&rba_FeeFs1x_Reorg_reorgUnknownCached_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    // as soon as any of the states exits the whole job, reset the state machine to idle
    if((retVal_en != rba_FeeFs1x_Pending_e) && !(rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.isSoftReorgOngoing_b))
    {
        rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.state_en = rba_FeeFs1x_Reorg_reorgUnknownCached_stm_idle_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCachedDo_selectBlk
 * statefunction of reorgUnknownCachedDo
 * checks how many cached blocks are left unchecked and decides whether all elements are checked
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgUnknownCachedDo_selectBlk(rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // select the next cache element to be checked
    fsm_pst->currCacheIdx_u16 = fsm_pst->nextCacheIdx_u16;

    // prepare the cycle by selecting the next block to be checked
    fsm_pst->nextCacheIdx_u16++;

    // if the current index is still inbound of the cached copies
    if(fsm_pst->currCacheIdx_u16 < fsm_pst->cntCachedElements_u16)
    {
        Std_ReturnType retValCacheUnkwn_en;
        boolean cpyHasRedBitActive_b = FALSE;     /* The default value is set to avoid warning when there are no redundant blocks configured */

        retValCacheUnkwn_en = rba_FeeFs1x_CacheUnkwn_isCachedCopyRedundantBitActive(fsm_pst->currCacheIdx_u16, &cpyHasRedBitActive_b);

        // block is to be checked for reorg necessity
        // if the block is configured as redundant block, search for two copies instead of one
        if(E_OK == retValCacheUnkwn_en)
        {
            // cache element redundancy extraction successful
            fsm_pst->isRedundantBlock_b = cpyHasRedBitActive_b;

            // switch to search that block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_searchBlk_e);

            if(fsm_pst->isSoftReorgOngoing_b)
            {
                // on soft reorg, pause for the possibility of accepting jobs
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // on hard reorg, directly continue with the search during the next cycle
                retVal_en = rba_FeeFs1x_Pending_e;
            }
        }
        else
        {
            // cache element is not allowed to be accessed.
            // Either an OOB against the RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS occured( catched at upper position)
            // or the current block doesn't have a valid cache entry.

            // no cache entry found --> reenter the state to select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_selectBlk_e);
            if(fsm_pst->isSoftReorgOngoing_b)
            {
                // on soft reorg, pause for the possibility of accepting jobs
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // on hard reorg, directly continue with the search during the next cycle
                retVal_en = rba_FeeFs1x_Pending_e;
            }
        }
    }
    else
    {
        // if all cached blocks are done ( also if 0 copies are cached), end the cache scan
        // end of known block reorg reached

        // reset the soft reorg state
        fsm_pst->isSoftReorgOngoing_b = FALSE;

        retVal_en = rba_FeeFs1x_JobOK_e;
    }
    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCachedDo_searchBlk
 * statefunction of reorgUnknownCachedDo
 * searches the selected block including consistency check
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgUnknownCachedDo_searchBlk(rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en;

    if(FEE_PRV_LIBENTRY)
    {
        // first state call
        // select the search type required for this block
        // isRedundant is extracted from unknown cache in the state before (selectBllk)
        if(fsm_pst->isRedundantBlock_b)
        {
            // for blocks the latest known copy is redundant, search for the 2 newest copies
            rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown(fsm_pst->currCacheIdx_u16);
        }
        else
        {
            // for blocks the latest known copy isn't redundant, search only the latest consistent copy
            rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown(fsm_pst->currCacheIdx_u16);
        }
    }

    // Clyclic call for Do function
    retValSearch_en = rba_FeeFs1x_Searcher_findCopiesDo();

    // evaluate the search result
    switch(retValSearch_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_Pending_e:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        {
            // no copy found --> no reorg necessity. Switch state to select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
        {
            // copie(s) were found --> check reorg necessity for this block
            fsm_pst->retValSearch_en = retValSearch_en;
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_checkReq_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e:
        {
            // unexpected search return or error
            // --> exit the known reorg, restart the whole reorg
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
        }break;
        case rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e:
        default:
        {
            // unexpected search return or error
            // --> exit the known reorg, restart the whole reorg
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCachedDo_checkReq
 * statefunction of reorgUnknownCachedDo
 * checks whether the found latest consistent copy is supposed to be reorged
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgUnknownCachedDo_checkReq(rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst)
{
    uint8                   ctrCopiesRequired_u8;
    rba_FeeFs1x_RetVal_ten  retVal_en;

    ctrCopiesRequired_u8 = rba_FeeFs1x_Reorg_calculateNrOfCopiesToBeTransferred(FALSE, fsm_pst->isRedundantBlock_b, fsm_pst->currCacheIdx_u16, fsm_pst->retValSearch_en);

    // select the required action based on the amount of copies to be transferred
    switch(ctrCopiesRequired_u8)
    {
        case 0u:
        {
            // no latest copy of this block is located within the sector 0
            // continue with the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case 1u:
        {
            // one transfer of the latest consistent copy required
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_copy1_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case 2u:
        {
            // two transfers of the latest consistent copy required
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_copy2_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        default:
        {
            // unexpected amount of writes
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy1
 * statefunction of reorgUnknownCachedDo
 * transfers the latest consistent copy once
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy1(rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCopy_en;

    if(FEE_PRV_LIBENTRY)
    {
        // initial state call, trigger a copy operation for one copy
        rba_FeeFs1x_Reorg_transfer1Copy(FALSE, fsm_pst->currCacheIdx_u16, FALSE);
    }

    retValCopy_en = rba_FeeFs1x_Reorg_transfer1CopyDo();

    // evaluate the job result
    switch(retValCopy_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // copying finished successfully, select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_JobFailed_e:
            // sector overflow detected, stop the reorg
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = retValCopy_en;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;

}
/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy2
 * statefunction of reorgUnknownCachedDo
 * transfers the latest consistent copy twice
 * \return  job result of the reorganisation
 * \retval  rba_FeeFs1x_Pending_e : job needs more cyclic calls
 * \retval  rba_FeeFs1x_JobOK_e : reorg finished
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error lead to a stop of the reorg
 * \retval  rba_FeeFs1x_ErrorExternal_e : error returned by external components, e.g. Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorgUnknownCachedDo_copy2(rba_FeeFs1x_Reorg_reorgUnknownCached_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCopy_en;

    if(FEE_PRV_LIBENTRY)
    {
        // initial state call: init the transfer of two copies
        rba_FeeFs1x_Reorg_transfer2Copies(FALSE, fsm_pst->currCacheIdx_u16);
    }

    // state do
    retValCopy_en = rba_FeeFs1x_Reorg_transfer2CopiesDo();


    // evaluate the search result
    switch(retValCopy_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // copying finished successfully, select the next block
            FEE_PRV_LIBSC(rba_FeeFs1x_Reorg_reorgUnknownCached_stm_selectBlk_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_JobFailed_e:
            // sector overflow detected, stop the known reorg
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = retValCopy_en;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_reorgUnknownCached_isSoftOngoing
 * function returing whether the unknown reorg cache scan is ongoing during a soft reorg
 * \return  is the soft reorg ongoing?
 * \retval  TRUE  : call the function again during the next background cycle
 * \retval  FALSE : job exited completely
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Reorg_reorgUnknownCached_isSoftOngoing(void)
{
    return rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.isSoftReorgOngoing_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Reorg_switchToHardReorg
 * reset all isSoftReorg flags to ensure a continuous reorg execution
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Reorg_switchToHardReorg(void)
{
    rba_FeeFs1x_Reorg_stm_st.isSoftReorgOngoing_b                       = FALSE;
    rba_FeeFs1x_Reorg_Known_data_st.isSoftReorgOngoing_b                = FALSE;
    rba_FeeFs1x_Reorg_reorgUnknown_data_st.isSoftReorgOngoing_b         = FALSE;
    rba_FeeFs1x_Reorg_reorgUnknownCached_data_st.isSoftReorgOngoing_b   = FALSE;
}

/**
 *********************************************************************
 *
 * Get current state of Soft reorg
 *
 * \return  TRUE = Soft reorg is running, FALSE =  Soft reorg not running
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_Reorg_isSoftReorgOngoing(void)
{
    return ((rba_FeeFs1x_Reorg_stm_idle_e != rba_FeeFs1x_Reorg_stm_st.state_en)
            && (rba_FeeFs1x_Reorg_stm_st.isSoftReorgOngoing_b));
}

/**
 *********************************************************************
 *
 * Get current state of Hard reorg
 *
 * \return  TRUE = Hard reorg is running, FALSE =  Hard reorg not running
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_Reorg_isHardReorgOngoing(void)
{
    return ((rba_FeeFs1x_Reorg_stm_idle_e != rba_FeeFs1x_Reorg_stm_st.state_en)
            && (!rba_FeeFs1x_Reorg_stm_st.isSoftReorgOngoing_b));
}


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

