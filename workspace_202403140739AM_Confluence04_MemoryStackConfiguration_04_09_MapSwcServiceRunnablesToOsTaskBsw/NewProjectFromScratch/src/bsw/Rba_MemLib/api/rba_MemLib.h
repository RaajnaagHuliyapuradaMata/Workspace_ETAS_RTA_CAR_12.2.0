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

#ifndef RBA_MEMLIB_H
#define RBA_MEMLIB_H

#include "Std_Types.h"
#include "rba_MemLib_Cfg.h"


/** ***********************************************************************
 *  Check for macro plausibility
 */
#if (defined(RBA_MEMLIB_VECU_ENABLE) && (RBA_MEMLIB_VECU_ENABLE != STD_ON))
/* VARIANT_EVAL_BUILD_ERROR */
#error Macro RBA_MEMLIB_VECU_ENABLE is wrongly defined
#endif


/** ***********************************************************************
 *  Type definition of an integer which can hold the current value of a
 *  pointer: rba_MemLib_UintPtr_tuo
 */

#if (defined(RBA_MEMLIB_VECU_ENABLE))
#include <stdint.h>
typedef uintptr_t  rba_MemLib_UintPtr_tuo;
#else
typedef uint32  rba_MemLib_UintPtr_tuo;
#endif




/**********************************************************************************************************************
COMPILE TIME ASSERTS
**********************************************************************************************************************/
#define RBA_MEMLIB_ASSERT(cdn, msg)     typedef unsigned char rba_MemLib_##msg[(cdn) ? 1 : -1];


/** ***********************************************************************
 * \brief   Information to other modules that the timer functionality is available
 * STD_ON:  Timer Functionality is available
 * STD_OFF: System has no timer functionality implemented
 *
 * Macro: RBA_MEMLIB_USE_TIMER
*/

/*
 **********************************************************************************************************************
 * Code part
 **********************************************************************************************************************
*/
#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"


/*
 **********************************************************************************************************************
 *Declaration for timer functions (for external usage)
 * Realized as inline function for optimization
 **********************************************************************************************************************
*/

/** ***********************************************************************
 * \brief   Get timer-ticks from a free-running virtually calculated 1us timer
 *          When using this timer, it is necessary that
 *          rba_MemLib_UpdateTimerUs is called sooner than the internal timer
 *          wraps around
 * \return  current timer-ticks (resolution 1us/LSB) (full 32bit range)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_GetTimerUs(void);

/*
 **********************************************************************************************************************
 * Declarations of Memory-Operation functions
 **********************************************************************************************************************
*/

/** ******************************************************************************************
 * \brief               Copy data from src_pcu8 to dst_pu8 in uint8 steps (increasing addresses, no overlap of src/dest)
 * \param   src_pcu8    Pointer to the source buffer - no alignment constraint
 * \param   dst_pu8     Pointer to the destination buffer - no alignment constraint
 * \param   length_u32  Number of bytes to copy (0 allowed)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_MemCopy(uint8 const *src_pcu8, uint8 *dst_pu8, uint32 length_u32);

/** ******************************************************************************************
 * \brief               Set a buffer to a defined value
 * \param   dst_pu8     Pointer to the destination buffer - no alignment constraint
 * \param   value_u8    Value to be set
 * \param   length_u32  Number of bytes to copy (0 allowed)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_MemSet(uint8 * dst_pu8, uint8 value_u8, uint32 length_u32);

/** ******************************************************************************************
 * \brief               Compare two buffers
 *                      note: if length=0, then return=TRUE
 * \param   bfr1_pcu8   Pointer to buffer 1 - no alignment constraint
 * \param   bfr2_pcu8   Pointer to buffer 2 - no alignment constraint
 * \param   length_u32  Number of bytes to compare (0 allowed)
 * \retval  TRUE        Buffers are fully equal
 * \retval  FALSE       At least one entry in the buffers are not same
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE boolean rba_MemLib_MemCompareBfr(uint8 const * bfr1_pcu8, uint8 const * bfr2_pcu8, uint32 length_u32);

/** ******************************************************************************************
 * \brief       Compare a buffer to a defined value
 * \param   bfr_pcu8    Pointer to the buffer - no alignment constraint
 * \param   value_u8    Value that should be compared to
 * \param   length_u32  Amount of bytes to check
 * \retval  TRUE        Whole buffer contains only value_u8
 * \retval  FALSE       At least one entry in the buffer is not equal to value_u8
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE boolean rba_MemLib_MemCompareValue(uint8 const * bfr_pcu8, uint8 value_u8, uint32 length_u32);

/*
 **********************************************************************************************************************
 * Declarations of Other MemLib functions
 **********************************************************************************************************************
*/

/** ******************************************************************************************
 * \brief   Get the maximum of 2 values
 * \return  MAX(a,b)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Max(uint32 a_u32, uint32 b_u32);

/** ******************************************************************************************
 * \brief   Get the minimum of 2 values
 * \return  MIN(a,b)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Min(uint32 a_u32, uint32 b_u32);

/** ******************************************************************************************
 * \brief   Compare a passed val_u32 to a threshold (thres_u32)
 *          if it is reach or exceeded a LimRes_u32 value is set
 *          otherwise the passed val_u32 is returned
 *          Example:
 *          rba_MemLib_LimHigh(value,10u,ERROR)    will return ERROR if value>=10u
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_LimHigh(uint32 val_u32, uint32 thres_u32, uint32 LimRes_u32);

/** ******************************************************************************************
 * \brief   Compare a passed val_u32 to a threshold (thres_u32)
 *          if it is reach or below a LimRes_u32 value is set
 *          otherwise the passed val_u32 is returned
 *          Example:
 *          rba_MemLib_LimLow(value,10u,ERROR)    will return ERROR if value<=10u
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_LimLow(uint32 val_u32, uint32 thres_u32, uint32 LimRes_u32);

/** ******************************************************************************************
 * \brief               Copy data from A to B in uint8 steps (increasing)
 * \param   src_pcu8    Pointer to the source buffer - no alignment constraint
 * \param   dst_pu8     Pointer to the destination buffer - no alignment constraint
 * \param   length_u32  Number of bytes to copy (0 allowed)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_MemCopyU8U8(uint8 const * src_pcu8, uint8 * dst_pu8, uint32 length_u32);

/** ******************************************************************************************
 * \brief               Copy data from A to B in uint32 steps (assuming 32bit alignment, increasing)
 * \param   src_pcu32   Pointer to the source buffer - uint32 alignment required
 * \param   dst_pu32    Pointer to the destination buffer - uint32 alignment required
 * \param   length_u32  Number of bytes to copy
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_MemCopyU32U32(uint32 const * src_pcu32, uint32 * dst_pu32, uint32 length_u32);

/** ******************************************************************************************
 * \brief               Copy data from A to B in uint8 steps (decreasing)
 * \param   srcEnd1_pcu8    Pointer to the source buffer END+1 - no alignment constraint
 * \param   dstEnd1_pu8     Pointer to the destination buffer END+1 - no alignment constraint
 * \param   length_u32  Number of bytes to copy (0 allowed)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_MemCopyU8U8_Dec(uint8 const * srcEnd1_pcu8, uint8 * dstEnd1_pu8, uint32 length_u32);

/** ******************************************************************************************
 * \brief               Copy data from A to B in uint32 steps (assuming 32bit alignment, decreasing)
 * \param   srcEnd1_pcu32   Pointer to the source buffer END+1 - uint32 alignment required
 * \param   dstEnd1_pu32    Pointer to the destination buffer END+1 - uint32 alignment required
 * \param   length_u32  Number of bytes to copy
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_MemCopyU32U32_Dec(uint32 const * srcEnd1_pcu32, uint32 * dstEnd1_pu32, uint32 length_u32);

/** ******************************************************************************************
 * \brief   Convert data from 4 bytes-array big endian to native uint32
 * (first byte is highest-value byte)
 *
 * \param   input_pcu8   pointer to a 4byte array (input value)
 * \return  converted 32bit data
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Cnv4ByteBigEndToNative_u32(uint8 const * input_pcu8);

/** ******************************************************************************************
 * \brief   Convert data from 3 bytes-array big endian to native 24bit value
 * (first byte is highest-value byte)
 *
 * \param   input_pcu8   pointer to a 3byte array (input value)
 * \return  converted 24bit data (stored in 32bit return value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Cnv3ByteBigEndToNative_u32(uint8 const * input_pcu8);

/** ******************************************************************************************
 * \brief   Convert data from 2 bytes big endian to native uint16
 * (first byte is highest-value byte)
 *
 * \param   input_pcu8   pointer to a 2byte array (input value)
 * \return  converted 16bit data
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint16 rba_MemLib_Cnv2ByteBigEndToNative_u16(uint8 const * input_pcu8);

/** ******************************************************************************************
 * \brief   Convert data from 4 bytes little endian to native uint32
 * (first byte is lowest-value byte)
 *
 * \param   input_pcu8   pointer to a 4byte array (input value)
 * \return  converted 32bit data
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Cnv4ByteLtlEndToNative_u32(uint8 const* input_pcu8);

/** ******************************************************************************************
 * \brief   Convert data from 3 bytes-array little endian to native 24bit value
 * (first byte is lowest-value byte)
 *
 * \param   input_pcu8   pointer to a 3byte array (input value)
 * \return  converted 24bit data (stored in 32bit return value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Cnv3ByteLtlEndToNative_u32(uint8 const * input_pcu8);

/** ******************************************************************************************
 * \brief   Convert data from 2 bytes little endian to native uint16
 * (first byte is lowest-value byte)
 *
 * \param   input_pcu8   pointer to a 2byte array (input value)
 * \return  converted 16bit data
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint16 rba_MemLib_Cnv2ByteLtlEndToNative_u16(uint8 const* input_pcu8);

/** ******************************************************************************************
 * \brief   Convert data from native uint32 to 4 bytes big endian array
 * (first byte is highest-value byte)
 *
 * \param   input_u32     32bit input value
 * \param   output_pu8    pointer to a 4byte array (output value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_CnvNativeToBigEnd4Byte(uint32 input_u32, uint8 * output_pu8);

/** ******************************************************************************************
 * \brief   Convert data from native 24bit-value to 3 bytes big endian array
 * (first byte is highest-value byte)
 *
 * \param   input_u32     24bit input value (value has to fit, not limited)
 * \param   output_pu8    pointer to a 3byte array (output value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_CnvNativeToBigEnd3Byte(uint32 input_u32, uint8 * output_pu8);

/** ******************************************************************************************
 * \brief   Convert data from native uint16 to 2 bytes big endian array
 * (first byte is highest-value byte)
 *
 * \param   input_u16     16bit input value
 * \param   output_pu8    pointer to a 2byte array (output value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_CnvNativeToBigEnd2Byte(uint16 input_u16, uint8 * output_pu8);

/** ******************************************************************************************
 * \brief   Convert data from native uint32 to 4 bytes little endian
 * (first byte is lowest-value byte)
 *
 * \param   input_u32     32bit input data
 * \param   output_pu8    pointer to a 4byte array (output value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_CnvNativeToLtlEnd4Byte(uint32 input_u32, uint8 *output_pu8);

/** ******************************************************************************************
 * \brief   Convert data from native 24bit-value to 3 bytes little endian array
 * (first byte is highest-value byte)
 *
 * \param   input_u32     24bit input value (value has to fit, not limited)
 * \param   output_pu8    pointer to a 3byte array (output value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_CnvNativeToLtlEnd3Byte(uint32 input_u32, uint8 * output_pu8);

/** ******************************************************************************************
 * \brief   Convert data from native uint16 to 2 bytes little endian
 * (first byte is lowest-value byte)
 *
 * \param   input_u16     16bit input data
 * \param   output_pu8    pointer to a 2byte array (output value)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void rba_MemLib_CnvNativeToLtlEnd2Byte(uint16 input_u16, uint8 *output_pu8);

/** ******************************************************************************************
 * \brief   Compare two free-running 32bit up-counters and determine the newer one.
 *  The time difference between the two counters has to be less than 2**31
 *  otherwise the wrong counter is selected.
 *
 * \param   cntr0_u32   First counter
 * \param   cntr1_u32   Second counter
 *
 * \retval  The newest amoung both the counters
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_GetCounterValNew(uint32 cntr0_u32, uint32 cntr1_u32);

/** ******************************************************************************************
 * \brief   Compare two free-running 32bit up-counters and determine the older one.
 *  The time difference between the two counters has to be less than 2**31
 *  otherwise the wrong counter is selected.
 *
 * \param   cntr0_u32   First counter
 * \param   cntr1_u32   Second counter
 *
 * \retval  The oldest amoung both the counters
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_GetCounterValOld(uint32 cntr0_u32, uint32 cntr1_u32);

/** ******************************************************************************************
 * \brief   Calculate the remainder of a division
 *
 * \param   dividend_u32   numerator
 * \param   divisor_u32    denominator (should be greater than zero!)
 *
 * \retval  The remainder of the division dividend_u32 / divisor_u32 (i.e. dividend_u32 % divisor_u32)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Modulo(uint32 dividend_u32, uint32 divisor_u32);

/** ******************************************************************************************
 * \brief   Calculate the remainder of a division
 *
 * \param   dividend_u32   numerator
 * \param   powerof2_u32   denominator which is power of 2 (should be greater than zero!)
 *
 * \retval  The remainder of the division dividend_u32 / divisor_u32 (i.e. dividend_u32 % divisor_u32)
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Modulo2Power(uint32 dividend_u32, uint32 powerof2_u32);

/** ******************************************************************************************
 * \brief   Find out whether a value is located within a certain range
 *
 * \param   val_u32        Value to check
 * \param   loBound_u32    Lower bound
 * \param   hiBound_u32    Upper bound
 *
 * \pre     loBound_u32 <= hiBound_u32
 *
 * \post    if loBound_u32 <= val_u32 <= hiBound_u32 then true else false
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE boolean rba_MemLib_IsValueInRange(uint32 val_u32, uint32 loBound_u32, uint32 hiBound_u32);

/** ******************************************************************************************
 * \brief   Round down an value to the next border.
 *
 * \param   value_u32   Value to round down
 * \param   unit_u32    Unit to which value_u32 should be rounded down - must be power of 2!
 *
 * \return  Value rounded down to the next border
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_FloorPwr2(uint32 value_u32, uint32 unit_u32);

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

/** ******************************************************************************************
 * \brief               to get the version info of the module
 * \param               VersioninfoPtr: Pointer to return the version
 *                          information of this module
*/
#if(defined(RBA_MEMLIB_VERSION_INFO) && (RBA_MEMLIB_VERSION_INFO != STD_OFF))
extern void rba_MemLib_GetVersionInfo(Std_VersionInfoType *VersionInfoPtr);
#endif

/** ******************************************************************************************
 * \brief               Copy data from src_pcu8 to dst_pu8 in uint8 steps (src and dst area might overlap)
 * \param   src_pcu8    Pointer to the source buffer - no alignment constraint
 * \param   dst_pu8     Pointer to the destination buffer - no alignment constraint
 * \param   length_u32  Number of bytes to copy (0 allowed)
*/
extern void rba_MemLib_MemCopyOverlap(uint8 const *src_pcu8, uint8 *dst_pu8, uint32 length_u32);

/** ******************************************************************************************
 * \brief       Function to update the internal us-timer of rba_MemLib
 *
*/
extern void rba_MemLib_UpdateTimerUs(void);

/** ******************************************************************************************
 * \brief       Function returns a 32bit pseudo-random number
 *              (initial seeding required)
 *              If nrRange_u16=0: then the full 32bit range is used for the random value
 *              else a random value in interval [0 ; nrRange_u16-1] is returned
 *
 *              Example: nrRange_u16=100: values from 0 to 99 are returned
*/
extern uint32 rba_MemLib_GetRandom_u32(uint16 nrRange_u16);

/** ******************************************************************************************
 * \brief       Random number seeding (call only once)
 *              Ideally, pass changing value like the system time
 *              If 0 is passed then a fixed internal seed is used
*/
extern void rba_MemLib_SetRandomSeed(uint32 nrSeed_u32);

/** ******************************************************************************************
 * \brief       Random number seeding (call only once)
 *              Determine whether the random generated is seeded already
 *              return=FALSE (=not seeded)
*/
extern boolean rba_MemLib_IsRandomSeeded(void);

/** ******************************************************************************************
 * \brief       Synchronuous write operations which ensures that
 *              data is actually stored to RAM (by performing a read-back)
 *              Note: Previous writes are completed before this write only if going to the same memory
 *              (execution duration corresponds to 2+1 copy-operations, incl. call)
*/
extern void rba_MemLib_SyncWrite8(volatile uint8* Var_pvu8, uint8 Value_u8);
extern void rba_MemLib_SyncWrite16(volatile uint16* Var_pvu16, uint16 Value_u16);
extern void rba_MemLib_SyncWrite32(volatile uint32* Var_pvu32, uint32 Value_u32);
extern void rba_MemLib_SyncWriteBool(volatile boolean* Var_pvb, boolean Value_b);

/** ******************************************************************************************
 * \brief    Provide memory barrier
 *           (execution duration corresponds to 0+1 copy-operations, incl. call)
*/
extern void rba_MemLib_MSync(void);

/** ******************************************************************************************
 * \brief   Find out whether two strings are equal
 *
 * \param   str1_pcchr      The first string
 * \param   str2_pcchr      The second string
 * \param   maxLength_u32   Never compare more than maxLength_u32 bytes to avoid any out of bound read
 *
 * \retval  TRUE    Both strings are equal until string terminator sign
 * \retval  FALSE   Both strings are not equal or maxLength_u32 reached
*/
extern boolean rba_MemLib_StrIsEqual(char const * str1_pcchr, char const * str2_pcchr, uint32 maxLength_u32);

/** ******************************************************************************************
 * \brief   Returns the length of a string
 *
 * \param   str_pcchr       The string
 * \param   maxLength_u32   Never inspect more than maxLength_u32 bytes to avoid any out of bound read
 *
 * \retval  The number of characters in the string, terminating \0 NOT included
 *          Will be == maxLength_u32 if the terminating \0 was not found
*/
extern uint32 rba_MemLib_StrLen(char const * str_pcchr, uint32 maxLength_u32);

/** ******************************************************************************************
 * \brief   Copy one string to another
 *
 * \param   srcStr_pcchr    The source string
 * \param   dstStr_pchr     The destination string
 * \param   maxLength_u32   Never copy more than maxLength_u32 bytes to avoid any out of bound write
 *
 * \retval  The number of characters copied, terminating \0 NOT included
 *          Will be == maxLength_u32 if the terminating \0 was not found
*/
extern uint32 rba_MemLib_StrCpy(char const * srcStr_pcchr, char * dstStr_pchr, uint32 maxLength_u32);

/** ******************************************************************************************
 * \brief   Concatenate two strings
 *
 * \param   srcStr_pcchr    The source string which shall be appended to dstStr
 * \param   dstStr_pchr     The destination string where the srcStr shall be appended at the end
 * \param   maxLength_u32   Never create a string longer than maxLength_u32 to avoid any out of bound write
 *
 * \retval  The number of characters in the final string, terminating \0 NOT included
 *          Will be == maxLength_u32 if the terminating \0 was not found
*/
extern uint32 rba_MemLib_StrCat(char const * srcStr_pcchr, char * dstStr_pchr, uint32 maxLength_u32);

#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"


/*
 **********************************************************************************************************************
 **********************************************************************************************************************
 * END of Public Part
 **********************************************************************************************************************
 **********************************************************************************************************************
*/




























/*
 **********************************************************************************************************************
 **********************************************************************************************************************
 * Inlines and internal functions  (do not use outside rba_MemLib)
 **********************************************************************************************************************
 **********************************************************************************************************************
*/

/* Include the actual definition of the provided inline functions */
#include "rba_MemLib_Inl.h"


#endif
