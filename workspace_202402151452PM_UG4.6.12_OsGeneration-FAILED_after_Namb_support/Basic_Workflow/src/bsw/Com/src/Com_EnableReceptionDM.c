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

# if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout)

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
 Function name    : Com_EnableReceptionDM
 Description      : Service enables the reception deadline monitoring for the I-PDUs within the given I-PDU group.
 Parameter        : idIpduGrp_u16 - I-PDU group id.
 Return value     : None
 **********************************************************************************************************************
*/
void Com_EnableReceptionDM(Com_IpduGroupIdType idIpduGrp_u16)
{
#  if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_EnableReceptionDM, COM_E_UNINIT);
    }
    else if (!Com_Prv_IsValidIpduGroupId(idIpduGrp_u16))
    {
        COM_DET_REPORT_ERROR(COMServiceId_EnableReceptionDM, COM_E_PARAM);
    }
    else
#  endif /* end of COM_PRV_ERROR_HANDLING */
    {
        if (Com_Prv_IsValidRxIpduGroupId(idIpduGrp_u16))
        {
            /* Function has to be called only when IpduGroup is reffered by atleast one Ipdu  */
            Com_Prv_EnableReceptionDM(idIpduGrp_u16);
        }
    }
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_EnableReceptionDM
 Description      : Service enables the reception deadline monitoring for the I-PDUs within the given I-PDU group.
 Parameter        : idIpduGrp_u16 - I-PDU group id.
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_EnableReceptionDM(Com_IpduGroupIdType idIpduGrp_u16)
{

    /* Local pointer which holds the address of the array which stores the ipdu id */
    const Com_IpduId_tuo *      ipduRefPtr_pcuo;
    /* Local pointer to hold the address of Ipdu group structure */
    Com_IpduGrpCfg_tpcst        ipduGrpConstPtr_pcst;
    Com_IpduGrpRam_tpst         ipduGrpRam_pst;
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;
    uint16_least                numOfIpdus_qu16;

    ipduGrpConstPtr_pcst = COM_GET_IPDUGRP_CONSTDATA(idIpduGrp_u16);
    ipduGrpRam_pst = ipduGrpConstPtr_pcst->ipduGroupRam_pst;

    if(ipduGrpRam_pst->ipduGrpDMStatus_b != COM_STOP)
    {
        /* Do nothing: As deadline monitoring of IPduGroup is already started */
    }
    else
    {
        /* Set the IPduGroup bit */
        ipduGrpRam_pst->ipduGrpDMStatus_b = COM_START;

        ipduRefPtr_pcuo = COM_GET_IPDUGRP_IPDUREF_CONSTDATA(ipduGrpConstPtr_pcst->idxToFirstIpdu_u16);

        numOfIpdus_qu16 = ipduGrpConstPtr_pcst->numOfIpdus_u16;

        while (numOfIpdus_qu16 > COM_ZERO)
        {
            rxIpduRamPtr_pst = COM_GET_RX_IPDU_CONSTDATA(*ipduRefPtr_pcuo)->ipduRam_pst;

            /* Below DM counter shall increment if latest state is started */
            ++rxIpduRamPtr_pst->cntrStartedIpduGrpDM_u8;

            /*If the state is changed from RESET to SET*/
            /*As the counters are already updated, no necessary actions are required the other way around */
            if (Com_GetRamValue(RXIPDU,_DMSTATUS,rxIpduRamPtr_pst->rxFlags_u16) == COM_STOP)
            {
                if (Com_Prv_EnableRxDeadlineMonitoring(*ipduRefPtr_pcuo))
                {
                    Com_SetRamValue(RXIPDU,_DMSTATUS,rxIpduRamPtr_pst->rxFlags_u16, COM_START);
                }
            }
            ipduRefPtr_pcuo++;
            numOfIpdus_qu16--;

        }/* while (numOfIpdus_qu16 > 0 ) */
    }
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

# endif /* # if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout) */

