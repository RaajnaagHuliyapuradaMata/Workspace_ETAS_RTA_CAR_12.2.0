/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2024, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Com_Prv.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_UserCbk_tst Com_Prv_UserCbk_acst[] =
{
    {NULL_PTR}
};
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/
#define COM_START_SEC_CODE
#include "Com_MemMap.h"

#ifdef COM_TxIPduNotification
#endif /* end of COM_TxIPduNotification */

#ifdef COM_ERRORNOTIFICATION
#endif /* end of COM_ERRORNOTIFICATION */

#ifdef COM_TxIPduTimeOutNotify
#endif /* end of COM_TxIPduTimeOutNotify */

#ifdef COM_RxIPduTimeoutNotify
#endif /* end of COM_RxIPduTimeoutNotify */


#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"


