/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef FEE_PRVTYPES_H
#define FEE_PRVTYPES_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
#if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

#define FEE_PRV_MAX_UINT8                   (0xFFu)
#define FEE_PRV_MAX_UINT16                  (0xFFFFu)
#define FEE_PRV_MAX_UINT32                  (0xFFFFFFFFu)
#define FEE_PRV_MAX_UINT64                  (0xFFFFFFFFFFFFFFFFuL)

/* Block status/property bits stored in PFLASH common for all file systems */
#define FEE_PRV_USER_BITS                   (0x0300u)
#define FEE_PRV_USER_POS                    (8u)

 # if(FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION < 4) ||\
    (\
        (FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION == 4) &&\
        (FEE_PRV_CFG_AR_RELEASE_MINOR_VERSION < 2)\
    )
#define FEE_PRV_CFG_AR42_API_COMPLIANCE STD_OFF
# else
#define FEE_PRV_CFG_AR42_API_COMPLIANCE STD_ON
# endif

#endif

/* FEE_PRVTYPES_H */
#endif
