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

#ifndef RBA_FEEFS1X_PRV_SECTORERASEJOB_H
#define RBA_FEEFS1X_PRV_SECTORERASEJOB_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv.h"
#include "Std_Types.h"

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

extern void rba_FeeFs1x_SectorEraseJob_cancel(void);
extern boolean rba_FeeFs1x_SectorEraseJob_isEraseRequired(void);

extern void                   rba_FeeFs1x_SectorEraseJob_erase(void);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_SectorEraseJob_eraseDo(void);

/*
 **********************************************************************************************************************
 * Inline functions
 **********************************************************************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_SectorEraseJob_init(void);
LOCAL_INLINE void rba_FeeFs1x_SectorEraseJob_init(void)
{
    rba_FeeFs1x_SectorEraseJob_cancel();
}
#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

