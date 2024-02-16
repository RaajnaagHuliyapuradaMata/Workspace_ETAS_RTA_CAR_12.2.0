/*####################################################################################################################
######################################################################################################################

NOTE:
* Integrator has to map each memmap defines to the required memory section.
* Below provided memory sections are mapping to BSW_MemMap sections as per the DGS requirement.It is an example configuration provided by the platform team for DGS product line.Other product lines has to modify this file as per their requirements.
*Project teams should verify the mapping before finalizing this file contents based on the BSW module version used in their project.
######################################################################################################################
######################################################################################################################
*/
/* MISRA RULE 19.15 VIOLATION: Memmap file must be included multiple times */
#define MEMMAP_ERROR

/* ============================   BSW   ============================ */

/* Used for global or static variables that are cleared to zero after every reset */
#if defined CAN_START_SEC_VAR_CLEARED_32
    /*  8 bit */
    #define BSW_START_SEC_VAR_CLEARED_32
    #include "Bsw_MemMap.h"
    #undef CAN_START_SEC_VAR_CLEARED_32
    #undef MEMMAP_ERROR
#elif defined CAN_STOP_SEC_VAR_CLEARED_32
    #define BSW_STOP_SEC_VAR_CLEARED_32
    #include "Bsw_MemMap.h"
    #undef CAN_STOP_SEC_VAR_CLEARED_32
    #undef MEMMAP_ERROR

#elif defined CAN_START_SEC_CONST_UNSPECIFIED
    /* Unspecified size */
    #define BSW_START_SEC_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef CAN_START_SEC_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR
#elif defined CAN_STOP_SEC_CONST_UNSPECIFIED
    #define BSW_STOP_SEC_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef CAN_STOP_SEC_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR

#elif defined CAN_START_SEC_VAR_INIT_8
    #define BSW_START_SEC_VAR_INIT_8
    #include "Bsw_MemMap.h"
    #undef CAN_START_SEC_VAR_INIT_8
    #undef MEMMAP_ERROR
#elif defined CAN_STOP_SEC_VAR_INIT_8
    #define BSW_STOP_SEC_VAR_INIT_8
    #include "Bsw_MemMap.h"
    #undef CAN_STOP_SEC_VAR_INIT_8
    #undef MEMMAP_ERROR

#elif defined (CAN_START_SEC_CODE_FAST)
    #define BSW_START_SEC_CODE_FAST
    #include "Bsw_MemMap.h"
    #undef  CAN_START_SEC_CODE_FAST
    #undef MEMMAP_ERROR
#elif defined (CAN_STOP_SEC_CODE_FAST)
    #define BSW_STOP_SEC_CODE_FAST
    #include "Bsw_MemMap.h"
    #undef  CAN_STOP_SEC_CODE_FAST
    #undef MEMMAP_ERROR

#elif defined CAN_START_SEC_CODE
    #define  BSW_START_SEC_CODE
    #include "Bsw_MemMap.h"
    #undef CAN_START_SEC_CODE
    #undef MEMMAP_ERROR

#elif defined CAN_STOP_SEC_CODE
    #define  BSW_STOP_SEC_CODE
    #include "Bsw_MemMap.h"
    #undef CAN_STOP_SEC_CODE
    #undef MEMMAP_ERROR

/* START OF POST BUILD SECTIONS*/

#elif defined CAN_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #define BSW_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    /* PB DATA  this section will be used in the Can_PBcfg.c for the actual PB cfg data struct*/
    /*#pragma ghs section rodata="postbuildSection"*//*example for GHS compiler*/
    /*#warning "This is the default implementation, please edit these lines (Can Memmap PBL section) to suit your project"*/
    #include "Bsw_MemMap.h"
    #undef CAN_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #undef MEMMAP_ERROR
#elif defined CAN_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #define BSW_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    /* Now go back to default rules */
    /*#pragma ghs section rodata=default*//*example for GHS compiler*/
   /*#warning "This is the default implementation, please edit these lines (Can Memmap PBL section) to suit your project"*/
    #include "Bsw_MemMap.h"
    #undef CAN_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #undef MEMMAP_ERROR
    /*STOP OF POST BUILD SECTIONS*/
#elif defined MEMMAP_ERROR
  #error "Can_MemMap.h, wrong #pragma command"
#endif

