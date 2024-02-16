
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "BswM.h"
#include "BswM_Prv.h"

/**********************************************************************************************************************
 *  Definition of Global Variables                                                                                                    
 **********************************************************************************************************************/

#define BSWM_START_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

/* RAM buffer to hold the mode value of ComM Indication type MRPs */
BswM_Cfg_MRP_PC_ComMIndicationType_tst BswM_Cfg_ComMIndicationModeInfo_ast[BSWM_NO_OF_MRP_COMM_INDICATION] =
		{ { /* generated as default value for BswM_MRP_ComM_Can0 */
		FALSE, COMM_NO_COMMUNICATION } };

/* RAM buffer to hold the mode value of Generic request, BswModeNotification, SwcModeNotification, and SwcModeRequest type MRPs */
BswM_Cfg_MRP_PC_GenReqType_tst BswM_Cfg_GenericReqModeInfo_ast[BSWM_NO_OF_GENREQ_TOTAL] =
		{ { /* generated as default value for BswM_MRP_ApplicationRequestPort */
		FALSE, 0 }, { /* generated as default value for BswM_MRP_RequestFullCom */
		FALSE, 0 }, { /* generated as default value for BswM_MRP_BswM_MDG */
		FALSE, 0 } };

/*
 **********************************************************************************************************************
 * Array of BswMModeRequestPorts having deferred request with contents: 
 *   # MRP/ERP Type 
 *   # MRP's/ERP's ChannelID
 *   # Base Address of Associated Rules Array
 *   # Number of Associated Rules
 *   # Deferred Request Flag 
 **********************************************************************************************************************
 */

BswM_Cfg_MRP_PC_DeferredReqst_tst BswM_Prv_DeferredReqstInfo_ast[BSWM_NO_OF_MRP_DEFERREDREQ] =
		{ {
		/* BswM_MRP_ApplicationRequestPort */
		BSWM_MRP_GENERIC_REQST, 0, NULL_PTR, 0, FALSE }, {
		/* BswM_MRP_BswM_MDG */
		BSWM_MRP_GENERIC_REQST, BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, NULL_PTR, 0,
				FALSE }, {
		/* BswM_MRP_RequestFullCom */
		BSWM_MRP_GENERIC_REQST, 1, NULL_PTR, 0, FALSE }

		};

/* 
 * BswM_GetDelayedReqInfoType_tpfct BswM_GetDelayedReqInfo_capfct [] : 
 * Array of function pointers, wherein each index indicates the function that 
 * is used to access the required information for a particular MRP Type
 */

BswM_GetDelayedReqInfoType_tpfct BswM_GetDelayedReqInfo_capfct[] = { NULL_PTR, /* BSWM_MRP_BSW_MODE_NOTIFICATION */
NULL_PTR, /* BSWM_MRP_CANSM_IND */
&BswM_Prv_GetComMIndicationInfo_en, /*BSWM_MRP_COMM_IND*/
NULL_PTR, /*BSWM_ERP_COMM_INITIATE_RESET*/
NULL_PTR, /*BSWM_MRP_COMM_PNC_REQST*/
NULL_PTR, /*BSWM_ERP_DCM_APPL_UPDATE_IND*/
NULL_PTR, /*BSWM_MRP_DCM_COM_MODE_REQST*/
NULL_PTR, /*BSWM_MRP_ECUM_IND*/
NULL_PTR, /*BSWM_MRP_ECUM_RUN_REQST_IND*/
NULL_PTR, /*BSWM_MRP_ECUM_WKUP_SOURCE*/
NULL_PTR, /*BSWM_MRP_ETHIF_PORT_GROUP_STATE*/
NULL_PTR, /*BSWM_MRP_ETHSM_IND*/
NULL_PTR, /*BSWM_MRP_FRSM_IND*/
&BswM_Prv_GetGenReqInfo_en, /*BSWM_MRP_BSW_MODE_NOTIFICATION + BSWM_MRP_GENERIC_REQST + BSWM_MRP_SWC_MODE_NOTIFICATION + BSWM_MRP_SWC_MODE_REQUEST*/
NULL_PTR, /*BSWM_MRP_J1939_DCM_BROADCAST_STATS*/
NULL_PTR, /*BSWM_MRP_J1939_NM_IND*/
NULL_PTR, /*BSWM_MRP_LINSM_IND*/
NULL_PTR, /*BSWM_MRP_LINSM_SCHEDULE_IND*/
NULL_PTR, /*BSWM_MRP_LINTP_MODE_REQST*/
NULL_PTR, /*BSWM_ERP_NM_CAR_WKUP_IND*/
NULL_PTR, /*BSWM_MRP_NM_STATE_CHNG_REQST*/
NULL_PTR, /*BSWM_MRP_NVM_JOB_MODE_IND*/
NULL_PTR, /*BSWM_MRP_NVM_REQST*/
NULL_PTR, /*BSWM_MRP_SD_CLNT_SERV_CURR_STATE*/
NULL_PTR, /*BSWM_MRP_SD_CNSMD_EVNT_GRP_CURR_STATE*/
NULL_PTR, /*BSWM_MRP_SD_EVNT_HNDLR_CURR_STATE*/
NULL_PTR, /*BSWM_MRP_SOAD_SO_CON_MODE_CHG*/
NULL_PTR, /*BSWM_MRP_SWC_MODE_NOTIFICATION*/
NULL_PTR, /*BSWM_MRP_SWC_MODE_REQUEST*/

};

#define BSWM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

/**********************************************************************************************************************
 *                                                                                                        
 **********************************************************************************************************************/
