/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef  CANSM_BSWM_H
#define  CANSM_BSWM_H



/***************************************************************************************************
* Includes
***************************************************************************************************
*/
#include "CanSM_Cfg.h"
/*
 ********************************************************************************
**                     Enumeration
*******************************************************************************
*/

/*CAN specific communication modes / states notified to the BswM module*/
typedef enum
{
CANSM_BSWM_NO_COMMUNICATION = 0,
CANSM_BSWM_SILENT_COMMUNICATION,
CANSM_BSWM_FULL_COMMUNICATION,
CANSM_BSWM_BUS_OFF,
CANSM_BSWM_CHANGE_BAUDRATE

}CanSM_BswMCurrentStateType;



#endif /*CANSM_BSWM_H*/






