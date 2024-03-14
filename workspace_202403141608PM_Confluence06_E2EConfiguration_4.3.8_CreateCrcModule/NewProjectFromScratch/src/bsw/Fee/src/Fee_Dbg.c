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

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
#if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "Fee_Prv_Dbg.h"

/* Disable unit when debug features aren't needed */
# if(defined(FEE_PRV_CFG_DBG) && (TRUE == FEE_PRV_CFG_DBG))

#include "Fee_Cfg_BlockDefines.h"
#include "Fee_PrvTypes.h"
#include "Fee_Prv_Config.h"
#include "Fee_Prv_ConfigTypes.h"
#include "Fee_Prv_FsIf.h"
#include "Fee_Prv_Lib.h"
#include "Fee_Rb_Idx.h"
#include "rba_MemLib.h"

#  if(!defined(RBA_MEMLIB_CFG_ENABLE_PROTOTYPE))
#  error Fee debugging features have been enabled but prototypes were not enabled for the Mem stack in rba_MemLib. \
         The debugging features were enabled under BCT container FeeRbDebug.                                       \
         Please enable the BCT switch rba_MemLibCentralEnablePrototype - for more details see rba_MemLib docu.     \
         Fee debugging features shall not be used in series environment.                                           \
         For quick debugging you can also delete these lines losing series maturity.
#  endif

#  if(TRUE == FEE_PRV_CFG_DBG_DUMP)
#include <stdio.h>
#include <string.h>
#include "rba_FlsVecu_Vecu.h"
#  endif

/*
 **********************************************************************************************************************
 * Local macros
 **********************************************************************************************************************
*/
#define FEE_PRV_DBG_OFFSET_FOR_READ_VALIDATION  0u
#define FEE_PRV_DBG_LENGTH_FOR_READ_VALIDATION  1u

/*
 **********************************************************************************************************************
 * Static declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"
#  if(TRUE == FEE_PRV_CFG_DBG_TIME)
static void Fee_Prv_DbgHistInsert(uint32 * hist_pu32, uint32 value_u32, uint32 histSize_u32);
#  endif
#  if( (TRUE == FEE_PRV_CFG_DBG_TIME) && (TRUE == FEE_PRV_CFG_DBG_BLOCK) )
static void Fee_Prv_DbgTeMerge(Fee_Prv_DbgTimeEntry_tst * t1_pst, Fee_Prv_DbgTimeEntry_tst const * t2_pcst);
#  endif

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/
#  if(TRUE == FEE_PRV_CFG_DBG_TIME)
/**
 * \brief   Insert a value into a histogram.
 *          The histogram is in logarithmic scale with basis of 2.
 *          The intervals of the baskets in the histogram are
 *          [0, 1[
 *          [1, 2[
 *          [2, 4[
 *          [4, 8[
 *          ...
 *          [log(2,histSize_u32 - 1),infinity[
 *
 * \param   hist_pu32       Pointer to the histogram
 * \param   value_u32       Value that shall be inserted into the histogram
 * \param   histSize_u32    Size of the histogram
*/
static void Fee_Prv_DbgHistInsert(uint32 * hist_pu32, uint32 value_u32, uint32 histSize_u32)
{
    /* Local variable */
    uint32 basket_u32 = 0u;

    /* Loop over intervals of histogram */
    /* Evaluation of while loop: The while loop will be exited after a maximum of histSize_u32 calls, which is a derived
     * from paramdef FeeRbDebugTimeHistogramSize - 1.
     * This configured value will be limited due to available RAM space and hence, the below loop will not be stuck infinitely.
     * Moreover, this code is used only for debugging purpose and is not part of the productive software.  */
    while( (value_u32 > (1u << basket_u32)) && (basket_u32 < (histSize_u32 - 1u)) )
    {
        /* Next interval */
        basket_u32++;
    }

    /* Insert value */
    hist_pu32[basket_u32]++;

    return;
}
#  endif

#  if( (TRUE == FEE_PRV_CFG_DBG_TIME) && (TRUE == FEE_PRV_CFG_DBG_BLOCK) )
/**
 * \brief   Merge histograms and maximum values of two time entries.
 *
 * \param   t1_pst      First time entry, will also be the target of the merge
 * \param   t2_pcst     Second time entry
*/
static void Fee_Prv_DbgTeMerge(Fee_Prv_DbgTimeEntry_tst * t1_pst, Fee_Prv_DbgTimeEntry_tst const * t2_pcst)
{
    /* Local variable */
    uint32 a_u32;

    /* Loop over source histogram */
    for(a_u32 = 0; a_u32 < FEE_PRV_CFG_DBG_TIME_HISTOGRAM_BASKETS; a_u32++)
    {
        /* Merge source into target histogram */
        t1_pst->hist_au32[a_u32] += t2_pcst->hist_au32[a_u32];
    }

    /* If maximum value in source histogram is greater ... */
    if(t1_pst->tiMax_u32 < t2_pcst->tiMax_u32)
    {
        /* ... merge this value into target histogram */
        t1_pst->tiMax_u32 = t2_pcst->tiMax_u32;
    }

    return;
}
#  endif

/**
 * \brief   Main function of the debug unit to execute cyclic debugging actions.
 *
 * \param   deviceName_en    Device instance for which job has to be performed
*/
void Fee_Prv_DbgMainFunction(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
#  if(TRUE == FEE_PRV_CFG_DBG_BLOCK)
    /* Local variables */
    Fee_Rb_DeviceName_ten   deviceName_en = deviceConfigTable_pcst->deviceName_en;
    Fee_Prv_Dbg_tst *       dbg_pst = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbg_st;
    uint8 *                 dbgBfr_pau8 = deviceConfigTable_pcst->feeData_pst->dbgData_st.dbgBfr_au8;
    MemIf_StatusType        status_en = Fee_Rb_Idx_GetStatus(deviceName_en);
    /*
     * Only start a write of the debug block under the following conditions:
     * - A write of the debug block is requested
     * - Fee_Rb_EndInit was called (Fee_Prv_EraseEnabled_b = TRUE)
     * - No effort was spent already
     * - The Fee is currently idle
     */
    if(Fee_Prv_LibNoEffortSpent(deviceName_en) && (dbg_pst->isDebugBlockWriteRequested_b) && (MEMIF_IDLE == status_en))
    {
#   if(TRUE == FEE_PRV_CFG_DBG_TIME)
        uint16                  dbgBfrSize_u16 = FEE_PRV_CFG_DBG_BLOCK_SIZE;
        uint32                  packAdr_u32    = 0u;
        Fee_Prv_DbgInfo_tst*    dbgInfo_pst    = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbgInfo_st;

        /* Space sufficient for the timing measurements? */
        if(dbgBfrSize_u16 >= (packAdr_u32 + sizeof(dbgInfo_pst->timer_ast)))
        {
            /* Pack timing measurements of debug struct into buffer */
            rba_MemLib_MemCopy(
                (uint8 const *)(&dbgInfo_pst->timer_ast[0]),
                &dbgBfr_pau8[packAdr_u32],
                sizeof(dbgInfo_pst->timer_ast)
            );
            packAdr_u32 += sizeof(dbgInfo_pst->timer_ast);
        }
#   endif

        /* Place a write-order at the Fee */
        if(E_OK == Fee_Rb_Idx_Write(deviceName_en, FeeConf_FeeBlockConfiguration_Fee_DebugBlock, &dbgBfr_pau8[0u]))
        {
            /* Track the number of debug block write for debugging purposes */
            dbg_pst->nrDbgBlockWrites_u32++;

            /* Reset write request for debug block */
            dbg_pst->isDebugBlockWriteRequested_b = FALSE;
        }
    }
#  else
    (void)deviceConfigTable_pcst;
#  endif

#  if(TRUE == FEE_PRV_CFG_DBG_TIME)
    /* Update the timing measurements periodically */
    rba_MemLib_UpdateTimerUs();
#  endif

    return;
}

#  if(TRUE == FEE_PRV_CFG_DBG_TIME)
/**
 * \brief   Start a timing measurement
 *
 * \param   deviceName_en   Device instance for which timer has to be started
 * \param   timer_en        Timer that has to be started
*/
void Fee_Prv_DbgWatchStart(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_DgbTimeAccessEnum_ten timer_en)
{
    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_DbgTimeEntry_tst *               time_pst;

    if(FEE_PRV_DBG_TIMER_NR_OF_TIMERS > timer_en)
    {
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
        time_pst = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbgInfo_st.timer_ast[timer_en];

        /* Take start-timestamp */
        rba_MemLib_UpdateTimerUs();
        time_pst->tiTmp_u32 = rba_MemLib_GetTimerUs();
    }

    return;
}

/**
 * \brief   Stop a previously started timing measurement, and evaluate its result
 *
 * \param   deviceName_en       Device instance for which timer has to be stopped
 * \param   timer_en            Timer that is running/has to be stopped
 * \param   updateDebugBlock_b  In case the time is a new maximum trigger storing the debug block
 *
 * \seealso Fee_Prv_DbgWatchStart
*/
void Fee_Prv_DbgWatchStop(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_DgbTimeAccessEnum_ten timer_en, boolean updateDebugBlock_b)
{
    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_DbgTimeEntry_tst *               time_pst;

    if(FEE_PRV_DBG_TIMER_NR_OF_TIMERS > timer_en)
    {
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

        time_pst               = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbgInfo_st.timer_ast[timer_en];

        /* Take stop-timestamp to update last ... */
        rba_MemLib_UpdateTimerUs();
        time_pst->tiLast_u32 = rba_MemLib_GetTimerUs() - time_pst->tiTmp_u32;

        /* ... and accumulated timing measurement */
        time_pst->tiAcc_u32 += time_pst->tiLast_u32;

        /* Last timing measurement exceeds the maximum? */
        if(time_pst->tiMax_u32 < time_pst->tiLast_u32)
        {
            /* Update maximum timing measurement */
            time_pst->tiMax_u32 = time_pst->tiLast_u32;

#   if(TRUE == FEE_PRV_CFG_DBG_BLOCK)
            /* If requested, trigger storing the debug info struct */
            if(updateDebugBlock_b)
            {
                Fee_Prv_Dbg_tst *    dbg_pst = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbg_st;

                dbg_pst->isDebugBlockWriteRequested_b = TRUE;
            }
#   else
            (void)updateDebugBlock_b;
#   endif
        }

        /* Insert new timing measurement into histogram */
        Fee_Prv_DbgHistInsert(&time_pst->hist_au32[0], time_pst->tiLast_u32, FEE_PRV_CFG_DBG_TIME_HISTOGRAM_BASKETS);
    }

    return;
}
#  endif  /* FEE_PRV_CFG_DBG_TIME */

#  if(TRUE == FEE_PRV_CFG_DBG_DUMP)
/**
 * \brief   Create a dump in the Fee development simulation.
 *          This is only active during development.
 *
 * \param   deviceName_en   Device instance for which job has to be performed
*/
void Fee_Prv_DbgDump(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Local variables */
    char filename_ac[50];
    char Path_achr[RBA_FLSVECU_FILEPATH_MAX];
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_Dbg_tst*                     dbg_pst                = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbg_st;

    /* Create numbered dump */
    (void)sprintf(filename_ac, "Fee_Dump%lu", dbg_pst->nrDump_u32);

    (void)strcpy(Path_achr, "_log\\");
    (void)strcat(Path_achr, filename_ac);
    (void)rba_FlsVecu_Vecu_SaveDump_Data(
            deviceConfigTable_pcst->deviceIdx_u8,
            Fee_Prv_FsIfGetEmulationStartAdr(deviceName_en),
            (Fee_Prv_FsIfGetEmulationEndAdr(deviceName_en) - Fee_Prv_FsIfGetEmulationStartAdr(deviceName_en) + 1u),
            Path_achr,
            RBA_FLSVECU_FILEFORMAT_TEXT );

    /* Prepare for next dump */
    dbg_pst->nrDump_u32++;

    return;
}

/**
 * \brief   Create a dump if the job result is failed.
 *          Useful for finding unexpected errors.
 *          This is only active during development.
 *
 * \param   deviceName_en   Device instance for which job has to be performed
 * \param   Job result
*/
void Fee_Prv_DbgFailDump(Fee_Rb_DeviceName_ten deviceName_en, MemIf_JobResultType result_en)
{
    /* Job has failed? */
    if(MEMIF_JOB_FAILED == result_en)
    {
        /* Create a dump */
        Fee_Prv_DbgDump(deviceName_en);
    }

    return;
}
#  endif  /* FEE_PRV_CFG_DBG_DUMP */

#  if(TRUE == FEE_PRV_CFG_DBG_BLOCK)
/**
 * \brief   Read out the debug block into the debug buffer.
 *          This is a synchronous job and the main functions are called within the function to complete the job.
 *
 * \param   deviceName_en   Device instance for which job has to be performed
*/
void Fee_Prv_DbgBlockRead(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_DbgData_tst*        dbgData_pst    = &deviceConfigTable_pcst->feeData_pst->dbgData_st;
#   if(TRUE == FEE_PRV_CFG_DBG_TIME)
    Fee_Prv_DbgInfo_tst *       dbgInfo_pst    = &deviceConfigTable_pcst->feeData_pst->dbgData_st.dbgInfo_st;
    uint32                      packAdr_u32;
    Fee_Prv_DbgTimeEntry_tst    init_st;
#   endif /* FEE_PRV_CFG_DBG_TIME */
    MemIf_JobResultType         jobResult_en;
    Fls_Rb_FlsDevice_ten        flsDeviceId_en = Fls_Rb_GetDeviceIdFromDrvIndex(deviceConfigTable_pcst->deviceIdx_u8);
    uint16                      dbgBfrSize_u16 = FEE_PRV_CFG_DBG_BLOCK_SIZE;

    /* Perform the read */
    if(E_OK == Fee_Rb_Idx_Read(deviceName_en,
                               FeeConf_FeeBlockConfiguration_Fee_DebugBlock,
                               FEE_PRV_DBG_OFFSET_FOR_READ_VALIDATION,
                               &dbgData_pst->dbgBfr_au8[0],
                               dbgBfrSize_u16))
    {
        do
        {
            Fee_Rb_Idx_MainFunction(deviceName_en);
            if(deviceConfigTable_pcst->syncLoopFlsMainHook_pfn != NULL_PTR)
            {
                deviceConfigTable_pcst->syncLoopFlsMainHook_pfn();
            }
            Fee_Prv_Lib_Fls_MainFunction(flsDeviceId_en);

            /* Query job status */
            jobResult_en = Fee_Rb_Idx_GetInternalJobResult(deviceName_en);
        } while(MEMIF_JOB_PENDING == jobResult_en);
        /* Evaluation of while loop: In worst case the while loop will be stuck infinitely when the given Fls job is
         * never completed. This can be handled by using the timeout feature of Fls.
         * Moreover, this code is used only for debugging purpose and is not part of the productive software.  */
    }
    else
    {
        /* Software should not reach here. anyways treat this as a read error. could also be changed to MEMIF_BLOCK_INCONSISTENT */
        jobResult_en = MEMIF_JOB_FAILED;
    }

    /* Read has failed? */
    if(MEMIF_JOB_OK != jobResult_en)
    {
        /* Set all to 0 */
        rba_MemLib_MemSet(dbgData_pst->dbgBfr_au8, 0u, dbgBfrSize_u16);
    }

#   if(TRUE == FEE_PRV_CFG_DBG_TIME)
    packAdr_u32 = 0u;
    /*
     * This function shall only be called at the end of Fee_Init.
     * At this place a value for the init time is already available.
     * This measurement shall not be overwritten but merged with the already available data.
     */
    init_st = dbgInfo_pst->timer_ast[FEE_PRV_DBG_TIMER_INIT_E];

    /* Space sufficient for the timing measurements? */
    if(dbgBfrSize_u16 >= (packAdr_u32 + sizeof(dbgInfo_pst->timer_ast)))
    {
        /* Copy the read timing measurements into debug struct */
        rba_MemLib_MemCopy(
            &dbgData_pst->dbgBfr_au8[packAdr_u32],
            (uint8 *)(&dbgInfo_pst->timer_ast[0]),
            sizeof(dbgInfo_pst->timer_ast)
        );
        packAdr_u32 += sizeof(dbgInfo_pst->timer_ast);
    }

    /* Merge read and taken (init) time measurement */
    Fee_Prv_DbgTeMerge(&dbgInfo_pst->timer_ast[FEE_PRV_DBG_TIMER_INIT_E], &init_st);
#   endif /* FEE_PRV_CFG_DBG_TIME */

    return;
}
#  endif  /* FEE_PRV_CFG_DBG_BLOCK */

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif  /* FEE_PRV_CFG_DBG */

#endif /* FEE_PRV_CFG_COMMON_ENABLED */
