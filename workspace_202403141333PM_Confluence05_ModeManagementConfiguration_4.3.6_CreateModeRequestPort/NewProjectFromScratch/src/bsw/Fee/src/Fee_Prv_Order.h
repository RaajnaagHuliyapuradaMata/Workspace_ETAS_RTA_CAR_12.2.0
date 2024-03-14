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
 * The order unit is responsible for handling all external calls to the Fee besides the main and init function.
 * It stores the received orders in internal order slots.
 * The main function will poll orders from the order unit and inform the order unit if an order is finished.
 */

#ifndef FEE_PRV_ORDER_H
#define FEE_PRV_ORDER_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))
#include "Fee_Cfg.h"

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern Std_ReturnType   Fee_Prv_OrderDetCheckDeviceName            (Fee_Rb_DeviceName_ten deviceName_en, uint8 apiId_u8);
extern Std_ReturnType   Fee_Prv_OrderDetCheckModuleInitAndStopMode (Fee_Rb_DeviceName_ten deviceName_en, uint8 apiId_u8);
extern Std_ReturnType   Fee_Prv_OrderDetCheckAdrPtr                (Fee_Rb_DeviceName_ten deviceName_en, uint8 apiId_u8, void const * bfr_pcv);

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"


# endif
/* FEE_PRV_ORDER_H */
#endif
