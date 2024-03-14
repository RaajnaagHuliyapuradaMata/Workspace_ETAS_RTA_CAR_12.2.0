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

#ifndef RBA_FEEFS1X_PRV_CACHEUNKWN_H
#define RBA_FEEFS1X_PRV_CACHEUNKWN_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

# if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

#include "rba_FeeFs1x_Prv_Cfg.h"

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


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

#  if(RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE != 0u)

extern void             rba_FeeFs1x_CacheUnkwn_init(void);

extern void             rba_FeeFs1x_CacheUnkwn_addNewerCopy(uint16 feeIndex_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b);
extern Std_ReturnType   rba_FeeFs1x_CacheUnkwn_addNewerCopyByPersID(uint16 persID_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b);

extern uint32           rba_FeeFs1x_CacheUnkwn_getLatestEntry(uint16 feeIndex_u16);
extern Std_ReturnType   rba_FeeFs1x_CacheUnkwn_getLatestEntry_Red(uint16 idxRedBlk_u16, uint32 * const nrPageOfLatestRed_cpu32);
extern uint16           rba_FeeFs1x_CacheUnkwn_getUnknownBlkCacheLevel(void);
extern uint16           rba_FeeFs1x_CacheUnkwn_getPersID(uint16 feeIdx_u16);

extern boolean          rba_FeeFs1x_CacheUnkwn_isCacheValid(uint16 feeIndex_u16);
extern Std_ReturnType   rba_FeeFs1x_CacheUnkwn_isCacheValid_Red(uint16 idxRedBlk_u16, boolean * const isCV_pb);

extern Std_ReturnType   rba_FeeFs1x_CacheUnkwn_isCachedCopyRedundantBitActive(uint16 idxRedBlk_u16, boolean * const isRed_pb);

extern void             rba_FeeFs1x_CacheUnkwn_invalidateCache(uint16 feeIndex_u16);
extern void             rba_FeeFs1x_CacheUnkwn_invalidateCache_Red(uint16 feeIndex_u16);
extern void             rba_FeeFs1x_CacheUnkwn_invalidateCaches_NatAndRed(uint16 feeIndex_u16);

extern void             rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache(void);
extern Std_ReturnType   rba_FeeFs1x_CacheUnkwn_reduceOwnPages(uint32 amountOfPages_u32);

extern boolean          rba_FeeFs1x_CacheUnkwn_isBlockCached(uint16 persID_u16);

extern void             rba_FeeFs1x_CacheUnkwn_restoreRed(uint16 idxRedBlk_u16);

extern void             rba_FeeFs1x_CacheUnkwn_setDataValid(uint16 feeIndex_u16);

extern void             rba_FeeFs1x_CacheUnkwn_updateRed(uint16 feeIndex_u16, uint32 newRedundantCopyPage_u32, boolean isDataValid_b , boolean hasRedBitActive_b);

#  else

/* There is no unknown block handling required, replace all public function calls with dummy functions.
 * None of the functions from this unit should be called, so return failure when the function is called accidently. */
/*
 **********************************************************************************************************************
 * Inline functions
 **********************************************************************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_init(void);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_init(void)
{
    return;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_addNewerCopy(uint16 feeIndex_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_addNewerCopy(uint16 feeIndex_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b)
{
    (void) feeIndex_u16;
    (void) isDataValidated_b;
    (void) pageNr_u32;
    (void) hasRedBitActive_b;
    return;
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_addNewerCopyByPersID(uint16 persID_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_addNewerCopyByPersID(uint16 persID_u16, boolean isDataValidated_b, uint32 pageNr_u32, boolean hasRedBitActive_b)
{
    (void) persID_u16;
    (void) isDataValidated_b;
    (void) pageNr_u32;
    (void) hasRedBitActive_b;
    return E_NOT_OK;
}

LOCAL_INLINE uint32 rba_FeeFs1x_CacheUnkwn_getLatestEntry(uint16 feeIndex_u16);
LOCAL_INLINE uint32 rba_FeeFs1x_CacheUnkwn_getLatestEntry(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return 0uL;
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_getLatestEntry_Red(uint16 idxRedBlk_u16, uint32 * const nrPageOfLatestRed_cpu32);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_getLatestEntry_Red(uint16 idxRedBlk_u16, uint32 * const nrPageOfLatestRed_cpu32)
{
    (void) idxRedBlk_u16;
    *nrPageOfLatestRed_cpu32 = 0uL; /* Modify this pointer parameter to avoid violation of MISRA rule 8.13 (QAC_3673) */
    return E_NOT_OK;
}

LOCAL_INLINE uint16 rba_FeeFs1x_CacheUnkwn_getUnknownBlkCacheLevel(void);
LOCAL_INLINE uint16 rba_FeeFs1x_CacheUnkwn_getUnknownBlkCacheLevel(void)
{
    return 0u;
}

LOCAL_INLINE uint16 rba_FeeFs1x_CacheUnkwn_getPersID(uint16 feeIdx_u16);
LOCAL_INLINE uint16 rba_FeeFs1x_CacheUnkwn_getPersID(uint16 feeIdx_u16)
{
    (void) feeIdx_u16;
    return 0xFFFFu;
}

LOCAL_INLINE boolean rba_FeeFs1x_CacheUnkwn_isCacheValid(uint16 feeIndex_u16);
LOCAL_INLINE boolean rba_FeeFs1x_CacheUnkwn_isCacheValid(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return FALSE;
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_isCacheValid_Red(uint16 idxRedBlk_u16, boolean * const isCV_pb);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_isCacheValid_Red(uint16 idxRedBlk_u16, boolean * const isCV_pb)
{
    (void) idxRedBlk_u16;
    *isCV_pb = FALSE; /* Modify this pointer parameter to avoid violation of MISRA rule 8.13 (QAC_3673) */
    return E_NOT_OK;
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_isCachedCopyRedundantBitActive(uint16 idxRedBlk_u16, boolean * const isRed_pb);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_isCachedCopyRedundantBitActive(uint16 idxRedBlk_u16, boolean * const isRed_pb)
{
    (void) idxRedBlk_u16;
    *isRed_pb = FALSE; /* Modify this pointer parameter to avoid violation of MISRA rule 8.13 (QAC_3673) */
    return E_NOT_OK;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCache(uint16 feeIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCache(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCache_Red(uint16 feeIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCache_Red(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCaches_NatAndRed(uint16 feeIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCaches_NatAndRed(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache(void);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_invalidateCompleteCache(void)
{
    return;
}

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_reduceOwnPages(uint32 amountOfPages_u32);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_CacheUnkwn_reduceOwnPages(uint32 amountOfPages_u32)
{
    (void) amountOfPages_u32;
    return E_OK; /* Avoid error during reorg -> this function is always called during reorg */
}

LOCAL_INLINE boolean rba_FeeFs1x_CacheUnkwn_isBlockCached(uint16 persID_u16);
LOCAL_INLINE boolean rba_FeeFs1x_CacheUnkwn_isBlockCached(uint16 persID_u16)
{
    (void) persID_u16;
    return FALSE;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_restoreRed(uint16 idxRedBlk_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_restoreRed(uint16 idxRedBlk_u16)
{
    (void) idxRedBlk_u16;
    return;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_setDataValid(uint16 feeIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_setDataValid(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return;
}

LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_updateRed(uint16 feeIndex_u16, uint32 newRedundantCopyPage_u32, boolean isDataValid_b, boolean hasRedBitActive_b);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_updateRed(uint16 feeIndex_u16, uint32 newRedundantCopyPage_u32, boolean isDataValid_b, boolean hasRedBitActive_b)
{
    (void) feeIndex_u16;
    (void) newRedundantCopyPage_u32;
    (void) isDataValid_b;
    (void) hasRedBitActive_b;
    return;
}

#  endif

/* Do not re-initalize cache during cancel request. The dummy functions are provided, so that this decision of doing
 * nothing for cancel is present in this unit itself and not spread this in the central cancel job function */
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_cancel(void);
LOCAL_INLINE void rba_FeeFs1x_CacheUnkwn_cancel(void)
{
    return;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif
#endif

