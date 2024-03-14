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

#ifndef FEE_CFG_H
#define FEE_CFG_H

/*
 ***************************************************************************************************
 * Type definition and enums
 ***************************************************************************************************
 */

typedef enum
{
    Fee_Rb_DeviceName = 0,
    /* following enums are not valid device-driver-enums */
    Fee_Rb_Device_Max      /* Number of configured Fee Devices */
} Fee_Rb_DeviceName_ten;

/* ******************************************************************************************************************
   ***************************** External declarations of hook functions provided to Fee ****************************
   ****************************************************************************************************************** */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/* Hook into synchronous Fls_MainFunction loop */

/* End of Fee section */
#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif /* FEE_CFG_H */
