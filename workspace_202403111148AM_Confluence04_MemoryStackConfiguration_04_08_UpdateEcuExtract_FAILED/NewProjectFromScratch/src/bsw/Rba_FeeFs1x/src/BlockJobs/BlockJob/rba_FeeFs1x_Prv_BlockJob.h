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

#ifndef RBA_FEEFS1X_PRV_BLOCKJOB_H
#define RBA_FEEFS1X_PRV_BLOCKJOB_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

# if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

#include "rba_FeeFs1x_Prv_Cfg.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"

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
// jobfunctions are published in rba_FeeFs1x.h
extern void rba_FeeFs1x_BlockJob_init(void);
extern void rba_FeeFs1x_BlockJob_cancel(void);

extern void rba_FeeFs1x_BlockJob_execWr(
        uint32 dataCRC_u32,
        rba_FeeFs1x_BlockJob_execWr_jobType_ten jobType_en,
        boolean isCpyRed_b,
        boolean isCopy1Destroyed_b,
        boolean isCopy2Destroyed_b
        );

extern void rba_FeeFs1x_BlockJob_execWrMaintain(
        boolean isCopy1Destroyed_b,
        boolean isCopy2Destroyed_b,
        boolean isNoFB_b,
        boolean writeTwice_b
        );

extern void rba_FeeFs1x_BlockJob_ReportBlkDataCrcError(void);
extern boolean rba_FeeFs1x_BlockJob_IsBlkDataCrcError(void);

extern void rba_FeeFs1x_BlockJob_jobDummy(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en);
extern rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_jobDoDummy(void);

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif
#endif

