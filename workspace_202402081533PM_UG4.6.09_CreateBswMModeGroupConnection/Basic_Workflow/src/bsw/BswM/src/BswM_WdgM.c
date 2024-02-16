/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*---------------------------------------------------------------------------------------*/
/*- Include files                                                                        -*/
/*---------------------------------------------------------------------------------------*/
#include "BswM.h"
#include "BswM_WdgM.h"
#include "BswM_Prv.h"


/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                        -*/
/*---------------------------------------------------------------------------------------*/

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/*****************************************************************************************
* Function name    :
* Description    :
* Parameter      :
* Return value    :
* Remarks        :
*****************************************************************************************/
void BswM_WdgM_RequestPartitionReset
(
   ApplicationType Application
)
{
    /*To avoid compiler warning of unused parameters*/
    (void)Application;
    return;
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/

