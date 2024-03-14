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

#ifndef FEE_PRV_PROC_H
#define FEE_PRV_PROC_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))
#include "MemIf_Types.h"
#include "Fee_Rb_Types.h"
#include "Fee_Prv_ProcTypes.h"
#include "Fee_PrvTypes.h"
#include "Fee_Prv_Config.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern void                         Fee_Prv_ProcDisableBackground(
                                        Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
extern void                         Fee_Prv_ProcEnableBackground(
                                        Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
extern void                         Fee_Prv_ProcSetDriverState(
                                        Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                        Fee_Prv_ProcDriverState_ten          driverState_en);
extern Fee_Prv_ProcDriverState_ten  Fee_Prv_ProcGetDriverState(Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_ModeType               Fee_Prv_ProcGetDriverMode(Fee_Rb_DeviceName_ten deviceName_en);
extern Fee_Rb_WorkingStateType_ten  Fee_Prv_ProcGetWorkingState(
                                        Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst);
extern boolean                      Fee_Prv_ProcIsStopModeActive(Fee_Rb_DeviceName_ten deviceName_en);
#  if(STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)
extern void                         Fee_Prv_ProcSetDriverMode(
                                        Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                        MemIf_ModeType                       mode);
#   if (FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION == 4) && (FEE_PRV_CFG_AR_RELEASE_MINOR_VERSION >=4)
extern void                         Fee_Prv_ProcSetDriverModeReq(
                                        Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                        MemIf_ModeType                       driverModeReq_en);
#   endif
#  endif
extern void                         Fee_Prv_ProcEnterCancel(
                                        Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                        Fee_Rb_User_ten                      user_en);
extern void                         Fee_Prv_ProcExitCancel(
                                        Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst);

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/

LOCAL_INLINE void Fee_Prv_ProcInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/
/*
 * \brief   Function called during Fee_Init(). Initializes all the variables of this unit.
 * To save code size + delay due to calling of function with parameter, the function is made inline.
 *
 *  * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE void Fee_Prv_ProcInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_ProcData_tst *  procData_pst;
    Fee_Prv_Proc_tst *      proc_pst;
    Fee_Prv_ProcLock_tst *  procLock_pst;

    procData_pst = &deviceConfigTable_pcst->feeData_pst->procData_st;

    proc_pst                              = &procData_pst->proc_st;
    proc_pst->driverState_ven             = FEE_PRV_PROC_DRIVERSTATE_UNINIT_E;
    proc_pst->driverModeActv_en           = MEMIF_MODE_SLOW;
#  if (FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION == 4) && (FEE_PRV_CFG_AR_RELEASE_MINOR_VERSION >=4)
    /* From Autosar 4.4 and newer */
    proc_pst->driverModeReqd_en           = MEMIF_MODE_SLOW;
#  endif
    proc_pst->backgroundState_en          = FEE_PRV_PROC_BACKGROUNDSTATE_DISABLED;
    proc_pst->backgroundCancelOngoing_b   = FALSE;
    proc_pst->stopModeActive_b            = FALSE;
    proc_pst->isWaitForNextChunk_b        = FALSE;
    proc_pst->isCnclAndRstrtFsm_b         = FALSE;
    proc_pst->user_en                     = FEE_RB_USER_MAX_E;

    procLock_pst                              = &procData_pst->procLock_st;
    procLock_pst->flgUsedSema_vb              = FALSE;
    procLock_pst->dbgReentrantMainFunction_vb = FALSE;

    procData_pst->procWorkingState_en         = FEE_RB_IDLE_E;


    return;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* FEE_PRV_CFG_COMMON_ENABLED */
# endif

/* FEE_PRV_PROC_H */
#endif
