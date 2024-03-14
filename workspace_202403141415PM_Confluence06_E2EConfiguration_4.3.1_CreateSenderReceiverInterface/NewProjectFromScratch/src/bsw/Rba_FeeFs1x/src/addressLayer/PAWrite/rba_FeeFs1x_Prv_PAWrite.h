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

#ifndef RBA_FEEFS1X_PRV_PAWRITE_H
#define RBA_FEEFS1X_PRV_PAWRITE_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv.h"
#include "Fee_Rb_Types.h"

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

typedef enum
{
    rba_FeeFs1x_PAWrite_reorg_e,
    rba_FeeFs1x_PAWrite_jobReq_e,
    rba_FeeFs1x_PAWrite_maxNr_e
} rba_FeeFs1x_PAWrite_jobType_ten;

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

extern void rba_FeeFs1x_PAWrite_cancel(void);
extern void rba_FeeFs1x_PAWrite_reorgPrepareSpace(uint32 noDataPages_u32, uint8 noHdrPages_u8, uint32* startPageForRequest_pu32);
extern void rba_FeeFs1x_PAWrite_jobReqPrepareSpace(uint32 noDataPages_u32, uint8 noHdrPages_u8, uint32* startPageForRequest_pu32, Fee_Rb_JobMode_ten jobMode_en);

extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo(rba_FeeFs1x_PAWrite_jobType_ten jobType_en);


extern void                   rba_FeeFs1x_PAWrite_copyFls2Fls(uint32 source_startPage_u32, uint32 noOfPages_u32, uint32 dest_startPage_u32 , boolean updateCurrWrPage_b);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo(void);

extern void                   rba_FeeFs1x_PAWrite_write(uint32 startPage_u32 , uint32 nrBytes_u32 , uint8 const * sourcebuffer_pcu8 , boolean updateCurrWrPage_b, Fee_Rb_JobMode_ten jobMode_en);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo(void);

/*
 **********************************************************************************************************************
 * Inlines
 **********************************************************************************************************************
*/
LOCAL_INLINE void rba_FeeFs1x_PAWrite_init(void);
LOCAL_INLINE void rba_FeeFs1x_PAWrite_init(void)
{
    rba_FeeFs1x_PAWrite_cancel();
}

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_reorgPrepareSpaceDo(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_reorgPrepareSpaceDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo(rba_FeeFs1x_PAWrite_reorg_e);
    return retVal_en;
}

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_jobReqPrepareSpaceDo(void);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_jobReqPrepareSpaceDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo(rba_FeeFs1x_PAWrite_jobReq_e);
    return retVal_en;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

