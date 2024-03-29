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


/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                      -*/
/*---------------------------------------------------------------------------------------*/

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

#if (defined(BSWM_NO_OF_MRP_CANSM_INDICATION) && (BSWM_NO_OF_MRP_CANSM_INDICATION > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetCanSMIndicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               CanSM Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetCanSMIndicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;
    const CanSM_BswMCurrentStateType CanSM_BswMCurrentStateType_caen[] =
    {
        CANSM_BSWM_NO_COMMUNICATION,
        CANSM_BSWM_SILENT_COMMUNICATION,
        CANSM_BSWM_FULL_COMMUNICATION,
        CANSM_BSWM_BUS_OFF,
        CANSM_BSWM_CHANGE_BAUDRATE
    };

    /* MRP RAM Buffer initialized at required index */

    BswM_Cfg_CanSMIndicationModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_CanSMIndicationModeInfo_ast[idxMRPChnl_u16].dataMode_en = CanSM_BswMCurrentStateType_caen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataCanSM_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataCanSM_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataCanSM_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}


#endif /* BSWM_NO_OF_MRP_CANSM_INDICATION > 0 */


#if (defined(BSWM_NO_OF_MRP_COMM_INDICATION) && (BSWM_NO_OF_MRP_COMM_INDICATION > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetComMIndicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               ComM Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetComMIndicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_ComMIndicationModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_ComMIndicationModeInfo_ast[idxMRPChnl_u16].dataMode_u8 = (ComM_ModeType)dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComM_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComM_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComM_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}

#endif /* BSWM_NO_OF_MRP_COMM_INDICATION > 0 */


#if (defined(BSWM_NO_OF_ERP_COMM_INITIATE_RESET) && (BSWM_NO_OF_ERP_COMM_INITIATE_RESET > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetComMInitiateResetInfo_en (uint16 idxERPChnl_u16, uint16 dataEventMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and ERP Ram Buffer of the
*               ComM Initiate Reset type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required ERP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxERPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataEventMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular ERP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular ERP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the ERP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the ERP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetComMInitiateResetInfo_en
(
 uint16 idxERPChnl_u16,
 uint16 dataEventMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_ComMInitiateResetModeInfo_ast[idxERPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_ComMInitiateResetModeInfo_ast[idxERPChnl_u16].dataMode_en = BSWM_EVENT_IS_SET;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMReset_st[idxERPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMReset_st[idxERPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMReset_st[idxERPChnl_u16].dataReqProcessing_en;

    (void)dataEventMode_u16;  /* To avoid MISRA warning of unused variable */
    return retVal;
}

#endif /* BSWM_NO_OF_MRP_COMM_INITIATE_RESET > 0 */


#if (defined(BSWM_NO_OF_MRP_COMMPNCS_REQUEST) && (BSWM_NO_OF_MRP_COMMPNCS_REQUEST > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetComMPncReqstInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               ComM Pnc Request type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetComMPncReqstInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;
    const ComM_PncModeType ComM_PncModeType_caen[] =
    {
        COMM_PNC_REQUESTED,
        COMM_PNC_READY_SLEEP,
        COMM_PNC_PREPARE_SLEEP,
        COMM_PNC_NO_COMMUNICATION
    };

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_ComMPncRequestModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_ComMPncRequestModeInfo_ast[idxMRPChnl_u16].dataMode_en = ComM_PncModeType_caen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMPnc_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMPnc_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMPnc_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}

#endif /* BSWM_NO_OF_MRP_COMMPNCS_REQUEST > 0 */


#if (defined(BSWM_NO_OF_ERP_DCM_APP_UPDATE_INDICATION) && (BSWM_NO_OF_ERP_DCM_APP_UPDATE_INDICATION > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetDcmAppUpdateIndicationInfo_en (uint16 idxERPChnl_u16, uint16 dataEventMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and ERP Ram Buffer of the
*               Dcm Application Updated Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required ERP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxERPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataEventMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular ERP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular ERP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the ERP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the ERP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetDcmAppUpdateIndicationInfo_en
(
 uint16 idxERPChnl_u16,
 uint16 dataEventMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* ERP RAM Buffer initialized at required index */
    BswM_Cfg_DcmApplicationUpdatedindicationModeInfo_ast[idxERPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_DcmApplicationUpdatedindicationModeInfo_ast[idxERPChnl_u16].dataMode_en = BSWM_EVENT_IS_SET;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmAppUpdateInd_st[idxERPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmAppUpdateInd_st[idxERPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmAppUpdateInd_st[idxERPChnl_u16].dataReqProcessing_en;

    (void)dataEventMode_u16;  /* To avoid MISRA warning of unused variable */
    return retVal;
}

#endif /* BSWM_NO_OF_MRP_DCM_APP_UPDATE_INDICATION > 0 */


#if (defined(BSWM_NO_OF_MRP_DCM_COM_MODE_REQ) && (BSWM_NO_OF_MRP_DCM_COM_MODE_REQ > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetDcmComModeReqstInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               Get Dcm Com Mode Request type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetDcmComModeReqstInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_DcmComModeRequestModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_DcmComModeRequestModeInfo_ast[idxMRPChnl_u16].dataMode_u8 = (Dcm_CommunicationModeType)dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmCom_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmCom_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmCom_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}

#endif /* BSWM_NO_OF_MRP_DCM_COM_MODE_REQ > 0 */


#if (defined(BSWM_NO_OF_MRP_ETHIF_PORTFGROUP) && (BSWM_NO_OF_MRP_ETHIF_PORTFGROUP > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetEthIfPortGroupStateInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  :
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetEthIfPortGroupStateInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;
    const EthTrcv_LinkStateType linkStateType_caen[] =
    {
        ETHTRCV_LINK_STATE_DOWN,
        ETHTRCV_LINK_STATE_ACTIVE
    };

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_EthIfPortGroupStateInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_EthIfPortGroupStateInfo_ast[idxMRPChnl_u16].dataMode_en = linkStateType_caen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthIfPortGroup_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthIfPortGroup_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthIfPortGroup_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}

#endif /* BSWM_NO_OF_MRP_ETHIF_PORTFGROUP > 0 */

#if (defined(BSWM_NO_OF_MRP_ETHSM_INDICATION) && (BSWM_NO_OF_MRP_ETHSM_INDICATION > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetEthSMIndicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               EthSM Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetEthSMIndicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;
    const EthSM_NetworkModeStateType EthSM_NetworkModeStateType_caen[] =
    {
        ETHSM_STATE_OFFLINE,
        ETHSM_STATE_WAIT_TRCVLINK,
        ETHSM_STATE_WAIT_ONLINE,
        ETHSM_STATE_ONLINE,
        ETHSM_STATE_ONHOLD,
        ETHSM_STATE_WAIT_OFFLINE
    };

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_EthSMIndicationModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_EthSMIndicationModeInfo_ast[idxMRPChnl_u16].dataMode_en = EthSM_NetworkModeStateType_caen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthSM_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthSM_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthSM_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}

#endif /* BSWM_NO_OF_MRP_ETHSM_INDICATION > 0 */


#if (defined(BSWM_NO_OF_MRP_FRSM_INDICATION) && (BSWM_NO_OF_MRP_FRSM_INDICATION > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetFrSMIndicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               FrSM Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetFrSMIndicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;
    const FrSM_BswM_StateType FrSM_BswM_StateType_caen[] =
    {
            FRSM_BSWM_READY,
            FRSM_BSWM_READY_ECU_PASSIVE,
            FRSM_BSWM_STARTUP,
            FRSM_BSWM_STARTUP_ECU_PASSIVE,
            FRSM_BSWM_WAKEUP,
            FRSM_BSWM_WAKEUP_ECU_PASSIVE,
            FRSM_BSWM_HALT_REQ,
            FRSM_BSWM_HALT_REQ_ECU_PASSIVE,
            FRSM_BSWM_KEYSLOT_ONLY,
            FRSM_BSWM_KEYSLOT_ONLY_ECU_PASSIVE,
            FRSM_BSWM_ONLINE,
            FRSM_BSWM_ONLINE_ECU_PASSIVE,
            FRSM_BSWM_ONLINE_PASSIVE,
            FRSM_BSWM_ONLINE_PASSIVE_ECU_PASSIVE

    };

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_FrSMIndicationModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_FrSMIndicationModeInfo_ast[idxMRPChnl_u16].dataMode_en = FrSM_BswM_StateType_caen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataFrSM_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataFrSM_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataFrSM_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}

#endif /* BSWM_NO_OF_MRP_FRSM_INDICATION > 0 */

#if (defined(BSWM_NO_OF_ECUMWKP_SOURCES) && (BSWM_NO_OF_ECUMWKP_SOURCES > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetEcuMWkpSrcInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               EcuM Wakeup Source type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetEcuMWkpSrcInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* EcuM Wakeup Source type MRP RAM Buffer initialized at required index */
    BswM_Cfg_EcuMWkpSrcInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_EcuMWkpSrcInfo_ast[idxMRPChnl_u16].dataState = (EcuM_WakeupStatusType)dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMWkpSrc_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMWkpSrc_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMWkpSrc_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_ECUMWKP_SOURCES > 0 */

#if (defined(BSWM_NO_OF_GENREQ_TOTAL) && (BSWM_NO_OF_GENREQ_TOTAL > 0 ))

/*******************************************************************************************
*Function name  : BswM_Prv_GetGenReqInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               GenericRequest type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetGenReqInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* Generic Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_GenericReqModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_GenericReqModeInfo_ast[idxMRPChnl_u16].dataMode_u16 = dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataGenericReq_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataGenericReq_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataGenericReq_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}

#endif /* BSWM_NO_OF_GENREQ_TOTAL > 0 */

#if (defined(BSWM_NO_OF_NM_CHANNELS) && (BSWM_NO_OF_NM_CHANNELS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetNmWkupIndInfo_en (uint16 idxERPChnl_u16, uint16 dataEventMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and ERP Ram Buffer of the
*               Nm wakeup indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required ERP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxERPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataEventMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular ERP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular ERP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the ERP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the ERP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetNmWkupIndInfo_en
(
 uint16 idxERPChnl_u16,
 uint16 dataEventMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* Nm wakeup indication type ERP RAM Buffer initialized at required index */
    BswM_Cfg_NmWKupIndInfo_ast[idxERPChnl_u16].isValidModePresent_b = TRUE;

    (void)dataEventMode_u16;/* To avoid compiler warning */

    BswM_Cfg_NmWKupIndInfo_ast[idxERPChnl_u16].dataMode_en = BSWM_EVENT_IS_SET;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmCarWakeupInd_ast[idxERPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmCarWakeupInd_ast[idxERPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmCarWakeupInd_ast[idxERPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_NM_CHANNELS > 0 */


#if (defined(BSWM_NO_OF_MRP_NM_STATE_CHG_NTF) && (BSWM_NO_OF_MRP_NM_STATE_CHG_NTF > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetNmStateChngNtfInfo_en (uint16 idxERPChnl_u16, uint16 dataEventMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               Nm state change notification type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxERPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataEventMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetNmStateChngNtfInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataEventMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    (void)dataEventMode_u16;/* To avoid compiler warning */

    /* Retrieving number of associated rules and reference to ListOfRules Array information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmStateChngInd_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmStateChngInd_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNmStateChngInd_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_MRP_NM_STATE_CHG_NTF > 0 */


#if  (defined(BSWM_NO_OF_ECUMRUNREQ_STATES) && (BSWM_NO_OF_ECUMRUNREQ_STATES > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetEcuMRUNReqIndicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               EcuMRUNReqIndication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/

BswM_ReqProcessing_ten BswM_Prv_GetEcuMRUNReqIndicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* MRP RAM Buffer initialized at required index */
    BswM_Cfg_EcuMRUNReqIndicationModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_EcuMRUNReqIndicationModeInfo_ast[idxMRPChnl_u16].dataMode_u8 = (EcuM_StateType)dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMRunReq_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMRunReq_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMRunReq_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_ECUMRUNREQ_STATES > 0 */

#if (defined(BSWM_NO_OF_ECUM_INDICATION) && (BSWM_NO_OF_ECUM_INDICATION > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetEcuMIndicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               EcuM Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/

BswM_ReqProcessing_ten BswM_Prv_GetEcuMIndicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* MRP RAM Buffer initialized at required index */

    BswM_Cfg_EcuMIndicationModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_EcuMIndicationModeInfo_ast[idxMRPChnl_u16].dataMode_u8 = (EcuM_StateType)dataReqMode_u16;


    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMInd_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMInd_ast[idxMRPChnl_u16].adrRulesRef_pu16;
    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEcuMInd_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_ECUM_INDICATION */

#if (defined(BSWM_NO_OF_NVM_REQUESTS) && (BSWM_NO_OF_NVM_REQUESTS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetNvMBlockModeInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               NvM Block mode type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetNvMBlockModeInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* NvM Block mode type MRP RAM Buffer initialized at required index */
    BswM_Cfg_NvMReqInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_NvMReqInfo_ast[idxMRPChnl_u16].dataMode_en = (NvM_RequestResultType)dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMReq_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMReq_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMReq_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_NVM_REQUESTS > 0 */

#if (defined(BSWM_NO_OF_NVM_SERVICE_IDS) && (BSWM_NO_OF_NVM_SERVICE_IDS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetNvMJobModeInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               NvM Job mode type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetNvMJobModeInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* NvM Job mode type MRP RAM Buffer initialized at required index */
    BswM_Cfg_NvMJobModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    BswM_Cfg_NvMJobModeInfo_ast[idxMRPChnl_u16].dataMode_en = (NvM_RequestResultType)dataReqMode_u16;


    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMJobMode_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMJobMode_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataNvMJobMode_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_NVM_SERVICE_IDS > 0 */

#if (defined(BSWM_NO_OF_J1939DCM_CHANNELS) && (BSWM_NO_OF_J1939DCM_CHANNELS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetJ1939DcmInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               J1939Dcm type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/

BswM_ReqProcessing_ten BswM_Prv_GetJ1939DcmInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* J1939Dcm Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_J1939DcmBroadCastStatusInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;

    if (dataReqMode_u16 == (uint16)BSWM_NO_SOURCE_STATUS_TRUE)
    {
        BswM_Cfg_J1939DcmBroadCastStatusInfo_ast[idxMRPChnl_u16].dataMode_b = TRUE;
    }
    else
    {
        BswM_Cfg_J1939DcmBroadCastStatusInfo_ast[idxMRPChnl_u16].dataMode_b = FALSE;
    }

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Dcm_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Dcm_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Dcm_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_J1939DCM_CHANNELS > 0 */


#if (defined(BSWM_NO_OF_J1939NM_CHANNELS) && (BSWM_NO_OF_J1939NM_CHANNELS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetJ1939NmInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               J1939Nm type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetJ1939NmInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    Nm_StateType dataNmState_aen[] =
    {
            NM_STATE_UNINIT,
            NM_STATE_BUS_SLEEP,
            NM_STATE_PREPARE_BUS_SLEEP,
            NM_STATE_READY_SLEEP,
            NM_STATE_NORMAL_OPERATION,
            NM_STATE_REPEAT_MESSAGE,
            NM_STATE_SYNCHRONIZE,
            NM_STATE_OFFLINE
    };

    /* J1939Nm Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_J1939NmStateChangeNotificationInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_J1939NmStateChangeNotificationInfo_ast[idxMRPChnl_u16].dataMode_en = dataNmState_aen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Nm_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Nm_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataJ1939Nm_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_J1939NM_CHANNELS > 0 */

#if (defined(BSWM_NO_OF_LINTP_CHANNELS) && (BSWM_NO_OF_LINTP_CHANNELS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetLinTpModeReqInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               LinTpModeReq type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetLinTpModeReqInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    LinTp_Mode dataLinTpStateState_aen[] =
    {
            LINTP_APPLICATIVE_SCHEDULE,
            LINTP_DIAG_REQUEST,
            LINTP_DIAG_RESPONSE
    };

    /* J1939Nm Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_LinTpModeReqInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_LinTpModeReqInfo_ast[idxMRPChnl_u16].dataMode_en = dataLinTpStateState_aen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinTpReq_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinTpReq_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinTpReq_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_LINTP_CHANNELS > 0 */

#if (defined(BSWM_NO_OF_LINSM_CHANNELS) && (BSWM_NO_OF_LINSM_CHANNELS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetLinSmIdicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               LinSM Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetLinSmIdicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    LinSM_ModeType dataLinSmIndState_aen[] =
    {
            LINSM_NO_COM,
            LINSM_FULL_COM
    };

    /* LinSM Indication Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_LinSmCurrentStateInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_LinSmCurrentStateInfo_ast[idxMRPChnl_u16].dataMode_en = dataLinSmIndState_aen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSMInd_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSMInd_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSMInd_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_LINSM_CHANNELS > 0 */

#if (defined(BSWM_NO_OF_LINSMSCH_CHANNELS) && (BSWM_NO_OF_LINSMSCH_CHANNELS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetLinSmScheduleIndicationInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               LinSM Schedule Indication type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetLinSmScheduleIndicationInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* LinSM Schedule indication Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_LinSmCurrentSchInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_LinSmCurrentSchInfo_ast[idxMRPChnl_u16].dataMode_u8 = (LinIf_SchHandleType)dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSchInd_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSchInd_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataLinSchInd_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_LINSMSCH_CHANNELS > 0 */

#if (defined(BSWM_NO_OF_SDCLIENTSRV_CS) && (BSWM_NO_OF_SDCLIENTSRV_CS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetSdClientServiceCurrentStateInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               SdClientServiceCurrentState type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetSdClientServiceCurrentStateInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    Sd_ClientServiceCurrentStateType dataSdClientServiceCurrentState_aen[] =
    {
            SD_CLIENT_SERVICE_DOWN,
            SD_CLIENT_SERVICE_AVAILABLE
    };

    /* Sd_ConsumedEventGroupCurrentState Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_SdClientSrvCSModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_SdClientSrvCSModeInfo_ast[idxMRPChnl_u16].dataMode_en = dataSdClientServiceCurrentState_aen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdClientSrv_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdClientSrv_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdClientSrv_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_SDCLIENTSRV_CS > 0 */

#if (defined(BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS) && (BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetSdConsumedEventGroupCurrentStateInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               SdConsumedEventGroupCurrentState type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetSdConsumedEventGroupCurrentStateInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    Sd_ConsumedEventGroupCurrentStateType dataSdConsumedEventGroupCurrentState_aen[] =
    {
            SD_CONSUMED_EVENTGROUP_DOWN   ,
            SD_CONSUMED_EVENTGROUP_AVAILABLE
    };

    /* Sd_ConsumedEventGroupCurrentState Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_SdCnsEvntGrpCSModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_SdCnsEvntGrpCSModeInfo_ast[idxMRPChnl_u16].dataMode_en = dataSdConsumedEventGroupCurrentState_aen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdCnsEvnt_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdCnsEvnt_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdCnsEvnt_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS > 0 */

#if (defined(BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS) && (BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_GetBSWM_NO_OF_SDEVNTHNDLR_HANDLE_CSInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               SdEventHandlerCurrentState type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_GetSdEventHandlerCurrentStateInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    Sd_EventHandlerCurrentStateType dataSdEventHandlerCurrentState_aen[] =
    {
            SD_EVENT_HANDLER_RELEASED,
            SD_EVENT_HANDLER_REQUESTED
    };

    /* Sd_EventHandlerCurrentState Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_SdEvntHndrCSModeInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_SdEvntHndrCSModeInfo_ast[idxMRPChnl_u16].dataMode_en = dataSdEventHandlerCurrentState_aen[dataReqMode_u16];

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdEvntHndlr_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdEvntHndlr_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSdEvntHndlr_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS > 0 */

#if (defined(BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG) && (BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG > 0 ))
/*******************************************************************************************
*Function name  : BswM_Prv_SoAdSoConModeChgInfo_en (uint16 idxMRPChnl_u16, uint16 dataReqMode_u16,
*                                            uint16* nrRules_pu16,  uint16* adrRules_pu16)
*
*Description  : This function accesses the PostBuild structure and MRP Ram Buffer of the
*               SoAdSoCOnGrpModeChg type at the specified index. The Ram Buffer is updated with the
*               requested mode value and the required MRP information is accessed from the PB structure.
*
*Parameters(in) :
*     idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*     dataReqMode_u16-  The mode currently requested.
*
*Parameters(out):
*   nrRules_pu16- The number of rules associated with the particular MRP.
*   adrRules_pu16- Pointer to the array containing the rule indexes associated with the particular MRP.
*
*Return value   : BswM_ReqProcessing_ten
*               BSWM_DEFERRED: The RequestProcessing configured for the MRP is Deferred.
*               BSWM_IMMEDIATE: The RequestProcessing configured for the MRP is Immediate.
*
******************************************************************************************/
BswM_ReqProcessing_ten BswM_Prv_SoAdSoConModeChgInfo_en
(
 uint16 idxMRPChnl_u16,
 uint16 dataReqMode_u16,
 uint16 * nrRules_pu16,
 const uint16 * *adrRules_pu16
)
{
    BswM_ReqProcessing_ten retVal;

    /* SoAdSoConGrpModeChg Request MRP RAM Buffer initialized at required index */
    BswM_Cfg_SoAdSoConModeChgInfo_ast[idxMRPChnl_u16].isValidModePresent_b = TRUE;
    BswM_Cfg_SoAdSoConModeChgInfo_ast[idxMRPChnl_u16].dataMode_en = (SoAd_SoConModeType)dataReqMode_u16;

    /* Retrieving information from the PostBuild pointer*/
    *nrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSoAdSoConModeChg_ast[idxMRPChnl_u16].nrAssociatedRules_u16;
    *adrRules_pu16 = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSoAdSoConModeChg_ast[idxMRPChnl_u16].adrRulesRef_pu16;

    retVal = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSoAdSoConModeChg_ast[idxMRPChnl_u16].dataReqProcessing_en;

    return retVal;
}
#endif /* BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG > 0 */

/*******************************************************************************************
*Function name  : BswM_Prv_ProcessDelayedReqst ( void )
*
*Description  : This function processes all the requests which have been stored in the interrupt queue.
*               The requests are de-queued and processed individually. Further, any incoming requests
*               which interrupt the processing of delayed requests are also queued.
*
*Parameters(in) :   None
*
*Parameters(out): None
*
*Return value  : None
*
******************************************************************************************/
void BswM_Prv_ProcessDelayedReqst
(
    void
)
{
    /* Variable declaration*/
    uint8 queueSize_u8 = 0;

    BswM_Cfg_MRPType_ten dataMRPType_en = BSWM_MRP_DEFAULT;
    uint16 idChannel_u16 = 0;
    uint16 idxMRPChnl_u16 = 0;
    uint16 dataReqMode_u16 = 0;
    Std_ReturnType queueOutRetVal_u8;

    uint8 mirror_u8 = 0;
    uint16 nrRules_u16 = 0;
    const uint16 * adrRules_pu16 = NULL_PTR;
    BswM_ReqProcessing_ten dataReqProc_u16;


    /* Set the Delayed Request processing flag to TRUE*/
    BswM_Prv_flgDelayedReqstProgress_b = TRUE;

    /*Enter Critical Section*/
    SchM_Enter_BswM();

    /* Store queue size in local variable*/
    queueSize_u8 = BswM_Prv_IntrptQueueInfo_st.dataSize_u8;

    /*Exit Critical Section*/
    SchM_Exit_BswM();

    while(queueSize_u8 != 0)
    {

        /*Enter Critical Section*/
        SchM_Enter_BswM();

        /* De-queue the mode request at the head of the queue*/
        queueOutRetVal_u8 = BswM_Prv_IntrptQueueOut(&dataMRPType_en, &idChannel_u16, &idxMRPChnl_u16, &dataReqMode_u16);

        /*Exit Critical Section*/
        SchM_Exit_BswM();

        if (E_OK == queueOutRetVal_u8)
        {
            /*Is a valid accessor function defined for the current MRP Type?*/
            if(BswM_GetDelayedReqInfo_capfct[dataMRPType_en] != NULL_PTR)
            {
                /* Call the relevant function via the array of function pointers*/
                dataReqProc_u16 = BswM_GetDelayedReqInfo_capfct[dataMRPType_en](idxMRPChnl_u16, dataReqMode_u16, &nrRules_u16, &adrRules_pu16);

                /* Request is configured for Deferred Request Processing?*/
                if (BSWM_DEFERRED == dataReqProc_u16)
                {
                    /* If the request is deferred, store it in the deferred request buffer */
                    BswM_Prv_StoreDeferredRequest(dataMRPType_en, idChannel_u16, adrRules_pu16, nrRules_u16);
                }
                else
                {
                    /* In case of Immediate Requests, call the rule arbitration function*/
                    BswM_Prv_Arbitrate_Rule(nrRules_u16, adrRules_pu16);
                }

                /* Resetting all the values before next iteration*/
                dataMRPType_en = BSWM_MRP_DEFAULT;
                idChannel_u16 = 0;
                idxMRPChnl_u16 = 0;
                dataReqMode_u16 = 0;
                nrRules_u16 = 0;
                adrRules_pu16 = NULL_PTR;
            }
            /* Decrement the queue size counter*/
            queueSize_u8--;


            if(queueSize_u8 == 0)
            {
                /*Enter Critical Section*/
                SchM_Enter_BswM();

                /* Store queue size in local variable*/
                mirror_u8 = BswM_Prv_IntrptQueueInfo_st.dataSize_u8;

                /*Exit Critical Section*/
                SchM_Exit_BswM();
                /*if further requests were delayed then update the local queue size variable to process them as well*/
                if(mirror_u8 != 0)
                {
                    /*Enter Critical Section*/
                    SchM_Enter_BswM();

                    /* Store queue size in local variable*/
                    queueSize_u8 = BswM_Prv_IntrptQueueInfo_st.dataSize_u8;

                    /*Exit Critical Section*/
                    SchM_Exit_BswM();
                }
            }

        }
        else
        {
            break;
        }
    }

    /*indicate that delayed requests were serviced*/
    BswM_Prv_isReqstDelayed_b = FALSE;

    /* Reset the Delayed Request processing flag */
    BswM_Prv_flgDelayedReqstProgress_b = FALSE;

    return;
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"
/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/
