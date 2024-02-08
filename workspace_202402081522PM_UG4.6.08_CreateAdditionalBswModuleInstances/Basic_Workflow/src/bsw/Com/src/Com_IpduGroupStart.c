/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
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
#include "Com_Prv_Inl.h"

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

LOCAL_INLINE void Com_Prv_TxIpduGroupStart(Com_IpduGroupIdType idIpduGrp_u16, boolean initialize_b);
LOCAL_INLINE void Com_Prv_RxIpduGroupStart(Com_IpduGroupIdType idIpduGrp_u16, boolean initialize_b);

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
 Function name    : Com_IpduGroupStart
 Description      : Service for starting the Ipdu's which comes under the Ipdu Group
 Parameter        : idIpduGrp_u16 - I-PDU group id.
                    initialize_b  - flag to request initialization of the I-PDUs which are newly started
 Return value     : None
 **********************************************************************************************************************
*/
void Com_IpduGroupStart(Com_IpduGroupIdType idIpduGrp_u16, boolean initialize_b)
{

#if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_IpduGroupStart, COM_E_UNINIT);
    }
    else if (!Com_Prv_IsValidIpduGroupId(idIpduGrp_u16))
    {
        COM_DET_REPORT_ERROR(COMServiceId_IpduGroupStart, COM_E_PARAM);
    }
    else
#endif /* end of COM_PRV_ERROR_HANDLING */
    {
        if (Com_Prv_IsValidTxIpduGroupId(idIpduGrp_u16))
        {
            /* Function has to be called only when IpduGroup is reffered by atleast one Ipdu  */
            Com_Prv_TxIpduGroupStart(idIpduGrp_u16, initialize_b);
        }
        else if (Com_Prv_IsValidRxIpduGroupId(idIpduGrp_u16))
        {
            /* Function has to be called only when IpduGroup is reffered by atleast one Ipdu  */
            Com_Prv_RxIpduGroupStart(idIpduGrp_u16, initialize_b);
        }
        else
        {
            /* Do nothing : IPduGroups does not contain IPdus */
        }
    }
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_TxIpduGroupStart
 Description      : Service for starting the Ipdu's which comes under the Ipdu Group
 Parameter        : idIpduGrp_u16 - I-PDU group id.
                    initialize_b  - flag to request initialization of the I-PDUs which are newly started
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_TxIpduGroupStart(Com_IpduGroupIdType idIpduGrp_u16, boolean initialize_b)
{

    /* Local pointer which holds the address of the array which stores the ipdu id */
    const Com_IpduId_tuo *      ipduRefPtr_pcuo;
    /* Local pointer to hold the address of Ipdu group structure */
    Com_IpduGrpCfg_tpcst        ipduGrpConstPtr_pcst;
    Com_IpduGrpRam_tpst         ipduGrpRam_pst;
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
    uint16_least                numOfPdus_qu16;

    ipduGrpConstPtr_pcst = COM_GET_IPDUGRP_CONSTDATA(idIpduGrp_u16);
    ipduGrpRam_pst = ipduGrpConstPtr_pcst->ipduGroupRam_pst;

    if(ipduGrpRam_pst->ipduGroupStatus_b != COM_STOP)
    {
        /* Do nothing: As IPduGroup is already started */
    }
    else
    {
        ipduGrpRam_pst->ipduGroupStatus_b = COM_START;

        ipduRefPtr_pcuo = COM_GET_IPDUGRP_IPDUREF_CONSTDATA(ipduGrpConstPtr_pcst->idxToFirstIpdu_u16);

        numOfPdus_qu16 = ipduGrpConstPtr_pcst->numOfIpdus_u16;

        while (numOfPdus_qu16 > COM_ZERO)
        {
            txIpduRamPtr_pst = COM_GET_TX_IPDU_CONSTDATA(*ipduRefPtr_pcuo)->ipduRam_pst;

            /* increment counter */
            ++txIpduRamPtr_pst->cntrStartedIpduGrp_u8;

            /* If the PDU state is changed from STOP to START */
            if (Com_GetRamValue(TXIPDU,_PDUSTATUS,txIpduRamPtr_pst->txFlags_u16) == COM_STOP)
            {
                /* NOTE: This below order is to be maintained, to avoid any interrupt related race conditions.
                * REASON: If the below function call is interrupted, by any other API,
                * as the _PDUSTATUS is set after the function returns,
                * the interrupting API returns without any effect */
                Com_Prv_TxIPduStart((*ipduRefPtr_pcuo), initialize_b);

                Com_SetRamValue(TXIPDU,_PDUSTATUS,txIpduRamPtr_pst->txFlags_u16,COM_START);
            }
            ipduRefPtr_pcuo++;
            numOfPdus_qu16--;
        }/* while (numOfPdus_qu16 > 0 ) */
    }
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxIpduGroupStart
 Description      : Service for starting the Ipdu's which comes under the Ipdu Group
 Parameter        : idIpduGrp_u16 - I-PDU group id.
                    initialize_b  - flag to request initialization of the I-PDUs which are newly started
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_RxIpduGroupStart(Com_IpduGroupIdType idIpduGrp_u16, boolean initialize_b)
{

    /* Local pointer which holds the address of the array which stores the ipdu id */
    const Com_IpduId_tuo *      ipduRefPtr_pcuo;
    /* Local pointer to hold the address of Ipdu group structure */
    Com_IpduGrpCfg_tpcst        ipduGrpConstPtr_pcst;
    Com_IpduGrpRam_tpst         ipduGrpRam_pst;
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;
    uint16_least                numOfPdus_qu16;

    ipduGrpConstPtr_pcst = COM_GET_IPDUGRP_CONSTDATA(idIpduGrp_u16);
    ipduGrpRam_pst = ipduGrpConstPtr_pcst->ipduGroupRam_pst;

    /* Check if IPduGroup is already set */
    if(ipduGrpRam_pst->ipduGroupStatus_b != COM_STOP)
    {
        /* Do nothing: As IPduGroup is already started */
    }
    else
    {
        /* Set the IPduGroup bit */
        ipduGrpRam_pst->ipduGroupStatus_b = COM_START;

# if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout)
        /* Invoked enabling reception deadline monitoring */
        Com_Prv_EnableReceptionDM(idIpduGrp_u16);
# endif

        ipduRefPtr_pcuo = COM_GET_IPDUGRP_IPDUREF_CONSTDATA(ipduGrpConstPtr_pcst->idxToFirstIpdu_u16);

        numOfPdus_qu16 = ipduGrpConstPtr_pcst->numOfIpdus_u16;

        while (numOfPdus_qu16 > COM_ZERO)
        {
            rxIpduRamPtr_pst = COM_GET_RX_IPDU_CONSTDATA(*ipduRefPtr_pcuo)->ipduRam_pst;

            /* Below counter shall increment if latest state is started */
            ++rxIpduRamPtr_pst->cntrStartedIpduGrp_u8;

            /* If the PDU state is changed from STOP to START */
            if (Com_GetRamValue(RXIPDU,_PDUSTATUS,rxIpduRamPtr_pst->rxFlags_u16) == COM_STOP)
            {
                Com_Prv_RxIPduStart((*ipduRefPtr_pcuo),initialize_b);

                Com_SetRamValue(RXIPDU,_PDUSTATUS,rxIpduRamPtr_pst->rxFlags_u16,COM_START);
            }

            ipduRefPtr_pcuo++;
            numOfPdus_qu16--;
        }/* while (numOfPdus_qu16 > 0 ) */
    }
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

