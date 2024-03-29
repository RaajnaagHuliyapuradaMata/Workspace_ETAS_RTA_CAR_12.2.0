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
/*- Include files                                                                       -*/
/*---------------------------------------------------------------------------------------*/

#include "BswM.h"
#include "BswM_Prv.h"

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/*****************************************************************************************
* Function name  : BswM_Prv_GetCanSMMRPIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  CanSM Indication MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_CANSM_INDICATION) && (BSWM_NO_OF_MRP_CANSM_INDICATION > 0))
boolean BswM_Prv_GetCanSMMRPIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_CANSM_INDICATION; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataCanSM_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_CANSM_INDICATION > 0 */


/*****************************************************************************************
* Function name  : BswM_Prv_GetComMMRPIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  ComM Indication MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_COMM_INDICATION) && (BSWM_NO_OF_MRP_COMM_INDICATION > 0))
boolean BswM_Prv_GetComMMRPIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_COMM_INDICATION; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComM_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_COMM_INDICATION > 0 */


/*****************************************************************************************
* Function name  : BswM_Prv_GetCurrentPncMRPIndex_b (PNCHandleType PNC_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Pnc is valid, and returns the index of the corresponding
*                  Pnc's MRP structure.
* Parameters     : PNCHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_COMMPNCS_REQUEST) && (BSWM_NO_OF_MRP_COMMPNCS_REQUEST > 0))
boolean BswM_Prv_GetCurrentPncMRPIndex_b
(
    PNCHandleType PNC_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_COMMPNCS_REQUEST; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMPnc_ast[cntr_u16].idPnc_u8 == PNC_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_COMMPNCS_REQUEST > 0 */
/*****************************************************************************************
* Function name  : BswM_Prv_GetEcuMRUNReqIndicationMRPIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed State is valid, and returns the index of the corresponding
*                  EcuMRunReqIndication MRP structure.
* Parameters     : EcuM_StateType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_ECUMRUNREQ_STATES) && (BSWM_NO_OF_ECUMRUNREQ_STATES > 0))
boolean BswM_Prv_GetEcuMRUNReqIndicationMRPIndex_b
(
    EcuM_StateType State,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_ECUMRUNREQ_STATES; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMRunReq_ast[cntr_u16].dataState_en == State)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_ECUMRUNREQ_STATES > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetEcuMWkpSrcMRPIndex_b (uint32 idEcuMWkpSrc_u32, uint16 * idx_pu16 )
* Description    : API checks whether the passed Wakeup Source Id is valid, and returns the index of the corresponding
*                  EcuM Wakeup Source MRP structure.
* Parameters(in) : idEcuMWkpSrc_u32 - The Wakeup Source Id used to identify the mode requester.
* Parameters(out): idx_pu16 - Index of the requester in the array of EcuM Wakeup Source ModeRequestPorts.
* Return value   : boolean
*                       -   TRUE : The WakeupSource Id is valid
*                       -   FALSE : The WakeupSource Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_ECUMWKP_SOURCES) && (BSWM_NO_OF_ECUMWKP_SOURCES > 0 ))
boolean BswM_Prv_GetEcuMWkpSrcMRPIndex_b
(
    uint32 idEcuMWkpSrc_u32,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_ECUMWKP_SOURCES; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMWkpSrc_ast[cntr_u16].dataWakeupSource_u32 == idEcuMWkpSrc_u32)
        {
            /* Wakeup source id available in data structure */
            isValid_b = TRUE;
            * idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_ECUMWKP_SOURCES > 0 */

#if (defined(BSWM_NO_OF_NM_CHANNELS) && (BSWM_NO_OF_NM_CHANNELS > 0 ))
/*****************************************************************************************
* Function name  : BswM_Prv_GetNmWkupIndERPIndex_b (uint8 idChannel_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Nm Channel Id is valid, and returns the index of the corresponding
*                  Nm CarWakeupIndication type ERP structure.
* Parameters(in) : idChannel_u8 - The Channel Id used to identify the mode requester.
* Parameters(out): idx_pu16 - Index of the requester in the array of Nm CarWakeupIndication type EventRequestPort.
* Return value   : boolean
*                       -   TRUE : The Channel Id is valid
*                       -   FALSE : The Channel Id is not valid
*****************************************************************************************/
boolean BswM_Prv_GetNmWkupIndERPIndex_b
(
    uint8 idChannel_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_NM_CHANNELS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmCarWakeupInd_ast[cntr_u16].idChannel_u8 == idChannel_u8)
        {
            /* Channel id available in data structure */
            isValid_b = TRUE;
            * idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_NM_CHANNELS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetNvMBlockModeMRPIndex_b (uint16 idBlock_u16, uint16 * idx_pu16 )
* Description    : API checks whether the passed NvM Block Id is valid, and returns the index of the corresponding
*                  NvM Request type MRP structure.
* Parameters(in) : idBlock_u16 - The Block Id used to identify the mode requester.
* Parameters(out): idx_pu16 - Index of the requester in the array of NvM Request type ModeRequestPorts.
* Return value   : boolean
*                       -   TRUE : The Block Id is valid
*                       -   FALSE : The Block Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_NVM_REQUESTS) && (BSWM_NO_OF_NVM_REQUESTS > 0 ))
boolean BswM_Prv_GetNvMBlockModeMRPIndex_b
(
    uint16 idBlock_u16,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_NVM_REQUESTS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMReq_ast[cntr_u16].idNvMBlk_u16 == idBlock_u16)
        {
            /* Block id available in data structure */
            isValid_b = TRUE;
            * idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_NVM_REQUESTS > 0 */


/*****************************************************************************************
* Function name  : BswM_Prv_GetNvMJobModeMRPIndex_b (uint8 idService_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed NvM Service Id is valid, and returns the index of the corresponding
*                  NvM Job Mode type MRP structure.
* Parameters(in) : idService_u8 - The Service Id used to identify the mode requester.
* Parameters(out): idx_pu16 - Index of the requester in the array of NvM Job Mode type ModeRequestPorts.
* Return value   : boolean
*                       -   TRUE : The Service Id is valid
*                       -   FALSE : The Service Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_NVM_SERVICE_IDS) && (BSWM_NO_OF_NVM_SERVICE_IDS > 0 ))
boolean BswM_Prv_GetNvMJobModeMRPIndex_b
(
    uint8 idService_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_NVM_SERVICE_IDS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMJobMode_ast[cntr_u16].idService_u8 == idService_u8)
        {
            /* Service id available in data structure */
            isValid_b = TRUE;
            * idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_NVM_SERVICE_IDS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetDcmComModeRequestMRPIndex_b (NetworkHandleType idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  DcmComModeRequest's MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_DCM_COM_MODE_REQ) && (BSWM_NO_OF_MRP_DCM_COM_MODE_REQ > 0))
boolean BswM_Prv_GetDcmComModeRequestMRPIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_DCM_COM_MODE_REQ; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmCom_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_COMMPNCS_REQUEST > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetEthIfMRPIndex_b (EthIf_SwitchPortGroupIdxType idxPortGroup_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Port Group Index is valid, and returns the index of the corresponding
*                  EthIf PortGroupStateChng MRP structure.
* Parameters     : EthIf_SwitchPortGroupIdxType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_ETHIF_PORTFGROUP) && (BSWM_NO_OF_MRP_ETHIF_PORTFGROUP > 0))
boolean BswM_Prv_GetEthIfMRPIndex_b
(
        EthIf_SwitchPortGroupIdxType idxPortGroup_u8,
        uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_ETHIF_PORTFGROUP; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthIfPortGroup_ast[cntr_u16].idPortGroup_u8 == idxPortGroup_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_ETHIF_PORTFGROUP > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetEthSMMRPIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  EthSM Indication MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_ETHSM_INDICATION) && (BSWM_NO_OF_MRP_ETHSM_INDICATION > 0))
boolean BswM_Prv_GetEthSMMRPIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_ETHSM_INDICATION; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthSM_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_ETHSM_INDICATION > 0 */


/*****************************************************************************************
* Function name  : BswM_Prv_GetFrSMMRPIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  FrSM Indication MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_FRSM_INDICATION) && (BSWM_NO_OF_MRP_FRSM_INDICATION > 0))
boolean BswM_Prv_GetFrSMMRPIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_FRSM_INDICATION; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataFrSM_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_FRSM_INDICATION > 0 */


/*****************************************************************************************
* Function name  : BswM_Prv_GetGenReqMRPIndex_b (uint16 idUser_u16, uint16 * idx_pu16 )
* Description    : API checks whether the passed UserId is valid, and returns the index of the corresponding
*                  GenericRequest MRP structure.
* Parameters(in) : idRequester_u16 - The NetworkId / HandleId / UserId used to identify the mode requester.
* Parameters(out): idx_pu16 - Index of the requester in the array of GenericRequest ModeRequestPorts.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_GENREQ_TOTAL) && (BSWM_NO_OF_GENREQ_TOTAL > 0 ))
boolean BswM_Prv_GetGenReqMRPIndex_b
(
    uint16 idRequester_u16,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_GENREQ_TOTAL; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataGenericReq_ast[cntr_u16].idUser_u16 == idRequester_u16)
        {
            isValid_b = TRUE;
            * idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_GENREQ_TOTAL > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetJ1939DcmIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  J1939DcmBroadCastStatus MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_J1939DCM_CHANNELS) && (BSWM_NO_OF_J1939DCM_CHANNELS > 0))
boolean BswM_Prv_GetJ1939DcmIndex_b
(
    uint16 idNetworkMask_u16,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_J1939DCM_CHANNELS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Dcm_ast[cntr_u16].idNetworkMask_u16 == idNetworkMask_u16)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_J1939DCM_CHANNELS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetJ1939NmNodeChnIndex_b (uint8 idNetwork_u8,uint8 idNode_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network and node is valid, and returns the index of the corresponding
*                  J1939Nm Indication MRP structure.
* Parameters     : NetworkHandleType,uint8, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_J1939NM_CHANNELS) && (BSWM_NO_OF_J1939NM_CHANNELS > 0))
boolean BswM_Prv_GetJ1939NmNodeChnIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint8 idNode_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_J1939NM_CHANNELS; cntr_u16++)
    {
        if ( (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Nm_ast[cntr_u16].idNetwork_u8 == idNetwork_u8) &&
                (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Nm_ast[cntr_u16].dataNode_u8 == idNode_u8))
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_J1939NM_CHANNELS > 0 */


/*****************************************************************************************
* Function name  : BswM_Prv_GetLinSmCurrentScheduleChnIndex_b (uint8 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  LinSmCurrentSchedule MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_LINSMSCH_CHANNELS) && (BSWM_NO_OF_LINSMSCH_CHANNELS > 0))
boolean BswM_Prv_GetLinSmCurrentScheduleChnIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_LINSMSCH_CHANNELS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSchInd_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_LINSMSCH_CHANNELS > 0 */
/*****************************************************************************************
* Function name  : BswM_Prv_GetLinTpModeReqChnIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  LinTpModeReq MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_LINTP_CHANNELS) && (BSWM_NO_OF_LINTP_CHANNELS > 0))
boolean BswM_Prv_GetLinTpModeReqChnIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_LINTP_CHANNELS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinTpReq_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_LINTP_CHANNELS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetLinSmCurrentStateIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network and node is valid, and returns the index of the corresponding
*                  LinTpModeReq MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_LINSM_CHANNELS) && (BSWM_NO_OF_LINSM_CHANNELS > 0))
boolean BswM_Prv_GetLinSmCurrentStateIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_LINSM_CHANNELS; cntr_u16++)
    {
        if(BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSMInd_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_LINSM_CHANNELS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetNmStateChngMRPIndex_b (NetworkHandleType idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  Nm state change MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_NM_STATE_CHG_NTF) && (BSWM_NO_OF_MRP_NM_STATE_CHG_NTF > 0))
boolean BswM_Prv_GetNmStateChngMRPIndex_b
(
    NetworkHandleType idNetwork_u8,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_NM_STATE_CHG_NTF; cntr_u16++)
    {
        if ( BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmStateChngInd_ast[cntr_u16].idNetwork_u8 == idNetwork_u8)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_NM_STATE_CHG_NTF > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetSdclientSrvCSMRPIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  Sd Indication MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_SDCLIENTSRV_CS) && (BSWM_NO_OF_SDCLIENTSRV_CS > 0))
boolean BswM_Prv_GetSdclientSrvCSMRPIndex_b
(
    uint16 idNetwork_u16,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_SDCLIENTSRV_CS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdClientSrv_ast[cntr_u16].idServHandle_u16 == idNetwork_u16)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_SDCLIENTSRV_CS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetSdConsumedEventGroupCurrentStateMRPIndex_b (uint16 idNetwork_u8, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  Sd Indication MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS) && (BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS > 0))
boolean BswM_Prv_GetSdConsEvntGrpCSMRPIndex_b
(
    uint16 idNetwork_u16,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdCnsEvnt_ast[cntr_u16].idEvntGrpHandle_u16 == idNetwork_u16)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetSdEventHandlerCurrentStateMRPIndex_b (uint16 idNetwork_u16, uint16 * idx_pu16 )
* Description    : API checks whether the passed Network is valid, and returns the index of the corresponding
*                  Sd Indication MRP structure.
* Parameters     : NetworkHandleType, uint16 *.
* Return value   : boolean
*                       -   TRUE : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS) && (BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS > 0))
boolean BswM_Prv_GetSdEvntHndrCSMRPIndex_b
(
    uint16 idNetwork_u16,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdEvntHndlr_ast[cntr_u16].idEvntHandle_u16 == idNetwork_u16)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_GetSoAdSoConModeChgMRPIndex_b (SoAd_SoConIdType idConGrp_u16, uint16 * idx_pu16)
* Description    : API checks whether the passed SoConId is valid, and returns the index of the corresponding
*                  SoAd Indication MRP structure.
* Parameters     : SoAd_SoConIdType, uint16 *.
* Return value   : boolean
*                       -   TRUE  : The User Id is valid
*                       -   FALSE : The User Id is not valid
*****************************************************************************************/
#if (defined(BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG) && (BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG > 0))
boolean BswM_Prv_GetSoAdSoConModeChgMRPIndex_b
(
    SoAd_SoConIdType idConGrp_u16,
    uint16 * idx_pu16
)
{
    boolean isValid_b = FALSE;
    uint16 cntr_u16 = 0;

    for (cntr_u16 = 0; cntr_u16 < BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG; cntr_u16++)
    {
        if (BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSoAdSoConModeChg_ast[cntr_u16].idSoAdConGrp_16 == idConGrp_u16)
        {
            isValid_b = TRUE;
            *idx_pu16 = cntr_u16;
            break;
        }
    }
    return isValid_b;

}
#endif /* BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG > 0 */

/*****************************************************************************************
* Function name  : BswM_Prv_ReqProcessOrQueue (void )
* Description    : This function checks if any requests are in progress and queues the current request if necessary.
* Parameters(in) : None
* Parameters(out): None
* Return value   : boolean
*                       -   TRUE : No Request is in progress currently
*                       -   FALSE : Another request is currently in progress
*****************************************************************************************/

boolean BswM_Prv_ReqProcessOrQueue_b
(
    BswM_Cfg_MRPType_ten dataMRPType_en,
    uint16 idChannel_u16,
    uint16 idxMRPChnl_u16,
    uint16 dataReqMode_u16
)
{
    boolean isNorequestInProgress_b = FALSE;
    /*Enter critical section*/
    SchM_Enter_BswM();

    /* Check that no request is in progress */
    isNorequestInProgress_b = BSWM_PRV_ISNOREQSTINPROGRESS;
    if (FALSE != isNorequestInProgress_b)
    {
        BswM_Prv_flgNewReqstProgress_b = TRUE;
    }
    else
    {
        /* Delay the request */
        /* Store the delayed request in the queue */
        BswM_Prv_IntrptQueueIn(dataMRPType_en, idChannel_u16, idxMRPChnl_u16, dataReqMode_u16);
    }

    /* Exit critical section */
    SchM_Exit_BswM();

    return isNorequestInProgress_b;
}

/*****************************************************************************************
* Function name  : BswM_Prv_RuleEval (BswM_ReqProcessing_ten, BswM_Cfg_MRPType_ten, uint16, const uint16 *, uint16)
* Description    : This function checks if the requests is immediate/deferred and accordingly processes it.
* Parameters(in) : BswM_ReqProcessing_ten, BswM_Cfg_MRPType_ten, uint16, const uint16 *, uint16
* Parameters(out): void
* Return value   : boolean
*                       -   TRUE : No Request is in progress currently
*                       -   FALSE : Another request is currently in progress
*****************************************************************************************/


void BswM_Prv_RuleEval
(
        BswM_ReqProcessing_ten dataReqProcessing_en,
        BswM_Cfg_MRPType_ten MRPType,
        uint16 requesting_user,
        const uint16 * adrRulesRef_pu16,
        uint16 nrAssociatedRules_u16
)
/* Check for deferred to the processing of the main function of BswM */
{
    if (BSWM_IMMEDIATE != dataReqProcessing_en)
    {
        /* Update deferred request buffer */
        BswM_Prv_StoreDeferredRequest(MRPType, requesting_user, adrRulesRef_pu16, nrAssociatedRules_u16);
    }
    else
    {
            /* Process Immediate request */
            BswM_Prv_Arbitrate_Rule(nrAssociatedRules_u16, adrRulesRef_pu16);
    }

    if (FALSE != BswM_Prv_isReqstDelayed_b)
    {
        BswM_Prv_ProcessDelayedReqst();
    }
}



#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

/*******************************************************************************************************
 *
 ******************************************************************************************************/

