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
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "ComM_Priv.h"


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


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"


/***************************************************************************************************
 Function name    : ComM_Nm_ForwardSynchronizedPncShutdown
 Syntax           : void ComM_Nm_ForwardSynchronizedPncShutdown(NetworkHandleType Channel_u8)
 Description      : Notification that the Synchronization Shutdown can be started.
 Parameter        : Channel_u8 -> Channel Id
 Return value     : void
 ***************************************************************************************************/
void ComM_Nm_ForwardSynchronizedPncShutdown(NetworkHandleType Channel_u8)
{

#if (COMM_SYNCHRONIZED_PNC_SHUTDOWN != STD_OFF)
    /* Local Variables Declaration */


    const ComM_ChannelType_tst *    channelConfigPtr_pcst;

    uint8 numPnc_u8;
    const uint8 * pncs_pcu8;
    uint8 pncId_u8;
    ComM_PncRamStructType_tst *         pncRamPtr_pst;

    /* Local Variables Declaration */
    /*****************************************DET START************************************************/
    /* Check if ComM is initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        /* Report DET with ComM not initialized*/
        COMM_DET_REPORT_ERROR(COMM_PNC_SYN_SHUTDOWN_APIID,COMM_E_NOT_INITED);
        return;
    }
    /* Check if the channel is present in the configured channels */
    if (Channel_u8 >= COMM_NO_OF_CHANNELS)
    {
        /* Report DET with Invalid channel parameter*/
        COMM_DET_REPORT_ERROR(COMM_PNC_SYN_SHUTDOWN_APIID,COMM_E_WRONG_PARAMETERS);
        return;
    }
    /*********************************************DET END************************************************/

    channelConfigPtr_pcst = &(COMM_GET_CHANNEL_LIST[Channel_u8]);
    pncs_pcu8 = channelConfigPtr_pcst->Pncs_pcu8;

    /* If Synchronize Shutdown Request Received from TOP-LEVEL COORDINATOR
    * then Store the Request to process in Pnc Mainfunction */
    if (channelConfigPtr_pcst->PncGwType_en == COMM_GATEWAY_TYPE_PASSIVE)
    {

        for(numPnc_u8 = C_ZERO; numPnc_u8 < channelConfigPtr_pcst->numPncs_u8; ++numPnc_u8)
        {
            pncId_u8 = pncs_pcu8[numPnc_u8];
            pncRamPtr_pst = &ComM_PncRamStruct[pncId_u8];

            pncRamPtr_pst->PncSynchronizeShutdownRequest_b = TRUE;

        }

    }
#else

    (void)Channel_u8;

#endif /* (COMM_SYNCHRONIZED_PNC_SHUTDOWN != STD_OFF) */
}
#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"


