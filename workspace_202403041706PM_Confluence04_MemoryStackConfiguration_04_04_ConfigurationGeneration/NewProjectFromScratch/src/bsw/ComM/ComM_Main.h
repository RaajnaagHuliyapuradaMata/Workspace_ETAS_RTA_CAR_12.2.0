/*
********************************************************************************************************************** 
*
* COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
* The reproduction, distribution and utilization of this document as well as the communication of its contents to
* others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
********************************************************************************************************************** 
*/

/*<VersionHead>
****************************************************************************************************
* This Configuration File is generated using versions (automatically filled in) as listed below.
*
* Generator__: ComM / AR45.5.0.0         Module Package Version
* Editor_____: AEEE-Pro_12.2.0            Tool Version
****************************************************************************************************
</VersionHead>*/


/* ---------------------------------------------------------------------*/
/* Include protection                                                   */
/* ---------------------------------------------------------------------*/
#ifndef  COMM_MAIN_H
#define  COMM_MAIN_H

#include "ComM_Cfg.h"

/* ---------------------------------------------------------------------*/
/* External declarations                                                */
/* ---------------------------------------------------------------------*/

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

/* ---------------------------------------------------------------------*/
/*  Name : ComM_MainFunction_Can_Network_Channel_Can_Network                                          */
/*  Description : Main function for Bus Type COMM_BUS_TYPE_CAN  channel Can_Network_Channel_Can_Network           */
/* ---------------------------------------------------------------------*/

#if ( COMM_ECUC_RB_RTE_IN_USE != STD_ON )
extern void ComM_MainFunction_Can_Network_Channel_Can_Network(void);
#endif


#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif
