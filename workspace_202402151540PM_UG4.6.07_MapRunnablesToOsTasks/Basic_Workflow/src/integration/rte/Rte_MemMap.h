/*
**********************************************************************************************************************
*
* COPYRIGHT RESERVED, 2016 Robert Bosch GmbH. All rights reserved.
* The reproduction, distribution and utilization of this document as well as the communication of its contents to
* others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
**********************************************************************************************************************
*/

#define RTE_OS_CDATA

/* Uninitialized RAM Variables */

#if defined (RTE_START_SEC_VAR_NOINIT_BOOLEAN)
   #undef      RTE_START_SEC_VAR_NOINIT_BOOLEAN
   #define MEMMAP_START_SEC_VAR_NOINIT_BOOLEAN
#elif defined (RTE_STOP_SEC_VAR_NOINIT_BOOLEAN)
   #undef      RTE_STOP_SEC_VAR_NOINIT_BOOLEAN
   #define MEMMAP_STOP_SEC_VAR_NOINIT_BOOLEAN

#elif defined (RTE_START_SEC_VAR_NOINIT_8BIT)
   #undef      RTE_START_SEC_VAR_NOINIT_8BIT
   #define MEMMAP_START_SEC_VAR_NOINIT_8BIT
#elif defined (RTE_STOP_SEC_VAR_NOINIT_8BIT)
   #undef      RTE_STOP_SEC_VAR_NOINIT_8BIT
   #define MEMMAP_STOP_SEC_VAR_NOINIT_8BIT

#elif defined (RTE_START_SEC_VAR_NOINIT_16BIT)
   #undef      RTE_START_SEC_VAR_NOINIT_16BIT
   #define MEMMAP_START_SEC_VAR_NOINIT_16BIT
#elif defined (RTE_STOP_SEC_VAR_NOINIT_16BIT)
   #undef      RTE_STOP_SEC_VAR_NOINIT_16BIT
   #define MEMMAP_STOP_SEC_VAR_NOINIT_16BIT

#elif defined (RTE_START_SEC_VAR_NOINIT_32BIT)
   #undef      RTE_START_SEC_VAR_NOINIT_32BIT
   #define MEMMAP_START_SEC_VAR_NOINIT_32BIT
#elif defined (RTE_STOP_SEC_VAR_NOINIT_32BIT)
   #undef      RTE_STOP_SEC_VAR_NOINIT_32BIT
   #define MEMMAP_STOP_SEC_VAR_NOINIT_32BIT

#elif defined (RTE_START_SEC_VAR_NOINIT_UNSPECIFIED)
   #undef      RTE_START_SEC_VAR_NOINIT_UNSPECIFIED
   #define MEMMAP_START_SEC_VAR_NOINIT_UNSPECIFIED
#elif defined (RTE_STOP_SEC_VAR_NOINIT_UNSPECIFIED)
   #undef      RTE_STOP_SEC_VAR_NOINIT_UNSPECIFIED
   #define MEMMAP_STOP_SEC_VAR_NOINIT_UNSPECIFIED

/* RAM Variables */

#elif defined (RTE_START_SEC_VAR_BOOLEAN)
   #undef      RTE_START_SEC_VAR_BOOLEAN
   #define MEMMAP_START_SEC_VAR_BOOLEAN
#elif defined (RTE_STOP_SEC_VAR_BOOLEAN)
   #undef      RTE_STOP_SEC_VAR_BOOLEAN
   #define MEMMAP_STOP_SEC_VAR_BOOLEAN

#elif defined (RTE_START_SEC_VAR_8BIT)
   #undef      RTE_START_SEC_VAR_8BIT
   #define MEMMAP_START_SEC_VAR_8BIT
#elif defined (RTE_STOP_SEC_VAR_8BIT)
   #undef      RTE_STOP_SEC_VAR_8BIT
   #define MEMMAP_STOP_SEC_VAR_8BIT

#elif defined (RTE_START_SEC_VAR_16BIT)
   #undef      RTE_START_SEC_VAR_16BIT
   #define MEMMAP_START_SEC_VAR_16BIT
#elif defined (RTE_STOP_SEC_VAR_16BIT)
   #undef      RTE_STOP_SEC_VAR_16BIT
   #define MEMMAP_STOP_SEC_VAR_16BIT

#elif defined (RTE_START_SEC_VAR_32BIT)
   #undef      RTE_START_SEC_VAR_32BIT
   #define MEMMAP_START_SEC_VAR_32BIT
#elif defined (RTE_STOP_SEC_VAR_32BIT)
   #undef      RTE_STOP_SEC_VAR_32BIT
   #define MEMMAP_STOP_SEC_VAR_32BIT

#elif defined (RTE_START_SEC_VAR_UNSPECIFIED)
   #undef      RTE_START_SEC_VAR_UNSPECIFIED
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_SEC_VAR_UNSPECIFIED)
   #undef      RTE_STOP_SEC_VAR_UNSPECIFIED
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED

/* RAM Variables (fast access) */

#elif defined (RTE_START_SEC_VAR_FAST_BOOLEAN)
   #undef      RTE_START_SEC_VAR_FAST_BOOLEAN
   #define MEMMAP_START_SEC_VAR_FAST_BOOLEAN
#elif defined (RTE_STOP_SEC_VAR_FAST_BOOLEAN)
   #undef      RTE_STOP_SEC_VAR_FAST_BOOLEAN
   #define MEMMAP_STOP_SEC_VAR_FAST_BOOLEAN

#elif defined (RTE_START_SEC_VAR_FAST_8BIT)
   #undef      RTE_START_SEC_VAR_FAST_8BIT
   #define MEMMAP_START_SEC_VAR_FAST_8BIT
#elif defined (RTE_STOP_SEC_VAR_FAST_8BIT)
   #undef      RTE_STOP_SEC_VAR_FAST_8BIT
   #define MEMMAP_STOP_SEC_VAR_FAST_8BIT

#elif defined (RTE_START_SEC_VAR_FAST_16BIT)
   #undef      RTE_START_SEC_VAR_FAST_16BIT
   #define MEMMAP_START_SEC_VAR_FAST_16BIT
#elif defined (RTE_STOP_SEC_VAR_FAST_16BIT)
   #undef      RTE_STOP_SEC_VAR_FAST_16BIT
   #define MEMMAP_STOP_SEC_VAR_FAST_16BIT

#elif defined (RTE_START_SEC_VAR_FAST_32BIT)
   #undef      RTE_START_SEC_VAR_FAST_32BIT
   #define MEMMAP_START_SEC_VAR_FAST_32BIT
#elif defined (RTE_STOP_SEC_VAR_FAST_32BIT)
   #undef      RTE_STOP_SEC_VAR_FAST_32BIT
   #define MEMMAP_STOP_SEC_VAR_FAST_32BIT

#elif defined (RTE_START_SEC_VAR_FAST_UNSPECIFIED)
   #undef      RTE_START_SEC_VAR_FAST_UNSPECIFIED
   #define MEMMAP_START_SEC_VAR_FAST_UNSPECIFIED
#elif defined (RTE_STOP_SEC_VAR_FAST_UNSPECIFIED)
   #undef      RTE_STOP_SEC_VAR_FAST_UNSPECIFIED
   #define MEMMAP_STOP_SEC_VAR_FAST_UNSPECIFIED

/* Constants */

#elif defined (RTE_START_SEC_CONST_BOOLEAN)
   #undef      RTE_START_SEC_CONST_BOOLEAN
   #define MEMMAP_START_SEC_CONST_BOOLEAN
#elif defined (RTE_STOP_SEC_CONST_BOOLEAN)
   #undef      RTE_STOP_SEC_CONST_BOOLEAN
   #define MEMMAP_STOP_SEC_CONST_BOOLEAN

#elif defined (RTE_START_SEC_CONST_8BIT)
   #undef      RTE_START_SEC_CONST_8BIT
   #define MEMMAP_START_SEC_CONST_8BIT
#elif defined (RTE_STOP_SEC_CONST_8BIT)
   #undef      RTE_STOP_SEC_CONST_8BIT
   #define MEMMAP_STOP_SEC_CONST_8BIT

#elif defined (RTE_START_SEC_CONST_16BIT)
   #undef      RTE_START_SEC_CONST_16BIT
   #define MEMMAP_START_SEC_CONST_16BIT
#elif defined (RTE_STOP_SEC_CONST_16BIT)
   #undef      RTE_STOP_SEC_CONST_16BIT
   #define MEMMAP_STOP_SEC_CONST_16BIT

#elif defined (RTE_START_SEC_CONST_32BIT)
   #undef      RTE_START_SEC_CONST_32BIT
   #define MEMMAP_START_SEC_CONST_32BIT
#elif defined (RTE_STOP_SEC_CONST_32BIT)
   #undef      RTE_STOP_SEC_CONST_32BIT
   #define MEMMAP_STOP_SEC_CONST_32BIT

#elif defined (RTE_START_SEC_CONST_UNSPECIFIED)
   #undef      RTE_START_SEC_CONST_UNSPECIFIED
   #define MEMMAP_START_SEC_CONST_UNSPECIFIED
#elif defined (RTE_STOP_SEC_CONST_UNSPECIFIED)
   #undef      RTE_STOP_SEC_CONST_UNSPECIFIED
   #define MEMMAP_STOP_SEC_CONST_UNSPECIFIED

/* Code */

#elif defined (RTE_START_SEC_CODE)
   #undef      RTE_START_SEC_CODE
   #define MEMMAP_START_SEC_CODE
/*   #pragma CODE_SEG RTE_CODE_ROM*/
#elif defined (RTE_STOP_SEC_CODE)
   #undef      RTE_STOP_SEC_CODE
   #define MEMMAP_STOP_SEC_CODE

#elif defined (RTE_START_SEC_LIBCODE)
   #undef      RTE_START_SEC_LIBCODE
   #define MEMMAP_START_SEC_CODE
/*   #pragma CODE_SEG RTE_CODE_ROM*/
#elif defined (RTE_STOP_SEC_LIBCODE)
   #undef      RTE_STOP_SEC_LIBCODE
   #define MEMMAP_STOP_SEC_CODE

#elif defined (RTE_START_SEC_TASKBODY)
   #undef      RTE_START_SEC_TASKBODY
   #define MEMMAP_START_SEC_CODE
/*   #pragma section code rtetask*/
#elif defined (RTE_STOP_SEC_TASKBODY)
   #undef      RTE_STOP_SEC_TASKBODY
/*    #pragma section code restore*/
   #define MEMMAP_STOP_SEC_CODE

#elif defined (RTE_START_SEC_APPL_CODE)
   #undef      RTE_START_SEC_APPL_CODE
   #define MEMMAP_START_SEC_APPL_CODE
/*   #pragma CODE_SEG RTE_CODE_ROM*/
#elif defined (RTE_STOP_SEC_APPL_CODE)
   #undef      RTE_STOP_SEC_APPL_CODE
   #define MEMMAP_STOP_SEC_APPL_CODE

/* RAM based data (used within example applications) */
#elif defined (RTE_START_RAM)
   #undef      RTE_START_RAM
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_RAM)
   #undef      RTE_STOP_RAM
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED

/* Example calibration data */
#elif defined (RTE_START_group1)
   #undef      RTE_START_group1
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_group1)
   #undef      RTE_STOP_group1
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED
#elif defined (RTE_START_group2)
   #undef      RTE_START_group2
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_group2)
   #undef      RTE_STOP_group2
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED
#elif defined (RTE_START_nullSWAddr)
   #undef      RTE_START_nullSWAddr
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_nullSWAddr)
   #undef      RTE_STOP_nullSWAddr
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED
#elif defined (RTE_START_swaddrmethod_1)
   #undef      RTE_START_swaddrmethod_1
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_swaddrmethod_1)
   #undef      RTE_STOP_swaddrmethod_1
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED

/* For TC_ModeSwitchBSW */
#elif defined (Can_25_Dev0815_START_SEC_CODE)
   #undef Can_25_Dev0815_START_SEC_CODE
   #define DEFAULT_START_SEC_CODE
#elif defined (Can_25_Dev0815_STOP_SEC_CODE)
   #undef Can_25_Dev0815_STOP_SEC_CODE
   #define DEFAULT_STOP_SEC_CODE
#elif defined (Can_too_25_Dev0815_START_SEC_CODE)
   #undef Can_25_too_Dev0815_START_SEC_CODE
   #define DEFAULT_START_SEC_CODE
#elif defined (Can_too_25_Dev0815_STOP_SEC_CODE)
   #undef Can_too_25_Dev0815_STOP_SEC_CODE
   #define DEFAULT_STOP_SEC_CODE

/* For TC_ImplicitAllocation */
#elif defined (RTE_START_SEC_VAR_IMPLICITSR_TASKA)
   #undef      RTE_START_SEC_VAR_IMPLICITSR_TASKA
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_SEC_VAR_IMPLICITSR_TASKA)
   #undef      RTE_STOP_SEC_VAR_IMPLICITSR_TASKA
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED
#elif defined (RTE_START_SEC_VAR_IMPLICITSR_TASKB)
   #undef      RTE_START_SEC_VAR_IMPLICITSR_TASKB
   #define MEMMAP_START_SEC_VAR_UNSPECIFIED
#elif defined (RTE_STOP_SEC_VAR_IMPLICITSR_TASKB)
   #undef      RTE_STOP_SEC_VAR_IMPLICITSR_TASKB
   #define MEMMAP_STOP_SEC_VAR_UNSPECIFIED

#else
   //#error "Rte_MemMap.h, wrong #pragma command"
#endif


/*
**********************************************************************************************************************
* End of the file
**********************************************************************************************************************
*/
