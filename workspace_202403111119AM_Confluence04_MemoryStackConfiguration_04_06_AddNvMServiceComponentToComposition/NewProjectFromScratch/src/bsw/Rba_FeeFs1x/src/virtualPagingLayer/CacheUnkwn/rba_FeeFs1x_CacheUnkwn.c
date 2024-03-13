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

#include "rba_FeeFs1x_Prv_Cfg.h"

# if (RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE != 0u)
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv_CacheUnkwn.h"
#include "rba_FeeFs1x_Prv_CacheUnkwnTypes.h"
#include "rba_FeeFs1x_Prv.h"

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

static rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_tst rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE];
static rba_FeeFs1x_CacheUnkwn_tst rba_FeeFs1x_CacheUnkwn_st; // Working variable for unknown cache handling

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_overstampNat(uint16 feeIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_overstampRed(uint16 idxRedBlk_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_addNewerCopy_pushToRedCache(uint16 feeIndex_u16);

static void rba_FeeFs1x_CacheUnkwn_addCacheEntry(rba_FeeFs1x_CacheUnkwn_unkwnCacheElement_tst * cacheElement_st, boolean dataValidated_b, uint32 pageNr_u32  , boolean hasRedBitActive_b);
static void rba_FeeFs1x_CacheUnkwn_overstamp(rba_FeeFs1x_CacheUnkwn_unkwnCacheElement_tst * cacheElement_st);
/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */


/**
 *********************************************************************
 * rba_FeeFs1x_Cache_init
 * initializes the static variables of the unit to the init values
 *
 * \param   None
 * \return  None
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_init(void)
{
    rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache();
}


/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_addCacheEntry
 * Sets the unkown block element to given values
 * \attention: Function should be called only with valid parameter value!
 * \param   cacheElement_st     : pointer to unknown block cache element
 * \param   dataValidated_b     : status of data validation
 * \param   pageNr_u32          : Virtual page number where the block is present in the memory
 * \param   hasRedBitActive_b   : redundancy statusbit of the new copy
 * \return  None
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_CacheUnkwn_addCacheEntry(rba_FeeFs1x_CacheUnkwn_unkwnCacheElement_tst * cacheElement_st, boolean dataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b)
{
    cacheElement_st->cacheValid_u1 = 1u;
    cacheElement_st->copyRedConfig_u1 = hasRedBitActive_b ? (1u) : (0u);
    cacheElement_st->dataValid_u1 = dataValidated_b ? (1u) : (0u);
    cacheElement_st->pageNr_uo = pageNr_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_overstamp
 * initializes the unkown block element to init value
 * \attention: Function should be called only with valid parameter value!
 * \param   cacheElement_st     : pointer to unknown block cache element
 * \return  None
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_CacheUnkwn_overstamp(rba_FeeFs1x_CacheUnkwn_unkwnCacheElement_tst * cacheElement_st)
{
    cacheElement_st->pageNr_uo   = 0xFFFFF; // 20 bit
    cacheElement_st->cacheValid_u1 = 0u;
    cacheElement_st->dataValid_u1  = 0u;
    cacheElement_st->copyRedConfig_u1 = 0u;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_overstampNat
 * initializes the unkown block in native cache to init value
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16 : Fee index of the unknown block cache
 * \return  None
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_overstampNat(uint16 feeIndex_u16)
{
    rba_FeeFs1x_CacheUnkwn_overstamp(&rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY]);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_overstampRed
 * initializes the unkown block in redundant cache to init value
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   idxRedBlk_u16    : Fee index of the redundant unknown block cache
 * \return  None
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_overstampRed(uint16 idxRedBlk_u16)
{
    rba_FeeFs1x_CacheUnkwn_overstamp(&rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY]);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_addNewerCopy
 * This function updates the address of an already cached unknown block. It first pushes the native cache content to
 * redundant cache.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16        : Fee index of the unknown block cache
 * \param   isDataValidated_b   : status of data validation
 * \param   pageNr_u32          : Virtual page number where the block is present in the memory
 * \param   hasRedBitActive_b   : redundancy statusbit of the new copy
 * \retval void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_addNewerCopy(
        uint16 feeIndex_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b)
{
    // Is the Fee index should be within the valid range RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE
    /* If a newer copy gets added for a redundant block, the current latest consistent copy
     * gets degraded to the pre-latest one
     * --> move the current native cache entry to the redundant cache entry of that block
     */
    rba_FeeFs1x_CacheUnkwn_addNewerCopy_pushToRedCache(feeIndex_u16);

    /* Update native cache contents */
    rba_FeeFs1x_CacheUnkwn_addCacheEntry(&rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY],
                                         isDataValidated_b,
                                         pageNr_u32,
                                         hasRedBitActive_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_addNewerCopy_pushToRedCache
 * This function updates the contents from the native cache to redundant cache.
 * \attention: Function should be called only with valid parameter value!
 * \param   feeIndex_u16        : Fee index of the unknown block cache
 * \retval  none.
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_addNewerCopy_pushToRedCache(uint16 feeIndex_u16)
{
    rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY] =
            rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY];
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_addNewerCopyByPersID
 * This function when called will add the new unknown block to the unknown block cache. It will first check if the block
 * is already cached, it will update the same entry, else it will add to a new entry. if the cache is full it returns
 * update not possible.
 *
 * \param   persID_u16          : Persistent index of the unknown block cache
 * \param   isDataValidated_b   : status of data validation
 * \param   pageNr_u32          : Virtual page number where the block is present in the memory
 * \param   hasRedBitActive_b   : redundancy statusbit of the new copy
 * \retval is cache update successful?
 * \return  E_OK                : Update to cache was successful.
 * \return  E_NOT_OK            : Cache update failed due to cache full or wrong parameter
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheUnkwn_addNewerCopyByPersID(
        uint16 persID_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b)
{
    Std_ReturnType retVal_en = E_NOT_OK;
    uint16 freeCachePos_u16 = rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16;
    uint16 idx_u16;

    // Check if the block is already cached
    for(idx_u16 = 0u;idx_u16 < freeCachePos_u16; idx_u16++)
    {
        // check if the persistent id matches
        if(rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].persID_u16 == persID_u16)
        {
            // block exisiting in the cache
            // update the fee index position
            rba_FeeFs1x_CacheUnkwn_addNewerCopy(idx_u16, isDataValidated_b, pageNr_u32, hasRedBitActive_b);
            retVal_en = E_OK;
            break;
        }
    }

    if(E_NOT_OK == retVal_en)
    {
        // block is not yet cached. Check if cache is full
        // Keep return E_NOT_OK in this case to indicate that the cache update was not successful.
        if(RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE > freeCachePos_u16)
        {
            // free space available in cache, update the cache contents
            rba_FeeFs1x_CacheUnkwn_addNewerCopy(freeCachePos_u16, isDataValidated_b, pageNr_u32, hasRedBitActive_b);
            // cache update was successful, store the persistent id
            rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[freeCachePos_u16].persID_u16 = persID_u16;

            // increment the cache position to next entry
            rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16++;

            retVal_en = E_OK;
        }
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_getLatestEntry
 * This function when called returns the cache entry (page number) from the Native unknown block cache.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIdx_u16          : Fee index of the unknown block cache
 * \retval  uint32              : Latest Entry
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_CacheUnkwn_getLatestEntry(uint16 feeIndex_u16)
{
    // Block should be already cached
    // fee index is valid, return the cache contents
    return(rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY].pageNr_uo);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_getLatestEntry_Red
 * This function when called returns the cache entry (page number) from the redundant unknown block cache.
 *
 * \param   idxRedBlk_u16           : Fee index of the redundant unknown block cache
 * \param   nrPageOfLatestRed_cpu32 : Pointer to return the cache content
 * \retval is fee index valid?
 * \return  E_OK        : valid fee index was passed and the page number is returned in the nrPageOfLatestRed_cpu32
 * \return  E_NOT_OK    : Invalid fee index was passed
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheUnkwn_getLatestEntry_Red(uint16 idxRedBlk_u16, uint32* const nrPageOfLatestRed_cpu32)
{
    Std_ReturnType retVal_en;

    if(  (idxRedBlk_u16 < rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16) &&
         (1u == rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY].cacheValid_u1))
    {
        // fee index is valid, return the cache contents
        *nrPageOfLatestRed_cpu32 = rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY].pageNr_uo;

        retVal_en = E_OK;
    }
    else
    {
        // block is not yet cached, incorrect fee index is passed
        retVal_en = E_NOT_OK;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Cache_getUnknownBlkCacheLevel
 * This returns the value of the unknown block pointer, which will be used for cache scan during reorganisastion.
 *
 * \param   none
 * \retval number of filled entries in cache
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint16 rba_FeeFs1x_CacheUnkwn_getUnknownBlkCacheLevel(void)
{
    return(rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_isCachedCopyRedundantBitActive
 * Returns the value of redundant status bit from the native unknown cache.
 *
 * \param   idxRedBlk_u16       : Fee index of the redundant unknown block cache
 * \param   isRed_pb            : Pointer to return the redundant status flag information
 * \retval is fee index valid?
 * \return  E_OK                : valid fee index was passed and the redundant status bit is returned in the isRed_pb
 * \return  E_NOT_OK            : Invalid fee index was passed
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType   rba_FeeFs1x_CacheUnkwn_isCachedCopyRedundantBitActive(uint16 idxRedBlk_u16 , boolean * const isRed_pb)
{
    Std_ReturnType retVal_en;

    if(  (idxRedBlk_u16 < rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16) &&
         (0u < rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY].cacheValid_u1))
    {
        // fee index is valid, return the cache contents
        *isRed_pb = (rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY].copyRedConfig_u1 > 0u);

        retVal_en = E_OK;
    }
    else
    {
        // block is not yet cached, incorrect fee index is passed
        retVal_en = E_NOT_OK;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_isCacheValid
 * Returns the status of cache valid bit from the native unknown cache.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIdx_u16          : Fee index of the unknown block cache
 * \retval is cache valid
 * \return  TRUE  : Cache is valid
 * \return  FASLE : Cache is invalid
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_CacheUnkwn_isCacheValid(uint16 feeIndex_u16)
{
    // fee index is valid, return the cache contents
    return (rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY].cacheValid_u1 > 0u);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_isCacheValid_Red
 * Returns the status of cache valid bit from the redundant unknown cache.
 *
 * \param   idxRedBlk_u16       : Fee index of the redundant unknown block cache
 * \param   isCV_pb             : Pointer to return the cache valid content
 * \retval is fee index valid?
 * \return  E_OK                : valid fee index was passed and the cache valid is returned in the isCV_pb
 * \return  E_NOT_OK            : Invalid fee index was passed
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheUnkwn_isCacheValid_Red(uint16 idxRedBlk_u16, boolean * const isCV_pb)
{
    Std_ReturnType retVal_en;

    if(idxRedBlk_u16 < rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16)
    {
        // fee index is valid, return the cache contents
        *isCV_pb = (rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY].cacheValid_u1 > 0u);

        retVal_en = E_OK;
    }
    else
    {
        // block is not yet cached, incorrect fee index is passed
        retVal_en = E_NOT_OK;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_getPersID
 * This function when called returns the persistent id of the unknown block.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16        : Fee index of the unknown block cache
 * \retval  uint16              : Presistent index
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint16 rba_FeeFs1x_CacheUnkwn_getPersID(uint16 feeIdx_u16)
{
    // fee index is valid, return the persistent index
    return(rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIdx_u16].persID_u16);

}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_invalidateCache
 * This function when called invalidates the cache entry for native unknown block cache.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16        : Fee index of the unknown block cache
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_invalidateCache(uint16 feeIndex_u16)
{
    // fee index is valid, invaldiate the native cache entry
    rba_FeeFs1x_CacheUnkwn_overstampNat(feeIndex_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_invalidateCache_Red
 * This function when called invalidates the cache entry for redundant unknown block cache.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16        : Fee index of the unknown block cache
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_invalidateCache_Red(uint16 feeIndex_u16)
{
    // fee index is valid, invaldiate the redunant cache entry
    rba_FeeFs1x_CacheUnkwn_overstampRed(feeIndex_u16);
}


/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnKwn_invalidateCaches_NatAndRed
 * invalidation of unknown native and redundant cache entries, if available
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16: index to be invalidated
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_invalidateCaches_NatAndRed(uint16 feeIndex_u16)
{
    // fee index should be valid
    // fee index is valid, invaldiate the native and redundant cache entries
    rba_FeeFs1x_CacheUnkwn_overstampNat(feeIndex_u16);
    rba_FeeFs1x_CacheUnkwn_overstampRed(feeIndex_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache
 * This function will invalidate all the entries in the cache (both native and redundant cache).
 *
 * \param   none
 * \retval  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache(void)
{
    uint16 idx_u16;

    // initialise unknown cache entries
    for(idx_u16 = 0u ; idx_u16 < RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE ; idx_u16++)
    {
        rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].persID_u16 = RBA_FEEFS1X_MAXUINT16;
        rba_FeeFs1x_CacheUnkwn_overstampNat(idx_u16);
        rba_FeeFs1x_CacheUnkwn_overstampRed(idx_u16);
    }

    // initialise the cache index pointer
    rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16 = 0u;

}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_isBlockCached
 * This function scans the unknown block cache to see the passed persistent id is already cached.
 *
 * \param   persID_u16          : Persistent index of the unknown block cache
 * \param   feeIdx_pu16         : Pointer to the cache position
 * \retval is the unknown block cached?
 * \return  TRUE                : Block is found in the cache
 * \return  FALSE               : Block not found in the cache
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_CacheUnkwn_isBlockCached(uint16 persID_u16)
{
    // return value would be changed to TRUE in case the block was found, else it will return the init value FALSE
    boolean retVal_b = FALSE;
    uint16  idx_u16;

    // scan the unknown block cache to check if the block is already cached
    for(idx_u16 = 0u;
        (idx_u16 < rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16);
        idx_u16++)
    {
        // check if the persistent id matches
        if(rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].persID_u16 == persID_u16)
        {
            // block is already cached, set return value to TRUE
            retVal_b = TRUE;
            break;
        }
    }

    return retVal_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_reduceOwnPages
 * This function reduces all the entries in the cache by the number of pages passed as argument.
 * If there is underflow, the entries are invalidated.
 *
 * \param   amountOfPages_u32   : Amount of pages by which the entries have to be reduced
 * \retval none
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_CacheUnkwn_reduceOwnPages(uint32 amountOfPages_u32)
{
    uint16 idx_u16;
    uint8 cpy_u8;
    Std_ReturnType retVal_en;

    // set default return value to E_OK, in case of error the value is changed to E_NOT_OK
    retVal_en = E_OK;

    // for all cache entries
    for(idx_u16 = 0 ; (idx_u16 < rba_FeeFs1x_CacheUnkwn_st.idxNextFreeCachePos_u16) ; idx_u16++)
    {
        // do this twice for native and redundant cache entry
        for(cpy_u8 = 0 ; (cpy_u8 < RBA_FEEFS1X_CACHEUNKWN_MAXCPY) ; cpy_u8++)
        {
            // check if cache entry is valid, otherwise ignore such entries
            if(rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].cpy_st[cpy_u8].cacheValid_u1 > 0u)
            {
                if(rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].cpy_st[cpy_u8].pageNr_uo >= amountOfPages_u32)
                {
                    rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].cpy_st[cpy_u8].pageNr_uo =
                        ((uint32)rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].cpy_st[cpy_u8].pageNr_uo - amountOfPages_u32);
                }
                else
                {
                    // possible underflow, software should never reach here, invalidate such cache entries
                    rba_FeeFs1x_CacheUnkwn_overstamp(&rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].cpy_st[cpy_u8]);

                    // is this a redundant cache entry
                    if(RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY == cpy_u8)
                    {
                        // error should be reported only for blocks which has redundant flag set in the native cache i.e.,
                        // if the block was supposed to be transffered twice but didnt transfer them.
                        if(rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idx_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY].copyRedConfig_u1 > 0u)
                        {
                            // return error to may be restart reorganisation as some blocks were not yet transferred during reorg
                            retVal_en = E_NOT_OK;
                        }
                    }
                    else
                    {
                        // return error to may be restart reorganisation as some blocks were not yet transferred during reorg
                        retVal_en = E_NOT_OK;
                    }
                }
            }
        }
    }

    return (retVal_en);
}


/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_restoreRed
 *  Restore (copy) the redundant cache entry to the native cache.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   idxRedBlk_u16   : index to be restored
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_restoreRed(uint16 idxRedBlk_u16)
{
    // fee index is valid, invaldiate the native cache entry
    rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY] =
            rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[idxRedBlk_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY];

    // invalidate the redundant cache entry
    rba_FeeFs1x_CacheUnkwn_overstampRed(idxRedBlk_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_updateRed
 * This function updates the redundant cache entry of the unknown block with new adress.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16                : Fee index of the unknown block cache
 * \param   isDataValid_b               : status of data validation
 * \param   newRedundantCopyPage_u32    : Virtual page number where the block is present in the memory
 * \param   hasRedBitActive_b           : redundancy statusbit of the new copy
 * \retval  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_updateRed(
        uint16 feeIndex_u16, uint32 newRedundantCopyPage_u32, boolean isDataValid_b , boolean hasRedBitActive_b)
{
    /* Update redundant cache contents */
    rba_FeeFs1x_CacheUnkwn_addCacheEntry(   &rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY],
                                            isDataValid_b,
                                            newRedundantCopyPage_u32,
                                            hasRedBitActive_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_CacheUnkwn_setDataValid
 *  This function sets the data valid bit in native unknown cache to true.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   feeIndex_u16    : index to be set.
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CacheUnkwn_setDataValid(uint16 feeIndex_u16)
{
    // fee index is valid, invaldiate the native cache entry
    rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_ast[feeIndex_u16].cpy_st[RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY].dataValid_u1 = 1u;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif

#endif
