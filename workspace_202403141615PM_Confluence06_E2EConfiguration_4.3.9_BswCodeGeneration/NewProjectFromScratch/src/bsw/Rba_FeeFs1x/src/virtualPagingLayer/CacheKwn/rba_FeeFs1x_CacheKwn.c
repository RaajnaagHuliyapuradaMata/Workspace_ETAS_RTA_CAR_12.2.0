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
#include "rba_FeeFs1x_Prv_CacheKwn.h"
#include "rba_FeeFs1x_Prv_CacheKwnTypes.h"
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "Fee_Prv_Config.h"

# if ((RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR * RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE) > 0xFFFFFuL)
#error "Nr of logical pages in the memory is more then 20bit position reserved for entry in cache. Either increase the number of bit positions in cache per entry or reduce the emulation memory area. Same applies for unknown blcok cache as well. "
# endif

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */


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


RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_CacheKwn_CacheElement_tst rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS])

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
static rba_FeeFs1x_CacheKwn_CacheElement_tst rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS];
# endif

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages_natCache(uint32 amountOfPages_u32);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_overstampNative(uint16 feeIndex_u16);
LOCAL_INLINE uint32 rba_FeeFs1x_CacheKwn_invalidateNatCacheEntriesFromPageNrAndRetHighestPageNr(uint32 page_u32);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateNatCacheEntryByPageNr(uint32 page_u32);
static void rba_FeeFs1x_CacheKwn_setEntry(rba_FeeFs1x_CacheKwn_CacheElement_tst * entry_pst, boolean dataValidated_b, uint32 pageNr_u32 , boolean hasRedBitActive_b);
static void rba_FeeFs1x_CacheKwn_overstampEntry(rba_FeeFs1x_CacheKwn_CacheElement_tst * entry_pst);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateCompleteNatCache(void);
LOCAL_INLINE uint32 rba_FeeFs1x_CacheKwn_invalidateRedCacheEntriesFromPageNrAndRetHighestPageNr(uint32 page_u32);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateRedCacheEntryByPageNr(uint32 page_u32);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache(uint32 amountOfPages_u32);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_addNewerCopy_pushToRedCache(uint16 feeIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_overstampRed(uint16 redIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_copyRedCacheToNatCache(uint16 idxNatCache_u16, uint16 idxRedCache_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateCompleteRedCache(void);
# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache_handleUnderflow(uint16 redIndexUnderflowing_u16);
static Std_ReturnType rba_FeeFs1x_CacheKwn_getRedCacheIdx(uint16 * redFeeIndex_pu16, uint16 feeIndex_u16);
# else
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_getRedCacheIdx(uint16 * redFeeIndex_pu16, uint16 feeIndex_u16);

/* Dummy functions when there are no redundant blocks present in the software */
LOCAL_INLINE uint32 rba_FeeFs1x_CacheKwn_invalidateRedCacheEntriesFromPageNrAndRetHighestPageNr(uint32 page_u32)
{
    (void) page_u32;
    return 0u;
}

LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateRedCacheEntryByPageNr(uint32 page_u32)
{
    (void) page_u32;
    return; /* Do nothing */
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache(uint32 amountOfPages_u32)
{
    (void) amountOfPages_u32;
    return E_OK; /* Avoid error during sector reorganisation */
}

LOCAL_INLINE void rba_FeeFs1x_CacheKwn_addNewerCopy_pushToRedCache(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return; /* Do nothing */
}

LOCAL_INLINE void rba_FeeFs1x_CacheKwn_overstampRed(uint16 redIndex_u16)
{
    (void) redIndex_u16;
    return; /* Do nothing */
}

LOCAL_INLINE void rba_FeeFs1x_CacheKwn_copyRedCacheToNatCache(uint16 idxNatCache_u16, uint16 idxRedCache_u16)
{
    (void) idxNatCache_u16;
    (void) idxRedCache_u16;
    return; /* Do nothing */
}

LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateCompleteRedCache(void)
{
    return; /* Do nothing */
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_getRedCacheIdx(uint16 * redFeeIndex_pu16, uint16 feeIndex_u16)
{
    *redFeeIndex_pu16 = 0u; /* Modify this pointer parameter to avoid violation of MISRA rule 8.13 (QAC_3673) */
    (void) feeIndex_u16;
    return E_NOT_OK; /* There are no redudant blocks configured */
}

# endif

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */


/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_init
 * initializes the static variables of the unit to the init values
 *********************************************************************
 */
void rba_FeeFs1x_CacheKwn_init(void)
{
    rba_FeeFs1x_CacheKwn_invalidateCompleteNatCache();
    rba_FeeFs1x_CacheKwn_invalidateCompleteRedCache();
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateCompleteNatCache
 * invalidates the complete Nat cache
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateCompleteNatCache(void)
{
    uint16 i_u16;

    for(i_u16 = 0; i_u16< RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS ; i_u16++)
    {
        rba_FeeFs1x_CacheKwn_overstampNative(i_u16);
    }
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateCompleteRedCache
 * invalidates the complete Red cache
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_invalidateCompleteRedCache(void)
{
    uint16 i_u16;

    for(i_u16 = 0; i_u16< RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS ; i_u16++)
    {
        rba_FeeFs1x_CacheKwn_overstampRed(i_u16);
    }
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_addNewerCopy
 * This function updates the cache for the known block with new page.
 * In case the block is a redundant copy, the function first moves the current
 * cached native copy to redundant cache and changes the native cache to the given parameters.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16      : index to be updated
 * \param   dataValidated_b   : data valid bit of the new copy
 * \param   pageNumber_u32    : pageNr of the new copy
 * \param   hasRedBitActive_b : redundancy statusbit of the new copy
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheKwn_addNewerCopy(uint16 feeIndex_u16, boolean dataValidated_b, uint32 pageNumber_u32 , boolean hasRedBitActive_b)
{
    /* If a newer copy gets added for a redundant block, the current latest consistent copy
     * gets degraded to the pre-latest one
     * --> move the current native cache entry to the redundant cache entry of that block
     */
    rba_FeeFs1x_CacheKwn_addNewerCopy_pushToRedCache(feeIndex_u16);

    rba_FeeFs1x_CacheKwn_setEntry(  &rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16],
                                    dataValidated_b,
                                    pageNumber_u32,
                                    hasRedBitActive_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_setEntry
 * This function updates the cache entry for a given known block with new page.
 * \attention: Function should be called only with valid parameter value!
 * \param   entry_pst       : pointer to cache element
 * \param   dataValidated_b : data valid bit of the new copy
 * \param   pageNumber_u32  : pageNr of the new copy
 * \param   hasRedBitActive_b : redundancy statusbit of the new copy
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_CacheKwn_setEntry(rba_FeeFs1x_CacheKwn_CacheElement_tst * entry_pst, boolean dataValidated_b, uint32 pageNr_u32 , boolean hasRedBitActive_b)
{
    entry_pst->cacheValid_u1 = 1u;
    entry_pst->copyRedundantConfig_u1 = hasRedBitActive_b ? (1u) : (0u);
    entry_pst->dataValid_u1 = dataValidated_b ? (1u) : (0u);
    entry_pst->pageNo_uo = pageNr_u32;
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_addNewerCopy_pushToRedCache
 * pushes the native cache entry to the redundant cache.
 * If the copy is native and has no redundant cache, the push also is successful.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index of the copy to be pushed
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_addNewerCopy_pushToRedCache(uint16 feeIndex_u16)
{
    uint16 redFeeIndex_u16 = 0u;

    if(E_OK == rba_FeeFs1x_CacheKwn_getRedCacheIdx(&redFeeIndex_u16,feeIndex_u16))
    {
        if((rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redFeeIndex_u16].pageNo_uo >= rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16].pageNo_uo) &&
           (rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redFeeIndex_u16].cacheValid_u1 > 0u) &&
           (rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16].cacheValid_u1 > 0u))
        {
            // Software should never reach here. As a robustness measure, it is better to invalidate the cache entry -> which will trigger manual search of the redundant copy later
            rba_FeeFs1x_CacheKwn_overstampRed(redFeeIndex_u16);
        }
        else
        {
            // for all elements of the cache entry, copy the native contents to the redundant cache
            rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redFeeIndex_u16] =
                    rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16];
        }
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_getRedCacheIdx
 * This function checks if a given block is configured redundand and
 * calculates the index of redundant block.
 * \attention: Function should be called only with valid parameter value!
 * \param   redFeeIndex_pu16: pointer to redundant block index
 * \param   feeIndex_u16 : index of the block to be checked
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
static Std_ReturnType rba_FeeFs1x_CacheKwn_getRedCacheIdx(uint16 * redFeeIndex_pu16, uint16 feeIndex_u16)
{
    Std_ReturnType retVal_en = E_NOT_OK;
    uint16_least   idxLowerBoundary_qu16       = 0u;
    uint16_least   idxUpperBloundary_qu16      = RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS;
    uint16_least   idxCurrentCheck_qu16        = 0u;
    boolean        blockFound_b = FALSE;

    if(Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr(feeIndex_u16, RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast))
    {
        /*
         * The rba_FeeFs1x_Prv_Cfg_RedBlks_acu16 array is stored ascending by Fee Block Index.
         * This function uses that fact and does a binary search.
         */
        /* Evaluation of while loop: The while loop will be exited after a maximum of RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS/2 calls, which is a constant value.
         * Hence, the while loop will not be stuck infinetly. */
        while(idxLowerBoundary_qu16 != idxUpperBloundary_qu16)
        {
            idxCurrentCheck_qu16 = idxLowerBoundary_qu16 + ((idxUpperBloundary_qu16 - idxLowerBoundary_qu16) / 2u);
            if(rba_FeeFs1x_Prv_Cfg_RedBlks_acu16[idxCurrentCheck_qu16] < feeIndex_u16)
            {
                if(idxCurrentCheck_qu16 == idxLowerBoundary_qu16)
                {
                    /*
                     * There are gaps allowed between the native indexes configured as redundant blocks.
                     * As soon as a native Fee index is not present in the layout,
                     * upper and lower boundary are narrowing down the position the index is missing at.
                     * As soon as the lower is pointing to the position one below and the upper at the following index,
                     * the search has to be aborted and the index is detected as not being in this list
                     */
                    idxUpperBloundary_qu16 = idxLowerBoundary_qu16;
                }
                else
                {
                    idxLowerBoundary_qu16 = idxCurrentCheck_qu16;
                }
            }
            else if(rba_FeeFs1x_Prv_Cfg_RedBlks_acu16[idxCurrentCheck_qu16] > feeIndex_u16)
            {
                idxUpperBloundary_qu16 = idxCurrentCheck_qu16;
            }
            else
            {
                // return the found redundant index
                *redFeeIndex_pu16  = (uint16)idxCurrentCheck_qu16;
                blockFound_b = TRUE;

                // fulfill the loop exit criterion
                idxUpperBloundary_qu16 = idxLowerBoundary_qu16;
            }
        }

        if(blockFound_b)
        {
            retVal_en = E_OK;
        }
        else
        {
            /* Block is not redundant block */
            retVal_en = E_NOT_OK;
        }
    }

    return retVal_en;
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_getLatestEntry
 * returns the page of the latest cached copy
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index of the copy to be pushed
 * \return  uint32:       Latest entry
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_CacheKwn_getLatestEntry(uint16 feeIndex_u16)
{
    // cache entry is valid
    return (rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16].pageNo_uo);
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_getLatestEntry_Red
 * returns the page of the redundant cached copy if a request for a redundantly stored block was given
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index of the copy to be pushed
 * \param   pageOfLatest_pu32 : reference to variable the return value should be stored in if no errors occured
 * \return  returns whether the operation was successful or had an internal error
 * \retval  E_OK : finished successfully, pageOfLatest content is updated
 * \retval  E_NOT_OK : aborted due to out of bounds request, config mismatch or non-valid cache entry
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheKwn_getLatestEntry_Red(uint16 feeIndex_u16, uint32* const pageOfLatest_pu32)
{
    uint16 redFeeIndex_u16 = 0u;
    Std_ReturnType retVal_en = rba_FeeFs1x_CacheKwn_getRedCacheIdx(&redFeeIndex_u16,feeIndex_u16);

    // Is the requested block configured as redundant?
    if(E_OK == retVal_en)
    {
        if(rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redFeeIndex_u16].cacheValid_u1 > 0u)
        {
            *pageOfLatest_pu32 = rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redFeeIndex_u16].pageNo_uo;
        }
        else
        {
            retVal_en = E_NOT_OK;
        }
    }

    return  retVal_en;
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_isCacheValid
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be checked for cacheValidity
 * \return  boolean: is cache valid ?
 * \retval  TRUE  : Cache is valid
 * \retval  FALSE : Cache is invalid
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_CacheKwn_isCacheValid(uint16 feeIndex_u16)
{
    return (rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16].cacheValid_u1 > 0u);
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_isCacheValid_Red
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be checked for cacheValidity of redundant entry
 * \param   isCV_pb : reference to return value, if the request was refering a valid index
 * \return  operation is executed successfully, return parameter allowed to be evaluated
 * \retval  E_OK : finished successfully
 * \retval  E_NOT_OK : error during processing, e.g. OOB request or config mimatch
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheKwn_isCacheValid_Red(uint16 feeIndex_u16 , boolean * const isCV_pb)
{
    uint16 redFeeIndex_u16 = 0u;
    Std_ReturnType retVal_en = rba_FeeFs1x_CacheKwn_getRedCacheIdx(&redFeeIndex_u16,feeIndex_u16);

    // Is the requested block configured as redundant?
    if(E_OK == retVal_en)
    {
        *isCV_pb = (rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redFeeIndex_u16].cacheValid_u1 > 0u);
    }

    return  retVal_en;
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateCache
 * This function when called invalidates the cache entry from the Native known block cache.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be invalidated
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheKwn_invalidateCache(uint16 feeIndex_u16)
{
    rba_FeeFs1x_CacheKwn_overstampNative(feeIndex_u16);
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateCache_Red
 *  This function when called invalidates the cache entry from the redundant known
 * block cache.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be invalidated
 * \return  operation is executed successfully, return parameter allowed to be evaluated
 * \retval  E_OK : finished successfully
 * \retval  E_NOT_OK : error during processing, e.g. OOB request or config mimatch
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheKwn_invalidateCache_Red(uint16 feeIndex_u16)
{
    uint16 redFeeIndex_u16 = 0u;
    Std_ReturnType retVal_en = rba_FeeFs1x_CacheKwn_getRedCacheIdx(&redFeeIndex_u16,feeIndex_u16);

    // Is the requested block configured as redundant?
    if(E_OK == retVal_en)
    {
        rba_FeeFs1x_CacheKwn_overstampRed(redFeeIndex_u16);
    }

    return retVal_en;
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateCaches_NatAndRed
 * invalidation of known cache entries from native and redundant (if available).
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16: index to be invalidated
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheKwn_invalidateCaches_NatAndRed(uint16 feeIndex_u16)
{
    rba_FeeFs1x_CacheKwn_invalidateCache(feeIndex_u16);
    (void) rba_FeeFs1x_CacheKwn_invalidateCache_Red(feeIndex_u16);
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateRedCacheEntriesFromPageNrAndRetHighestPageNr
 * This function will invalidate all entries in the redundant cache that points to value greater or equal
 * then the page number passed by the function parameter.
 * After invalidation function will return the highest page number left.
 * \param   page_u32 : lower page limit, every cache entry >= this page gets invalidated
 * \return  uint32: highest Page number left after invalidation
 *********************************************************************
 */
LOCAL_INLINE uint32 rba_FeeFs1x_CacheKwn_invalidateRedCacheEntriesFromPageNrAndRetHighestPageNr(uint32 page_u32)
{
    uint32 retVal_u32 = 0u;
    uint32 cachePageNo_u32;
    uint16 i_u16;

    for(i_u16 = 0; i_u16< RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS ; i_u16++)
    {
        cachePageNo_u32 = rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[i_u16].pageNo_uo;

        if(cachePageNo_u32 >= page_u32)
        {
            rba_FeeFs1x_CacheKwn_overstampRed(i_u16);
        }
        else
        {
            if(cachePageNo_u32 > retVal_u32)
            {
                retVal_u32 = cachePageNo_u32;
            }
        }
    }

    return (retVal_u32);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateRedCacheEntryByPageNr
 * This function will invalidate the entry in the native cache with the given page number by the function parameter
 * \param   page_u32 : lower page limit, every cache entry >= this page gets invalidated
 *********************************************************************
 */
static void rba_FeeFs1x_CacheKwn_invalidateRedCacheEntryByPageNr(uint32 page_u32)
{
    uint16 i_u16;

    for(i_u16 = 0; i_u16< RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS ; i_u16++)
    {
        if(page_u32 == rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[i_u16].pageNo_uo)
        {
            rba_FeeFs1x_CacheKwn_overstampRed(i_u16);
        }
    }
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateNatCacheEntriesFromPageNrAndRetHighestPageNr
 * This function will invalidate all entries in the native cache that points to value greater or equal
 * then the page number passed by the function parameter.
 * After invalidation function will return the highest page number left.
 * \param   page_u32 : lower page limit, every cache entry >= this page gets invalidated
 * \return  uint32: highest Page number left after invalidation
 *********************************************************************
 */
LOCAL_INLINE uint32 rba_FeeFs1x_CacheKwn_invalidateNatCacheEntriesFromPageNrAndRetHighestPageNr(uint32 page_u32)
{
    uint32 retVal_u32 = 0u;
    uint32 cachePageNo_u32;
    uint16 i_u16;

    for(i_u16 = 0; i_u16< RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS ; i_u16++)
    {
        cachePageNo_u32 = rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[i_u16].pageNo_uo;

        if(cachePageNo_u32 >= page_u32)
        {
            rba_FeeFs1x_CacheKwn_overstampNative(i_u16);
        }
        else
        {
            if(cachePageNo_u32 > retVal_u32)
            {
                retVal_u32 = cachePageNo_u32;
            }
        }
    }

    return (retVal_u32);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateNatCacheEntryByPageNr
 * This function will invalidate the entry in the native cache with the given page number by the function parameter
 * \param   page_u32 : lower page limit, every cache entry >= this page gets invalidated
 *********************************************************************
 */
static void rba_FeeFs1x_CacheKwn_invalidateNatCacheEntryByPageNr(uint32 page_u32)
{
    uint16 i_u16;

    for(i_u16 = 0; i_u16< RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS ; i_u16++)
    {
        if(page_u32 == rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[i_u16].pageNo_uo)
        {
            rba_FeeFs1x_CacheKwn_overstampNative(i_u16);
        }
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_invalidateCacheEntriesFromPageNoAndHighestPageNoLeft
 * This function will invalidate all entries in the cache that points to value greater or equal
 * then the page number passed by the function parameter.
 * In addition entry with highest page no in one of both caches will be removed to avoid data CRC error
 * when overlapping block is present.
 * This might lead to a non overlapping block to be removed from cache (e.g. a block which exactly end at sector end).
 * This is ok because it will be found during cache build up call which follows immediatly after in sector overlow handling.
 * Function was designed to be used in sector overflow handling only - Please pay attention when reusing this function.
 * \param   page_u32 : lower page limit, every cache entry >= this page gets invalidated
 *********************************************************************
 */
void rba_FeeFs1x_CacheKwn_invalidateCacheEntriesFromPageNoAndHighestPageNoLeft(uint32 page_u32)
{
    uint32 highestNatCachePageNo_u32 = rba_FeeFs1x_CacheKwn_invalidateNatCacheEntriesFromPageNrAndRetHighestPageNr(page_u32);
    uint32 highestRedCachePageNo_u32 = rba_FeeFs1x_CacheKwn_invalidateRedCacheEntriesFromPageNrAndRetHighestPageNr(page_u32);

    if(highestNatCachePageNo_u32 >= highestRedCachePageNo_u32)
    {
        rba_FeeFs1x_CacheKwn_invalidateNatCacheEntryByPageNr(highestNatCachePageNo_u32);
    }
    else
    {
        rba_FeeFs1x_CacheKwn_invalidateRedCacheEntryByPageNr(highestRedCachePageNo_u32);
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_reduceOwnPages
 *  This function is the "update" of the observer pattern. If a sector was reorganised, the virtual page number 0
 *  addresses another logical sector start page than before. All page numbers in other units get updated
 *  with such a function call of reduceOwnPages
 * \param   amountOfPages_u32 : amount of pages all cache entries get reduced by
 * \return  job successful?
 * \retval  E_OK: successfully reduced all entries
 * \retval  E_NOT_OK: error during reduction
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages(uint32 amountOfPages_u32)
{
    Std_ReturnType  retVal_en;
    Std_ReturnType  retValNat_en;
    Std_ReturnType  retValRed_en;

    // for all native cache entries
    retValNat_en = rba_FeeFs1x_CacheKwn_reduceOwnPages_natCache(amountOfPages_u32);

    // For all redundant cache entries
    retValRed_en = rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache(amountOfPages_u32);

    if((E_OK ==retValNat_en) && (E_OK == retValRed_en))
    {
        retVal_en = E_OK;
    }
    else
    {
        retVal_en = E_NOT_OK;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_reduceOwnPages_natCache
 *  subfunction of reduceOwnPages for reducing the native cache elements
 * \param   amountOfPages_u32 : amount of pages all cache entries get reduced by
 * \return  job successful?
 * \retval  E_OK: successfully reduced all entries
 * \retval  E_NOT_OK: error during reduction
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages_natCache(uint32 amountOfPages_u32)
{
    uint16 i_u16;
    Std_ReturnType retVal_en;

    retVal_en = E_OK;

    for(i_u16 = 0 ; i_u16 < RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS ; i_u16++)
    {
        if(rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[i_u16].cacheValid_u1 > 0u)
        {
            if(rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[i_u16].pageNo_uo >= amountOfPages_u32)
            {
                // If the cache is valid and the pageNo is greater than the requested reduce-amount, exec the reduce
                rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[i_u16].pageNo_uo =
                        ((uint32)rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[i_u16].pageNo_uo) - amountOfPages_u32;
            }
            else
            {
                // If a cache entry still is pointing to the area below the reduction pageno, destroy the cache entry
                rba_FeeFs1x_CacheKwn_overstampNative(i_u16);

                // return error to may be restart reorganisation as some blocks were not yet transferred during reorg
                retVal_en = E_NOT_OK;
            }
        }
    }

    return retVal_en;
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache
 *  subfunction of reduceOwnPages for reducing the redundant cache elements
 * \param   amountOfPages_u32 : amount of pages all cache entries get reduced by
 * \return  job successful?
 * \retval  E_OK: successfully reduced all entries
 * \retval  E_NOT_OK: error during reduction
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache(uint32 amountOfPages_u32)
{
    Std_ReturnType retVal_en;
    uint16 i_u16;

    retVal_en = E_OK;

    for(i_u16 = 0 ; i_u16 < RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS; i_u16++)
    {
        if(rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[i_u16].cacheValid_u1 > 0u)
        {
            if(rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[i_u16].pageNo_uo >= amountOfPages_u32)
            {
                // If the cache is valid and the pageNo is greater than the requested reduce-amount, exec the reduce
                rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[i_u16].pageNo_uo =
                        ((uint32)rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[i_u16].pageNo_uo) - amountOfPages_u32;
            }
            else
            {
                retVal_en = rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache_handleUnderflow(i_u16);
            }
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache_handleUnderflow
 *  subfunction of rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache. The redundant copy is within the 0th sector
 *  --> check whether this has to cause an error reaction
 * \param   redIndexUnderflowing_u16 : redundant index of the block
 * \return  job successful?
 * \retval  E_OK: underflow is allowed
 * \retval  E_NOT_OK: underflow is an error and should have been reorged
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheKwn_reduceOwnPages_redCache_handleUnderflow(uint16 redIndexUnderflowing_u16)
{
    uint16 natFeeIdx_u16;
    Std_ReturnType retVal_en;

    if (redIndexUnderflowing_u16 < RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS)
    {
        // If a cache entry still is pointing to the area below the reduction pageno, destroy the cache entry
        rba_FeeFs1x_CacheKwn_overstampRed(redIndexUnderflowing_u16);

        // error should be reported only for blocks which has redundant flag set in the native cache i.e.,
        // if the block was supposed to be transffered twice but didnt transfer them.
        // get the native cache position
        natFeeIdx_u16 = rba_FeeFs1x_Prv_Cfg_RedBlks_acu16[redIndexUnderflowing_u16];
        if(rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[natFeeIdx_u16].copyRedundantConfig_u1 > 0u)
        {
            // return error to may be restart reorganisation as some blocks were not yet transferred during reorg
            retVal_en = E_NOT_OK;
        }
        else
        {
            // underflow for an older copy of a native copy is allowed
            retVal_en = E_OK;
        }
    }
    else
    {
        // incorrect index passed. this should never occur. return Error to may be retry or lock down
        retVal_en = E_NOT_OK;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_copyRedCacheToNatCache
 *  Function copies the contents from redundant cache to native cache.
 *  There is no validation of parameters, these should be performed by the calling function
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   idxNatCache_u16 : index of the native cache
 * \param   idxRedCache_u16 : index of the redundant cache
 * \return  no return value
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_copyRedCacheToNatCache(uint16 idxNatCache_u16, uint16 idxRedCache_u16)
{
    rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[idxNatCache_u16] =
            rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[idxRedCache_u16];
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_restoreRed
 *  Restore (copy) the redundant cache entry to the native cache.
 *  If a block is not redundant, invalidate the native cache
 *  If the index is configured as redundant but doesn't have a red cache entry, the restore doesn't get executed
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be restored
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheKwn_restoreRed(uint16 feeIndex_u16)
{
    uint16 redFeeIndex_u16 = 0u;
    Std_ReturnType retVal_en = rba_FeeFs1x_CacheKwn_getRedCacheIdx(&redFeeIndex_u16,feeIndex_u16);

    // Is the requested block configured as redundant?
    if(E_OK == retVal_en)
    {
        // copy all infos from redundant cache entry of the requested block to the native cache
        rba_FeeFs1x_CacheKwn_copyRedCacheToNatCache(feeIndex_u16, redFeeIndex_u16);

        // invalidate the redundant cache entry because this copy already is cached at the native entry
        rba_FeeFs1x_CacheKwn_overstampRed(redFeeIndex_u16);
    }
    else
    {
        // block is not a redundant block --> destroy the native entry
        rba_FeeFs1x_CacheKwn_overstampNative(feeIndex_u16);
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_setDataValid
 *  This function updates the data validity status of the cache entry to true.
 * \param   feeIndex_u16 : index to be processed.
 * \attention: Function should be called only with valid parameter value!
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheKwn_setDataValid(uint16 feeIndex_u16)
{
    rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16].dataValid_u1 = 1u;
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_updateRedundant
 *  This function updates the redundant cache entry of the known block with new address.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16 : feeIndex to be processed
 * \return  operation is executed successfully
 * \retval  E_OK        : finished successfully
 * \retval  E_NOT_OK    : error during processing, e.g. OOB request or config mimatch
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheKwn_updateRed(uint16 feeIndex_u16, uint32 newRedundantCopyPage_u32,
        boolean isDataValid_b , boolean hasRedBitActive_b)
{
    uint16 redFeeIndex_u16 = 0u;
    Std_ReturnType retVal_en = rba_FeeFs1x_CacheKwn_getRedCacheIdx(&redFeeIndex_u16,feeIndex_u16);

    // Is the requested block configured as redundant?
    if(E_OK == retVal_en)
    {
        // If the red index of the native index is found,
        // set the red cache entry accoring to the parameters

        rba_FeeFs1x_CacheKwn_setEntry(  &rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redFeeIndex_u16],
                                        isDataValid_b,
                                        newRedundantCopyPage_u32,
                                        hasRedBitActive_b);
    }

    return  retVal_en;
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_overstampEntry
 * This function destroys a cache entry w/o further checks.
 * This is a static function for combining the actual invalidations used from functions
 * which already executed the checks
 * Function can be used for native and redundant cache entries
 * \attention: Function should be called only with valid parameter value!
 * \param   entry_pst : pointer to cache element
 *********************************************************************
 */
static void rba_FeeFs1x_CacheKwn_overstampEntry(rba_FeeFs1x_CacheKwn_CacheElement_tst * entry_pst)
{
    entry_pst->pageNo_uo   = 0xFFFFF; // 20 bit
    entry_pst->cacheValid_u1 = 0u;
    entry_pst->dataValid_u1  = 0u;
    entry_pst->copyRedundantConfig_u1 = 0u;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_overstampNative
 *  This function destroys a native cache entry w/o further checks.
 *  This is a static function for combining the actual invalidations used from functions
 *  which already executed the checks
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : feeIndex to be destroyed
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_overstampNative(uint16 feeIndex_u16)
{
    rba_FeeFs1x_CacheKwn_overstampEntry(&rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16]);
}

# if (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_overstampRed
 *  This function destroys a redundant cache entry w/o further checks.
 *  This is a static function for combining the actual invalidations used from functions
 *  which already executed the checks
 * \attention: Function should be called only with valid parameter value!
 * \param   redIndex_u16 : redundant cache index to be destroyed. Translation from feeIndex required before calling
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheKwn_overstampRed(uint16 redIndex_u16)
{
    rba_FeeFs1x_CacheKwn_overstampEntry(&rba_FeeFs1x_CacheKwn_redundantKnownBlockCache_ast[redIndex_u16]);
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_CacheKwn_isCachedCopyRedundantBitActive
 *  This function returns if the cached copy was redundant block or not.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16 : index to be processed
 * \param   isRed_pb     : pointer where the redundant information is returned. TRUE -> is redundant copy, else FALSE
 * \return  cache validity
 * \retval  E_OK        : cache entry is valid
 * \retval  E_NOT_OK    : cache entry is invalid -> isRed_pb is not updated with any value
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheKwn_isCachedCopyRedundantBitActive(uint16 feeIndex_u16 , boolean * const isRed_pb)
{
    Std_ReturnType retVal_en;

    if(rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16].cacheValid_u1 > 0u)
    {
        // request on valid cache entry
        // --> return the redundancy status of this copy
        if(rba_FeeFs1x_CacheKwn_nativeKnownBlockCache_ast[feeIndex_u16].copyRedundantConfig_u1 > 0u)
        {
            // copy has the redundant bit active
            *isRed_pb = TRUE;
            retVal_en = E_OK;
        }
        else
        {
            // copy doesn't have the redundant cache active
            *isRed_pb = FALSE;
            retVal_en = E_OK;
        }
    }
    else
    {
        // request for empty cache entry
        retVal_en = E_NOT_OK;
    }

    return retVal_en;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

