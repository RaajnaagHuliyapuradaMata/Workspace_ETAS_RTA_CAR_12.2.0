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
#ifndef RBA_FEEFS1X_PRV_SEARCHERTYPES_H
#define RBA_FEEFS1X_PRV_SEARCHERTYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv.h"
#include "rba_FeeFs1x_Prv_BC.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#define RBA_FEEFS1X_SEARCHER_NR_BYTES_PREFETCH_UNKWN_BLK (32uL)


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

typedef enum
{
    rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e,
    rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e,
    rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e,
    rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e,
    rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e,
    rba_FeeFs1x_Searcher_RetVal_Pending_e
}rba_FeeFs1x_Searcher_RetVal_ten;


typedef enum
{
    rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known_e,
    rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known_e,
    rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known_e,
    rba_FeeFs1x_Searcher_find2LatestCopies_Known_e,
    rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known_e,
    rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown_e,
    rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known_e,
    rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown_e
}rba_FeeFs1x_Searcher_findCopyJob_ten;


typedef enum
{
    rba_FeeFs1x_Searcher_findCopyStm_idle_e,
    rba_FeeFs1x_Searcher_findCopyStm_initCopy_e
}rba_FeeFs1x_Searcher_findCopyStm_ten;

typedef enum
{
    rba_FeeFs1x_Searcher_searchManuallyStm_idle_e,
    rba_FeeFs1x_Searcher_searchManuallyStm_analysePageContent_e,
    rba_FeeFs1x_Searcher_searchManuallyStm_checkPersId_e,
    rba_FeeFs1x_Searcher_searchManuallyStm_validateHeader_e,
    rba_FeeFs1x_Searcher_searchManuallyStm_checkCont_e,
    rba_FeeFs1x_Searcher_searchManuallyStm_error_e
}rba_FeeFs1x_Searcher_searchManuallyStm_ten;

typedef enum
{
    rba_FeeFs1x_Searcher_cacheTypeKwn_e,
    rba_FeeFs1x_Searcher_cacheTypeUnkwn_e
} rba_FeeFs1x_Searcher_cacheTypes_ten;

typedef struct
{
    uint32 *                                    pageNr_pu32;
    uint32                                      currPageNr_u32;

    uint16                                      persistentId_u16;
    boolean                                     isBackwardSearch_b;
    uint8                                       allowedRetries_u8;

    rba_FeeFs1x_Searcher_searchManuallyStm_ten  state_en;
    boolean                                     entry_b;
}rba_FeeFs1x_Searcher_searchManually_tst;

typedef enum
{
    rba_FeeFs1x_Searcher_buildupCacheStates_idle_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_extractHdr_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_extractHdrSync_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_isCachingReq_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_valHdr_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_addCache_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_updRedCache_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e,
    rba_FeeFs1x_Searcher_buildupCacheStates_exit_e
}rba_FeeFs1x_Searcher_buildupCacheStates_ten;

typedef struct
{
    uint32 currentPage_u32;
    uint32 highestProgrPageNr_u32;

    uint16 ctrFinishedNatCacheEntries_u16;
    uint16 ctrFinishedRedCacheEntries_u16;

    boolean redCopyToBeUpdated_b;
    boolean entry_b;
    boolean isUnkwnBlkDetected_b;
    boolean  isSynchrounousExecution_b;

    uint16 feeIndex_u16;
    uint8 allowedRetries_u8;
    rba_FeeFs1x_Searcher_buildupCacheStates_ten state_en;
}rba_FeeFs1x_Searcher_buildupCache_tst;


typedef enum
{
    rba_FeeFs1x_Searcher_findCopiesDo_stm_idle_e,
    rba_FeeFs1x_Searcher_findCopiesDo_stm_isCacheValid_e,
    rba_FeeFs1x_Searcher_findCopiesDo_stm_initBlockCopy_e,
    rba_FeeFs1x_Searcher_findCopiesDo_stm_checkDataConsistency_e,
    rba_FeeFs1x_Searcher_findCopiesDo_stm_restoreRedBlockData_e,
    rba_FeeFs1x_Searcher_findCopiesDo_stm_manualSearch_e,
    rba_FeeFs1x_Searcher_findCopiesDo_stm_natCopyFound_e,
    rba_FeeFs1x_Searcher_findCopiesDo_stm_redCopyFound_e

}rba_FeeFs1x_Searcher_findCopiesDo_stm_ten;

typedef enum
{
    rba_FeeFs1x_Searcher_findCopiesDo_mode_oneValidatedCopy_e,
    rba_FeeFs1x_Searcher_findCopiesDo_mode_twoCopies_e,
    rba_FeeFs1x_Searcher_findCopiesDo_mode_firstUnvalidatedCopy_e,
    rba_FeeFs1x_Searcher_findCopiesDo_mode_nextUnvalidatedCopy_e

}rba_FeeFs1x_Searcher_findCopiesDo_mode_ten;

typedef struct
{
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten * currCopy_pen;
    uint32 pageNr_u32;

    uint16 feeIdx_u16;
    uint16 persistentId_u16;

    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copyNat_en;
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copyRed_en;
    rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en;
    rba_FeeFs1x_Searcher_findCopiesDo_mode_ten mode_en;

    rba_FeeFs1x_Searcher_findCopiesDo_stm_ten state_en;
    boolean isBlkDoubleStorage_b;
    boolean entry_b;
}rba_FeeFs1x_Searcher_findCopiesDo_data_tst;

typedef enum
{
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_idle_e          ,
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_extractHdr_e    ,
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_isCachingReq_e  ,
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_valHdr_e        ,
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_addCache_e      ,
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_updRedCache_e   ,
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_checkCont_e     ,
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_exit_e

}rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_ten;

typedef struct
{
    uint32 currPage_u32;

    uint16 persIDCurrBlock_u16;
    uint8 allowedRetries_u8;
    boolean entry_b;

    boolean isCacheFull_b;
    boolean isCachedAlready_b;
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_ten state_en;

}rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst;
/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#endif

