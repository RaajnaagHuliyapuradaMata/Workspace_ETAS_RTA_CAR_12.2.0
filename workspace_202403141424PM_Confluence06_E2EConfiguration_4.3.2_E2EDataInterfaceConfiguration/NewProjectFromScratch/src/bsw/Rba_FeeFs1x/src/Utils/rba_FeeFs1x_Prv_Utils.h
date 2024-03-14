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
#ifndef RBA_FEEFS1X_PRV_UTILS_H
#define RBA_FEEFS1X_PRV_UTILS_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv.h"
#include "Fee_Prv_Proc.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
/* Limit the number of bytes that can be processed in one step. Higher value means more execution time for Fee_MainFunction().
 * Lower value means more MainFunction calls needed to do a job. The main function execution time was found to be within
 * limit when 1024u was used, hence the value is fixed. It would be more complex to derive this value based on the actual
 * limit for the MainFunction which can be different for different product lines. */
#define RBA_FEEFS1X_UTILS_RAMCRC_CALC_RESTRICTION (1024u)

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
 * Inline declarations
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern void                   rba_FeeFs1x_Utils_computeUserBufferCrc(uint8 const * bufferPtr_pcu8, uint32 length_u32, uint32 * crc_pu32);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Utils_computeUserBufferCrcDo(void);

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif /*RBA_FEEFS1X_UTILS_H*/
