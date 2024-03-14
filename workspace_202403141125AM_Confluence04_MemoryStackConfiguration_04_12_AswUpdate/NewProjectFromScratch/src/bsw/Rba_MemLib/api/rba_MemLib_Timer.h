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

#ifndef RBA_MEMLIB_TIMER_H
#define RBA_MEMLIB_TIMER_H

#include "Std_Types.h"
#include "rba_MemLib_Cfg.h"

/*
 **********************************************************************************************************************
 * This header is used to access the low-level timer interfaces
 * The functionality is only working if
 * RBA_MEMLIB_USE_TIMER == STD_ON
 **********************************************************************************************************************
 */

/** ***********************************************************************
 * \brief   Get timer-ticks from a free-running/wrap-around timer (e.g. system-timer)
 * \return  current timer-ticks (resolution <= 1us/LSB), value range might be smaller than 32bit
*/
#if (RBA_MEMLIB_USE_TIMER != STD_OFF)
#define rba_MemLib_GetTimerTicks()    rba_MemLib_Callout_GetTimerTicks()
#else
#define rba_MemLib_GetTimerTicks()                      (0u)
#endif

/** ***********************************************************************
 * \brief   Convert microseconds to timer-ticks
 * \param   tiUs_u32  microsecond value (1us/LSB resolution), passed values must be <=100000us.
 * \return  Microseconds converted to timer-ticks
*/
#if (RBA_MEMLIB_USE_TIMER != STD_OFF)
#define  rba_MemLib_ConvertUsToTimerTicks(tiUs_u32)  rba_MemLib_Callout_ConvertUsToTimerTicks(tiUs_u32)
#else
#define rba_MemLib_ConvertUsToTimerTicks(tiUs_u32)      (0u)
#endif

/*
 **********************************************************************************************************************
 * Code part
 **********************************************************************************************************************
*/
#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"

/*
 **********************************************************************************************************************
 * Declaration for timer functions (for external usage)
 * Realized as inline function for optimization
 **********************************************************************************************************************
*/

/** ***********************************************************************
 * \brief   Convert a timer-difference in timer-ticks to microseconds(us)
 * \param   Timer-ticks
 * \return  Timer-ticks converted to 1us/LSB resolution
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_ConvertTimerTicksToUs(uint32 tiTicks_u32);

/** ***********************************************************************
 * \brief   Calculate time difference of two time stamps captured
 *          via rba_MemLib_GetTimerTicks()
 *          Wrap around is considered.
 * \param   tiCurr Timer-ticks of current time reading
 * \param   tiRef  Timer-ticks of reference
 * \return  Difference of tiCurr - tiRef with considered wrap-around
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_CalcDiffInTimerTicks(uint32 tiCurr_u32, uint32 tiRef_u32);


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/



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
#include "rba_MemLib_Timer_Inl.h"

#endif      /* RBA_MEMLIB_TIMER_H */

