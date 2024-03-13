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

#ifndef FEE_PRV_PERS_H
#define FEE_PRV_PERS_H


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
#  if(defined(FEE_PRV_CFG_RB_PERSIST_DATA) && (TRUE == FEE_PRV_CFG_RB_PERSIST_DATA))

#include "Fee_Prv_Config.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* Magic value for detection of saved zone losses */
#define FEE_PRV_PERS_SAVED_ZONE_ALIVE    (0xABCDEFABu)


#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

extern void              Fee_Prv_PersInit               (Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst);
extern void              Fee_Prv_PersDataRead           (Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst);
extern uint8 const *     Fee_Prv_PersDataFillWriteBuf   (
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint8 const *            dataFsSpc_pcu8,
        uint32                   nrLenFsSpc_u32);
extern Std_ReturnType    Fee_Prv_PersDataWrite          (Fee_Rb_DeviceName_ten deviceName_en);

#   if(defined(FEE_PRV_CFG_RB_STRESS_MEASMT) && (TRUE == FEE_PRV_CFG_RB_STRESS_MEASMT))
extern void              Fee_Prv_PersStressMeasmtUpd    (Fee_Rb_DeviceName_ten deviceName_en, uint32 nrBytProgd_u32);
extern Std_ReturnType    Fee_Prv_PersStressMeasmtRstrt  (Fee_Rb_DeviceName_ten deviceName_en);
extern Std_ReturnType    Fee_Prv_PersStressMeasmtGetRes (
        Fee_Rb_DeviceName_ten    deviceName_en,
        boolean                  isSncePredStrtReq_b,
        uint32 *                 nrDrvCycMaxRea_pu32);
#   endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* #  if(defined(FEE_PRV_CFG_RB_PERSIST_DATA) && (TRUE == FEE_PRV_CFG_RB_PERSIST_DATA)) */
#  else

/* When this unit is disabled, the below functions must be present but can be dummy */
#define    Fee_Prv_PersInit(xCfgDevTbl_pcst)
#define    Fee_Prv_PersDataRead(xCfgDevTbl_pcst)

/* #  if(!defined(FEE_PRV_CFG_RB_PERSIST_DATA) || (FALSE == FEE_PRV_CFG_RB_PERSIST_DATA)) */
#  endif

#  if(!defined(FEE_PRV_CFG_RB_PERSIST_DATA)  || (FALSE == FEE_PRV_CFG_RB_PERSIST_DATA) || \
      !defined(FEE_PRV_CFG_RB_STRESS_MEASMT) || (FALSE == FEE_PRV_CFG_RB_STRESS_MEASMT)   )

/* When this unit or stress measurement is disabled, the below function must be present but can be dummy */
#define    Fee_Prv_PersStressMeasmtUpd(deviceName_en, nrBytProgd_u32)

#  endif

/* # if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED)) */
# endif

/* #ifndef FEE_PRV_PERS_H */
#endif
