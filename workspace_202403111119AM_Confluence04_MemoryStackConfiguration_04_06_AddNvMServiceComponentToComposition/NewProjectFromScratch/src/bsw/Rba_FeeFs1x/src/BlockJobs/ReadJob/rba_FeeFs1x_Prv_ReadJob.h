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

#ifndef RBA_FEEFS1X_PRV_READJOB_H
#define RBA_FEEFS1X_PRV_READJOB_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"
#include "Fee_Prv_Job.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */


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

extern void rba_FeeFs1x_RdJob_cancel(void);
extern void rba_FeeFs1x_RdJob_initJob(Fee_Prv_JobDesc_tst const * orderStruct_pcst);

/* checkRdJob functions */
extern void rba_FeeFs1x_RdJob_checkRdReq(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
extern rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_RdJob_checkRdReqDo(void);

/* execRdJob functions */
extern void rba_FeeFs1x_RdJob_execRd(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
extern rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_RdJob_execRdDo(void);

/*
 **********************************************************************************************************************
 * Inline functions
 **********************************************************************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_RdJob_init(void);
LOCAL_INLINE void rba_FeeFs1x_RdJob_init(void)
{
    rba_FeeFs1x_RdJob_cancel();
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

