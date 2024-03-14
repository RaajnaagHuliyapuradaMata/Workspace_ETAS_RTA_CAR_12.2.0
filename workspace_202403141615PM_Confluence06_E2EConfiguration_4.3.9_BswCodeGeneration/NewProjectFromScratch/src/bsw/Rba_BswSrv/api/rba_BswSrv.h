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


#ifndef RBA_BSWSRV_H
#define RBA_BSWSRV_H


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "rba_BswSrv_Cfg.h"
#if (RBA_BSWSRV_CFG_COMPILER_LIBRARY_SUPPORT == STD_ON)
#include "string.h"
#endif

#include "rba_BswSrv_Fifo.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* Vendor and module identification */
#define RBA_BSWSRV_VENDOR_ID   (6)
#define RBA_BSWSRV_MODULE_ID   (255)
#define RBA_BSWSRV_INSTANCE_ID (0)

/* Module version information */
#define RBA_BSWSRV_SW_MAJOR_VERSION (8)
#define RBA_BSWSRV_SW_MINOR_VERSION (0)
#define RBA_BSWSRV_SW_PATCH_VERSION (0)

#define RBA_BSWSRV_PARAM_UNUSED(param)          (void)(param)

#ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT
/* IGNORE RULE 3411: The warning is uncritical and non-removable since unbalanced brackets are implemented intentionally to guarantee that
                     the macros to be invoked in correct order. Brackets are balanced if Release/Resume macro is invoked after Get/Suspend macro.
                     Approved by MISRA2012_SUPPORT_TEAM. */
#  define RBA_BSWSRV_SUSPEND_CORE_LOCAL_INT()       { rba_BswSrv_SuspendCoreLocalInt()
#  define RBA_BSWSRV_RESUME_CORE_LOCAL_INT()        rba_BswSrv_ResumeCoreLocalInt(); } RBA_BSWSRV_SET_SEQUENCE_POINT()
#  define RBA_BSWSRV_GET_LOCK_COMMON()              { rba_BswSrv_GetLockCommon()
#  define RBA_BSWSRV_RELEASE_LOCK_COMMON()          rba_BswSrv_ReleaseLockCommon(); } RBA_BSWSRV_SET_SEQUENCE_POINT()
#  define RBA_BSWSRV_GET_LOCK_NONEST(Lock_pst)      { rba_BswSrv_GetLockNonest(Lock_pst)
#  define RBA_BSWSRV_RELEASE_LOCK_NONEST(Lock_pst)  rba_BswSrv_ReleaseLockNonest(Lock_pst); } RBA_BSWSRV_SET_SEQUENCE_POINT()

#ifndef RBA_BSWSRV_NUM_USED_CORE
#  define RBA_BSWSRV_NUM_USED_CORE 3
#endif

/* Define for maximum return value of rba_BswSrv_GetPhysCore() + 1 */
#if (RBA_BSWSRV_NUM_USED_CORE == 1)
#  define RBA_BSWSRV_MAX_PHYS_CORE  1u
#else
#  define RBA_BSWSRV_MAX_PHYS_CORE  3u
#endif

#  define RBA_BSWSRV_SET_SEQUENCE_POINT()       asm volatile ("" ::: "memory")

/* Logging level replacement value for configuration backward compatibility */
#ifndef RBA_BSWSRV_CFG_LOG_LEVEL
#  define RBA_BSWSRV_CFG_LOG_LEVEL 0
#endif


#endif // #ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

#if(RBA_BSWSRV_CFG_MACHINE_SUPPORT_64_BIT == STD_ON)
typedef uint64 rba_BswSrv_uint64;
#endif

#ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT
/* structure content may change without notice as it depends on implementation */
typedef struct
{
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
    volatile uint32 Prv_CoreLock_vu32;      /* bit 0-7:  n=0: no core has lock
                                                         n>0: physical Core n-1 has lock */
                                            /* bit 8-31: lock nesting counter */
    uint32          Prv_adActivation_u32;   /* address at which Lock was activated (log level RBA_BSWSRV_PRV_LOG_LOCK) */
    volatile uint16 Prv_ctLockUse_vu16;     /* lock use counter (may overflow, log level RBA_BSWSRV_PRV_LOG_LOCK)  */
    volatile uint16 Prv_ctLockWait_vu16;    /* lock wait counter (may overflow, log level RBA_BSWSRV_PRV_LOG_LOCK) */
    volatile uint16 Prv_ctLockNest_vu16;    /* lock self nesting counter (may overflow, log level RBA_BSWSRV_PRV_LOG_LOCK) */
#else
    uint8           Prv_Dummy_u8;           /* minimum data structure for single core use */
#endif
} rba_BswSrv_Lock_to;

/* structure content may change without notice as it depends on implementation */
typedef struct
{
    volatile uint32 Prv_CoreLock_vu32;      /* bit 0-7:  n=0: no core has lock
                                                         n>RBA_BSWSRV_MAX_PHYS_CORE: physical Core ((n-1)-RBA_BSWSRV_MAX_PHYS_CORE) has SimpleLock */
                                            /* bit 8-31: always 0 for SimpleLock */
} rba_BswSrv_SimpleLock_to;

/* end of #ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT */
#endif
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
#if (RBA_BSWSRV_CFG_COMPILER_LIBRARY_SUPPORT == STD_ON)

// Just using rba_BswSrv_MemCopy as an alias name for memcpy defined by the C-standard
#define rba_BswSrv_MemCopy  memcpy

// Just using rba_BswSrv_MemSet as an alias name for memset defined by the C-standard
#define rba_BswSrv_MemSet   memset

// rba_BswSrv_MemCompare cannot be used identical as memcmp because it has different return values but same parameters
#define rba_BswSrv_MemCompare(xSrc1_pcv,xSrc2_pcv,numBytes_u32)   ((0 != memcmp(xSrc1_pcv,xSrc2_pcv,numBytes_u32)) ? (1) : (0) )

#else

#define RBA_BSWSRV_START_SEC_CODE
#include "rba_BswSrv_MemMap.h"
extern void* rba_BswSrv_MemCopy(void* xDest_pv, const void* xSrc_pcv, uint32 numBytes_u32);
extern void* rba_BswSrv_MemSet(void* xDest_pv, sint32 xPattern_s32, uint32 numBytes_u32);
extern sint32 rba_BswSrv_MemCompare(const void* xSrc1_pcv, const void* xSrc2_pcv, uint32 numBytes_u32);
#define RBA_BSWSRV_STOP_SEC_CODE
#include "rba_BswSrv_MemMap.h"

#endif

LOCAL_INLINE uint32 rba_BswSrv_CountLeadingZero32(uint32 Input_u32);
LOCAL_INLINE uint32 rba_BswSrv_ByteOrderSwap32(uint32 Input_u32);
LOCAL_INLINE uint16 rba_BswSrv_ByteOrderSwap16(uint16 Input_u16);

LOCAL_INLINE void   rba_BswSrv_MemCopy32(uint32* xDest_pu32, const uint32* xSrc_pcu32, uint32 numBytes_u32);
LOCAL_INLINE void   rba_BswSrv_MemCopy16(uint16* xDest_pu16, const uint16* xSrc_pcu16, uint32 numBytes_u32);
LOCAL_INLINE void   rba_BswSrv_MemCopy8(uint8* xDest_pu8, const uint8* xSrc_pcu8, uint32 numBytes_u32);

LOCAL_INLINE uint32 rba_BswSrv_MemCompare32(const uint32* xSrc1_pcu32, const uint32* xSrc2_pcu32, uint32 numBytes_u32);
LOCAL_INLINE uint32 rba_BswSrv_MemCompare16(const uint16* xSrc1_pcu16, const uint16* xSrc2_pcu16, uint32 numBytes_u32);
LOCAL_INLINE uint32 rba_BswSrv_MemCompare8(const uint8* xSrc1_pcu8, const uint8* xSrc2_pcu8, uint32 numBytes_u32);

LOCAL_INLINE void   rba_BswSrv_MemSet32(uint32* xDest_pu32, uint32 xPattern_u32, uint32 numBytes_u32);
LOCAL_INLINE void   rba_BswSrv_MemSet16(uint16* xDest_pu16, uint32 xPattern_u32, uint32 numBytes_u32);
LOCAL_INLINE void   rba_BswSrv_MemSet8(uint8* xDest_pu8, uint32 xPattern_u32, uint32 numBytes_u32);

#if(RBA_BSWSRV_CFG_MACHINE_SUPPORT_64_BIT == STD_ON)
LOCAL_INLINE void   rba_BswSrv_MemCopy64(rba_BswSrv_uint64* xDest_pu64, const rba_BswSrv_uint64* xSrc_pcu64, uint32 numBytes_u32);
LOCAL_INLINE void   rba_BswSrv_MemSet64(rba_BswSrv_uint64* xDest_pu64, rba_BswSrv_uint64 xPattern_u64, uint32 numBytes_u32);
LOCAL_INLINE uint32 rba_BswSrv_MemCompare64(const rba_BswSrv_uint64* xSrc1_pcu64, const rba_BswSrv_uint64* xSrc2_pcu64, uint32 numBytes_u32);
#endif

#ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT
LOCAL_INLINE void rba_BswSrv_SuspendCoreLocalInt(void);
LOCAL_INLINE void rba_BswSrv_ResumeCoreLocalInt(void);
LOCAL_INLINE void rba_BswSrv_GetLockCommon(void);
LOCAL_INLINE void rba_BswSrv_ReleaseLockCommon(void);
LOCAL_INLINE void rba_BswSrv_GetLockNonest(rba_BswSrv_Lock_to* Lock_pst);
LOCAL_INLINE void rba_BswSrv_ReleaseLockNonest(rba_BswSrv_Lock_to* Lock_pst);
LOCAL_INLINE uint8_least rba_BswSrv_TryToGetSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst);
LOCAL_INLINE void rba_BswSrv_ReleaseSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst);

#define RBA_BSWSRV_START_SEC_CODE
#include "rba_BswSrv_MemMap.h"
extern uint8_least rba_BswSrv_PrvTryToGetSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst);
extern void rba_BswSrv_PrvReleaseSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst);
extern void rba_BswSrv_PrvSuspendCoreLocalInt(void);
extern void rba_BswSrv_PrvResumeCoreLocalInt(void);
extern void rba_BswSrv_PrvGetLockCommon(void);
extern void rba_BswSrv_PrvReleaseLockCommon(void);
extern void rba_BswSrv_PrvGetLockNonest(rba_BswSrv_Lock_to* Lock_pst);
extern void rba_BswSrv_PrvReleaseLockNonest(rba_BswSrv_Lock_to* Lock_pst);
#define RBA_BSWSRV_STOP_SEC_CODE
#include "rba_BswSrv_MemMap.h"

/* end of #ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT */
#endif
/*
 ***************************************************************************************************
 * Inline function definitions
 ***************************************************************************************************
 */

/**
 *********************************************************************
 * rba_BswSrv_CountLeadingZero32
 *
 * count leading zeros in 32 bit value.
 *
 * \param   Input_u32: input value
 * \return  number of leading zeros
 * \retval  uint32
 * \seealso
 * \usedresources
 *********************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE uint32 rba_BswSrv_CountLeadingZero32(uint32 Input_u32)
{
    uint32 numLeadingZero_u32;

    /* count leading zeros */
    numLeadingZero_u32 = 32;
    while(Input_u32 != 0u)
    {
        Input_u32 >>= 1;
        numLeadingZero_u32--;
    }
    /* MR12 RULE 9.1 VIOLATION: variable is set by inline assembly but the checker does not understand this */
    return numLeadingZero_u32;
}

/**
 *********************************************************************
 * rba_BswSrv_ByteOrderSwap32
 *
 * Swaps the byte order of a 32 bit value for endianess conversion.
 *
 * \param   Input_u32: input value
 * \return  value with swapped byte order
 * \retval  uint32
 * \seealso
 * \usedresources
 *********************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE uint32 rba_BswSrv_ByteOrderSwap32(uint32 Input_u32)
{
    uint32 retVal_u32;

    retVal_u32 = (Input_u32 << 24) | ((Input_u32 & 0xFF00u) << 8) | ((Input_u32 & 0x00FF0000u) >> 8) | (Input_u32 >> 24);
    /* MR12 RULE 9.1 VIOLATION: variable is set by inline assembly but the checker does not understand this */
    return retVal_u32;
}

/**
 *********************************************************************
 * rba_BswSrv_ByteOrderSwap16
 *
 * Swaps the byte order of a 16 bit value for endianess conversion.
 *
 * \param   Input_u16: input value
 * \return  value with swapped byte order
 * \retval  uint16
 * \seealso
 * \usedresources
 *********************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE uint16 rba_BswSrv_ByteOrderSwap16(uint16 Input_u16)
{
    uint16 retVal_u16;

    retVal_u16 = ((Input_u16 & 0x00FFu) << 8) | ((Input_u16 & 0xFF00u) >> 8);
    /* MR12 RULE 9.1 VIOLATION: variable is set by inline assembly but the checker does not understand this */
    return retVal_u16;
}


/**
 ***************************************************************************************************
 * rba_BswSrv_MemCopy64 - copy non overlapping memory in 64 Bit blocks
 *
 * Memory copy routine. Source and destination must not overlap. Pointer parameters must be 8 Byte
 * aligned. The size should be a multiple of 8. If not, it is rounded down to the next multiple
 * of 8.
 *
 * \param   xDest_pu64      destination address
 * \param   xSrc_pcu64      source address
 * \param   numBytes_u32    number of bytes to be copied
 * \seealso     rba_BswSrv_MemCopy8, rba_BswSrv_MemCopy16, rba_BswSrv_MemCopy32
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
#if(RBA_BSWSRV_CFG_MACHINE_SUPPORT_64_BIT == STD_ON)
LOCAL_INLINE void rba_BswSrv_MemCopy64(rba_BswSrv_uint64* xDest_pu64, const rba_BswSrv_uint64* xSrc_pcu64, uint32 numBytes_u32)
{
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < (numBytes_u32 / 8u); ctLoop_u32++)
    {
        *xDest_pu64 = *xSrc_pcu64;
        xDest_pu64++;
        xSrc_pcu64++;
    }
    return;
}
#endif
/**
 ***************************************************************************************************
 * rba_BswSrv_MemCopy32 - copy non overlapping memory in 32 Bit blocks
 *
 * Memory copy routine. Source and destination must not overlap. Pointer parameters must be 4 Byte
 * aligned. The size should be a multiple of 4. If not, it is rounded down to the next multiple
 * of 4.
 *
 * \param   xDest_pu32      destination address
 * \param   xSrc_pcu32      source address
 * \param   numBytes_u32    number of bytes to be copied
 * \seealso     rba_BswSrv_MemCopy8, rba_BswSrv_MemCopy16, rba_BswSrv_MemCopy64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE void rba_BswSrv_MemCopy32(uint32* xDest_pu32, const uint32* xSrc_pcu32, uint32 numBytes_u32)
{
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0u; ctLoop_u32 < (numBytes_u32 / 4u); ctLoop_u32++)
    {
        *xDest_pu32 = *xSrc_pcu32;
        xDest_pu32++;
        xSrc_pcu32++;
    }

    return;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemCopy16 - copy non overlapping memory in 16 Bit blocks
 *
 * Memory copy routine. Source and destination must not overlap. Pointer parameters must be 2 Byte
 * aligned. The size should be a multiple of 2. If not, it is rounded down to the next multiple
 * of 2.
 *
 * \param   xDest_pu16      destination address
 * \param   xSrc_pcu16      source address
 * \param   numBytes_u32    number of bytes to be copied
 * \seealso     rba_BswSrv_MemCopy8, rba_BswSrv_MemCopy32, rba_BswSrv_MemCopy64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE void rba_BswSrv_MemCopy16(uint16* xDest_pu16, const uint16* xSrc_pcu16, uint32 numBytes_u32)
{
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0u; ctLoop_u32 < (numBytes_u32 / 2u); ctLoop_u32++)
    {
        *xDest_pu16 = *xSrc_pcu16;
        xDest_pu16++;
        xSrc_pcu16++;
    }

    return;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemCopy8 - copy non overlapping memory
 *
 * Memory copy routine. Source and destination must not overlap.
 *
 * \param   xDest_pu8       destination address
 * \param   xSrc_pcu8       source address
 * \param   numBytes_u32    number of bytes to be copied
 * \seealso     rba_BswSrv_MemCopy16, rba_BswSrv_MemCopy32, rba_BswSrv_MemCopy64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE void rba_BswSrv_MemCopy8(uint8* xDest_pu8, const uint8* xSrc_pcu8, uint32 numBytes_u32)
{
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < numBytes_u32; ctLoop_u32++)
    {
        *xDest_pu8 = *xSrc_pcu8;
        xDest_pu8++;
        xSrc_pcu8++;
    }

    return;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemCompare64 - compare memory in 64 Bit blocks
 *
 * Memory compare routine. Pointer parameters must be 8 Byte aligned. The size should be a
 * multiple of 8. If not, it is rounded down to the next multiple of 8.
 *
 * \param   xSrc1_pcu32     first source address
 * \param   xSrc2_pcu32     second source address
 * \param   numBytes_u32    number of bytes to be compared
 * \return  stEqual_u32     result of comparison
 * \retval  0               memory area contents are equal
 * \retval  1               memory area contents differ
 * \seealso     rba_BswSrv_MemCompare8, rba_BswSrv_MemCompare16, rba_BswSrv_MemCompare32
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
#if(RBA_BSWSRV_CFG_MACHINE_SUPPORT_64_BIT == STD_ON)
LOCAL_INLINE uint32 rba_BswSrv_MemCompare64(const rba_BswSrv_uint64* xSrc1_pcu64, const rba_BswSrv_uint64* xSrc2_pcu64, uint32 numBytes_u32)
{
    uint32 stEqual_u32 = 0u;
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < (numBytes_u32 / 8u); ctLoop_u32++)
    {
        /* MR12 RULE 13.3 VIOLATION: The warning is not critical and non-removable since value using from postfix increment is required for code optimization */
        if (*xSrc1_pcu64++ != *xSrc2_pcu64++)
        {
            stEqual_u32 = 1u;
            break;
        }
    }

    return stEqual_u32;
}
#endif

/**
 ***************************************************************************************************
 * rba_BswSrv_MemCompare32 - compare memory in 32 Bit blocks
 *
 * Memory compare routine. Pointer parameters must be 4 Byte aligned. The size should be a
 * multiple of 4. If not, it is rounded down to the next multiple of 4.
 *
 * \param   xSrc1_pcu32     first source address
 * \param   xSrc2_pcu32     second source address
 * \param   numBytes_u32    number of bytes to be compared
 * \return  stEqual_u32     result of comparison
 * \retval  0               memory area contents are equal
 * \retval  1               memory area contents differ
 * \seealso     rba_BswSrv_MemCompare8, rba_BswSrv_MemCompare16, rba_BswSrv_MemCompare64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE uint32 rba_BswSrv_MemCompare32(const uint32* xSrc1_pcu32, const uint32* xSrc2_pcu32, uint32 numBytes_u32)
{
    uint32 stEqual_u32 = 0u;
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0u; ctLoop_u32 < (numBytes_u32 / 4u); ctLoop_u32++)
    {
        /* MR12 RULE 13.3 VIOLATION: The warning is not critical and non-removable since value using from postfix increment is required for code optimization */
        if (*xSrc1_pcu32++ != *xSrc2_pcu32++)
        {
            stEqual_u32 = 1u;
            break;
        }
    }
    return stEqual_u32;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemCompare16 - compare memory in 16 Bit blocks
 *
 * Memory compare routine. Pointer parameters must be 2 Byte aligned. The size should be a
 * multiple of 2. If not, it is rounded down to the next multiple of 2.
 *
 * \param   xSrc1_pcu16     first source address
 * \param   xSrc2_pcu16     second source address
 * \param   numBytes_u32    number of bytes to be compared
 * \return  stEqual_u32     result of comparison
 * \retval  0               memory area contents are equal
 * \retval  1               memory area contents differ
 * \seealso     rba_BswSrv_MemCompare8, rba_BswSrv_MemCompare32, rba_BswSrv_MemCompare64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE uint32 rba_BswSrv_MemCompare16(const uint16* xSrc1_pcu16, const uint16* xSrc2_pcu16, uint32 numBytes_u32)
{
    uint32 stEqual_u32 = 0u;
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < (numBytes_u32 / 2u); ctLoop_u32++)
    {
        /* MR12 RULE 13.3 VIOLATION: The warning is not critical and non-removable since value using from postfix increment is required for code optimization */
        if (*xSrc1_pcu16++ != *xSrc2_pcu16++)
        {
            stEqual_u32 = 1u;
            break;
        }
    }
    return stEqual_u32;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemCompare8 - compare memory
 *
 * Memory compare routine.
 *
 * \param   xSrc1_pcu8      pointer to the first source address
 * \param   xSrc2_pcu8      pointer to the second source address
 * \param   numBytes_u32    number of bytes to be compared
 * \return  stEqual_u32     result of comparison
 * \retval  0               memory area contents are equal
 * \retval  1               memory area contents differ
 * \seealso     rba_BswSrv_MemCompare16, rba_BswSrv_MemCompare32, rba_BswSrv_MemCompare64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE uint32 rba_BswSrv_MemCompare8(const uint8* xSrc1_pcu8, const uint8* xSrc2_pcu8, uint32 numBytes_u32)
{
    uint32 stEqual_u32 = 0u;
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < numBytes_u32; ctLoop_u32++)
    {
        /* MR12 RULE 13.3 VIOLATION:The warning is not critical and non-removable since value using from postfix increment is required for code optimization */
        if (*xSrc1_pcu8++ != *xSrc2_pcu8++)
        {
            stEqual_u32 = 1u;
            break;
        }
    }
    return stEqual_u32;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemSet64 - initialize memory with 64 Bit data
 *
 * Memory initialization routine. 64 Bit Pattern is written to the destination.
 * Destination must be 4 Byte aligned. The size should be a multiple of 8. If not, it is rounded
 * down to the next multiple of 8.
 *
 * \param   xDest_pu32      destination address
 * \param   Pattern_u32     fill pattern
 * \param   numBytes_u32    number of bytes to be initialized
 * \seealso     rba_BswSrv_MemSet8, rba_BswSrv_MemSet16, rba_BswSrv_MemSet32
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
#if(RBA_BSWSRV_CFG_MACHINE_SUPPORT_64_BIT == STD_ON)
LOCAL_INLINE void rba_BswSrv_MemSet64(rba_BswSrv_uint64* xDest_pu64, rba_BswSrv_uint64 xPattern_u64, uint32 numBytes_u32)
{

    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < (numBytes_u32 / 8u); ctLoop_u32++)
    {
        *xDest_pu64 = xPattern_u64;
        xDest_pu64++;
    }

    return;
}
#endif
/**
 ***************************************************************************************************
 * rba_BswSrv_MemSet32 - initialize memory with 32 Bit data
 *
 * Memory initialization routine. 32 Bit Pattern is written to the destination.
 * Destination must be 4 Byte aligned. The size should be a multiple of 4. If not, it is rounded
 * down to the next multiple of 4.
 *
 * \param   xDest_pu32      destination address
 * \param   Pattern_u32     fill pattern
 * \param   numBytes_u32    number of bytes to be initialized
 * \seealso     rba_BswSrv_MemSet8, rba_BswSrv_MemSet16, rba_BswSrv_MemSet64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE void rba_BswSrv_MemSet32(uint32* xDest_pu32, uint32 xPattern_u32, uint32 numBytes_u32)
{
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < (numBytes_u32 / 4u); ctLoop_u32++)
    {
        *xDest_pu32 = xPattern_u32;
        xDest_pu32++;
    }
    return;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemSet16 - initialize memory with 16 Bit data
 *
 * Memory initialization routine. 16 Bit Pattern is written to the destination.
 * Destination must be 2 Byte aligned. The size should be a multiple of 2. If not, it is rounded
 * down to the next multiple of 2.
 *
 * \param   xDest_pu32      destination address
 * \param   Pattern_u32     fill pattern
 * \param   numBytes_u32    number of bytes to be initialized
 * \seealso     rba_BswSrv_MemSet8, rba_BswSrv_MemSet32, rba_BswSrv_MemSet64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE void rba_BswSrv_MemSet16(uint16* xDest_pu16, uint32 xPattern_u32, uint32 numBytes_u32)
{
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < (numBytes_u32 / 2u); ctLoop_u32++)
    {
        *xDest_pu16 = (uint16)xPattern_u32;
        xDest_pu16++;
    }
    return;
}

/**
 ***************************************************************************************************
 * rba_BswSrv_MemSet8 - initialize memory with 8 Bit data
 *
 * Memory initialization routine. 8 Bit Pattern is written to the destination.
 *
 * \param   xDest_pu8       destination address
 * \param   xPattern_u32    fill pattern, only lower 8 bit used
 * \param   numBytes_u32    number of bytes to be initialized
 * \seealso     rba_BswSrv_MemSet16, rba_BswSrv_MemSet32, rba_BswSrv_MemSet64
 ***************************************************************************************************
 */
/* MR12 RULE 8.6 VIOLATION: INLINE functions will be defined in Header */
LOCAL_INLINE void rba_BswSrv_MemSet8(uint8* xDest_pu8, uint32 xPattern_u32, uint32 numBytes_u32)
{
    uint32 ctLoop_u32;

    for(ctLoop_u32 = 0; ctLoop_u32 < numBytes_u32; ctLoop_u32++)
    {
        *xDest_pu8 = (uint8)xPattern_u32;
        xDest_pu8++;
    }
    return;
}

#ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT
/**
 *********************************************************************
 * rba_BswSrv_TryToGetSimpleLock
 *
 * Try to get a SimpleLock without interrupt suspend.
 *********************************************************************
 */
LOCAL_INLINE uint8_least rba_BswSrv_TryToGetSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst)
{
    uint8_least stReturn_qu8;
    stReturn_qu8 = rba_BswSrv_PrvTryToGetSimpleLock(Lock_pst);
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */

    return stReturn_qu8;
}

/**
 *********************************************************************
 * rba_BswSrv_ReleaseSimpleLock
 *
 * release a SimpleLock without interrupt suspend.
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_ReleaseSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst)
{
    rba_BswSrv_PrvReleaseSimpleLock(Lock_pst);
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */
}

/**
 *********************************************************************
 * rba_BswSrv_SuspendCoreLocalInt
 *
 * Suspend interrupt processing of current core
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_SuspendCoreLocalInt(void)
{
    rba_BswSrv_PrvSuspendCoreLocalInt();
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */
}

/**
 *********************************************************************
 * rba_BswSrv_ResumeCoreLocalInt
 *
 * Resume interrupt processing of current core
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_ResumeCoreLocalInt(void)
{
    rba_BswSrv_PrvResumeCoreLocalInt();
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */
}

/**
 *********************************************************************
 * rba_BswSrv_GetLockCommon
 *
 * Get common spinlock. Must not be called, if a nonest lock is already
 * active for the current core.
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_GetLockCommon(void)
{
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
    rba_BswSrv_PrvGetLockCommon();
#else
    rba_BswSrv_PrvSuspendCoreLocalInt();
#endif
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */
}

/**
 *********************************************************************
 * rba_BswSrv_ReleaseLockCommon
 *
 * Release common spinlock. Must not be called, if a nonest lock is already
 * active for the current core.
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_ReleaseLockCommon(void)
{
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
    rba_BswSrv_PrvReleaseLockCommon();
#else
    rba_BswSrv_PrvResumeCoreLocalInt();
#endif
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */
}

/**
 *********************************************************************
 * rba_BswSrv_GetLockNonest
 *
 * Get specified nonest lock. Must not be called, if a different nonest lock
 * is already active for the current core.
 *
 * \param   Lock_pst        pointer to nonest lock
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_GetLockNonest(rba_BswSrv_Lock_to* Lock_pst)
{
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
    rba_BswSrv_PrvGetLockNonest(Lock_pst);
#else
    RBA_BSWSRV_PARAM_UNUSED(Lock_pst);
    rba_BswSrv_PrvSuspendCoreLocalInt();
#endif
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */
}

/**
 *********************************************************************
 * rba_BswSrv_ReleaseLockNonest
 *
 * Release specified nonest lock. Must not be called, if a different nonest lock
 * is already active for the current core.
 *
 * \param   Lock_pst        pointer to nonest lock
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_ReleaseLockNonest(rba_BswSrv_Lock_to* Lock_pst)
{
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
    rba_BswSrv_PrvReleaseLockNonest(Lock_pst);
#else
    RBA_BSWSRV_PARAM_UNUSED(Lock_pst);
    rba_BswSrv_PrvResumeCoreLocalInt();
#endif
    RBA_BSWSRV_SET_SEQUENCE_POINT();  /* forces call instead of jump to previous function */
}
/* end of #ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT */
#endif
/* RBA_BSWSRV_H */
#endif
