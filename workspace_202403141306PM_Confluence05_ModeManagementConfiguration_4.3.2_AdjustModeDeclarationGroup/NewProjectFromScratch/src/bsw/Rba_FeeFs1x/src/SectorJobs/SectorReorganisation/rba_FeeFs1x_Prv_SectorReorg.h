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

#ifndef RBA_FEEFS1X_PRV_SECTORREORG_H
#define RBA_FEEFS1X_PRV_SECTORREORG_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv.h"


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

extern void rba_FeeFs1x_Reorg_cancel(void);

extern void                   rba_FeeFs1x_Reorg_reorganiseSector(boolean isSoftReorg_b);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Reorg_reorganiseSectorDo(void);

extern boolean rba_FeeFs1x_Reorg_isSoftReorgRequired(void);

extern boolean rba_FeeFs1x_Reorg_checkHSRNecessity(uint32 lastPageOfNewBlock_u32);

extern boolean rba_FeeFs1x_Reorg_isSoftReorgOngoing(void);
extern boolean rba_FeeFs1x_Reorg_isHardReorgOngoing(void);

/*
 **********************************************************************************************************************
 * Inline functions
 **********************************************************************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Reorg_init(void);
LOCAL_INLINE void rba_FeeFs1x_Reorg_init(void)
{
    rba_FeeFs1x_Reorg_cancel();
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

