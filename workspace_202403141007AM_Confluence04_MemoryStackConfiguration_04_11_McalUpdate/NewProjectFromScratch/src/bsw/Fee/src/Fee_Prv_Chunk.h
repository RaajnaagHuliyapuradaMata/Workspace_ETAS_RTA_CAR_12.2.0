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

#ifndef FEE_PRV_CHUNK_H
#define FEE_PRV_CHUNK_H


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"


/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

/* Disable this unit when not needed */
#  if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS))

#include "Fee_Rb_Types.h"
#include "Fee_Prv_Config.h"

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#   if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
extern void    Fee_Prv_ChunkInit (void);
#   else
/* When FeeFs1 isn't used, there are no static variables to initalize in this unit */
#define Fee_Prv_ChunkInit()
#   endif

extern void    Fee_Prv_ChunkDoneCbk (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        Fee_Rb_JobResultType_ten                 stRes_en);

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#  else

/* When this unit is disabled, there are no static variables to initalize */
#define Fee_Prv_ChunkInit()
/* #  if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS)) */

#  endif

/* # if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED)) */
# endif

/* #ifndef FEE_PRV_CHUNK_H */
#endif
