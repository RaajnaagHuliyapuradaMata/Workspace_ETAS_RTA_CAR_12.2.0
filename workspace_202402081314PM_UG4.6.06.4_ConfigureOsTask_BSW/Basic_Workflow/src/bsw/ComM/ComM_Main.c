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
/* Inlcude section                                                      */
/* ---------------------------------------------------------------------*/
#include "ComM_Priv.h"

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

/*
 *  Name : ComM_MainFunction_Can_Network_Channel_Can_Network
 *  Description : Main function for Bus Type COMM_BUS_TYPE_CAN  channel Can_Network_Channel_Can_Network
 *
 */
void ComM_MainFunction_Can_Network_Channel_Can_Network(void)
{
    ComM_Prv_ChannelMainFunction(0) ;
}


#if (COMM_PNC_GW_ENABLED == STD_ON)

#endif /*  #if (COMM_PNC_GW_ENABLED == STD_ON)  */

#if (COMM_PNC_ENABLED == STD_ON)
/* EIRA call backs are generated for all possible bus types, depending on the bus types configred in
   ComMChannel container. This is to ensure that call backs are always available in Post-build configuration.
   Precompile : same approach is followed to keep the code same. */

/*
 *  Name : ComM_EIRACallBack_COMM_BUS_TYPE_CAN
 *  Description : EIRA callback for bus type COMM_BUS_TYPE_CAN
 *                  This function will be called whenever EIRA signal for this bus type changes
 */

void ComM_EIRACallBack_COMM_BUS_TYPE_CAN(void)
{
    ComM_Prv_EIRA_CallBack(0);
}


#endif /* #if (COMM_PNC_ENABLED == STD_ON)  */

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

/*----------------------------------------------------------------------*/

/************************************************************************/
