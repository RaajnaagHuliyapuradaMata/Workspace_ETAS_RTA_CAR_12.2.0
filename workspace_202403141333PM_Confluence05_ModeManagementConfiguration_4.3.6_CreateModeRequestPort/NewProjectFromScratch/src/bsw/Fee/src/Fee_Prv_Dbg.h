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

#ifndef FEE_PRV_DBG_H
#define FEE_PRV_DBG_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))
#include "Fee_Prv_Config.h"
#include "Fee_Prv_DbgTypes.h"
#include "Fee_PrvTypes.h"
#include "Fee_Rb_Types.h"
#include "rba_MemLib.h"

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

LOCAL_INLINE void Fee_Prv_DbgDummyFunc(void)
{
    return;
}

#  if(TRUE == FEE_PRV_CFG_DBG)
extern void Fee_Prv_DbgMainFunction(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);

/*
 * \brief init function for the variables used in the Dbg unit
 * To save code size + delay due to calling of function with parameter, the function is made inline.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE void Fee_Prv_DbgInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void Fee_Prv_DbgInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    /* The contents of dbgInfo_pst depends on compiler switch settings,
     * i.e. an initialization with rba_MemLib_MemSet is easiest. */
    Fee_Prv_DbgInfo_tst *   dbgInfo_pst = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbgInfo_st;
    Fee_Prv_Dbg_tst *       dbg_pst     = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbg_st;
#   if(TRUE == FEE_PRV_CFG_DBG_BLOCK)
    uint8 *                 dbgBfr_pau8 = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbgBfr_au8[0];

    rba_MemLib_MemSet(dbgBfr_pau8, 0u, FEE_PRV_CFG_DBG_BLOCK_SIZE);

    dbg_pst->nrDbgBlockWrites_u32 = 0u;
    dbg_pst->isDebugBlockWriteRequested_b = FALSE;
#   endif
#  if(TRUE == FEE_PRV_CFG_DBG_DUMP)
    dbg_pst->nrDump_u32 = 0u;
#  endif
    dbg_pst->freeze_b = FALSE;

    rba_MemLib_MemSet((uint8 *)(dbgInfo_pst), 0u, sizeof(Fee_Prv_DbgInfo_tst));

    return;
}

#  else
#define Fee_Prv_DbgMainFunction(deviceConfigTable_pcst)   Fee_Prv_DbgDummyFunc()
#define Fee_Prv_DbgInit(dbgData_pst)                      Fee_Prv_DbgDummyFunc()
#  endif

#  if(defined(RBA_FEEFS2_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS2_PRV_CFG_ENABLED))
#   if(defined(FEE_PRV_CFG_DBG_GRAY_BOX_TEST) && (TRUE == FEE_PRV_CFG_DBG_GRAY_BOX_TEST))
extern void Fee_Test_Prv_UTIL_05_CheckFatEntries(void);
#define Fee_Dbg_CallOut_CheckFatEntries() Fee_Test_Prv_UTIL_05_CheckFatEntries()
#   else
#define Fee_Dbg_CallOut_CheckFatEntries()                 Fee_Prv_DbgDummyFunc()
#   endif
#  endif

#  if(TRUE == FEE_PRV_CFG_DBG_TIME)

extern void Fee_Prv_DbgWatchStart(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_DgbTimeAccessEnum_ten timer_en);
extern void Fee_Prv_DbgWatchStop (Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_DgbTimeAccessEnum_ten timer_en, boolean updateDebugBlock_b);

LOCAL_INLINE Fee_Prv_DgbTimeAccessEnum_ten Fee_Prv_DbgWatchGetTmrForJob(Fee_Rb_JobType_ten idJobType_en);

/**
 * \brief   Determine the timer for a given job
 *
 * \param   idJobType_en   Job for which timer is requested
 *
 * \return  Timer for given job
*/
LOCAL_INLINE Fee_Prv_DgbTimeAccessEnum_ten Fee_Prv_DbgWatchGetTmrForJob(Fee_Rb_JobType_ten idJobType_en)
{
    /* Return variable */
    Fee_Prv_DgbTimeAccessEnum_ten timer_en;

    switch(idJobType_en)
    {
        case FEE_RB_JOBTYPE_READ_E:             timer_en = FEE_PRV_DBG_TIMER_READ_E;       break;
        case FEE_RB_JOBTYPE_WRITE_E:            timer_en = FEE_PRV_DBG_TIMER_WRITE_E;      break;
        case FEE_RB_JOBTYPE_INVALIDATE_E:       timer_en = FEE_PRV_DBG_TIMER_INVALIDATE_E; break;
        case FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E: timer_en = FEE_PRV_DBG_TIMER_MAINTAIN_E;   break;
        default:                                timer_en = FEE_PRV_DBG_TIMER_NR_OF_TIMERS; break;
    }

    return(timer_en);
}

#  else

#define Fee_Prv_DbgWatchStart(A, B)    Fee_Prv_DbgDummyFunc()
#define Fee_Prv_DbgWatchStop(A, B, C)  Fee_Prv_DbgDummyFunc()

#  endif

#  if(TRUE == FEE_PRV_CFG_DBG_DUMP)

extern void Fee_Prv_DbgDump(Fee_Rb_DeviceName_ten deviceName_en);
extern void Fee_Prv_DbgFailDump(Fee_Rb_DeviceName_ten deviceName_en, MemIf_JobResultType result_en);

#  else

#define Fee_Prv_DbgDump(A)             Fee_Prv_DbgDummyFunc()
#define Fee_Prv_DbgFailDump(A, B)      Fee_Prv_DbgDummyFunc()

#  endif

#  if(TRUE == FEE_PRV_CFG_DBG_BLOCK)

extern void Fee_Prv_DbgBlockRead(Fee_Rb_DeviceName_ten deviceName_en);

#  else

#define Fee_Prv_DbgBlockRead(A)        Fee_Prv_DbgDummyFunc()

#  endif


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"


/* FEE_PRV_CFG_COMMON_ENABLED */
# endif


/* FEE_PRV_DBG_H */
#endif
