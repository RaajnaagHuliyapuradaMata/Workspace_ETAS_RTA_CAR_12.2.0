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

#ifndef RBA_MEMLIB_PRV_H
#define RBA_MEMLIB_PRV_H

#include "rba_MemLib_Cfg.h"
#include "rba_MemLib_CheckProto.h"


/* Check for enabled prototypes in the Mem stack */
#if defined(RBA_MEMLIB_CFG_ENABLE_PROTOTYPE)
# if defined(__TASKING__)
/* VARIANT_EVAL_BUILD_WARN */
/* Usage of Prototypes has been enabled for the Mem package, this configuration is limited for testing purposes only - IT IS NOT RELEASED FOR SERIES PRODUCTION! */
/* The tasking compiler does not support hash warning, because of that here is only a comment - STILL THIS IS ONLY PROTOTYPE! */
# else
/* For all other compilers generate a compiler warning */
/* MR12 RULE 1.2,20.13 Dir 1.1 VIOLATION: warning pre-processor comment only available for some compilers */
#  warning (EnablePrototype) Usage of Prototypes has been enabled for the Mem package, this configuration is limited for testing purposes only - IT IS NOT RELEASED FOR SERIES PRODUCTION!
# endif
#endif




/**********************************************************************************************************************
COMPILE TIME ASSERTS
**********************************************************************************************************************/

#if RBA_MEMLIB_MEAS_ENABLE== STD_ON
typedef enum {
   RBA_MEMLIB_ENUM8_FIRST=0,
   RBA_MEMLIB_ENUM8_LAST =255,
} rba_MemLib_enum8_t;

typedef enum {
   RBA_MEMLIB_ENUM16_FIRST=0,
   RBA_MEMLIB_ENUM16_LAST =65535,
} rba_MemLib_enum16_t;

/* Note: if following ASSERTs cause an error, then the setting i the EcuC parameter
 * rba_MemLibEnumFlexible is not fitting to the compiler settings. Please change the related setting */
/* MR12 RULE 1.1 VIOLATION: static checker tools is not capable to correctly evaluate this target specific check - no issue as this is a compile-time check */
RBA_MEMLIB_ASSERT(sizeof(rba_MemLib_enum8_t)  == RBA_MEMLIB_SIZE_ENUM_8,  Incorrect_size_of_enum8_Please_Correct_EcuC_Parameter_rba_MemLibEnumFlexible)
RBA_MEMLIB_ASSERT(sizeof(rba_MemLib_enum16_t) == RBA_MEMLIB_SIZE_ENUM_16, Incorrect_size_of_enum16_Please_Correct_EcuC_Parameter_rba_MemLibEnumFlexible)
#endif


#endif
