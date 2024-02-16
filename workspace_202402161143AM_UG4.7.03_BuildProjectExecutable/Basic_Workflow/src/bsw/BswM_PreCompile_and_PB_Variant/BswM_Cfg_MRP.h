#ifndef BSWM_CFG_MRP_H
#define BSWM_CFG_MRP_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "BswM_Types.h"

/*
 **********************************************************************************************************************
 * Enum definitions
 **********************************************************************************************************************
 */

/*
 BswM Request Processing type Enum--
 Defines when the processing of a mode arbitration request is done :
 a. Deferred : In the context of the BswM main function
 b. Immediate : In the context of the MRP itself
 */

typedef enum {
	BSWM_DEFERRED, BSWM_IMMEDIATE
} BswM_ReqProcessing_ten;

/* Enumeration literal to represent the BswM Rule State */
typedef enum {
	BSWM_FALSE, BSWM_TRUE, BSWM_UNDEFINED
} BswM_RuleStateType_ten;

/* Enumeration literal to represent the BswM Timer MRP State */
typedef enum {
	BSWM_TIMER_STARTED, BSWM_TIMER_EXPIRED, BSWM_TIMER_STOPPED
} BswM_TimerStateType_ten;

/*
 BswM Mode Request Port type Enum--
 Defines the various types of mode request sources
 Default value = 0xFF has been defined to avoid MISRA warnings when 
 assigning a default value for variables of this enum type 
 (in particular, when clearing the interrupt queue, the MRPType/ERPType member 
 is assigned with the default value defined in this enum) 
 */

typedef enum {
	/* TODO : verify for all MRP */
	BSWM_MRP_BSW_MODE_NOTIFICATION,
	BSWM_MRP_CANSM_IND,
	BSWM_MRP_COMM_IND,
	BSWM_ERP_COMM_INITIATE_RESET,
	BSWM_MRP_COMM_PNC_REQST,
	BSWM_ERP_DCM_APPL_UPDATE_IND,
	BSWM_MRP_DCM_COM_MODE_REQST,
	BSWM_MRP_ECUM_IND,
	BSWM_MRP_ECUM_RUN_REQST_IND,
	BSWM_MRP_ECUM_WKUP_SOURCE,
	BSWM_MRP_ETHIF_PORTFGROUP,
	BSWM_MRP_ETHSM_IND,
	BSWM_MRP_FRSM_IND,
	BSWM_MRP_GENERIC_REQST,
	BSWM_MRP_J1939_DCM_BROADCAST_STATS,
	BSWM_MRP_J1939_NM_IND,
	BSWM_MRP_LINSM_IND,
	BSWM_MRP_LINSM_SCHEDULE_IND,
	BSWM_MRP_LINTP_MODE_REQST,
	BSWM_ERP_NM_CAR_WKUP_IND,
	BSWM_MRP_NM_STATE_CHNG_REQST,
	BSWM_MRP_NVM_JOB_MODE_IND,
	BSWM_MRP_NVM_REQST,
	BSWM_MRP_SD_CLNT_SERV_CURR_STATE,
	BSWM_MRP_SD_CNSMD_EVNT_GRP_CURR_STATE,
	BSWM_MRP_SD_EVNT_HNDLR_CURR_STATE,
	BSWM_MRP_SOAD_SO_CON_MODE_CHG,
	BSWM_MRP_SWC_MODE_NOTIFICATION,
	BSWM_MRP_SWC_MODE_REQUEST,
	BSWM_MRP_TIMER,
	BSWM_MRP_DEFAULT = 0xFF
} BswM_Cfg_MRPType_ten;

/* Enumeration literal to represent the state of BswM Event Request Port  */
typedef enum {
	BSWM_EVENT_IS_CLEARED, BSWM_EVENT_IS_SET
} BswM_MC_ERPType_ten;

/*
 BswM No Source Status type Enum--
 Defines requested mode type for MRPs with No source :
 a. BSWM_NO_SOURCE_STATUS_FALSE : MRP Initial Mode Type
 b. BSWM_NO_SOURCE_STATUS_TRUE  : MRP Requested Mode Type
 */
typedef enum {
	BSWM_NO_SOURCE_STATUS_FALSE, BSWM_NO_SOURCE_STATUS_TRUE
} BswM_Cfg_NoSourceType_en;

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

/*Count of configured ports for each Mode Request Source type*/

#define BSWM_NO_OF_MRP_CANSM_INDICATION             0u
#define BSWM_NO_OF_MRP_COMM_INDICATION              1u
#define BSWM_NO_OF_MRP_COMMPNCS_REQUEST             0u
#define BSWM_NO_OF_MRP_DCM_COM_MODE_REQ             0u
#define BSWM_NO_OF_ECUM_INDICATION                  0u
#define BSWM_NO_OF_ECUMRUNREQ_STATES                0u
#define BSWM_NO_OF_ECUMWKP_SOURCES                  0u
#define BSWM_NO_OF_MRP_ETHIF_PORTFGROUP             0u
#define BSWM_NO_OF_MRP_ETHSM_INDICATION             0u
#define BSWM_NO_OF_MRP_FRSM_INDICATION              0u
#define BSWM_NO_OF_GENREQS                          2u
#define BSWM_NO_OF_BSWMODENOTIFICATION              1u 
#define BSWM_NO_OF_J1939DCM_CHANNELS                0u
#define BSWM_NO_OF_J1939NM_CHANNELS                 0u
#define BSWM_NO_OF_LINSM_CHANNELS                   0u
#define BSWM_NO_OF_LINSMSCH_CHANNELS                0u
#define BSWM_NO_OF_LINTP_CHANNELS                   0u
#define BSWM_NO_OF_NVM_REQUESTS                     0u
#define BSWM_NO_OF_NVM_SERVICE_IDS                  0u
#define BSWM_NO_OF_SDCLIENTSRV_CS                   0u    
#define BSWM_NO_OF_SDCNSEVGRP_HANDLE_CS            0u 
#define BSWM_NO_OF_SDEVNTHNDLR_HANDLE_CS           0u 
#define BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG        0u 
#define BSWM_NO_OF_TIMER                           0u
#define BSWM_NO_OF_MRP_NM_STATE_CHG_NTF            0u 

#define BSWM_NO_OF_MRP_SWCMODENOTIF                 0u
#define BSWM_NO_OF_MRP_SWCMODEREQ                   0u
#define BSWM_NO_OF_GENREQ_TOTAL                     (BSWM_NO_OF_GENREQS + BSWM_NO_OF_BSWMODENOTIFICATION + BSWM_NO_OF_MRP_SWCMODENOTIF + BSWM_NO_OF_MRP_SWCMODEREQ)
#define BSWM_MRP_WITH_NO_CHANNEL_SOURCE             0u
#define BSWM_ERP_WITH_NO_CHANNEL_SOURCE             0u

#define BSWM_NO_OF_MRP_DEFERREDREQ                  3u

/* BswMGenericRequest - IDs of GenericRequests */
#define BSWM_CFG_USERID_BSWM_MRP_APPLICATIONREQUESTPORT    0

#define BSWM_CFG_USERID_BSWM_MRP_REQUESTFULLCOM    1

/* BswMGenericRequest - IDs of BswMRbGenericReqUserRef */

/* BswMBswModeNotification - IDs of BswMBswModeNotification */
#define BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG    2

/* MRP array indexes for ComM Indication MRPs */
/* ComM Indication -- Indexes of the ComM Indication referenced by BswM -- BswMModeRequesterId*/
#define BSWM_IDX_BSWM_MRP_COMM_CAN0         0

/* Generic requests -- Indexes of the Generic requests referenced by BswM -- BswMModeRequesterId*/
#define BSWM_IDX_BSWM_MRP_APPLICATIONREQUESTPORT               0

#define BSWM_IDX_BSWM_MRP_REQUESTFULLCOM               1

/* Generic requests -- Indexes of the Generic requests referenced by BswM -- BswMRbGenericReqUserRef */

/* BswMBswModeNotification - Indexes of BswMBswModeNotification */
#define BSWM_IDX_BSWM_MRP_BSWM_MDG         2

/*Count of configured ports for each Event Request Source type*/
/* #define BSWM_NO_OF_ERP_REQ                       (BSWM_NO_OF_ERP_COMM_INITIATE_RESET + BSWM_NO_OF_ERP_DCM_APP_UPDATE_INDICATION + BSWM_NO_OF_NM_CHANNELS) */
#define BSWM_NO_OF_ERP_REQ                          0u
#define BSWM_NO_OF_ERP_COMM_INITIATE_RESET          0u
#define BSWM_NO_OF_ERP_DCM_APP_UPDATE_INDICATION    0u
#define BSWM_NO_OF_NM_CHANNELS                      0u

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/* Structure to hold the mode value of ComM Indication MRPs */
typedef struct {
	boolean isValidModePresent_b;
	ComM_ModeType dataMode_u8;
} BswM_Cfg_MRP_PC_ComMIndicationType_tst;

/* Structure to hold the mode value of Generic Request MRPs */
typedef struct {
	boolean isValidModePresent_b;
	BswM_ModeType dataMode_u16;
} BswM_Cfg_MRP_PC_GenReqType_tst;

/* Structure to hold the Deferred Requested for RPs */
typedef struct {
	const BswM_Cfg_MRPType_ten dataMRPType_en;
	const uint32 dataChannelID_u32;
	const uint16 *adrAssociatedRules_pu16;
	uint16 nrAssociatedRules_u16;
	boolean isDeferredReqstPresent_b;
} BswM_Cfg_MRP_PC_DeferredReqst_tst;

/*
 **********************************************************************************************************************
 * Extern Variables
 **********************************************************************************************************************
 */

#define BSWM_START_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

extern BswM_Cfg_MRP_PC_ComMIndicationType_tst BswM_Cfg_ComMIndicationModeInfo_ast[BSWM_NO_OF_MRP_COMM_INDICATION];

extern BswM_Cfg_MRP_PC_GenReqType_tst BswM_Cfg_GenericReqModeInfo_ast[BSWM_NO_OF_GENREQ_TOTAL];

extern BswM_Cfg_MRP_PC_DeferredReqst_tst BswM_Prv_DeferredReqstInfo_ast[BSWM_NO_OF_MRP_DEFERREDREQ];

#define BSWM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

#endif /* BSWM_CFG_MRP_H */
/*********************************************************************************************************************/

