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

#ifndef FEE_PRV_PERSTYPES_H
#define FEE_PRV_PERSTYPES_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"


/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* Collection of all environment variables of this unit */
typedef struct
{
#  if(defined(FEE_PRV_CFG_RB_PERSIST_DATA) && (TRUE == FEE_PRV_CFG_RB_PERSIST_DATA))
    uint32    cntrBytProgdInIni_u32;                     /* Amount of data programmed during Fs-initialization phase */
#  else
    uint8     xResv_u8;                                  /* Dummy to keep this typedef/struct compilable             */
#  endif
} Fee_Prv_PersData_tst;

/* Fee high level persistent data */
typedef struct
{
    uint64 cntrBytProgdTot_u64;         /* Amount of data programmed over ECU lifetime (in bytes)                    */
    uint64 cntrBytProgdPredStrt_u64;    /* Amount of data programmed at measurement/prediction start (in bytes)      */
    uint32 cntrDrvCycTot_u32;           /* Driving cycles over ECU lifetime                                          */
    uint32 cntrDrvCycPredStrt_u32;      /* Driving cycles at measurement/prediction start                            */
    uint32 cntrPersBlkWrites_u32;       /* Number of Fee-internal persistent data block updates over ECU lifetime    */
/* It's not necessary to have the variable below stored inside the block, but we should avoid padding gaps here (due */
/* to 64-bit alignment). Rather than adding 4 spare bytes, we can also put the driving cycle prediction result here. */
/* It's also more convinient for the user when all variables for the stress measurement are inside the same struct.  */
    uint32 nrDrvCycMaxRea_u32;          /* Last max reachable driving cycles, for measurement start or ECU lifetime) */
} Fee_Prv_PersDataHl_tst;

/* Reset-save data of this unit */
typedef struct
{
#  if(defined(FEE_PRV_CFG_RB_PERSIST_DATA) && (TRUE == FEE_PRV_CFG_RB_PERSIST_DATA))
#   if(defined(FEE_PRV_CFG_RB_STRESS_MEASMT) && (TRUE == FEE_PRV_CFG_RB_STRESS_MEASMT))
    Fee_Prv_PersDataHl_tst     dataHl_st;              /* Fee high level persistent data                             */
/* It's not necessary to have the device's size or erase&program cycles inside the saved zone; they could be located */
/* in ROM or (zero-)initialized RAM. They have been put here for the sake of user comfort (so that they are close to */
/* the stress measurement and driving cycle prediction variables.                                                    */
#   endif
    uint32                     nrDeviceSize_u32;       /* Size of data-NvM                                           */
    uint32                     nrEraseProgCycPtd_u32;  /* Number of permitted erase&program cycles                   */
    uint32                     idSavedZoneMarker_u32;  /* Marker to check status of saved zone                       */
    boolean                    isBlkStrd_b;            /* TRUE if consistent persistent data block present on medium */
#  else
    uint8                      xResv_u8;                             /* Dummy to keep this typedef/struct compilable */
#  endif
} Fee_Prv_PersSavedZone_tst;

/* # if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED)) */
# endif

/* #ifndef FEE_PRV_PERSTYPES_H */
#endif
