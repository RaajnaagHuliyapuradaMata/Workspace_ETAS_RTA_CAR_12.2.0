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


/***********************************************************************************/
/*                                        INCLUDES                                 */
/***********************************************************************************/
#include "ComStack_Types.h"
#include "ComM_Cfg.h"

#if ( COMM_ECUC_RB_RTE_IN_USE != STD_OFF )
/*  If RTE is present in the project then, some API prototypes gets generated from RTE also. This will lead to
 *  double declaration warning since these declarations will be provided from ComM as well.
 *  To avoid such warnings in projects below switch is defined */
#define COMM_RTE_GENERATED_APIS
#include "Rte_ComM.h"
#endif

#include "ComM_Priv.h"
#include "Std_Types.h"

#if(COMM_CAN == STD_ON)
#include "CanSM_ComM.h"
#endif
#if(COMM_LIN == STD_ON)
#include "LinSM.h"
#endif
#if(COMM_FLXRY == STD_ON)
#include "FrSM.h"
#endif
#if(COMM_ETH == STD_ON)
#include "EthSM.h"
#endif
#if(COMM_CDD == STD_ON)
#endif

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

#if(COMM_RTE_SUPPORT != STD_OFF)

/*****************************************Start of RTE API's*******************************************/
void ComM_Prv_Rte_Switch_UM_currentMode(ComM_UserHandleType UserId_u8, uint8 lowestMode_u8)
{
    Std_ReturnType retVal = E_OK;
    (void) lowestMode_u8;

    switch (UserId_u8)
    {
        /*Mode-Switch interface to the user will be  not be generated if comMUserDisableRtePort configured true.*/
        /* MR12 RULE 12.3 VIOLATION: "Rte_Switch_UM" generated contents are based on "RTE-VFB trace". This has no
           functional impact, hence the warning is suppressed. */
        case (ComMConf_ComMUser_ComMUser_Can_Network_Channel_Can_Network):
        {
            retVal = (Std_ReturnType)(Rte_Switch_UM_ComMUser_Can_Network_Channel_Can_Network_currentMode(lowestMode_u8));
        }
        break;
        default:
        {
            retVal = E_OK;
        }
        break;
    }
    (void)retVal;
}

#if ((COMM_FULLCOMREQ_NOTIF != STD_OFF))

void ComM_CurrentChannelRequest_Rte_Write(uint8 ChannelId, const ComM_UserHandleArrayType * data)
{
    Std_ReturnType retVal;
    switch (ChannelId)
    {
        default:
        {
            retVal = E_OK;
        }
        break;
    }
    (void)retVal;
}

#endif  /* #if ((COMM_FULLCOMREQ_NOTIF != STD_OFF) */

#endif  /* #if(COMM_RTE_SUPPORT != STD_OFF) */

/*****************************************End of RTE API's*******************************************/


/**********************************Declaration and definition of dummy api's****************************/

static Std_ReturnType ComM_Dummy_RequestComMode(NetworkHandleType NetworkHandle, ComM_ModeType ComM_Mode);

static Std_ReturnType ComM_Dummy_GetCurrentComMode(NetworkHandleType NetworkHandle, ComM_ModeType * ComM_ModePtr);


static Std_ReturnType ComM_Dummy_RequestComMode(NetworkHandleType NetworkHandle, ComM_ModeType ComM_Mode)
{
    (void) NetworkHandle;
    (void) ComM_Mode;
    return E_OK;
}

static Std_ReturnType ComM_Dummy_GetCurrentComMode(NetworkHandleType NetworkHandle, ComM_ModeType * ComM_ModePtr)
{
    *ComM_ModePtr = ComM_ChannelStruct[NetworkHandle].ChannelMode_u8;
    return E_OK;
}


#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#define COMM_START_SEC_CONST_UNSPECIFIED
#include "ComM_MemMap.h"
const ComM_BusSmApiType_tst ComM_BusSmApi_acst[] =
{
    /* MR12 RULE 8.3 VIOLATION:Different <Bus>SM's use different parameters. Hence it is not possible to
    solve this warning */
    {
        /* COMM_BUS_TYPE_CAN */
        &CanSM_RequestComMode,
        &CanSM_GetCurrentComMode
    },
    {
        /* COMM_BUS_TYPE_ETH */
        &ComM_Dummy_RequestComMode,
        &ComM_Dummy_GetCurrentComMode
    },
    {
        /* COMM_BUS_TYPE_FR */
        &ComM_Dummy_RequestComMode,
        &ComM_Dummy_GetCurrentComMode
    },
    {
        /* COMM_BUS_TYPE_INTERNAL */
        &ComM_Dummy_RequestComMode,
        &ComM_Dummy_GetCurrentComMode
    },
    {
        /* COMM_BUS_TYPE_LIN */
        &ComM_Dummy_RequestComMode,
        &ComM_Dummy_GetCurrentComMode
    },
    {
        /* COMM_BUS_TYPE_CDD */
        &ComM_Dummy_RequestComMode,
        &ComM_Dummy_GetCurrentComMode
    }
};
#define COMM_STOP_SEC_CONST_UNSPECIFIED
#include "ComM_MemMap.h"

#if ((COMM_LIMIT_TO_NOCOM_ENABLED != STD_OFF) || (COMM_PREVENT_WAKEUP_ENABLED != STD_OFF))
#define COMM_START_SEC_CONST_8
#include "ComM_MemMap.h"
const ComM_InhibitionStatusType ComM_EcuGroupClassification_Init = 0x00;

#define COMM_STOP_SEC_CONST_8
#include "ComM_MemMap.h"
#endif /* #if (COMM_MODE_LIMITATION != STD_OFF) || (COMM_WAKEUP_INHIBITION != STD_OFF)  */
