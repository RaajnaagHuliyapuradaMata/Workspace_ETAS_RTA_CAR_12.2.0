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

#ifndef FEE_PRV_LIBTYPES_H
#define FEE_PRV_LIBTYPES_H


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "Fls.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

#  if ((!defined(FLS_RB_VENDOR_ID)) || (0x06 != FLS_RB_VENDOR_ID))

#define Fls_Rb_GetDeviceIdFromDrvIndex(param)    (0u)


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

typedef uint8     Fls_Rb_FlsDevice_ten;

typedef uint8*    Fls_Rb_tpu8;

typedef struct
{
    Fls_Rb_AddressType    adSectorStart_uo;
    Fls_Rb_LengthType     nrSectorSize_uo;
    uint32                nrSpecifiedEraseCycles_u32;
    uint8                 idErasedValue_u8;
} Fls_Rb_SectorInfo_tst;

#  endif


/* Unions to combine read-only and read/write buffer */
typedef union
{
    void *           dataChg_pv;        /* Pointer to changeable buffer - e.g. for read jobs */
    void const *     dataCon_pcv;       /* Pointer to constant buffer - e.g. for write jobs  */
    void const **    dataLocn_ppcv;     /* Pointer to location where data is located         */
} Fee_Prv_LibBuffer_tun;

typedef union
{
    uint8 *          dataChg_pu8;       /* Pointer to changeable buffer - e.g. for read jobs */
    uint8 const *    dataCon_pcu8;      /* Pointer to constant buffer - e.g. for write jobs  */
    void  const **   dataLocn_ppcv;     /* Pointer to location where data is located         */
} Fee_Prv_LibBufferU8_tun;

/* The following operations are considered when limiting the duration of a main function call */
typedef enum
{
    FEE_PRV_LIMIT_CRCINRAM_CPYRAM_E = 0,    /* CRC calculation in RAM or copy process from RAM to RAM           */
    FEE_PRV_LIMIT_CRCINFLS_E        = 1,    /* CRC calculation in flash                                         */
    FEE_PRV_LIMIT_HDR_E             = 2,    /* Handling one FAT entry - no matter what "handling" exactly means */
    FEE_PRV_LIMIT_CACHEREORG_E      = 3,    /* Reorganizing the cache is expensive                              */
    FEE_PRV_LIMIT_MAX_E             = 4     /* Maximum enum value used for defining an array                    */
} Fee_Prv_LibEffortLimit_ten;

/* Struct to measure the amount of already executed effort. */
typedef struct
{
    uint32  effortCtr_u32;  /* Effort that can still be spent - if 0 the main function shall not continue   */
    boolean enabled_b;      /* Effort limitation enabled, TRUE = yes, FALSE = no                            */
} Fee_Prv_LibEffortMeasure_tst;

typedef struct
{
    uint8 *     bfr_pu8;            /* Pointer to buffer where data is stored (= user buffer)   */
    uint32      nrBytesDone_u32;    /* Already considered number of bytes                       */
    uint32      length_u32;         /* Total number of bytes to consider                        */
    uint32 *    crc_pu32;           /* Pointer to the result                                    */
    boolean     firstCall_b;        /* First CRC call yes or no                                 */
} Fee_Prv_LibCalcCrcRam_tst;

/* RAM variable for the unit. Here all the data objects of the unit are collected */
typedef struct
{
    Fee_Prv_LibEffortMeasure_tst    libEffortMeasure_st;
    Fee_Prv_LibCalcCrcRam_tst       libCalcCrcRam_st;
}Fee_Prv_LibData_tst;

# endif
#endif  /* FEE_PRV_LIBTYPES_H */
