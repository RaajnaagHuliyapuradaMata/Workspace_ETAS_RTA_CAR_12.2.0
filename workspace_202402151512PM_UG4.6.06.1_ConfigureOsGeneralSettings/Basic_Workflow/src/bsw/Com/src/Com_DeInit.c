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


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

/*
 **********************************************************************************************************************
 Function name    : Com_DeInit
 Description      : Service for COM DeInitialization
 Parameter        : void
 Return value     : None
 **********************************************************************************************************************
*/
void Com_DeInit(void)
{
    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;
    Com_IpduGrpCfg_tpcst        ipduGrpCfg_pcst;
    Com_IpduGrpRam_tpst         ipduGrpRam_pst;
    uint16_least                index_qu16;

    /*Interrupt is locked due to the updation of Com_InitStatus_en, Ipdu Group Status & IPdu Group DM Status
    * In most of the API's, if Ipdu group is stopped, no further processing of data is performed.*/

    /* Check if the module is already Initialised */
    if (Com_InitStatus_en == COM_INIT)
    {
        /* Reset the UNINIT flag */
        Com_InitStatus_en = COM_UNINIT;

        /* TRACE[SWS_Com_00129] The AUTOSAR COM module's function Com_DeInit shall stop
        all started I-PDU groups. */
        ipduGrpCfg_pcst = COM_GET_IPDUGRP_CONSTDATA(0);

        for (index_qu16 = COM_ZERO; index_qu16 < COM_GET_NUM_TOTAL_IPDU_GRP; index_qu16++)
        {
            ipduGrpRam_pst = ipduGrpCfg_pcst->ipduGroupRam_pst;

            if (ipduGrpRam_pst != NULL_PTR)
            {
                ipduGrpRam_pst->ipduGroupStatus_b = COM_STOP;

#if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout)
                ipduGrpRam_pst->ipduGrpDMStatus_b = COM_STOP;
#endif
            }
            ipduGrpCfg_pcst++;
        }

        /* Start: Reset all Tx Flags */
        txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(0);

        for (index_qu16 = COM_ZERO; index_qu16 < COM_GET_NUM_TX_IPDU; index_qu16++)
        {
            txIpduRamPtr_pst = txIpduConstPtr_pcst->ipduRam_pst;

            txIpduRamPtr_pst->txFlags_u16           = COM_ZERO;

            txIpduRamPtr_pst->cntrStartedIpduGrp_u8 = COM_ZERO;

#ifdef COM_TxIPduTimeOut
            txIpduRamPtr_pst->cntrTxTimeout_u16     = COM_ZERO;
#endif
            txIpduRamPtr_pst->cntrRepetitions_u8    = COM_ZERO;
            txIpduRamPtr_pst->cntrMinDelayTime_u16  = COM_ZERO;

            txIpduConstPtr_pcst++;
        }
        /* End: Reset all Tx Flags */

        /* Start: Reset all Rx Flags */
        rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(0);

        for (index_qu16 = COM_ZERO; index_qu16 < COM_GET_NUM_RX_IPDU; index_qu16++)
        {
            rxIpduRamPtr_pst = rxIpduConstPtr_pcst->ipduRam_pst;

            rxIpduRamPtr_pst->rxFlags_u16             = COM_ZERO;

            rxIpduRamPtr_pst->cntrStartedIpduGrp_u8   = COM_ZERO;

# if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout)
            rxIpduRamPtr_pst->cntrStartedIpduGrpDM_u8 = COM_ZERO;
# endif

            rxIpduConstPtr_pcst++;
        }
    }
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

