 /*
  **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Based on AUTOSAR_SWS_PlatformTypes.pdf; AR4.2; Rev 2 and BSW_CodingGuidelines_Cur_v.1.10.pdf
 **********************************************************************************************************************
 */
 /*TRACE[SWS_Platform_00002]: It is not allowed to add any project or supplier specific extension to this file.
                   Any extension invalidates the AUTOSAR conformity.*/

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#include "Platform_Types_Cfg.h" /*header for project specific definitions*/
/*
 **********************************************************************************************************************
 * Types and Defines
 **********************************************************************************************************************
 */

/* TRACE[Rule BSW_VersionInfo_002]: Module Vendor Identifier. */
/* TRACE[SRS_BSW_00374]  */
#define PLATFORM_VENDOR_ID 0x0006u

/* TRACE[Rule BSW_VersionInfo_003]: Module Identifier. */
/* TRACE[SRS_BSW_00374]  */
#define PLATFORM_MODULE_ID 0x00C7u

/* TRACE[Rule BSW_VersionInfo_004]: Software Version Number and AUTOSAR Specification Version Number. */
/* TRACE[SRS_BSW_00374]  */
#define PLATFORM_SW_MAJOR_VERSION 0x05u
#define PLATFORM_SW_MINOR_VERSION 0x00u
#define PLATFORM_SW_PATCH_VERSION 0x00u
#define PLATFORM_AR_RELEASE_MAJOR_VERSION 0x04u
#define PLATFORM_AR_RELEASE_MINOR_VERSION 0x05u
#define PLATFORM_AR_RELEASE_REVISION_VERSION 0x00u

/*TRACE[SWS_Platform_00064] */
/*This symbol shall be defined as #define having one of the values CPU_TYPE_8, CPU_TYPE_16,
CPU_TYPE_32 or CPU_TYPE_64 according to the platform.*/

#define CPU_TYPE_8 8 /*Indicating a 8 bit processor*/
#define CPU_TYPE_16 16 /*Indicating a 16 bit processor*/
#define CPU_TYPE_32 32 /*Indicating a 32 bit processor*/
#define CPU_TYPE_64 64 /*Indicating a 64 bit processor*/

#define MSB_FIRST 0
#define LSB_FIRST 1

#define HIGH_BYTE_FIRST 0
#define LOW_BYTE_FIRST 1

/* TRACE[SWS_Platform_00044], TRACE[SWS_Platform_00045] & TRACE[SWS_Platform_00064]: CPU_TYPE */
#define CPU_TYPE CPU_TYPE_32

/*TRACE[SWS_Platform_00038], TRACE[SWS_Platform_00043], TRACE[SWS_Platform_00048] and TRACE[SWS_Platform_00049]:
  CPU_BIT_ORDER*/
#define CPU_BIT_ORDER LSB_FIRST

/*TRACE[SWS_Platform_00039],TRACE[SWS_Platform_00046], TRACE[SWS_Platform_00050] & TRACE[SWS_Platform_00051]:
 CPU_BYTE_ORDER */
#define CPU_BYTE_ORDER LOW_BYTE_FIRST

/* Standard Symbols for TRUE and FALSE */
/*TRACE[SWS_Platform_00055], TRACE[SWS_Platform_00056]: Only to be used in conjunction with the boolean type */
/*TRACE[SWS_Platform_00054]: In case of in-built compiler support of the symbols,
                       redefinitions shall be avoided using a conditional check.*/
#ifndef TRUE
    #define TRUE (1 != 0)
#endif

#ifndef FALSE
    #define FALSE (0 != 0)
#endif


/*-------Standard Integer Data Types--------*/
/*TRACE[SWS_Platform_00016]:This standard AUTOSAR type shall be of 8 bit signed. */
 /* Range : -128 .. +127 */
/*           0x80..0x7F */
typedef signed char     sint8;

/* TRACE[SWS_Platform_00013]: This standard AUTOSAR type shall be of 8 bit unsigned.*/
 /* Range : 0 .. 255     */
 /*         0x00 .. 0xFF */
typedef unsigned char   uint8;

/* TRACE[SWS_Platform_00017]: This standard AUTOSAR type shall be of 16 bit signed. */
/* Range : -32768 .. +32767 */
/*          0x8000..0x7FFF  */
typedef signed short    sint16;

/*TRACE[SWS_Platform_00014]: This standard AUTOSAR type shall be of 16 bit unsigned. */
/* Range : 0 .. 65535      */
/*         0x0000..0xFFFF  */
typedef unsigned short  uint16;

/*TRACE[SWS_Platform_00018]:This standard AUTOSAR type shall be 32 bit signed. */
/*Range : -2147483648 .. +2147483647   */
/*         0x80000000..0x7FFFFFFF      */
typedef signed int     sint32;

/*TRACE[SWS_Platform_00067]: This standard AUTOSAR type shall be 64 bit signed. */
/*Range: -9223372036854775808..+9223372036854775807*/
/*        0x8000000000000000..0x7FFFFFFFFFFFFFFF   */
typedef signed long long sint64;

/*TRACE[SWS_Platform_00015]:This standard AUTOSAR type shall be 32 bit unsigned. */
/* Range:  0 .. 4294967295           */
/*         0x00000000..0xFFFFFFFF    */
typedef unsigned int   uint32;

/*TRACE[SWS_Platform_00066]: This standard AUTOSAR type shall be 64 bit unsigned.*/
/* Range :   0 ..18446744073709551615              */
/*           0x0000000000000000..0xFFFFFFFFFFFFFFFF*/
typedef unsigned long long   uint64;

/* Standard Float Data Types */
/* ------------------------- */
/* TRACE[SWS_Platform_00041]: This standard AUTOSAR type shall follow the 32-bit binary interchange format
  according to IEEE 754-2008 with encoding parameters specified in chapter 3.6, table 3.5, column "binary32".*/
typedef float   float32;
/*TRACE[SWS_Platform_00042]:This standard AUTOSAR type shall follow the 64-bit binary interchange format according
            to IEEE 754-2008 with encoding parameters specified in chapter 3.6, table 3.5, column "binary64". */
typedef double  float64;

/*-------- Boolean Data Type--------- */
/* TRACE[SWS_Platform_00026]:This standard AUTOSAR type shall only be used together with the definitions TRUE and
FALSE. */
/* MR12 DIR 1.1 VIOLATION: the type _Bool is mapped here to the AUTOSAR type boolean to prevent the direct use of 
the type _Bool */

/*Adaptation of boolean type for target, VECU and SIL environments*/

#if defined(RBA_VECU_ENABLED)
  #if defined(RBA_VECU_ENABLED_BOOL_CONFIGURABLE)
    typedef unsigned char  boolean;
  #else
    typedef unsigned char  boolean;
  #endif
#else
  #ifdef SIL_BUILD_ENABLED
    typedef _Bool boolean
  #else
    #if !defined (_WIN32) || !defined (WIN32) || !defined (_WIN64) || !defined (WIN64)
      #if !defined (__cplusplus)
        typedef unsigned char    boolean;
      #else
        typedef bool boolean;
      #endif
    #endif
  #endif
#endif


/* Optimized Integer Data Types */
/* ---------------------------- */
/* TRACE[SWS_Platform_00023]:This optimized AUTOSAR type shall be at least 8 bit signed. */
 /* At least -128..+127 */
/*   0x80..0x7F  */
typedef signed long     sint8_least;

/* TRACE[SWS_Platform_00020]:This optimized AUTOSAR type shall be at least 8 bit unsigned. */
typedef unsigned long   uint8_least;    /* At least 0..255  */

/* TRACE[SWS_Platform_00024]:This optimized AUTOSAR type shall be at least 16 bit signed. */
   /*At least -32768..+32767*/
   /* 0x8000..0x7FFF */
typedef signed long     sint16_least;

/* TRACE[SWS_Platform_00021]:This optimized AUTOSAR type shall be at least 16 bit unsigned. */
   /*At least 0..65535 */
  /*0x0000..0xFFFF   */
typedef unsigned long   uint16_least;

/* TRACE[SWS_Platform_00025]:This optimized AUTOSAR type shall be at least 32 bit signed.*/
/* At least -2147483648..+2147483647*/
/*0x80000000..0x7FFFFFFF*/
typedef signed long sint32_least;

/* SWS_Platform_00022:This optimized AUTOSAR type shall be at least 32 bit unsigned. */
/* At least 0..4294967295*/
/* 0x00000000..0xFFFFFFFF */
typedef unsigned long   uint32_least;

/* SWS_Platform_91001:This standard AUTOSAR type shall be a void pointer. 
Note: This type shall be used for buffers that contain data returned to the caller */
typedef void* VoidPtr;

/* SWS_Platform_91002:This standard AUTOSAR type shall be a void pointer to const. 
Note: This type shall be used for buffers that are passed to the callee.*/
typedef const void* ConstVoidPtr;

#endif /* PLATFORM_TYPES_H */
