/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef RBA_FEEFS1X_PRV_WRITEJOB_H
#define RBA_FEEFS1X_PRV_WRITEJOB_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

# if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

#include "rba_FeeFs1x_Prv_Cfg.h"
#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"

#include "Fee_Prv_Job.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#  if(RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS == 0u)
/* No redundant blocks present, replace calls of public functions via pointer with calls of dummy functions */
#define rba_FeeFs1x_WrJob_execWr_Red          rba_FeeFs1x_BlockJob_jobDummy
#define rba_FeeFs1x_WrJob_execWr_NoFBRed      rba_FeeFs1x_BlockJob_jobDummy
#  endif

#  if((FALSE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_WRITE) && (FALSE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_SURVIVAL_BLOCK_WRITE))
/* No unknown block write, replace call of public function via pointer with call of dummy function */
#define rba_FeeFs1x_WrJob_execWr_UnknownBlk   rba_FeeFs1x_BlockJob_jobDummy
#  endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"
extern void rba_FeeFs1x_WrJob_cancel(void);
extern void rba_FeeFs1x_WrJob_initJob(Fee_Prv_JobDesc_tst const * orderStruct_pcst);

/* checkWrJob function */
extern void rba_FeeFs1x_WrJob_checkWrReq(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);

/* execWrJob functions */
extern void rba_FeeFs1x_WrJob_execWr_Native(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
#  if(RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
extern void rba_FeeFs1x_WrJob_execWr_Red(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
#  endif

/* checkWrJob_Do function */
extern rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo(void);

/* execWrJob_NoFB functions */
extern void rba_FeeFs1x_WrJob_execWr_NoFBNative(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
#  if(RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
extern void rba_FeeFs1x_WrJob_execWr_NoFBRed(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
#  endif

/* execWrJob_UnknownBlk function */
#  if((TRUE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_WRITE) || (TRUE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_SURVIVAL_BLOCK_WRITE))
extern void rba_FeeFs1x_WrJob_execWr_UnknownBlk(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
#  endif

/*
 **********************************************************************************************************************
 * Inline functions
 **********************************************************************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_WrJob_init(void);
LOCAL_INLINE void rba_FeeFs1x_WrJob_init(void)
{
    rba_FeeFs1x_WrJob_cancel();
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif
#endif /* RBA_FEEFS1X_PRV_WRITEJOB_H */

