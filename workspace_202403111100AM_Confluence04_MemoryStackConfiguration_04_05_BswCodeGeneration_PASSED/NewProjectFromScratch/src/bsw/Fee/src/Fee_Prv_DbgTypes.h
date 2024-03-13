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

#ifndef FEE_PRV_DBGTYPES_H
#define FEE_PRV_DBGTYPES_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* ATTENTION: Don't forget to adapt code of debug block size calculation in
 * FeeDebugConfig.java when modifying size or structure of Fee_Prv_DbgTimeEntry_tst */
typedef struct
{
    uint32 hist_au32 [FEE_PRV_CFG_DBG_TIME_HISTOGRAM_BASKETS];
    uint32 tiAcc_u32;
    uint32 tiMax_u32;
    uint32 tiLast_u32;
    uint32 tiTmp_u32;
} Fee_Prv_DbgTimeEntry_tst;

/* ATTENTION: In case the order or elements are changed for the below enum, don't forget to adapt code in the ECU-TEST
 * packages (*.pkg). Due to limitation in using macros in the scripts, the value of enum are used directly. */
typedef enum
{
    FEE_PRV_DBG_TIMER_INVALIDATE_E = 0u,
    FEE_PRV_DBG_TIMER_MAINTAIN_E,
    FEE_PRV_DBG_TIMER_READ_E,
    FEE_PRV_DBG_TIMER_WRITE_E,
    FEE_PRV_DBG_TIMER_INIT_E,
    FEE_PRV_DBG_TIMER_READALL_E,
    FEE_PRV_DBG_TIMER_PROCMAIN_E,
    FEE_PRV_DBG_TIMER_FREEUSE00_E,
    FEE_PRV_DBG_TIMER_FREEUSE01_E,
    FEE_PRV_DBG_TIMER_FREEUSE02_E,
    FEE_PRV_DBG_TIMER_FREEUSE03_E,
    FEE_PRV_DBG_TIMER_FREEUSE04_E,
    FEE_PRV_DBG_TIMER_NR_OF_TIMERS
} Fee_Prv_DgbTimeAccessEnum_ten;

/* ATTENTION: Don't forget to adapt code of debug block size calculation in
 * FeeDebugConfig.java when modifying size or structure of Fee_Prv_DbgInfo_tst */
typedef struct
{
#  if(TRUE == FEE_PRV_CFG_DBG_TIME)
    Fee_Prv_DbgTimeEntry_tst timer_ast[FEE_PRV_DBG_TIMER_NR_OF_TIMERS];
#  endif
    boolean                  dummy_b;
} Fee_Prv_DbgInfo_tst;

typedef struct
{
#  if(TRUE == FEE_PRV_CFG_DBG_DUMP)
    uint32              nrDump_u32;
#  endif
#  if(TRUE == FEE_PRV_CFG_DBG_BLOCK)
    uint32              nrDbgBlockWrites_u32;
    boolean             isDebugBlockWriteRequested_b;
#  endif
    boolean volatile    freeze_b;
} Fee_Prv_Dbg_tst;

/* RAM variable for the unit. Here all the data objects of the unit are collected */
typedef struct
{
    Fee_Prv_DbgInfo_tst dbgInfo_st;
    Fee_Prv_Dbg_tst     dbg_st;
#  if(TRUE == FEE_PRV_CFG_DBG_BLOCK)
    uint8               dbgBfr_au8[FEE_PRV_CFG_DBG_BLOCK_SIZE];
#  endif
}Fee_Prv_DbgData_tst;

# endif

/* FEE_PRV_DBGTYPES_H */
#endif
