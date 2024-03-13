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

#ifndef RBA_FEEFS1X_PRV_RB_MAINTAINJOB_H
#define RBA_FEEFS1X_PRV_RB_MAINTAINJOB_H
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

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#  if(FALSE == RBA_FEEFS1X_PRV_CFG_MAINTAIN)
/* Maintain is deactivated, replace calls of public functions via pointer with calls of dummy functions */
#define rba_FeeFs1x_MtJob_checkMtReq     rba_FeeFs1x_BlockJob_jobDummy
#define rba_FeeFs1x_MtJob_execMt         rba_FeeFs1x_BlockJob_jobDummy
#define rba_FeeFs1x_MtJob_execMt_NoFB    rba_FeeFs1x_BlockJob_jobDummy
#define rba_FeeFs1x_MtJob_checkMtReqDo   rba_FeeFs1x_BlockJob_jobDoDummy
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

#  if(FALSE != RBA_FEEFS1X_PRV_CFG_MAINTAIN)

extern void rba_FeeFs1x_MtJob_cancel(void);
LOCAL_INLINE void rba_FeeFs1x_MtJob_init(void);
LOCAL_INLINE void rba_FeeFs1x_MtJob_init(void)
{
    rba_FeeFs1x_MtJob_cancel();
}
extern void rba_FeeFs1x_MtJob_initJob(uint16 feeIndex_u16);


extern void rba_FeeFs1x_MtJob_checkMtReq(rba_FeeFs1x_Searcher_RetVal_ten searchresult_en);

extern void rba_FeeFs1x_MtJob_execMt(rba_FeeFs1x_Searcher_RetVal_ten searchresult_en);
extern void rba_FeeFs1x_MtJob_execMt_NoFB(rba_FeeFs1x_Searcher_RetVal_ten searchresult_en);

extern rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_MtJob_checkMtReqDo(void);

#  else
/* Maintain is deactivated, replace public function calls with dummy functions */

LOCAL_INLINE void rba_FeeFs1x_MtJob_cancel(void);
LOCAL_INLINE void rba_FeeFs1x_MtJob_cancel(void)
{
    return; /* Do nothing */
}


LOCAL_INLINE void rba_FeeFs1x_MtJob_init(void);
LOCAL_INLINE void rba_FeeFs1x_MtJob_init(void)
{
    return; /* Do nothing */
}

LOCAL_INLINE void rba_FeeFs1x_MtJob_initJob(uint16 feeIndex_u16);
LOCAL_INLINE void rba_FeeFs1x_MtJob_initJob(uint16 feeIndex_u16)
{
    (void) feeIndex_u16;
    return; /* Do nothing */
}

#  endif // (FALSE != RBA_FEEFS1X_PRV_CFG_MAINTAIN)

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif // (defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))
#endif // #ifndef RBA_FEEFS1X_PRV_RB_MAINTAINJOB_H
