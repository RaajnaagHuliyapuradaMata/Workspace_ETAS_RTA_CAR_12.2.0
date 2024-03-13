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
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "rba_FeeFs1x_Prv_Searcher.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"

#include "Fee_Prv_Lib.h"

#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_CacheKwn.h"
#include "rba_FeeFs1x_Prv_CacheUnkwn.h"

#include "rba_FeeFs1x_Prv_PASrv.h"
#include "rba_FeeFs1x_Prv_PARead.h"
#include "rba_FeeFs1x_Prv_PAMap.h"
/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
# if ((RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u) || (TRUE == RBA_FEEFS1X_PRV_CFG_SURVIVAL_EVAL) || (TRUE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_REORG))
#define RBA_FEEFS1X_SEARCHER_RDNT_BLOCK_HANDLING_NEEDED     (TRUE)
# else
#define RBA_FEEFS1X_SEARCHER_RDNT_BLOCK_HANDLING_NEEDED     (FALSE)
# endif

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

static rba_FeeFs1x_Searcher_buildupCache_tst rba_FeeFs1x_Searcher_buildupCache_st;

static rba_FeeFs1x_Searcher_searchManually_tst rba_FeeFs1x_Searcher_searchManually_st;

static rba_FeeFs1x_Searcher_findCopiesDo_data_tst rba_FeeFs1x_Searcher_findCopiesDo_data_st;

static rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st;

static boolean rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b;

static uint32 rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32;

static rba_FeeFs1x_PAMap_passAddrRange_tst rba_FeeFs1x_Searcher_passAddrRange_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE boolean                    rba_FeeFs1x_Searcher_buildupCacheDo_isRedCacheBuildupComplete(uint16 ctrFinishedRedCacheEntries_u16);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten     rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkRedCache(uint16 feeIndex_u16);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten     rba_FeeFs1x_Searcher_buildupCacheDo_updRedCache(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS == 0u)
LOCAL_INLINE boolean rba_FeeFs1x_Searcher_buildupCacheDo_isRedCacheBuildupComplete(uint16 ctrFinishedRedCacheEntries_u16)
{
    (void) ctrFinishedRedCacheEntries_u16;
    return TRUE; /* When there is no redundant block cache, indicate that its built up is completed. */
}

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkRedCache(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return rba_FeeFs1x_ErrorInternal_e; /* This return value prevents further action for redundant cache. */
}

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_updRedCache(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    /* To avoid violation of MISRA rule 8.13 (QAC_3673: The object addressed by the pointer parameter 'fsm_pst' is not
     * modified and so the pointer could be of type 'pointer to const'), a member of fsm_pst needs be be modified. Use
     * same logic like in default branch of rba_FeeFs1x_Searcher_buildupCacheDo. */
    fsm_pst->state_en = rba_FeeFs1x_Searcher_buildupCacheStates_idle_e;
    return rba_FeeFs1x_ErrorInternal_e; /* This return value prevents further action for redundant cache. */
}
# endif

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_isCacheValid_Red(uint16 feeIdx_u16 , boolean * const isCV_pb, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_getLatestEntry_Red(uint16 feeIdx_u16, uint32 * const pageOfLatest_pu32, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_updateRed(
        uint16 feeIdx_u16, uint32 pageFoundManually_u32, boolean hasRedBitActive_b, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);

# if (FALSE == RBA_FEEFS1X_SEARCHER_RDNT_BLOCK_HANDLING_NEEDED)
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_isCacheValid_Red(uint16 feeIdx_u16 , boolean * const isCV_pb, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    (void) feeIdx_u16;
    *isCV_pb = FALSE; /* Modify this pointer parameter to avoid violation of MISRA rule 8.13 (QAC_3673) */
    (void) cacheType_en;
    return E_NOT_OK; /* Without redundant block handling, this function should never be called. */
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_getLatestEntry_Red(uint16 feeIdx_u16, uint32 * const pageOfLatest_pu32, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    (void) feeIdx_u16;
    *pageOfLatest_pu32 = 0uL; /* Modify this pointer parameter to avoid violation of MISRA rule 8.13 (QAC_3673) */
    (void) cacheType_en;
    return E_NOT_OK; /* Without redundant block handling, this function should never be called. */
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_updateRed(uint16 feeIdx_u16, uint32 pageFoundManually_u32, boolean hasRedBitActive_b, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    (void) feeIdx_u16;
    (void) pageFoundManually_u32;
    (void) hasRedBitActive_b;
    (void) cacheType_en;
    return E_NOT_OK; /* Without redundant block handling, this function should never be called. */
}
# endif

static boolean                        rba_FeeFs1x_Searcher_checkNSelectNextPageForBackwardSearch  (uint32 * nrCurrentPage_pu32);
static boolean                        rba_FeeFs1x_Searcher_checkNSelectNextPageForForwardSearch   (uint32 * nrCurrentPage_pu32);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Searcher_buildupCacheDo_extractHdr              (rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Searcher_buildupCacheDo_extractHdrSync          (rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Searcher_buildupCacheDo_valHdr                  (rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Searcher_buildupCacheDo_addCache                (rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);
LOCAL_INLINE void                     rba_FeeFs1x_Searcher_buildupCacheDo_addCache_updHighestPg   (rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Searcher_buildupCacheDo_checkCont               (rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Searcher_buildupCacheDo_exit                    (rba_FeeFs1x_Searcher_buildupCache_tst const * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReqForExtractedBlock(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_ActionOnValidPersId
                                        (uint16 feeIdx_u16,
                                        rba_FeeFs1x_Searcher_buildupCacheStates_ten * followUpState_pen,
                                        rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkWrPage  (uint16 ctrFinishedNatCacheEntries_u16);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkNatCache(uint16 feeIndex_u16);

static void                                  rba_FeeFs1x_Searcher_searchManually(uint16 persistentId_u16, uint32 * pageNr_pu32, boolean isBackwardSearch_b);
static rba_FeeFs1x_Searcher_RetVal_ten       rba_FeeFs1x_Searcher_searchManuallyDo(void);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_analysePageContent(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_checkPersId       (rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_validateHeader    (rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_checkCont(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_error(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst);

static void                                  rba_FeeFs1x_Searcher_findCopies(uint16 feeIndex_u16, const rba_FeeFs1x_Searcher_findCopyJob_ten copyType_en);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_isCacheValid(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_initBlockCopy(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_checkDataConsistency(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_restoreRedBlockData(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_manualSearch(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_natCopyFound(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_redCopyFound(rba_FeeFs1x_Searcher_findCopiesDo_data_tst const * fsm_pst);

static boolean rba_FeeFs1x_Searcher_Cache_isCacheValid    (uint16 feeIdx_u16 , rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
static uint32 rba_FeeFs1x_Searcher_Cache_getLatestEntry    (uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);

LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_setDataValid(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_invalidateCache(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_invalidateRedCache(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_invalidateNatAndRedCaches(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_restoreRed(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_addNewerCopy(uint16 feeIdx_u16, boolean dataValidated_b, uint32 pageNumber_u32,
        boolean hasRedBitActive_b, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_extractHdr  (rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst);
LOCAL_INLINE boolean                rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_check2BeConsidered(void);
LOCAL_INLINE boolean                rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_isCpyToBeCached(boolean isToBeConsidered_b ,
        boolean isCacheFull_b ,
        boolean isBlockCached_b);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_valHdr       (rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_addCache     (rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_checkCont    (rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_exit         (rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst const * fsm_pst);

LOCAL_INLINE void rba_FeeFs1x_Searcher_findCopy_setIteratorDirection(rba_FeeFs1x_Searcher_cacheTypes_ten CacheType_en , uint16 idxBlk_u16);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_init
 * Init function of the Searcher unit
 * \return  init completed successfully?
 * \retval  E_OK : Init passed
 * \retval  E_NOT_OK : Init failed, lock down the FeeFs1x
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_Searcher_init(void)
{
    Std_ReturnType retval_en;

    rba_FeeFs1x_RetVal_ten retVal_cacheBuildup_en;

    rba_FeeFs1x_Searcher_resetSearchForUnknownCacheBuildup();
    rba_FeeFs1x_Searcher_cancel();

    // Execute the cache buildup
    if(E_OK == rba_FeeFs1x_Searcher_buildupCache(TRUE))
    {
        // Cache buildup job accepted --> execute the asynchronous handler until completed / failed
        do
        {
            retVal_cacheBuildup_en = rba_FeeFs1x_Searcher_buildupCacheDo();
        }while(retVal_cacheBuildup_en == rba_FeeFs1x_Pending_e);
        /* Evaluation of while loop: The function is called only during init phase where Fee operates in synchronous mode.
         * The while loop is required to go through the different states of cache build up. The state tranistions are
         * checked and would not lead to infinite loops. As long as Fee_Medium has proper exit criteria,
         * the while loop here is safe (Refer handling of while loop in Fee_Medium) */

        if(retVal_cacheBuildup_en == rba_FeeFs1x_JobOK_e)
        {
            //cache buildup finished successfully
            retval_en = E_OK;
        }
        else
        {
            // cache buildup exited with errors
            retval_en = E_NOT_OK;
        }
    }
    else
    {
        // If the buildupCache job was rejected, the current write currPageNr_u32 is 0-> no cache buildup allowed
        // Clear the flag to indicate that there is no unknown block cache building requried (as there are no blocks in the dump)
        rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b = FALSE;
        retval_en = E_OK;
    }

    return retval_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_cancel
 * Cancel function of the Searcher unit
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_cancel(void)
{
    rba_FeeFs1x_Searcher_buildupCache_st.state_en = rba_FeeFs1x_Searcher_buildupCacheStates_idle_e;
    rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.state_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_idle_e;

    rba_FeeFs1x_Searcher_findCopiesDo_data_st.state_en = rba_FeeFs1x_Searcher_findCopiesDo_stm_idle_e;

    rba_FeeFs1x_Searcher_searchManually_st.state_en = rba_FeeFs1x_Searcher_searchManuallyStm_idle_e;
}

#if(RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_reduceOwnPages
 * reduces the searcher attributes after a reordering of sectors
 * \param   amountOfPages_u32
 * \return reduction was possible
 * \reval  E_OK : all relevant values were reducible
 * \retval E_NOT_OK : at least one relevant value was smaller than the amount of pages
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_Searcher_reduceOwnPages(uint32 amountOfPages_u32)
{
    Std_ReturnType retVal_en;
    if(rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32 >= amountOfPages_u32)
    {
        rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32 -= amountOfPages_u32;
        retVal_en = E_OK;
    }
    else
    {
        if( ! rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b)
        {
            // no further unknown cache buildups are required. The page limit is irrelevant
            retVal_en = E_OK;
        }
        else
        {
            // rebuilding the unknown cache is required, but the pagenumber was pointing into the old sector
            retVal_en = E_NOT_OK;
        }

    }
    return retVal_en;
}
#endif
/**
 *********************************************************************
 * executes the "isCacheValid" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be checked for cacheValidity
 * \param   cacheType_en: known / unknown cache
 * \return  operation is executed successfully, return parameter allowed to be evaluated
 * \retval  TRUE  : Cache is valid
 * \retval  FALSE : Cache is invalid
 * \seealso
 * \usedresources
 *********************************************************************
 */
static boolean rba_FeeFs1x_Searcher_Cache_isCacheValid(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        return( rba_FeeFs1x_CacheKwn_isCacheValid(feeIdx_u16));
    }
    else
    {
        /* call cache function for unknown cache */
        return( rba_FeeFs1x_CacheUnkwn_isCacheValid(feeIdx_u16));
    }
}

/**
 *********************************************************************
 * executes the "isCacheValid_Red" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be checked for cacheValidity of redundant entry
 * \param   isCV_pb : reference to return value, if the request was refering a valid index
 * \param   cacheType_en: known / unknown cache
 * \return  operation is executed successfully, return parameter allowed to be evaluated
 * \retval  E_OK : finished successfully
 * \retval  E_NOT_OK : error during processing, e.g. OOB request or config mimatch
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if(FALSE != RBA_FEEFS1X_SEARCHER_RDNT_BLOCK_HANDLING_NEEDED)
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_isCacheValid_Red(uint16 feeIdx_u16, boolean * const isCV_pb, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    Std_ReturnType retVal_en;

    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        retVal_en = rba_FeeFs1x_CacheKwn_isCacheValid_Red(feeIdx_u16, isCV_pb);
    }
    else
    {
        /* call cache function for unknown cache */
        retVal_en = rba_FeeFs1x_CacheUnkwn_isCacheValid_Red(feeIdx_u16, isCV_pb);
    }

    return (retVal_en);
}
# endif
/**
 *********************************************************************
 * executes the "getLatestEntry" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index of the copy to be pushed
 * \param   cacheType_en: known / unknown cache
 * \return  uint32: Latest Entry
 * \seealso
 * \usedresources
 *********************************************************************
 */
static uint32 rba_FeeFs1x_Searcher_Cache_getLatestEntry(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    uint32 pageOfLatest_u32;

    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        pageOfLatest_u32 = rba_FeeFs1x_CacheKwn_getLatestEntry(feeIdx_u16);
    }
    else
    {
        /* call cache function for unknown cache */
        pageOfLatest_u32 = rba_FeeFs1x_CacheUnkwn_getLatestEntry(feeIdx_u16);
    }

    return (pageOfLatest_u32);
}

/**
 *********************************************************************
 * executes the "getLatestEntry_Red" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index of the copy to be pushed
 * \param   pageOfLatest_pu32 : reference to variable the return value should be stored in if no errors occured
 * \param   cacheType_en: known / unknown cache
 * \return  returns whether the operation was successful or had an internal error
 * \retval  E_OK : finished successfully
 * \retval  E_NOT_OK : aborted due to out of bounds request or invalid cache entry
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if(FALSE != RBA_FEEFS1X_SEARCHER_RDNT_BLOCK_HANDLING_NEEDED)
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_getLatestEntry_Red(uint16 feeIdx_u16, uint32* const pageOfLatest_pu32, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    Std_ReturnType retVal_en;

    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        retVal_en = rba_FeeFs1x_CacheKwn_getLatestEntry_Red(feeIdx_u16, pageOfLatest_pu32);
    }
    else
    {
        /* call cache function for unknown cache */
        retVal_en = rba_FeeFs1x_CacheUnkwn_getLatestEntry_Red(feeIdx_u16, pageOfLatest_pu32);
    }

    return (retVal_en);
}
# endif

/**
 *********************************************************************
 * executes the "updateRed" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index of the copy to be pushed
 * \param   pageFoundManually_u32 : new page to be stored as redudant copy page
 * \param   hasRedBitActive_b   : redundancy statusbit of the red copy
 * \param   cacheType_en: known / unknown cache
 * \return  returns whether the operation was successful or had an internal error
 * \retval  E_OK : finished successfully
 * \retval  E_NOT_OK : aborted due to out of bounds request or invalid cache entry
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if(FALSE != RBA_FEEFS1X_SEARCHER_RDNT_BLOCK_HANDLING_NEEDED)
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Searcher_Cache_updateRed(
       uint16 feeIdx_u16, uint32 pageFoundManually_u32, boolean hasRedBitActive_b, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
   Std_ReturnType retVal_en;

   if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
   {
       /* call cache function for known cache */
       retVal_en = rba_FeeFs1x_CacheKwn_updateRed(feeIdx_u16, pageFoundManually_u32, FALSE, hasRedBitActive_b);
   }
   else
   {
       /* call cache function for unknown cache */
       rba_FeeFs1x_CacheUnkwn_updateRed(feeIdx_u16, pageFoundManually_u32, FALSE, hasRedBitActive_b);
       retVal_en = E_OK;
   }

   return (retVal_en);
}
# endif

/**
 *********************************************************************
 * executes the "setDataValid" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be processed.
 * \param   cacheType_en: known / unknown cache
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_setDataValid(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        rba_FeeFs1x_CacheKwn_setDataValid(feeIdx_u16);
    }
    else
    {
        /* call cache function for unknown cache */
        rba_FeeFs1x_CacheUnkwn_setDataValid(feeIdx_u16);
    }
}

/**
 *********************************************************************
 * executes the "invalidateCache" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be invalidated
 * \param   cacheType_en: known / unknown cache
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_invalidateCache(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        rba_FeeFs1x_CacheKwn_invalidateCache(feeIdx_u16);
    }
    else
    {
        /* call cache function for unknown cache */
        rba_FeeFs1x_CacheUnkwn_invalidateCache(feeIdx_u16);
    }
}

/**
 *********************************************************************
 * executes the "invalidateRedCache" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be invalidated
 * \param   cacheType_en: known / unknown cache
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_invalidateRedCache(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        (void) rba_FeeFs1x_CacheKwn_invalidateCache_Red(feeIdx_u16);
    }
    else
    {
        /* call cache function for unknown cache */
        rba_FeeFs1x_CacheUnkwn_invalidateCache_Red(feeIdx_u16);
    }
}

/**
 *********************************************************************
 * invalidation of native and redundant cache of the selected cache type
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : cache index to be invalidated
 * \param   cacheType_en: known / unknown cache
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_invalidateNatAndRedCaches(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        rba_FeeFs1x_CacheKwn_invalidateCaches_NatAndRed(feeIdx_u16);
    }
    else
    {
        /* call cache function for unknown cache */
        rba_FeeFs1x_CacheUnkwn_invalidateCaches_NatAndRed(feeIdx_u16);
    }
}

/**
 *********************************************************************
 * executes the "invalidateCache" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be popped.
 * \param   cacheType_en: known / unknown cache
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_restoreRed(uint16 feeIdx_u16, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        rba_FeeFs1x_CacheKwn_restoreRed(feeIdx_u16);
    }
    else
    {
        /* call cache function for unknown cache */
        rba_FeeFs1x_CacheUnkwn_restoreRed(feeIdx_u16);
    }
}

/**
 *********************************************************************
 * executes the "addNewerCopy" function selected by cacheType_en
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be updated
 * \param   dataValidated_b : data valid bit of the new copy
 * \param   pageNumber_u32 : pageNr of the new copy
 * \param   hasRedBitActive_b : redundancy statusbit of the new copy
 * \param   cacheType_en: known / unknown cache
 * \return  void
 * \retval  E_OK : Job accepted and executed
 * \retval  E_NOT:OK : Job rejected, e.g. OOB or config mismatch
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_Cache_addNewerCopy(uint16 feeIdx_u16, boolean dataValidated_b, uint32 pageNumber_u32,
        boolean hasRedBitActive_b, rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en)
{
    if(cacheType_en == rba_FeeFs1x_Searcher_cacheTypeKwn_e)
    {
        /* call cache function for known cache */
        rba_FeeFs1x_CacheKwn_addNewerCopy(feeIdx_u16, dataValidated_b, pageNumber_u32, hasRedBitActive_b);
    }
    else
    {
        /* call cache function for unknown cache */
        rba_FeeFs1x_CacheUnkwn_addNewerCopy(feeIdx_u16, dataValidated_b, pageNumber_u32, hasRedBitActive_b);
    }
}


/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCache
 * init function of the cache buildup
 * \return  cache buildup needs to be executed?
 * \retval  E_OK : schedule the rba_FeeFs1x_Searcher_buildupCacheDo until done
 * \retval  E_NOT_OK : rba_FeeFs1x_Searcher_buildupCacheDo not required, erased DFLASH
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_Searcher_buildupCache(boolean isSynchrounousExecution_b)
{
    uint32 currWrPage_u32;
    Std_ReturnType retVal_en;
    boolean isJobAccepted_b;
    // switch the pagebuffering to backwards direction
    rba_FeeFs1x_PARead_setIteratorPreferences(TRUE , RBA_FEEFS1X_PRV_MAX_BUFFER_SIZE_TO_BE_PREFETCHED);

    // set the first currPageNr_u32 to be checked for a header to the currPageNr_u32 before the current write currPageNr_u32
    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();

    // Has the ECU a meaningful blankcheck?
    if(rba_FeeFs1x_Prv_isBlankChkRequired())
    {
        // The device has a meaningful blankcheck -->no complete header can be found,
        // if the current write page is 0 or 1
        if(currWrPage_u32 > 1u)
        {
            // At least one header extraction has to be executed during cache buildup

            // A header read on differential flashes never is allowed to surpass the currWrPage
            rba_FeeFs1x_Searcher_buildupCache_st.currentPage_u32 = currWrPage_u32 - 2u;

            isJobAccepted_b = TRUE;
        }
        else
        {
            // no complete header can be found
            rba_FeeFs1x_Searcher_buildupCache_st.currentPage_u32 = 0u;
            isJobAccepted_b = FALSE;
        }
    }
    else
    {
        // On a device without a speaking blankcheck,
        if(currWrPage_u32 > 0u)
        {
            // At least one header extraction has to be executed during cache buildup

            // A header on a single ended flash could start at the page directly before the currWrPAge
            // (Header and Data CRCs match the Erase Pattern)
            rba_FeeFs1x_Searcher_buildupCache_st.currentPage_u32 = currWrPage_u32 - 1u;

            isJobAccepted_b = TRUE;
        }
        else
        {
            // no complete header can be found
            rba_FeeFs1x_Searcher_buildupCache_st.currentPage_u32 = 0u;
            isJobAccepted_b = FALSE;
        }
    }


    if(isJobAccepted_b)
    {
        // prevent access overstepping sector boundaries
        if(rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(rba_FeeFs1x_Searcher_buildupCache_st.currentPage_u32))
        {
            // If the first page to be checked is overstepping a sector boundary, decrement it again
            rba_FeeFs1x_Searcher_buildupCache_st.currentPage_u32--;
        }

        rba_FeeFs1x_PAMap_passAddrRange_init(&rba_FeeFs1x_Searcher_passAddrRange_st, rba_FeeFs1x_Searcher_buildupCache_st.currentPage_u32);

        rba_FeeFs1x_Searcher_buildupCache_st.isSynchrounousExecution_b = isSynchrounousExecution_b;

        //switch the state to extract header
        if(isSynchrounousExecution_b)
        {
            rba_FeeFs1x_Searcher_buildupCache_st.state_en = rba_FeeFs1x_Searcher_buildupCacheStates_extractHdrSync_e;
        }
        else
        {
            rba_FeeFs1x_Searcher_buildupCache_st.state_en = rba_FeeFs1x_Searcher_buildupCacheStates_extractHdr_e;
        }


        rba_FeeFs1x_Searcher_buildupCache_st.entry_b = TRUE;

        // reset the retry counter
        rba_FeeFs1x_Searcher_buildupCache_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;

        // Reset the block cached counters
        rba_FeeFs1x_Searcher_buildupCache_st.ctrFinishedNatCacheEntries_u16 = 0u;
        rba_FeeFs1x_Searcher_buildupCache_st.ctrFinishedRedCacheEntries_u16 = 0u;

        // Set the variable for detecting the highest unblank page to the page before the write page
        rba_FeeFs1x_Searcher_buildupCache_st.highestProgrPageNr_u32 = currWrPage_u32 - 1u;

        rba_FeeFs1x_Searcher_buildupCache_st.isUnkwnBlkDetected_b = FALSE;

        retVal_en = E_OK;
    }
    else
    {
        retVal_en  = E_NOT_OK;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo
 * Statemachine function executing a cache buildup. Has to be called over multiple cycles until finished
 * If the medium is switched to synchronous mode during init,
 * the function can be called in a loop to complete the cache buildup
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error lead to failed cache buildup
 * \retval  rba_FeeFs1x_JobOk_e : cache buildup finished successfully
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: read error, execution of cache buildup failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_Searcher_buildupCache_st.state_en)
    {
        case rba_FeeFs1x_Searcher_buildupCacheStates_extractHdr_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_extractHdr(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        case rba_FeeFs1x_Searcher_buildupCacheStates_extractHdrSync_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_extractHdrSync(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        case rba_FeeFs1x_Searcher_buildupCacheStates_isCachingReq_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReqForExtractedBlock(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        case rba_FeeFs1x_Searcher_buildupCacheStates_valHdr_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_valHdr(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        case rba_FeeFs1x_Searcher_buildupCacheStates_addCache_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_addCache(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        case rba_FeeFs1x_Searcher_buildupCacheStates_updRedCache_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_updRedCache(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        case rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_checkCont(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        case rba_FeeFs1x_Searcher_buildupCacheStates_exit_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_buildupCacheDo_exit(&rba_FeeFs1x_Searcher_buildupCache_st);
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_Searcher_buildupCache_st.state_en = rba_FeeFs1x_Searcher_buildupCacheStates_idle_e;
    }

    return  retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_extractHdr
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. extracts a header on a currPageNr_u32 to search for block headers
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: read error, execution of cache buildup failed
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error lead to failed cache buildup*
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_extractHdr(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{

    rba_FeeFs1x_RetVal_ten retValExtractHdr_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    // Adapted LIBENTRY handling
    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_extractHdr(fsm_pst->currentPage_u32);
    }

    /* Cyclic Call the Do function */
    retValExtractHdr_en = rba_FeeFs1x_BC_extractHdrDo();

    switch(retValExtractHdr_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_isCachingReq_e);

            // continue the operation
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_JobFailed_e:
        {
            // Page doesn't contain a header(preamble check)
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e);
            // continue the operation
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // read error in extraction
            // retry (limited by retry counter over whole cache buildup)
            if(fsm_pst->allowedRetries_u8 > 0)
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_extractHdr_e);
                fsm_pst->allowedRetries_u8--;
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                retVal_en = rba_FeeFs1x_ErrorExternal_e;
            }
            break;
        }
        case rba_FeeFs1x_Pending_e:
        {
            // Continue in next cycle
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        default:
        {
            // unknown return value --> internal error
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }

    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_extractHdrSync
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. extracts a header on a currPageNr_u32 to search for block headers
 * As long as no page contains a preamble, the function loops over the next pages
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: read error, execution of cache buildup failed
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error lead to failed cache buildup*
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_extractHdrSync(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{

    rba_FeeFs1x_RetVal_ten retValExtractHdr_en;
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint32 readAddr_u32;
    uint32 page_u32;

    boolean isExitLoop_b;


    isExitLoop_b = FALSE;

    do
    {
        // update the medium iterator to the next address
        readAddr_u32 = rba_FeeFs1x_PAMap_passAddrRange_getAddress(&rba_FeeFs1x_Searcher_passAddrRange_st);
        retValExtractHdr_en = rba_FeeFs1x_BC_extractHdrSync(readAddr_u32);

        switch(retValExtractHdr_en)
        {
            case rba_FeeFs1x_JobOK_e:
            {
                // page contains a block header preamble
                page_u32 = rba_FeeFs1x_PAMap_passAddrRange_getPage(&rba_FeeFs1x_Searcher_passAddrRange_st);
                fsm_pst->currentPage_u32 = page_u32;
                rba_FeeFs1x_BC_extractHdrSync_UpdatePage(page_u32);

                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_isCachingReq_e);
                isExitLoop_b = TRUE;
                // continue the operation
                retVal_en = rba_FeeFs1x_Pending_e;
                break;
            }
            case rba_FeeFs1x_JobFailed_e:
            {
                // Page doesn't contain a block header preamble
                if(rba_FeeFs1x_PAMap_passAddrRange_decrementAddress(&rba_FeeFs1x_Searcher_passAddrRange_st))
                {
                    // all pages are scanned -> actualize the page and exit
                    fsm_pst->currentPage_u32 = 0;
                    isExitLoop_b = TRUE;
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_exit_e);
                }

                // continue the operation
                retVal_en = rba_FeeFs1x_Pending_e;
                break;
            }
            case rba_FeeFs1x_ErrorExternal_e:
            {
                // read error in extraction
                // retry (limited by retry counter over whole cache buildup)
                if(fsm_pst->allowedRetries_u8 > 0)
                {
                    // extract the same page again in the next cycle
                    fsm_pst->allowedRetries_u8--;
                    retVal_en = rba_FeeFs1x_Pending_e;
                }
                else
                {
                    isExitLoop_b = TRUE;
                    retVal_en = rba_FeeFs1x_ErrorExternal_e;
                }
                break;
            }
            case rba_FeeFs1x_Pending_e: // must not come to state pending
            default:
            {
                // unknown return value --> internal error
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
                isExitLoop_b = TRUE;
                break;
            }

        }
    }while(!isExitLoop_b);
    /* Evaluation of while loop: The function is called only during init phase where Fee operates in synchronous mode.
     * The while loop here is required to speed up the execution and go through the different states of extracting the block header.
     * The state tranistions are checked and would not lead to infinite loops. As long as Fee_Medium
     * has proper exit criteria, the while loop here is safe (Refer handling of while loop in Fee_Medium) */

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReqForExtractedBlock
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. checks whether a copy needs to be added to cache
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error lead to failed cache buildup
 * \retval  rba_FeeFs1x_JobOk_e : cache buildup finished successfully
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: read error, execution of cache buildup failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReqForExtractedBlock(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    uint16 persID_u16;
    uint16 feeIdx_u16;

    rba_FeeFs1x_RetVal_ten retValWrP_en;
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCalcCont_en;

    rba_FeeFs1x_Searcher_buildupCacheStates_ten followUpState_en;

    // Check whether a comparison of the last currPageNr_u32 of the block to the currWrPage allows a cache update for this copy
    retValWrP_en = rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkWrPage(fsm_pst->ctrFinishedNatCacheEntries_u16);

    switch(retValWrP_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // extract the block persistent ID and check for the necessity of a cache update
            persID_u16 = rba_FeeFs1x_BC_getPersID(rba_FeeFs1x_BC_Cpy_Internal_e);
            feeIdx_u16 = Fee_Prv_ConfigGetBlockNrByPersistentId(persID_u16, RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
            retValCalcCont_en = rba_FeeFs1x_Searcher_buildupCacheDo_ActionOnValidPersId
                    (feeIdx_u16, &followUpState_en, fsm_pst);

            if(rba_FeeFs1x_Pending_e == retValCalcCont_en)
            {
                //switch state
                FEE_PRV_LIBSC(followUpState_en);
            }
            retVal_en = retValCalcCont_en;
        }break;
        case rba_FeeFs1x_JobFailed_e:
        {
            // the copy is overlapping the currWrPage on a device with speaking blankcheck
            // ignore the header and continue with next page
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e);
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
 * rba_FeeFs1x_Searcher_buildupCacheDo_ActionOnValidPersId
 * helper function calculating the follow up state and internal variables on successful extraction of the persistent Id
 * The follow up state depends from the result of the length compare between configuration and evaluated block
 * \param   feeIdx_u16:    fee index
 * \param   followUpState_pen:  var. reference of follow up state
 * \param   fsm_pst : statemachine data pointer
 * \return  result of evaluation
 * \retval  rba_FeeFs1x_ErrorInternal_e: internal error lead to failed cache buildup
 * \retval  rba_FeeFs1x_Pending_e:      call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_ActionOnValidPersId
                                        (uint16 feeIdx_u16,
                                        rba_FeeFs1x_Searcher_buildupCacheStates_ten * followUpState_pen,
                                        rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValNatCache_en;
    rba_FeeFs1x_RetVal_ten retValRedCache_en;

    // last page of the block was calculated
    if(feeIdx_u16 == 0xFFFFuL)
    {
        // possible unkown block found, so unknown block cache build up would be required
        fsm_pst->isUnkwnBlkDetected_b = TRUE;
        // unknown block
        // ignore the header and continue with next page
        *followUpState_pen = rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e;
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // index found for persID --> known block  AND
        // the copy isn't conflicting with the current write currPageNr_u32

        // check whether native or redundant cache needs to be updated
        retValNatCache_en = rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkNatCache(feeIdx_u16);
        switch(retValNatCache_en)
        {
            case rba_FeeFs1x_JobOK_e:
            {
                // The native cache needs to be updated
                fsm_pst->redCopyToBeUpdated_b = FALSE;
                fsm_pst->feeIndex_u16 = feeIdx_u16;

                // switch to header validation
                *followUpState_pen = rba_FeeFs1x_Searcher_buildupCacheStates_valHdr_e;
                retVal_en = rba_FeeFs1x_Pending_e;
            }break;
            case rba_FeeFs1x_ErrorInternal_e:
            {
                // Native cache doesn't need an update
                // Check redundant cache update necessity
                retValRedCache_en = rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkRedCache(feeIdx_u16);
                switch(retValRedCache_en)
                {
                    case rba_FeeFs1x_JobOK_e:
                    {
                        // The red cache needs to be updated
                        // set the flag for native cache update
                        fsm_pst->redCopyToBeUpdated_b = TRUE;
                        fsm_pst->feeIndex_u16 = feeIdx_u16;

                        // switch to header validation
                        *followUpState_pen = rba_FeeFs1x_Searcher_buildupCacheStates_valHdr_e;
                        retVal_en = rba_FeeFs1x_Pending_e;
                    }break;
                    case rba_FeeFs1x_ErrorInternal_e:
                    {
                        // The red cache doesn't need to be updated
                        // extract next currPageNr_u32
                        *followUpState_pen = rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e;
                        retVal_en = rba_FeeFs1x_Pending_e;
                    }break;
                    case rba_FeeFs1x_ErrorExternal_e:
                    default:
                    {
                        // OOB / validity issues during accessing other unit's variables
                        *followUpState_pen = rba_FeeFs1x_Searcher_buildupCacheStates_idle_e;
                        retVal_en = rba_FeeFs1x_ErrorInternal_e;
                    }break;
                }break;
            }
            case rba_FeeFs1x_ErrorExternal_e:
            default:
            {
                // OOB / validity issues during accessing other unit's variables
                *followUpState_pen = rba_FeeFs1x_Searcher_buildupCacheStates_idle_e;
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }break;
        }
    }
    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkWrPage
 * subfunction of statefunction of rba_FeeFs1x_Searcher_buildupCacheDo.
 * If the currently scanned block is the first one to be added to cahce, this function
 * checks the found copie's length against the current write currPageNr_u32. Depending on the blankcheck possibilities
 * of the device, it allows or forbids to consider a copy for cache buildup
 * \param ctrFinishedNatCacheEntries_u16 : amount of copies already added to the native cache during this cache buildup
 * \return  cache add allowed
 * \retval  rba_FeeFs1x_JobOk_e        : write address check doesn't forbid the caching of the copy
 * \retval  rba_FeeFs1x_JobFailed_e    : Header doesn't need to be validated; continue with next currPageNr_u32
 * \retval  rba_FeeFs1x_ErrorInternal_e: OOB request or invalid copy object
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkWrPage(uint16 ctrFinishedNatCacheEntries_u16)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // only execute the check for the newest copy
    if(0u == ctrFinishedNatCacheEntries_u16)
    {
        // only compare against the current write page on devices with meaningful blankcheck (differential flashes)
        if(rba_FeeFs1x_Prv_isBlankChkRequired())
        {
            uint32 lastPageOfBlock_u32;

            // Try to get the last page of the block for comparing it against the currWrPage
            lastPageOfBlock_u32 = rba_FeeFs1x_BC_getPageNrOfLastPage(rba_FeeFs1x_BC_Cpy_Internal_e);
            // is the block fully fitting below the currWrPage?
            if(rba_FeeFs1x_PAMap_getCurrWrPage() <= lastPageOfBlock_u32 )
            {
                // With a meaningful blankcheck, if the lastPage of the block is surpassing the current write page,
                // the current write page would be updated to a higher value,
                // the block copy is incompletely written and shall not be considered
                retVal_en = rba_FeeFs1x_JobFailed_e;
            }
            else
            {
                // With a meaningful blankcheck, if the lastPage of the block is not surpassing the current write page,
                // update the cache if required
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
        }
        else
        {
            // without speaking blankcheck (single ended flash), the cache buildup should cache copies overlapping the current write page
            // and update the write page after finished cache buildup
            retVal_en = rba_FeeFs1x_JobOK_e;
        }
    }
    else
    {
        // there already are valid copies found -> don't perform the Wr Adr check any more
        retVal_en = rba_FeeFs1x_JobOK_e;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkNatCache
 * subfunction of statefunction of rba_FeeFs1x_Searcher_buildupCacheDo.
 * checks whether the cache has already an entry for the found block
 * \return  cache add allowed
 * \retval  rba_FeeFs1x_ErrorInternal_e : Copy already cached by native cache
 * \retval  rba_FeeFs1x_JobOk_e : Needs a cache update --> continue with validation for native cache add
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkNatCache(uint16 feeIndex_u16)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // check cache for necessity of caching for the given index
    if(rba_FeeFs1x_CacheKwn_isCacheValid(feeIndex_u16))
    {
        // Already cached copy
        // doesn't need to be updated
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }
    else
    {
        // Caching for native copy required
        retVal_en = rba_FeeFs1x_JobOK_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkRedCache
 * subfunction of statefunction of rba_FeeFs1x_Searcher_buildupCacheDo.
 * checks whether the cache has already a red entry for the found block
 * \return  cache add allowed
 * \retval  rba_FeeFs1x_ErrorInternal_e : Copy already cached by red cache
 * \retval  rba_FeeFs1x_JobOk_e : Needs a cache update --> continue with validation for red cache add
 * \retval  rba_FeeFs1x_ErrorExternal_e: access error on other unit's variables
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_isCachingReq_checkRedCache(uint16 feeIndex_u16)
{
    boolean isCV_Red_b;
    rba_FeeFs1x_RetVal_ten retVal_en;
    if(Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr(feeIndex_u16, RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast))
    {
        // Try to get the redundant cache state for that block
        if(rba_FeeFs1x_CacheKwn_isCacheValid_Red(feeIndex_u16, &isCV_Red_b) == E_OK)
        {
            if(isCV_Red_b)
            {
                // red cache doesn't need an update
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }
            else
            {
                // red cache needs an update
                retVal_en = rba_FeeFs1x_JobOK_e;
            }

        }
        else
        {
            // inconsistent configuration regarding redundant storage
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
        }
    }
    else
    {
        // Copy is not redundant and doesn't need a cache update --> continue with next page check
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }
    return retVal_en;
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_valHdr
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. executes the missing steps for copies
 * which where detected to need a cache buildup
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error lead to failed cache buildup
 * \retval  rba_FeeFs1x_JobOk_e : cache buildup finished successfully
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: read error, execution of cache buildup failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_valHdr(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_ValHdrDo_en;
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint16 nrBytesBlkLengthConfigured_u16;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_validateHdr(TRUE);
    }
    retVal_ValHdrDo_en = rba_FeeFs1x_BC_validateHdrDo();

    switch(retVal_ValHdrDo_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            nrBytesBlkLengthConfigured_u16 = Fee_Prv_ConfigGetBlockLengthByBlockNr(fsm_pst->feeIndex_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
            // Check whether the copie's length is matching the config and that header does not surpass the sector end
            if(rba_FeeFs1x_BC_extract_isInternalCpyLengthMatching(nrBytesBlkLengthConfigured_u16))
            {
                //validation returned, that the currPageNr_u32 contains a valid header of a block to be cached

                // If the native copy is missing, add to native, if only the redundant is missing, updateRed
                if( ! (fsm_pst->redCopyToBeUpdated_b) )
                {
                    // native cache to be updated
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_addCache_e);
                }
                else
                {
                    // redundant cache to be updated
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_updRedCache_e);
                }
            }
            else
            {
                // length of analysed copy and configured length is mismatching
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e);
            }
            // continue the operation
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_JobFailed_e:
        {
            // header is not valid
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e);
            // continue the cache buildup with next currPageNr_u32
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // read error during validation
            // try to retry (limited by retry counter over whole cache buildup)
            if(fsm_pst->allowedRetries_u8 > 0)
            {
                if(fsm_pst->isSynchrounousExecution_b)
                {
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_extractHdrSync_e);
                }
                else
                {
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_extractHdr_e);
                }
                fsm_pst->allowedRetries_u8--;
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                retVal_en = rba_FeeFs1x_ErrorExternal_e;
            }

            break;
        }
        case rba_FeeFs1x_ErrorInternal_e:
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = retVal_ValHdrDo_en;
            break;
        }
        default:
        {
            // unknown return value --> error
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }


    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_addCache
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. adds a copy to the native cache
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_addCache(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    uint16 feeIndex_u16;
    uint32 pageNr_u32;
    boolean hasRedBitActive_b;
    const boolean dataValidated_b = FALSE;

    // get the required values for the cache adding
    feeIndex_u16 = fsm_pst->feeIndex_u16;
    pageNr_u32   = fsm_pst->currentPage_u32;

    hasRedBitActive_b = rba_FeeFs1x_BC_getCpyRedBit(rba_FeeFs1x_BC_Cpy_Internal_e);

    // If the status bit extraction worked, try to add the copy to the cache
    rba_FeeFs1x_CacheKwn_addNewerCopy(feeIndex_u16 , dataValidated_b , pageNr_u32, hasRedBitActive_b);
    // increment the counter of completed redundant cache entries limiting the cache buildup pagescan
    fsm_pst->ctrFinishedNatCacheEntries_u16++;

    // update the highest programmed currPageNr_u32 if the block is overlapping the initially detected wrPage
    rba_FeeFs1x_Searcher_buildupCacheDo_addCache_updHighestPg(fsm_pst);
    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e);

    return rba_FeeFs1x_Pending_e;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_addCache_updHighestPg
 * subfunction of  of rba_FeeFs1x_Searcher_buildupCacheDo_addCache state: searches for the highest written currPageNr_u32
 * \param   fsm_pst : statemachine data pointer
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_buildupCacheDo_addCache_updHighestPg(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    uint32 lastPage_u32;
    // if the last page of the current block is greater than the remembered highest written page, update the variable
    lastPage_u32 = rba_FeeFs1x_BC_getPageNrOfLastPage(rba_FeeFs1x_BC_Cpy_Internal_e);
    if( lastPage_u32 > fsm_pst->highestProgrPageNr_u32)
    {
        fsm_pst->highestProgrPageNr_u32 = lastPage_u32;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_updRedCache
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. adds a copy to the red cache
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error lead to failed cache buildup
 * \retval  rba_FeeFs1x_JobOk_e : cache buildup finished successfully
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: read error, execution of cache buildup failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_updRedCache(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    Std_ReturnType updateRedRetVal_en;

    uint16 feeIndex_u16;
    uint32 pageNr_u32;
    boolean hasRedBitActive_b;

    // get the required values for the cache adding
    feeIndex_u16 = fsm_pst->feeIndex_u16;
    pageNr_u32   = fsm_pst->currentPage_u32;

    hasRedBitActive_b = rba_FeeFs1x_BC_getCpyRedBit(rba_FeeFs1x_BC_Cpy_Internal_e);

    // If the status bit extraction worked, try to add the copy to the cache
    updateRedRetVal_en = rba_FeeFs1x_CacheKwn_updateRed(feeIndex_u16, pageNr_u32, FALSE, hasRedBitActive_b);

    if(E_OK == updateRedRetVal_en)
    {
        // increment the counter of completed redundant cache entries limiting the cache buildup pagescan
        fsm_pst->ctrFinishedRedCacheEntries_u16++;

        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_checkCont_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // cache rejected the adding
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }

    return retVal_en;
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_checkNSelectNextPageForBackwardSearch
 * Function is used when searching for blocks in the memory in backdward direction (latest logical address to oldest logical address).
 * It checks whether the current page is reaching page 0. If 0 is not reached yet, decrement the scan page.
 * Also, if the next header extraction would cross a sector boundary, decrement again.

 * \param   nrCurrentPage_pu32 : pointer to the current page being searched in backward direction;
 *                               gets decremented if the search has to be continued
 *
 * \return  search can be continued?
 * \retval  TRUE               : continue the header extraction on the decremented page number
 * \retval  FALSE              : address 0 reached, no more search possible
 * \seealso
 * \usedresources
 *********************************************************************
 */
static boolean rba_FeeFs1x_Searcher_checkNSelectNextPageForBackwardSearch(uint32 * nrCurrentPage_pu32)
{
    boolean isToBeContinued_b;
    // Are pages left to be scanned?
    if(*nrCurrentPage_pu32 > 0uL)
    {
        // not all pages were scanned yet
        *nrCurrentPage_pu32 = *nrCurrentPage_pu32 - 1u;

        // if a header in the current page is surpassing a sector boundary, skip this page
        if(rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(*nrCurrentPage_pu32))
        {
            // The check applied onto the current page executed by isNextHdrCheckSurpassingSectorEnd is preventing
            // the possibility of an underflow of the current page
            // ( calling isNextHdr.. with page == 1 always returns FALSE)
            *nrCurrentPage_pu32 = *nrCurrentPage_pu32 - 1u;
        }
        isToBeContinued_b = TRUE;
    }
    else
    {
        // all pages are scanned, cache buildup can be stopped
        isToBeContinued_b = FALSE;
    }

    return isToBeContinued_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_checkNSelectNextPageForForwardSearch
 * Function is used when searching for blocks in the memory in forward direction (lowest logical address to latest logical address).
 * It checks whether the current page is reaching either end of memory or write address. If end not reached yet, increment the scan page.
 * Also, if the next header extraction would cross a sector boundary, increments again.
 *
 * \param   nrCurrentPage_pu32 : pointer to the current page being searched in forward direction;
 *                               gets incremented if the search has to be continued
 *
 * \return  search can be continued?
 * \retval  TRUE               : continue the header extraction on the incremented page number
 * \retval  FALSE              : end address reached, no more search possible
 * \seealso
 * \usedresources
 *********************************************************************
 */
static boolean rba_FeeFs1x_Searcher_checkNSelectNextPageForForwardSearch(uint32 * nrCurrentPage_pu32)
{
    boolean isToBeContinued_b;

    uint32 nrPageLastHdrPage_u32;

    *nrCurrentPage_pu32 = *nrCurrentPage_pu32 + 1u;

    // prevent address access OOB on last pages in sector
    // (request given to Fee_Medium leaving the defined memory area in the latest physical area)
    if(rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(*nrCurrentPage_pu32))
    {
        *nrCurrentPage_pu32 = *nrCurrentPage_pu32 + 1u;
    }

    nrPageLastHdrPage_u32 = *nrCurrentPage_pu32 + (RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR - 1uL);

    if( nrPageLastHdrPage_u32 < rba_FeeFs1x_PASrv_getCurrWritePage())
    {
        // cache buildup needs to be continued
        isToBeContinued_b = TRUE;
    }
    else
    {
       isToBeContinued_b = FALSE;
    }

    return isToBeContinued_b;
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_isRedCacheBuildupComplete
 * state sub function of rba_FeeFs1x_Searcher_buildupCacheDo. checks whether the cache buildup for redundant blocks is complete.
 * If all redundant cache entries are filled, returns TRUE.
 * \param   ctrFinishedRedCacheEntries_u16: Current number of redundant block copies cached in the redundant cache
 *
 * \return  cache buildup for redundant block complete?
 * \retval  TRUE : all redundant blocks are cached
 * \retval  FALSE: redundnat blocks should still be searched and cached
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Searcher_buildupCacheDo_isRedCacheBuildupComplete(uint16 ctrFinishedRedCacheEntries_u16)
{
    return(!(ctrFinishedRedCacheEntries_u16 < RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS));
}
# endif
/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_checkCont
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. checks whether the cache buildup needs to be continued.
 * If all cache entries are filled, stop the execution. Otherwise continue up to currPageNr_u32 0
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error lead to failed cache buildup
 * \retval  rba_FeeFs1x_JobOk_e : cache buildup finished successfully
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorExternal_e: read error, execution of cache buildup failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_checkCont(rba_FeeFs1x_Searcher_buildupCache_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // are the known block caches filled already?
    if
    (
            (fsm_pst->ctrFinishedNatCacheEntries_u16 < RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS) ||
            (!rba_FeeFs1x_Searcher_buildupCacheDo_isRedCacheBuildupComplete(fsm_pst->ctrFinishedRedCacheEntries_u16))
    )
    {
        // The caches are not filled completely yet
        // Select the corresponding extractHdr state based on the synchronous execution state
        if(fsm_pst->isSynchrounousExecution_b)
        {
            if(rba_FeeFs1x_PAMap_passAddrRange_decrementAddress(&rba_FeeFs1x_Searcher_passAddrRange_st))
            {
                // all pages are scanned --> cache buildup completed, update the page number and exit
                fsm_pst->currentPage_u32 = 0;

                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_exit_e);
            }
            else
            {
                // pages are remaining to be scanned and the caches are not filled yet
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_extractHdrSync_e);
            }
        }
        else
        {
            if(rba_FeeFs1x_Searcher_checkNSelectNextPageForBackwardSearch(&fsm_pst->currentPage_u32))
            {
                // pages are remaining to be scanned and the caches are not filled yet
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_extractHdr_e);
            }
            else
            {
                // all pages scanned --> cache buildup completed
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_exit_e);
            }
        }
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // Both native and redundant cache are completely filled --> cache buildup completed
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildupCacheStates_exit_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildupCacheDo_exit
 * statefunction of rba_FeeFs1x_Searcher_buildupCacheDo. updates the current write currPageNr_u32
 * if required and finalizes the used block copy
 * \param   fsm_pst : statemachine data pointer
 * \return  cache buildup successful?
 * \retval  rba_FeeFs1x_JobOk_e : cache buildup finished successfully
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildupCacheDo_exit(rba_FeeFs1x_Searcher_buildupCache_tst const * fsm_pst)
{
    // any unknown blocks found while scanning the whole DFLASH?
    if((fsm_pst->currentPage_u32 == 0uL) && (!fsm_pst->isUnkwnBlkDetected_b))
    {
        // complete memory was scanned and there was no unknown block found, so no cache build up also required
        rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b = FALSE;
    }

    // Check whether the DFLASH has a meaningful blankcheck
    if(!rba_FeeFs1x_Prv_isBlankChkRequired())
    {
        // The flash has no speaking blankcheck, e.g. a single ended flash with a passing ECC check on erased cells
        // If the highest page addressed by a valid block copy is greater than the detected write currPageNr_u32,
        // update to the currPageNr_u32 after
        if(rba_FeeFs1x_PASrv_getCurrWritePage() <= fsm_pst->highestProgrPageNr_u32)
        {
            rba_FeeFs1x_PAMap_setCurrWrPage(fsm_pst->highestProgrPageNr_u32 + 1uL);
        }
    }

    rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_Internal_e);
    // deletion of the cache buildup data done

    return rba_FeeFs1x_JobOK_e;
}

/**
 *********************************************************************
 * initializes the search to find the latest consistent known copy of the requested block.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIdx_u16 : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known(uint16 feeIdx_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIdx_u16,rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known_e);
}

/**
 *********************************************************************
 * initializes the search to find the latest unverified known copy of the requested block.
 * (This will find the first known copy only)
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIdx_u16 : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known(uint16 feeIdx_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIdx_u16,rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known_e);
}

/**
 *********************************************************************
 * initializes the search to find the latest unverified known copy of the requested block.
 * (This will find the next known copy after an earlier copy is known to be inconsistent)
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIdx_u16 : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known(uint16 feeIdx_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIdx_u16,rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known_e);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_find2LatestCopies_Known
 * Searches 2 copies of the requested block. The latest gets validated against the data CRC,
 * the redundant only gets returned
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_find2LatestCopies_Known(uint16 feeIndex_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIndex_u16,rba_FeeFs1x_Searcher_find2LatestCopies_Known_e);
}

/**
 *********************************************************************
 * sector reorganisation:
 * initializes the search to find the latest consistent known copy of the requested block.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIdx_u16 : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known(uint16 feeIndex_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIndex_u16,rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known_e);
}

/**
 *********************************************************************
 * sector reorganisation:
 * initializes the search to find the latest consistent unknown copy of the requested block.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIdx_u16 : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown(uint16 feeIndex_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIndex_u16,rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown_e);
}

/**
 *********************************************************************
 * sector reorganisation:
 * Searches 2 known copies of the requested block. The latest gets validated against the data CRC,
 * the redundant only gets returned
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known(uint16 feeIndex_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIndex_u16,rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known_e);
}

/**
 *********************************************************************
 * sector reorganisation:
 * Searches 2 unknown copies of the requested block. The latest gets validated against the data CRC,
 * the redundant only gets returned
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex : block index to be searched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown(uint16 feeIndex_u16)
{
    rba_FeeFs1x_Searcher_findCopies(feeIndex_u16,rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown_e);
}

/**
 *********************************************************************
 * sector reorganisation:
 * initializes the search to find next valid block starting from the passed address (only header validation is done).
 * Function searches both known and unknown block. Search is done in forward direction starting from the address
 * passed and till the write address.
 *
 * \param   pageNr_pu32 : Pointer containing the start page from which the block has to be searched in forward direction
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_SR_findNextValidBlockHdr(uint32 * pageNr_pu32)
{
    // Generic manual search function called with default persistent id and performing search in forward direction.
    rba_FeeFs1x_Searcher_searchManually(RBA_FEEFS1X_MAXUINT16, pageNr_pu32, FALSE);
}

/**
 *********************************************************************
 * Do function for searching the next valid block
 * \param   none
 * \return  status of search
 * \seealso rba_FeeFs1x_Searcher_SR_findNextValidBlockHdr()
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_SR_findNextValidBlockHdrDo(void)
{
    rba_FeeFs1x_Searcher_RetVal_ten searchManuallyDoRetVal_en;

    /* execute the manual search */
    searchManuallyDoRetVal_en = rba_FeeFs1x_Searcher_searchManuallyDo();

    if(rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e == searchManuallyDoRetVal_en)
    {
        /* Search was complete and one copy was found ->
         * Copy the contents from internal block copy to rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e */
        rba_FeeFs1x_BC_copyObjectInternal(rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e);
    }

    return(searchManuallyDoRetVal_en);
}

/**
 *********************************************************************
 * initialisation function for rba_FeeFs1x_Searcher_findCopies
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIdx_u16 : block index to be searched
 * \param   copyType_en: find copy action type
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_Searcher_findCopies(uint16 feeIndex_u16, const rba_FeeFs1x_Searcher_findCopyJob_ten copyType_en)
{
    uint16 persistentId_u16;
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copyNat_en;
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copyRed_en;
    rba_FeeFs1x_Searcher_cacheTypes_ten cacheType_en;
    rba_FeeFs1x_Searcher_findCopiesDo_mode_ten mode_en;
    boolean isBlkDoubleStorage_b;

    switch(copyType_en)
    {
        case rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known_e:
        case rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known_e:
        case rba_FeeFs1x_Searcher_find2LatestCopies_Known_e:
        {
            copyNat_en = rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e;
        } break;
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown_e:
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known_e:
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown_e:
        {
            copyNat_en = rba_FeeFs1x_BC_Cpy_SectorReorg_Latest_e;
        } break;
        default:
        {
            copyNat_en = rba_FeeFs1x_BC_Cpy_Internal_e;
        } break;
    }

    switch(copyType_en)
    {
        case rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known_e:
        case rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known_e:
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown_e:
        {
            copyRed_en = rba_FeeFs1x_BC_Cpy_NrOfCopies_e;
        } break;
        case rba_FeeFs1x_Searcher_find2LatestCopies_Known_e:
        {
            copyRed_en = rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e;
        } break;
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known_e:
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown_e:
        {
            copyRed_en = rba_FeeFs1x_BC_Cpy_SectorReorg_Redundant_e;
        } break;
        default:
        {
            copyRed_en = rba_FeeFs1x_BC_Cpy_Internal_e;
        } break;
    }

    switch(copyType_en)
    {
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown_e:
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown_e:
        {
            cacheType_en = rba_FeeFs1x_Searcher_cacheTypeUnkwn_e;
            persistentId_u16 = rba_FeeFs1x_CacheUnkwn_getPersID(feeIndex_u16);
            /* isBlkDoubleStorage_b is used for detection of block config changes */
            /* We don't need this for unknown block so we force it to TRUE that we never retry */
            isBlkDoubleStorage_b = TRUE;
        } break;
        case rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known_e:
        case rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known_e:
        case rba_FeeFs1x_Searcher_find2LatestCopies_Known_e:
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known_e:
        default:
        {
            cacheType_en = rba_FeeFs1x_Searcher_cacheTypeKwn_e;
            persistentId_u16 = Fee_Prv_ConfigGetBlockPersistentIdByBlockNr(feeIndex_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
            isBlkDoubleStorage_b = Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr(feeIndex_u16, RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
        } break;
    }

    switch(copyType_en)
    {
        case rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known_e:
        {
            mode_en = rba_FeeFs1x_Searcher_findCopiesDo_mode_firstUnvalidatedCopy_e;
        } break;
        case rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known_e:
        {
            mode_en = rba_FeeFs1x_Searcher_findCopiesDo_mode_nextUnvalidatedCopy_e;
        } break;
        case rba_FeeFs1x_Searcher_find2LatestCopies_Known_e:
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Known_e:
        case rba_FeeFs1x_Searcher_SR_find2LatestCopies_Unknown_e:
        {
            mode_en = rba_FeeFs1x_Searcher_findCopiesDo_mode_twoCopies_e;
        } break;
        case rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Known_e:
        case rba_FeeFs1x_Searcher_SR_find1LatestConsistentCopy_Unknown_e:
        default:
        {
            mode_en = rba_FeeFs1x_Searcher_findCopiesDo_mode_oneValidatedCopy_e;
        } break;
    }

    rba_FeeFs1x_Searcher_findCopiesDo_data_st.currCopy_pen = &rba_FeeFs1x_Searcher_findCopiesDo_data_st.copyNat_en;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.copyNat_en = copyNat_en;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.copyRed_en = copyRed_en;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.cacheType_en = cacheType_en;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.mode_en = mode_en;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.state_en = rba_FeeFs1x_Searcher_findCopiesDo_stm_isCacheValid_e;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.feeIdx_u16 = feeIndex_u16;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.persistentId_u16 = persistentId_u16;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.isBlkDoubleStorage_b = isBlkDoubleStorage_b;
    rba_FeeFs1x_Searcher_findCopiesDo_data_st.entry_b = TRUE;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_findCopiesDo
 * shared handler function for finding one or two copies for all requesters
 * \return  Returns how many copies are found
 * \retval  rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e : no (consistent) copies were found
 * \retval  rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e : one copy was found
 * \retval  rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e : two copies were found
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e : internal error, job aborted
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo(void)
{
    rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst = &rba_FeeFs1x_Searcher_findCopiesDo_data_st;
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;

    switch(fsm_pst->state_en)
    {
        case rba_FeeFs1x_Searcher_findCopiesDo_stm_isCacheValid_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_findCopiesDo_isCacheValid(fsm_pst);
        }break;
        case rba_FeeFs1x_Searcher_findCopiesDo_stm_initBlockCopy_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_findCopiesDo_initBlockCopy(fsm_pst);
        }break;
        case rba_FeeFs1x_Searcher_findCopiesDo_stm_checkDataConsistency_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_findCopiesDo_checkDataConsistency(fsm_pst);
        }break;
        case rba_FeeFs1x_Searcher_findCopiesDo_stm_restoreRedBlockData_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_findCopiesDo_restoreRedBlockData(fsm_pst);
        }break;
        case rba_FeeFs1x_Searcher_findCopiesDo_stm_manualSearch_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_findCopiesDo_manualSearch(fsm_pst);
        }break;
        case rba_FeeFs1x_Searcher_findCopiesDo_stm_natCopyFound_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_findCopiesDo_natCopyFound(fsm_pst);
        }break;
        case rba_FeeFs1x_Searcher_findCopiesDo_stm_redCopyFound_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_findCopiesDo_redCopyFound(fsm_pst);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_Searcher_RetVal_Pending_e)
    {
        if(fsm_pst->copyRed_en == *fsm_pst->currCopy_pen)
        {
            if(rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e == retVal_en)
            {
                /* Native and redundant copies were found - update return to 2 copies found */
                retVal_en = rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e;
            }
            else if(rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e == retVal_en)
            {
                /* Only native copy was found - update return to one copy found */
                retVal_en = rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e;
            }
            else
            {
                /* do nothing */
            }
        }

        rba_FeeFs1x_Searcher_findCopiesDo_data_st.state_en = rba_FeeFs1x_Searcher_findCopiesDo_stm_idle_e;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * checks if a valid cache entry is found
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  Returns if a valid cache entry was found
 * \retval  rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e : no cache entry was found, therefore no block data available
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Ongoing - call again during next cycle
 * \return  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e: an error occured
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_isCacheValid(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst)
{
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
    Std_ReturnType redCacheRetVal_en = E_OK;
    boolean isCacheValid_b = FALSE;

    if(fsm_pst->copyRed_en == *fsm_pst->currCopy_pen)
    {
        /* Redundant copy - check for redundant cache validity */
        redCacheRetVal_en = rba_FeeFs1x_Searcher_Cache_isCacheValid_Red(fsm_pst->feeIdx_u16, &isCacheValid_b, fsm_pst->cacheType_en);
    }
    else
    {
        /* Native copy - check for native cache validity */
        isCacheValid_b = rba_FeeFs1x_Searcher_Cache_isCacheValid(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);
    }

    if((E_OK == redCacheRetVal_en) && isCacheValid_b)
    {
        if(rba_FeeFs1x_Searcher_findCopiesDo_mode_nextUnvalidatedCopy_e == fsm_pst->mode_en)
        {
            fsm_pst->pageNr_u32 = rba_FeeFs1x_Searcher_Cache_getLatestEntry(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_restoreRedBlockData_e);
        }
        else
        {
            /* Cache entry is valid - init the found copy */
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_initBlockCopy_e);
        }
    }
    else
    {
        if(fsm_pst->copyNat_en == *fsm_pst->currCopy_pen)
        {
            /* Native copy */
            /* Cache entry is invalid, so there is no block data available */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;
        }
        else
        {
            /* Redundant copy */
            if((E_OK == redCacheRetVal_en) || (!fsm_pst->isBlkDoubleStorage_b))
            {
                /* Start manual search of redundant copy because either */
                /* 1. Redundant cache entry is invalid */
                /* 2. Block in DFLASH (old configuration) is configured as redundant but in PFLASH (new configuration) is native */
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_manualSearch_e);
            }
            else
            {
                /* Error from cache, e.g. this copy is not redundant */
                retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
            }
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * initializes the block copy object and executes the block copy
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  Returns how many copies are found
 * \retval  rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e : one copy was found
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e : internal error, job aborted
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e : external error, job aborted
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_initBlockCopy(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst)
{
    uint32 pageNrFromCache_u32;
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValInitCpyDo_en = rba_FeeFs1x_Pending_e;
    Std_ReturnType redCacheRetVal_en = E_OK;

    if(FEE_PRV_LIBENTRY)
    {
        if(fsm_pst->copyRed_en == *fsm_pst->currCopy_pen)
        {
            /* Redundant copy - get latest cache entry */
            redCacheRetVal_en = rba_FeeFs1x_Searcher_Cache_getLatestEntry_Red(fsm_pst->feeIdx_u16, &pageNrFromCache_u32, fsm_pst->cacheType_en);
        }
        else
        {
            /* Native copy - get latest cache entry */
            pageNrFromCache_u32 = rba_FeeFs1x_Searcher_Cache_getLatestEntry(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);

            /* Remember the page of the native copy for manual search in backward direction in case */
            /* 1. Native copy will be inconsistent */
            /* 2. Native copy is consistent but manual search of redundant copy is necessary later */
            fsm_pst->pageNr_u32 = pageNrFromCache_u32;
        }

        if((E_OK == redCacheRetVal_en))
        {
            /* Set buffer direction and prefetch length */
            rba_FeeFs1x_Searcher_findCopy_setIteratorDirection(fsm_pst->cacheType_en , fsm_pst->feeIdx_u16);

            rba_FeeFs1x_BC_initCopy(*fsm_pst->currCopy_pen, fsm_pst->persistentId_u16, pageNrFromCache_u32);
        }
        else
        {
            /* Getting the cache entry of a previously valid cache element was rejected */
            /* If getting cache entry fails because of changed configuration */
            /* Going to manual search was done isCacheVaild state, so in this case we never enter here */
            /* Error from cache, e.g. this copy is not redundant */
            retValInitCpyDo_en = rba_FeeFs1x_ErrorInternal_e;
        }
    }

    if(rba_FeeFs1x_Pending_e == retValInitCpyDo_en)
    {
        /* Cyclic call the Do-function */
        retValInitCpyDo_en = rba_FeeFs1x_BC_initCopyDo();
    }

    switch(retValInitCpyDo_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            /* reenter function until execution is finished */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        case rba_FeeFs1x_JobOK_e:
        {
            if( (fsm_pst->copyRed_en == *fsm_pst->currCopy_pen) ||
                (rba_FeeFs1x_Searcher_findCopiesDo_mode_firstUnvalidatedCopy_e == fsm_pst->mode_en) ||
                (rba_FeeFs1x_Searcher_findCopiesDo_mode_nextUnvalidatedCopy_e == fsm_pst->mode_en))
            {
                // return the completion of the findRedundant job
                retVal_en = rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e;
            }
            else
            {
                /* data consistency has to be checked, switch state and stay pending */
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_checkDataConsistency_e);
                retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            }
            break;
        }
        case rba_FeeFs1x_JobFailed_e:
        {
            if(fsm_pst->copyRed_en == *fsm_pst->currCopy_pen)
            {
                // invalidate the red cache entry and go to manual search for searching next valid copy
                rba_FeeFs1x_Searcher_Cache_invalidateRedCache(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);

                // manual search for the redundant copy
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_manualSearch_e);
            }
            else
            {
                /* block header is not valid */
                /* invalidate the cache entry */
                rba_FeeFs1x_Searcher_Cache_invalidateCache(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);
                /* try to restore redundant block copy */
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_restoreRedBlockData_e);
            }

            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }

        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e;
            break;
        }

        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            /* something went wrong */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * checks the data consistency of a given block copy
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  Returns if block copy data is consistent
 * \retval  rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e : block copy data is consistent
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e : internal error, job aborted
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_checkDataConsistency(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst)
{
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValValDatDo_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_readVldtFls2Ram( *fsm_pst->currCopy_pen,
                                        0u,
                                        0u,
                                        NULL_PTR,
                                        FALSE);
    }

    /* Cyclic call the Do function */
    retValValDatDo_en = rba_FeeFs1x_BC_readVldtFls2RamDo();

    switch(retValValDatDo_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            /* keep state and stay pending */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        case rba_FeeFs1x_JobOK_e:
        {
            /* block data is consistent */
            /* set cache data to valid */
            rba_FeeFs1x_Searcher_Cache_setDataValid(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);

            /* Is search of redundant copy is required? */
            if(rba_FeeFs1x_Searcher_findCopiesDo_mode_twoCopies_e == fsm_pst->mode_en)
            {
                /* restart the job for redundant copy */
                fsm_pst->currCopy_pen = &fsm_pst->copyRed_en;
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_isCacheValid_e);
                retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            }
            else
            {
                /* valid block copy is found */
                retVal_en = rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e;
            }

            break;
        }
        case rba_FeeFs1x_JobFailed_e:
        {
            /* block data is inconsistent */
            rba_FeeFs1x_BlockJob_ReportBlkDataCrcError();
            /* invalidate the cache entry */
            rba_FeeFs1x_Searcher_Cache_invalidateCache(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);
            /* try to restore redundant block copy */
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_restoreRedBlockData_e);
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
            break;
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * if possible a redundant block copy is restored, intermediate state
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  always pending
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_restoreRedBlockData(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst)
{
    boolean isCacheValid_b;

    rba_FeeFs1x_Searcher_Cache_restoreRed(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);
    /* restore cache entry from redundant copy */
    /* check, if restauration of redundant data was successful */
    isCacheValid_b = rba_FeeFs1x_Searcher_Cache_isCacheValid(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);

    if(isCacheValid_b)
    {
        /* restauration of cache entry was succesfull */
        /* switch state to initBlockCopy */
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_initBlockCopy_e);
    }
    else
    {
        /* restauration of cache entry failed, start a manual search */
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_manualSearch_e);
    }

    return(rba_FeeFs1x_Searcher_RetVal_Pending_e);
}

/**
 *********************************************************************
 * perform a manual search starting at current read page
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  always pending if no error occured
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e : internal error, job aborted
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Call again during next cycle
 * \retval  rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e : manual search didn't find a block copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_manualSearch(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst)
{
    rba_FeeFs1x_Searcher_RetVal_ten searchManuallyDoRetVal_en;
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* initialize the manual search in backward direction */
        rba_FeeFs1x_Searcher_searchManually(fsm_pst->persistentId_u16, &(fsm_pst->pageNr_u32), TRUE);
    }
    /* execute the manual search */
    searchManuallyDoRetVal_en = rba_FeeFs1x_Searcher_searchManuallyDo();

    switch(searchManuallyDoRetVal_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_Pending_e:
        {
            /* pending, keep state */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        {
            if(fsm_pst->copyNat_en == *fsm_pst->currCopy_pen)
            {
                /* no copy was found, so invalidate eventually available native/redundant, known/unknown cache entries */
                rba_FeeFs1x_Searcher_Cache_invalidateNatAndRedCaches(fsm_pst->feeIdx_u16, fsm_pst->cacheType_en);
            }
            retVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;
            break;
        }
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        {
            if(fsm_pst->copyRed_en == *fsm_pst->currCopy_pen)
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_redCopyFound_e);
            }
            else
            {
                /* one copy was found -> cache update: reenter state initBlockCopy return pending */
                /* fsm_pst->pageNr now contains the manually found page */
                /* Update cache (native or redundant) with the new found copy */
                //switch state
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_natCopyFound_e);
            }
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        default:
        {
            /* something unexpected happened */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
            break;
        }
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * update the cache when searching a native copy, if a block copy was found
 *
 * \param   *followUpState_pen: follow up state
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  always pending if no error occured
 * \retval  rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e : one block copy data was found
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_natCopyFound(rba_FeeFs1x_Searcher_findCopiesDo_data_tst * fsm_pst)
{
    uint16 nrBytesBlkLenCfgd_u16;
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;
    boolean isIntCpyLenMatching_b;

    if(rba_FeeFs1x_Searcher_cacheTypeKwn_e == fsm_pst->cacheType_en)
    {
        nrBytesBlkLenCfgd_u16 = Fee_Prv_ConfigGetBlockLengthByBlockNr(fsm_pst->feeIdx_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
        isIntCpyLenMatching_b = rba_FeeFs1x_BC_extract_isInternalCpyLengthMatching(nrBytesBlkLenCfgd_u16);

    }
    else
    {
        isIntCpyLenMatching_b = TRUE;
    }

    if(isIntCpyLenMatching_b)
    {
        rba_FeeFs1x_Searcher_Cache_addNewerCopy(    fsm_pst->feeIdx_u16,
                                                    FALSE,
                                                    fsm_pst->pageNr_u32,
                                                    rba_FeeFs1x_BC_getCpyRedBit(rba_FeeFs1x_BC_Cpy_Internal_e),
                                                    fsm_pst->cacheType_en);

        /* Search was complete and one copy was found ->
         * Copy the contents from internal block copy to requested copy to be searched */
        rba_FeeFs1x_BC_copyObjectInternal(*fsm_pst->currCopy_pen);

        if( (rba_FeeFs1x_Searcher_findCopiesDo_mode_firstUnvalidatedCopy_e == fsm_pst->mode_en) ||
            (rba_FeeFs1x_Searcher_findCopiesDo_mode_nextUnvalidatedCopy_e == fsm_pst->mode_en))
        {
            retVal_en = rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e;
        }
        else
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_checkDataConsistency_e);
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
        }
    }
    else
    {
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_findCopiesDo_stm_manualSearch_e);
        retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * update the cache when searching a redundant copy, if a block copy was found
 *
 * \param   *followUpState_pen: follow up state
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  always pending if no error occured
 * \retval  rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e : one block copy data was found
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : Call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_findCopiesDo_redCopyFound(rba_FeeFs1x_Searcher_findCopiesDo_data_tst const * fsm_pst)
{
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;
    Std_ReturnType cacheRetVal_en;

    // update the redundant cache entry of the current copy
    cacheRetVal_en = rba_FeeFs1x_Searcher_Cache_updateRed(  fsm_pst->feeIdx_u16,
                                                            fsm_pst->pageNr_u32,
                                                            rba_FeeFs1x_BC_getCpyRedBit(rba_FeeFs1x_BC_Cpy_Internal_e),
                                                            fsm_pst->cacheType_en);

    // update the redundant cache entry of the current copy
    if((E_OK == cacheRetVal_en) || ((!fsm_pst->isBlkDoubleStorage_b) && (rba_FeeFs1x_Searcher_cacheTypeKwn_e == fsm_pst->cacheType_en)))
    {
        // cache update successful, Copy the contents from internal block copy to requested copy to be searched

        // cache update rejected
        // it can happen that the block in DFLASH (old configuration) is configured as redundant but in PFLASH (new configuration) is Native
        // for such case, there is no cache entry present for redunant copy, so skip the update of cache
        // but this has to checked only for known blocks
        rba_FeeFs1x_BC_copyObjectInternal(*fsm_pst->currCopy_pen);
        retVal_en = rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e;
    }
    else
    {
        // error from cache, e.g. this copy is not redundant
        retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * initialize a manual search for a block copy with given persistent Id
 * \param   persistentId_u16    : persistent Id for which a block copy shall be searched
 * \param   pageNr_pu32         : pointer to page variable containing the upper/lower search limit at start and
 *                                returned to if a copy is found
 * \param   isBackwardSearch_b  : = TRUE -> block has to be searched in backward direction, else in forward direction
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_Searcher_searchManually(uint16 persistentId_u16, uint32 *  pageNr_pu32, boolean isBackwardSearch_b)
{
    /* limit range of upperSearchLimitPage_u32 is checked in searchManuallyDo() */
    rba_FeeFs1x_Searcher_searchManually_st.persistentId_u16 = persistentId_u16;
    rba_FeeFs1x_Searcher_searchManually_st.isBackwardSearch_b = isBackwardSearch_b;

    /* Manual search starts in the state rba_FeeFs1x_Searcher_searchManuallyStm_checkCont_e.
     * In this state the page number is either incremented (when doing forward search) or decremented (when doing backward search)
     * before doing the first block search.
     * During forward search it is important that the page that is being passed is checked -> so its important that the
     * initial value is adapted to one page lesser. */
    if(!isBackwardSearch_b)
    {
        // For safety check for underflow.
        if(0u != *pageNr_pu32)
        {
            *pageNr_pu32 = *pageNr_pu32 - 1u;
        }
    }

    rba_FeeFs1x_Searcher_searchManually_st.pageNr_pu32 = pageNr_pu32;
    rba_FeeFs1x_Searcher_searchManually_st.currPageNr_u32 = *pageNr_pu32;
    rba_FeeFs1x_Searcher_searchManually_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;

    // The first state is checkCont --> implicitly checking the sector end surpassing
    rba_FeeFs1x_Searcher_searchManually_st.state_en = rba_FeeFs1x_Searcher_searchManuallyStm_checkCont_e;
    rba_FeeFs1x_Searcher_searchManually_st.entry_b = TRUE;

    /* switch the pagebuffering to backwards direction */
    rba_FeeFs1x_PARead_setIteratorPreferences(isBackwardSearch_b , RBA_FEEFS1X_PRV_MAX_BUFFER_SIZE_TO_BE_PREFETCHED);
}

/**
 *********************************************************************
 * perform a manual search for a block copy with given persistent Id
 * \param   void
 * \return  result from the manual search
 * \retval  rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e: all pages scanned for a block header without finding a block header of a given persistent Id
 * \retval  rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e: successful extraction and validation of a block header of a given persistent Id
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e: block header extraction ongoing
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e: an error occured
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e: an external error occured
 * \seealso
 * \usedresources
 *********************************************************************
 */
static rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo(void)
{
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_Searcher_searchManually_st.state_en)
    {
        case rba_FeeFs1x_Searcher_searchManuallyStm_analysePageContent_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_searchManuallyDo_analysePageContent(&rba_FeeFs1x_Searcher_searchManually_st);
            break;
        }
        case rba_FeeFs1x_Searcher_searchManuallyStm_checkPersId_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_searchManuallyDo_checkPersId(&rba_FeeFs1x_Searcher_searchManually_st);
            break;
        }
        case rba_FeeFs1x_Searcher_searchManuallyStm_validateHeader_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_searchManuallyDo_validateHeader(&rba_FeeFs1x_Searcher_searchManually_st);
            break;
        }
        case rba_FeeFs1x_Searcher_searchManuallyStm_checkCont_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_searchManuallyDo_checkCont(&rba_FeeFs1x_Searcher_searchManually_st);
            break;
        }
        case rba_FeeFs1x_Searcher_searchManuallyStm_error_e:
        {
            retVal_en = rba_FeeFs1x_Searcher_searchManuallyDo_error(&rba_FeeFs1x_Searcher_searchManually_st);
            break;
        }
        default:
        {
            rba_FeeFs1x_Searcher_searchManually_st.state_en = rba_FeeFs1x_Searcher_searchManuallyStm_idle_e;
            retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
            break;
        }
    }
    if(retVal_en != rba_FeeFs1x_Searcher_RetVal_Pending_e)
    {
        rba_FeeFs1x_Searcher_searchManually_st.state_en = rba_FeeFs1x_Searcher_searchManuallyStm_idle_e;
    }
    return  retVal_en;
}

/**
 *********************************************************************
 * extract block header from a given currPageNr_u32
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  returns the result of block header extraction
 * \retval rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e: no block copy is found
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e: block header extraction ongoing
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e: an error occured
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_analysePageContent(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst)
{
    rba_FeeFs1x_RetVal_ten          retValExtractHdr_en;
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;
    uint32                          ofsCurrPageInBuffer_u32;
    boolean                         isLoopExit_b, continueSearch_b;

    isLoopExit_b = FALSE;

    /* Loop multiple times over the Medium Buffer:
       As long as the extractHdrDo call directly exits with JobOk or JobFailed, the page already was buffered
       --> the next page can be scanned within the same cycle.
       Just checking isLoopExit_b would be sufficient, but to prevent any unintended endless loop an additional counter exit criterion is implemented */
    for(ofsCurrPageInBuffer_u32 = 0u;
            (ofsCurrPageInBuffer_u32 < (RBA_FEEFS1X_PRV_MAX_BUFFER_SIZE_TO_BE_PREFETCHED/RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE) ) && (!isLoopExit_b);
            ofsCurrPageInBuffer_u32++)
    {
        if(FEE_PRV_LIBENTRY)
        {
            rba_FeeFs1x_BC_extractHdr(fsm_pst->currPageNr_u32);
        }

        /* Cyclic call the Do fi=unction */
        retValExtractHdr_en = rba_FeeFs1x_BC_extractHdrDo();

        switch(retValExtractHdr_en)
        {
            case rba_FeeFs1x_Pending_e:
            {
                /* keep state pending */
                retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
                /* If rba_FeeFs1x_BC_extractHdrDo() returns pending, data is not available in the fast accessible medium buffer.
                   New data has to be fetched from DFlash to the medium buffer.
                   If new data is available in the medium buffer, reenter the loop. */
                isLoopExit_b = TRUE;
                break;
            }
            case rba_FeeFs1x_JobOK_e:
            {
                /* currPageNr_u32 contains a preamble */
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_checkPersId_e);
                /* the loop must be exited for state change */
                retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
                isLoopExit_b = TRUE;
                break;
            }
            case rba_FeeFs1x_JobFailed_e:
            {
                /* calculate the next page to be scanned */
                if(!fsm_pst->isBackwardSearch_b)
                {
                    continueSearch_b = rba_FeeFs1x_Searcher_checkNSelectNextPageForForwardSearch(&fsm_pst->currPageNr_u32);
                }
                else
                {
                    continueSearch_b = rba_FeeFs1x_Searcher_checkNSelectNextPageForBackwardSearch(&fsm_pst->currPageNr_u32);
                }

                if(continueSearch_b)
                {
                    /* continue the search with the next page */
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_analysePageContent_e);
                    retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
                    isLoopExit_b = FALSE;
                }
                else
                {
                    /* page 0 reached -> no copy found */
                    retVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;
                    isLoopExit_b = TRUE;
                }
                break;
            }
            case rba_FeeFs1x_ErrorExternal_e:
            {
                /* read error occured, go to the error handler to perform retries */
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_error_e);
                retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
                isLoopExit_b = TRUE;
                break;
            }
            case rba_FeeFs1x_ErrorInternal_e:
            default:
            {
                /* something unexpected occured -> change state to idle and return an error */
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_idle_e);
                retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
                isLoopExit_b = TRUE;
                break;
            }
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * state checking the persistent Id and performing corresponding state switch
 *
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  job result
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : ongoing, call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_checkPersId(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst)
{
    uint16 persistentId_u16;

    /* get the persistent Id of the block */
    persistentId_u16 = rba_FeeFs1x_BC_getPersID(rba_FeeFs1x_BC_Cpy_Internal_e);

    /* check, if the persistent Id of the found block copy matches */
    if((persistentId_u16 == fsm_pst->persistentId_u16) || (RBA_FEEFS1X_MAXUINT16 == fsm_pst->persistentId_u16))
    {
        /* either 1. persistent Id matches -> validate the header or 2. any valid block should be returned */
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_validateHeader_e);
    }
    else
    {
        /* wrong persistent Id -> check continue */
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_checkCont_e);
    }

    return rba_FeeFs1x_Searcher_RetVal_Pending_e;
}

/**
 *********************************************************************
 * validate an extracted block header
 * \param   *fsm_pst: pointer to statemachine struct
 * \return  rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e: successful validation of a block header
 * \return  rba_FeeFs1x_Searcher_RetVal_Pending_e: block header extraction ongoing
 * \return  rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e: an error occured
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_validateHeader(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_BC_validateHdr_en;
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_validateHdr(FALSE);
    }

    /* Cyclic call the Do function */
    retVal_BC_validateHdr_en = rba_FeeFs1x_BC_validateHdrDo();

    switch(retVal_BC_validateHdr_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            /* keep state and stay pending */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        case rba_FeeFs1x_JobOK_e:
        {
            /* job finished successfully -> change state to idle, return one copy found */

            // return the found page to the job caller
            *(fsm_pst->pageNr_pu32) = fsm_pst->currPageNr_u32;

            retVal_en = rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e;
            break;
        }
        case rba_FeeFs1x_JobFailed_e:
        {
            /* block header is not valid -> check, if search in the previous page is possible, stay pending */
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_checkCont_e);
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        case rba_FeeFs1x_ErrorExternal_e:
        {
            /* read error occured, go to the error handler to perform retries */
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_error_e);
            retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
            break;
        }
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            /* something unexpected happened -> change state to idle and return an error */
            retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
            break;
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_searchManuallyDo_checkCont
 * statefunction of searchManuallyDo
 * \param   fsm_pst : statemachine data pointer
 * \return  job result
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e : all pages are checked
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_checkCont(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst)
{
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;
    boolean                         continueSearch_b;

    if(!fsm_pst->isBackwardSearch_b)
    {
        continueSearch_b = rba_FeeFs1x_Searcher_checkNSelectNextPageForForwardSearch(&fsm_pst->currPageNr_u32);
    }
    else
    {
        continueSearch_b = rba_FeeFs1x_Searcher_checkNSelectNextPageForBackwardSearch(&fsm_pst->currPageNr_u32);
    }

    if(continueSearch_b)
    {
        /* continue the search with the next page */
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_analysePageContent_e);
        retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
    }
    else
    {
        /* end of search reached -> no copy found */
        retVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_searchManuallyDo_error
 * statefunction of searchManuallyDo to handle errors during search.
 *
 * \param   fsm_pst                                     : statemachine data pointer
 *
 * \return  job result
 * \retval  rba_FeeFs1x_Searcher_RetVal_Pending_e       : search retried, call again during next cycle
 * \retval  rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e : search aborted due to external errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_Searcher_searchManuallyDo_error(rba_FeeFs1x_Searcher_searchManually_tst *fsm_pst)
{
    rba_FeeFs1x_Searcher_RetVal_ten retVal_en;

    // check whether a retry is allowed
    if(fsm_pst->allowedRetries_u8 > 0u)
    {
        fsm_pst->allowedRetries_u8--;
        retVal_en = rba_FeeFs1x_Searcher_RetVal_Pending_e;
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_searchManuallyStm_analysePageContent_e);
    }
    else
    {
        // retry maximum reached, exit the job with an error
        retVal_en = rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCache
 * job init function for unknown block cache buildup
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_buildUpUnknownBlkCache(void)
{

    if (rba_FeeFs1x_Prv_isReorgUnknown() || rba_FeeFs1x_Prv_isSurvivalActive())
    {
        rba_FeeFs1x_PARead_setIteratorPreferences(FALSE, RBA_FEEFS1X_PRV_MAX_BUFFER_SIZE_TO_BE_PREFETCHED);

        // allow the unknown cache buildup
        rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.currPage_u32 = rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32;
        // a sector end is no possible value of the page limit:
        // Either less unknown (to be considered) blocks than the cache size get found --> disable further cache buildups
        // Or not all found unknown copies did fit into the cache
        // -> point to the (extracted and thus not sector overlapping) page

        rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;
        rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.state_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_extractHdr_e;
        rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.entry_b = TRUE;

        rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.isCacheFull_b = FALSE;
    }
    else
    {
        // Don't allow the unknown cache buildup
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo
 * job do function of the unknown block cache buildup
 * The job starts at the last detected not cachable block during the last execution
 * The job exits after scanning the Flash up to the write page and has updated the cache entries to the newest copy available
 * As soon as a block was found which didn't fit into the cache, the currPageNr_u32 of this block is remembered for the next scan
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e : job finished successfully, one cycle of cache buildup is done
 * \retval  rba_FeeFs1x_Pending_e : job needs more cycles to complete
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : read error by Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(rba_FeeFs1x_Prv_isReorgUnknown() || rba_FeeFs1x_Prv_isSurvivalActive())
    {
        switch(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.state_en)
        {
            case rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_extractHdr_e   :
            {
                retVal_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_extractHdr(&rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st);
            }break;
            case rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_isCachingReq_e :
            {
                retVal_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq(&rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st);
            }break;
            case rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_valHdr_e       :
            {
                retVal_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_valHdr(&rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st);
            }break;
            case rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_addCache_e     :
            {
                retVal_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_addCache(&rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st);
            }break;
            case rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_checkCont_e    :
            {
                retVal_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_checkCont(&rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st);
            }break;
            case rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_exit_e    :
            {
                retVal_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_exit(&rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st);
            }break;
            default:
            {
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }break;
        }

        if(retVal_en != rba_FeeFs1x_Pending_e)
        {
            /* On errors, throw away any partially built up unknown cache. */
            if(rba_FeeFs1x_JobOK_e != retVal_en)
            {
                rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache();
            }

            rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.state_en = rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_idle_e;
        }
    }
    else
    {
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }

    return  retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_extractHdr
 * statefunction of buildUpUnknownBlkCacheDo
 * extracts the header out of the current currPageNr_u32
 * \param   fsm_pst : pointer to the statemachine data
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_JobOK_e : job finished successfully, one cycle of cache buildup is done
 * \retval  rba_FeeFs1x_Pending_e : job needs more cycles to complete
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : read error by Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_extractHdr(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValExtractHdr_en;
    uint32 ofsCurrPageInBuffer_u32;
    boolean isLoopExit_b;


    isLoopExit_b = FALSE;

    // Loop multiple times over the Medium Buffer:
    // As long as the extractHdrDo call directly exits with JobOk or JobFailed, the page already was buffered
    // --> the next page can be scanned within the same cycle
    for(ofsCurrPageInBuffer_u32 = 0u;
            (ofsCurrPageInBuffer_u32 < (RBA_FEEFS1X_PRV_MAX_BUFFER_SIZE_TO_BE_PREFETCHED/RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE) ) && (!isLoopExit_b) ;
            ofsCurrPageInBuffer_u32++)
    {
        if(FEE_PRV_LIBENTRY)
        {
            // on state entry, init the extractHdr to extract the current loop page
             rba_FeeFs1x_BC_extractHdr(fsm_pst->currPage_u32);
        }

        // Cyclic call the Do function
        retValExtractHdr_en = rba_FeeFs1x_BC_extractHdrDo();

        switch(retValExtractHdr_en)
        {
            case rba_FeeFs1x_JobOK_e:
            {
                // block header detected (preamble check)
                // check the necessity of adding this copy to the unknown block cache
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_isCachingReq_e);
                retVal_en = rba_FeeFs1x_Pending_e;
                //leave the extraction loop
                isLoopExit_b = TRUE;
            }break;
            case rba_FeeFs1x_JobFailed_e:
            {
                // Page doesn't contain a header(preamble check)
                // -> check whether another page should be scanned

                if(rba_FeeFs1x_Searcher_checkNSelectNextPageForForwardSearch(&fsm_pst->currPage_u32))
                {
                    // continue the extraction with the next page (updated by checkNSelectNextPage)
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_extractHdr_e);
                }
                else
                {
                    // no further continuing of the cache buildup required -> exit the cache buildup
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_exit_e);
                    isLoopExit_b = TRUE;
                }
                // continue the operation
                retVal_en = rba_FeeFs1x_Pending_e;

                // stay in loop
            }   break;
            case rba_FeeFs1x_ErrorExternal_e:
            {
                // read error during validation
                // try to retry (limited by retry counter over whole cache buildup)
                if(fsm_pst->allowedRetries_u8 > 0)
                {
                    FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_extractHdr_e);
                    fsm_pst->allowedRetries_u8--;
                    retVal_en = rba_FeeFs1x_Pending_e;
                }
                else
                {
                    //return the errorExternal
                    retVal_en = rba_FeeFs1x_ErrorExternal_e;
                }

                // repeat extract in next cycle or exit the statemachine
                isLoopExit_b = TRUE;

            }break;
            case rba_FeeFs1x_Pending_e:
            case rba_FeeFs1x_ErrorInternal_e:
            {
                retVal_en = retValExtractHdr_en;
                //leave the extraction loop
                isLoopExit_b = TRUE;
            }break;
            default:
            {
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
                //leave the extraction loop
                isLoopExit_b = TRUE;
            }break;
        }
    }

    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq
 * statefunction of buildUpUnknownBlkCacheDo
 * checks whether a found copy should be considered in this cache buildup
 * \param   fsm_pst : pointer to the statemachine data
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_JobOK_e : job finished successfully, one cycle of cache buildup is done
 * \retval  rba_FeeFs1x_Pending_e : job needs more cycles to complete
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : read error by Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint16 persIDFoundBlock_u16;
    uint16 feeIndexFoundBlock_u16;

    persIDFoundBlock_u16 = rba_FeeFs1x_BC_getPersID(rba_FeeFs1x_BC_Cpy_Internal_e);

    feeIndexFoundBlock_u16 = Fee_Prv_ConfigGetBlockNrByPersistentId(persIDFoundBlock_u16, RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);

    if( feeIndexFoundBlock_u16 == 0xFFFFuL)
    {
        boolean isToBeConsidered_b;

        // If the general unknown reorg is active, take all unknown blocks
        // if only survival is active, take only the unknown block with survival bit active
        isToBeConsidered_b = rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_check2BeConsidered();

        //store the persistent ID for following steps
        fsm_pst->persIDCurrBlock_u16 = persIDFoundBlock_u16;

        // store whether the block is cached already
        fsm_pst->isCachedAlready_b = rba_FeeFs1x_CacheUnkwn_isBlockCached(persIDFoundBlock_u16);

        if
        (
            rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_isCpyToBeCached
            (
                isToBeConsidered_b ,
                fsm_pst->isCacheFull_b ,
                fsm_pst->isCachedAlready_b
            )
        )
        {
            // block copy shall be validated based on the config and cache state
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_valHdr_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            // block is not supposed to be considered --> switch to next page
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_checkCont_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
    }
    else
    {
        // known block --> ignore and scan the next page
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_checkCont_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_check2BeConsidered
 * subfunction of isCachingReq state
 * checks the copy against the configured reorg level and survival status
 * \return  boolean
 * \retval  TRUE  : Copy to be considered
 * \retval  FALSE : Ignore this copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_check2BeConsidered(void)
{
    boolean isToBeConsidered_b;
    boolean isSurvival_b;

    // if only survival blocks shall be reorged, only consider the block as to be cached if it is marked as survival
    if (!rba_FeeFs1x_Prv_isReorgUnknown() && rba_FeeFs1x_Prv_isSurvivalActive())
    {
        // if only survival blocks are supposed to be reorged,
        // check the survival status of the found unknown block
        isSurvival_b = rba_FeeFs1x_BC_getCpySurvivalBit(rba_FeeFs1x_BC_Cpy_Internal_e);

        if (isSurvival_b)
        {
            // consider
            isToBeConsidered_b = TRUE;
        }
        else
        {
            // ignore this copy
            isToBeConsidered_b = FALSE;
        }
    }
    else
    {
        // all unknown copies are supposed to be reorged
        //--> unknown AND survival or unknown WITHOUT survival.
        // (Both deactivated is catched in the job init function)
        isToBeConsidered_b = TRUE;
    }

    return isToBeConsidered_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_isCpyToBeCached
 * subfunction of isCachingReq state
 * checks the copie's consideration and cache status
 * \param   isToBeConsidered_b :consideration status. TRUE: consider, FALSE: ignore this copy
 * \param   isCacheFull_b      : cache is full, only consider already added copies
 * \param   isBlockCached_b    : block is already present in the cache
 * \return  should the copy be considered for caching?
 * \retval  TRUE :   validate the copy Header for caching
 * \retval  FALSE :  overjump the currPageNr_u32
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_isCachingReq_isCpyToBeCached(boolean isToBeConsidered_b , boolean isCacheFull_b , boolean isBlockCached_b)
{
    boolean retVal_b;
    // if the cache is full, only consider already cached blocks
    if(isToBeConsidered_b)
    {
        // copy is to be considered based on the selected unknown reorg level

        if(isCacheFull_b)
        {
            // cache is full, only add if it is an already cached copy
            if(isBlockCached_b)
            {
                // validate this copy
                retVal_b = TRUE;
            }
            else
            {
                // block can't be put into cache, continue with next page
                retVal_b = FALSE;
            }
        }
        else
        {
            // cache is not full yet, add any copy found (which is supposed to be considered)
            retVal_b = TRUE;
        }
    }
    else
    {
        // block is not supposed to be considered (unknown non-survival if survival is active)
        retVal_b = FALSE;
    }


    return retVal_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_valHdr
 * statefunction of buildUpUnknownBlkCacheDo
 * validates the copy header
 * \param   fsm_pst : pointer to the statemachine data
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_JobOK_e : job finished successfully, one cycle of cache buildup is done
 * \retval  rba_FeeFs1x_Pending_e : job needs more cycles to complete
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : read error by Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_valHdr(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValInner_en;
    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_validateHdr(FALSE);
    }
    retValInner_en = rba_FeeFs1x_BC_validateHdrDo();

    switch(retValInner_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // block header is valid
            // add the copy to the cache
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_addCache_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_JobFailed_e:
        {
            // header validation failed, select next currPageNr_u32 for scanning
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_checkCont_e);
            // continue the operation
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // read error during validation
            // try to retry the same currPageNr_u32(limited by retry counter over whole cache buildup)
            if(fsm_pst->allowedRetries_u8 > 0)
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_extractHdr_e);
                fsm_pst->allowedRetries_u8--;
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                //return the errorExternal
                retVal_en = rba_FeeFs1x_ErrorExternal_e;
            }
        }break;
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
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
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_valHdr
 * statefunction of buildUpUnknownBlkCacheDo
 * adds valid copies to cache
 * \param   fsm_pst : pointer to the statemachine data
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_JobOK_e : job finished successfully, one cycle of cache buildup is done
 * \retval  rba_FeeFs1x_Pending_e : job needs more cycles to complete
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : read error by Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_addCache    (rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst)
{
    boolean                 hasRedBitActive_b;
    rba_FeeFs1x_RetVal_ten  retVal_en;
    Std_ReturnType          retValAddCopy_en;

    hasRedBitActive_b = rba_FeeFs1x_BC_getCpyRedBit(rba_FeeFs1x_BC_Cpy_Internal_e);

    // If the status bit extraction worked, try to add the copy to the cache
    retValAddCopy_en = rba_FeeFs1x_CacheUnkwn_addNewerCopyByPersID( fsm_pst->persIDCurrBlock_u16, FALSE, fsm_pst->currPage_u32 , hasRedBitActive_b);
    if(E_OK == retValAddCopy_en)
    {
        // cache update successful, continue with the next currPageNr_u32
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_checkCont_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // unknown block cache rejected the adding --> the cache is full.
        // remember this page number for the next unknown block reorg
        if(!fsm_pst->isCacheFull_b)
        {
            rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32 = fsm_pst->currPage_u32;
            fsm_pst->isCacheFull_b = TRUE;
            FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_checkCont_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            // Error, the isCacheFull only should be set once per unknown cache buildup cycle and getting reset in init
            // state isCachingReq should have catched copies not allowed to be updated in cacheFull situations
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }
    }

    return retVal_en;

}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_checkCont
 * statefunction of buildUpUnknownBlkCacheDo
 * current currPageNr_u32 is completed, is the end of the cache buildup reached?
 * \param   fsm_pst : pointer to the statemachine data
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_JobOK_e : job finished successfully, one cycle of cache buildup is done
 * \retval  rba_FeeFs1x_Pending_e : job needs more cycles to complete
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e : read error by Fls
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_checkCont(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if( rba_FeeFs1x_Searcher_checkNSelectNextPageForForwardSearch(&fsm_pst->currPage_u32))
    {
        // cache buildup needs to be continued
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_extractHdr_e);
        retVal_en = rba_FeeFs1x_Pending_e;

    }
    else
    {
        // cache buildup completed
        FEE_PRV_LIBSC(rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_stm_exit_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_exit
 * statefunction of buildUpUnknownBlkCacheDo
 * cache buildup finished successfully, checks whether further cache buildups are required and exit the statemachine
 * \param   fsm_pst : pointer to the statemachine data
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_JobOK_e : job finished successfully, one cycle of cache buildup is done
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Searcher_buildUpUnknownBlkCacheDo_exit   (rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_tst const * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // cache buildup is completed

    if(!(fsm_pst->isCacheFull_b) && (0u == rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32))
    {
        // cache never got full during a cache buildup executed over the whole DFLASH
        // --> all unknown copies to be considered did fit
        // --> don't buildup the cache again

        // The value stays at true if any uncachable copy was found
        // In this situation, the pagenumber is remembered to be returned
        rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b = FALSE;
    }

    retVal_en = rba_FeeFs1x_JobOK_e;

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_getUnknownBlkReorgLimit
 * returns the page number the last unknown cache buildup stopped at due to full cache
 * \return  page number the first not cachable unknown block was found at (inited with 0)
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_Searcher_getUnknownBlkReorgLimit(void)
{
    return  rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_checkUnknownBlkCacheFullAndSetReorgLimit
 * Sets the page number where the last unknown cache buildup got full.
 * Once the cache is already marked as full, the limit should not be changed to avoid the old blocks which could not be
 * updated due to cache being full.
 *
 * \param  page number the first not cachable unknown block was found at
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_checkUnknownBlkCacheFullAndSetReorgLimit(uint32 pageNr_u32)
{
    if(!rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.isCacheFull_b)
    {
        rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32 = pageNr_u32;
        rba_FeeFs1x_Searcher_buildUpUnknownBlkCache_data_st.isCacheFull_b = TRUE;
    }
    rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_isUnknownBlkReorgRequired
 * \return  returns whether an unknown cache buildup needs to be executed
 * \retval  TRUE : unknown block cache buildup needs to be executed
 * \retval  FALSE : unknown block cache buildup  doesn't need to be executed
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_Searcher_isUnknownBlkCacheBuildupRequired(void)
{
    return rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_isUnknownBlkCacheBuilt
 * \return  returns whether an unknown cache is built or not
 * \retval  TRUE  : unknown block cache has been built
 * \retval  FALSE : unknown block cache has not yet built
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_Searcher_isUnknownBlkCacheBuilt(void)
{
    /* There are two scenarios to consider:
     * 1. When unknown cache was built up and there was at least one unknown block in dflash
     *      -> this scenario is covered when there is at least one entry in unknown block cache (stored in element idxNextFreeCachePos_u16 of CacheUnkwn unit)
     *         or when the page limit is till which unknown cache is built up is non zero (rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32).
     *      To keep it simple, the variable rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32 is used instead of using CacheUnkwn interface,
     * 2. When unknown cache was built up and there was no unknown copies in the memory
     *      -> in this scenario, both page limit and cache entries are 0 but the flag rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b
     *         is set to FALSE -> indicating that the cache has been built up */
    boolean retVal_b = FALSE;

    if(0u != rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32)
    {
        retVal_b = TRUE;
    }
    else
    {
        if(!rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b)
        {
            retVal_b = TRUE;
        }
    }

    return(retVal_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_resetSearchForUnknownCacheBuildup
 * unknown cache build up has to be executed, search for unknown blocks is restarted from page 0
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Searcher_resetSearchForUnknownCacheBuildup(void)
{
    rba_FeeFs1x_Searcher_pageLimitUnknownCache_u32 = 0u;
    rba_FeeFs1x_Searcher_areFurtherUnknownCacheBuildupsRequired_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Searcher_findCopy_setIteratorDirection
 * Sets the PARead iterator direction for findCopies for known copies to the expected block copy size
 * and for unknown copies to a constant value
 * \retval TRUE    Survival reorg is activated
 * \retval FALSE   Survival reorg is disabled
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Searcher_findCopy_setIteratorDirection(rba_FeeFs1x_Searcher_cacheTypes_ten CacheType_en , uint16 idxBlk_u16)
{
    uint32 nrBytesPrefetch_u32;
    if(rba_FeeFs1x_Searcher_cacheTypeKwn_e == CacheType_en)
    {
        uint32 nrBytesBlkData_u32;
        // set the prefetch length to the block copie's size including header rounded up to a pagesize
        nrBytesBlkData_u32 = Fee_Prv_ConfigGetBlockLengthByBlockNr(idxBlk_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);

        nrBytesPrefetch_u32 = Fee_Prv_LibCeil((nrBytesBlkData_u32 + RBA_FEEFS1X_PRV_BLK_HDR_SIZE), RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);
    }
    else
    {
        // set the prefetch length to a value many small blocks fit in
        nrBytesPrefetch_u32 = RBA_FEEFS1X_SEARCHER_NR_BYTES_PREFETCH_UNKWN_BLK;
    }

    rba_FeeFs1x_PARead_setIteratorPreferences(FALSE, nrBytesPrefetch_u32);
}


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

