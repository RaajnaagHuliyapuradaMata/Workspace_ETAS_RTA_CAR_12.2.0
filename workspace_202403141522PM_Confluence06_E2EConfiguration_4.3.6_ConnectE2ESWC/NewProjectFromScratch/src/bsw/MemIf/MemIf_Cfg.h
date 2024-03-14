/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef MEMIF_CFG_H
#define MEMIF_CFG_H

#include "Std_Types.h"

#include "MemIf_Types.h"
#include "Fee.h"



/* Information regarding supplier of this software*/

#define MEMIF_VENDOR_ID                                      (6u)                 /* Supplier */
#define MEMIF_MODULE_ID                                      (22u)                /* Module ID of MemIf */
#define MEMIF_INSTANCE_ID                                    (0u)                 /* Instance ID of MemIf */

/* MemIf Software Version */
#define MEMIF_SW_MAJOR_VERSION                               (22u)
#define MEMIF_SW_MINOR_VERSION                               (0u)
#define MEMIF_SW_PATCH_VERSION                               (1u)

/* MemIf compatible Autosar Version */
#define MEMIF_AR_RELEASE_MAJOR_VERSION                       (4u)
#define MEMIF_AR_RELEASE_MINOR_VERSION                       (5u)
#define MEMIF_AR_RELEASE_REVISION_VERSION                    (0u)

/*
* Switch for version info api
*/
#define MEMIF_VERSION_INFO_API                               (STD_OFF)

/* Switch for dev error detect */
#define MEMIF_DEV_ERROR_DETECT                               (STD_OFF)
#define MEMIF_FEE_USED                                       (STD_ON)
#define MEMIF_RB_RTE_MAINFUNC_USED                           (STD_ON)

#define MEMIF_NUM_OF_EA_DEVICES                              (0u)
#define MEMIF_MAX_NUM_DEVICES                                (1u)
#define MEMIF_RB_NUM_OF_FEE_DEVICES                          (1u)

#define MEMIF_MIN_NUM_DEVICES                                (1)


#define MEMIF_FEE_AND_EA_USED                                (STD_OFF)


#endif /* MEMIF_CFG_H */
