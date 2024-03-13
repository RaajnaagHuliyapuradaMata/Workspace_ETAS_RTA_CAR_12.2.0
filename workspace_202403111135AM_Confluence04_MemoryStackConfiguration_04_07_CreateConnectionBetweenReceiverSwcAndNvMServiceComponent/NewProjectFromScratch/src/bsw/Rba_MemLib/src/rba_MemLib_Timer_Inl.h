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

#ifndef RBA_MEMLIB_TIMER_INL_H
#define RBA_MEMLIB_TIMER_INL_H

#include "Std_Types.h"

/*
 **********************************************************************************************************************
 * Declarations - DO NOT USE OUTSIDE rba_MemLib!
 **********************************************************************************************************************
*/

#if RBA_MEMLIB_USE_TIMER != STD_OFF


#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"

/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_Callout_GetTimerTicks(void);
LOCAL_INLINE uint32 rba_MemLib_Callout_ConvertTimerTicksToUs(uint32 tiTicks_u32);
LOCAL_INLINE uint32 rba_MemLib_Callout_ConvertUsToTimerTicks(uint32 tiUs_u32);

#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"

/*
 **********************************************************************************************************************
 * Project Extensions/replacements
 **********************************************************************************************************************
*/

/* Include the configured template file that implements parts of the actual functionality */
#include "rba_MemLib_Cfg_Timer_Callouts.h"
#endif

/*
 **********************************************************************************************************************
 * Default values for silent macros to be defined in rba_MemLib_Cfg_Timer_Callouts.h
 **********************************************************************************************************************
*/
/* Selection of the timer check per default */
#ifndef RBA_MEMLIB_USE_TIMER_CHECK
#define RBA_MEMLIB_USE_TIMER_CHECK STD_ON
#endif

#ifndef RBA_MEMLIB_BYPASS_TIMER_US
#define RBA_MEMLIB_BYPASS_TIMER_US  (STD_OFF)
#endif

/*
 **********************************************************************************************************************
 * Defines (derived from configured template)
 **********************************************************************************************************************
*/

#if RBA_MEMLIB_USE_TIMER != STD_OFF
/* Definition of the Bitmask for wrap-around handling */
#if  (!defined(RBA_MEMLIB_TIMERBITSIZE)) || (RBA_MEMLIB_TIMERBITSIZE<20u) || (RBA_MEMLIB_TIMERBITSIZE>32u)
/* VARIANT_EVAL_BUILD_ERROR */
#error  RBA_MEMLIB_TIMERBITSIZE needs to be defined in range betwen 20u and 32u depending on used timer in rba_MemLib_Cfg_Callout.h
#endif

#if (RBA_MEMLIB_TIMERBITSIZE >= 32u)
#define RBA_MEMLIB_TIMERBITMASK    (0xFFFFFFFFu)
#else
#define RBA_MEMLIB_TIMERBITMASK    (((uint32)1u<<RBA_MEMLIB_TIMERBITSIZE)-1u)
#endif

#else
#define RBA_MEMLIB_TIMERBITMASK    (0x0u)
#endif



/*
 **********************************************************************************************************************
 * Protected variable - do not use outside rba_MemLib
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"


/** ***********************************************************************
 * \brief   Convert a timer-difference in timer-ticks to microseconds(us)
 * \param   Timer-ticks
 * \return  Timer-ticks converted to microseconds
*/
#if (RBA_MEMLIB_USE_TIMER != STD_OFF)
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_ConvertTimerTicksToUs(uint32 tiTicks_u32)
{
    uint32 tiDiff_u32;

    /* Limit the passed time tick value to the valid bitmask / handling wrap-around */
    /* MR12 RULE 10.3 VIOLATION: A non-constant expression is assigned to unsigned int - no issue (configuration dependent) */
    tiDiff_u32 = (uint32)(tiTicks_u32 & RBA_MEMLIB_TIMERBITMASK);

    /* Convert corrected difference to us */
    tiDiff_u32 = rba_MemLib_Callout_ConvertTimerTicksToUs(tiDiff_u32);
    return tiDiff_u32;
}
#else
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue*/
LOCAL_INLINE uint32 rba_MemLib_ConvertTimerTicksToUs(uint32 tiTicks_u32)
{
    (void)tiTicks_u32;
    return 0u;
}
#endif


/** ***********************************************************************
 * \brief   Calculate time difference of two time stamps captured
 *          via rba_MemLib_GetTimerTicks()
 *          Wrap around is considered.
 * \param   tiCurr Timer-ticks of current time reading
 * \param   tiRef  Timer-ticks of reference
 * \return  Difference of tiCurr - tiRef with considered wrap-around
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE uint32 rba_MemLib_CalcDiffInTimerTicks(uint32 tiCurr_u32, uint32 tiRef_u32)
{
    uint32 tiDiff_u32;

    /* Difference without considering the bit-size and the wrap around properties */
    tiDiff_u32 = tiCurr_u32 - tiRef_u32;

    /* cut to the relevant bitmask to handle a wrap-around depending
     * on the bit-size of the timer */
    /* MR12 RULE 10.3 VIOLATION: A non-constant expression is assigned to unsigned int - no issue  (configuration dependent) */
    return (uint32)(tiDiff_u32 & RBA_MEMLIB_TIMERBITMASK);
}


#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"


#endif   /* Header include */

