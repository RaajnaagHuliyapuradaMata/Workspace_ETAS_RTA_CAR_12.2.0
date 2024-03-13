
/***********************************************************************************************************************
 * Include files
 **********************************************************************************************************************/
#include "BswM.h"

#if (BSWM_RTE_IN_USE == TRUE)
#include "Rte_BswM.h"
#include "SchM_BswM.h"
#include "Rte_Main.h"
#endif
#include "BswM_Prv.h"

/***********************************************************************************************************************
 * Type Definition
 **********************************************************************************************************************/
typedef void (*BswM_Cfg_vFunPtrv_t)(void);
typedef Std_ReturnType (*BswM_Cfg_u8FunPtrv_t)(void);

/* User Include Files */
#include "Mcu.h"

#include "CanIf.h"

#include "Can.h"

#include "Rte_Main.h"

#include "CanSM.h"

#include "NvM.h"

#include "CanTp.h"

#include "Fim.h"

#include "WdgM.h"

#include "Fls.h"

#include "Fee.h"

#include "Nvm_MultiBlockCallback.h"

#include "Xcp.h"

#include "Dcm.h"

#include "Det.h"

/***********************************************************************************************************************
 * BswMComMAllowCom Action/s Object.
 * # ComMAllowed
 * # NetworkReference
 ***********************************************************************************************************************/
#define BSWM_START_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"

const BswM_Cfg_AC_ComMAllowComType_tst BswM_Cfg_AC_BswMComMAllowCom_cast[BSWM_NO_OF_AC_COMM_ALLOW_COM] =
		{ {
		/* BswM_AI_ComMCommAllowed_Can0 */
		TRUE, ComMConf_ComMChannel_Can_Network_Channel_Can_Network }, {
		/* BswM_AI_ComMCommNotAllowed_Can0 */
		FALSE, ComMConf_ComMChannel_Can_Network_Channel_Can_Network } };

#define BSWM_STOP_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"    

/***********************************************************************************************************************
 * BswMComMModeSwitch Action/s Object.
 * # ComMRequestMode
 * # ComUserReference
 ***********************************************************************************************************************/
#define BSWM_START_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"

const BswM_Cfg_AC_ComMModeSwitchType_tst BswM_Cfg_AC_BswMComMModeSwitch_cast[BSWM_NO_OF_AC_COMM_MODE_SWITCH] =
		{ {
		/* BswM_AI_ComMReqFullComm_User0 */
		BSWM_COMM_FULL_COMMUNICATION,
				ComMConf_ComMUser_ComMUser_Can_Network_Channel_Can_Network }, {
		/* BswM_AI_ComMReqNoComm_User0 */
		BSWM_COMM_NO_COMMUNICATION,
				ComMConf_ComMUser_ComMUser_Can_Network_Channel_Can_Network } };

#define BSWM_STOP_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"    

/***********************************************************************************************************************
 * IDs for SchMSwitch Functions.
 * # ID
 ***********************************************************************************************************************/
#define BSWM_START_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"

const BswM_Cfg_AC_SchMSwitchType_tst BswM_Cfg_AC_BswMSchMSwitch_cast[BSWM_NO_OF_AC_SCHM_SWITCH] =
		{ {
		/* BswM_AI_BswMSwitchAppRun */
		0 }, {
		/* BswM_AI_BswMSwitchPostRun */
		1 }, {
		/* BswM_AI_BswMSwitchPrepShutdown */
		2 }, {
		/* BswM_AI_BswMSwitchRun */
		3 }, {
		/* BswM_AI_BswMSwitchStartupTwo */
		4 } };

#define BSWM_STOP_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"    

#define BSWM_START_SEC_CODE 
#include "BswM_MemMap.h"                
/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_SchMSwitch_0_BswM_AI_BswMSwitchAppRun
 * Description     :       SchMSwitch Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       Std_ReturnType
 * Remarks         :   
 *****************************************************************************************/
static Std_ReturnType BswM_Cfg_AC_SchMSwitch_0_BswM_AI_BswMSwitchAppRun(void) {
	Std_ReturnType dataRet_u8 = BSWM_NO_RET_VALUE;
	Std_ReturnType action_RetVal_pu8 = BSWM_NO_RET_VALUE;

	/*
	 SchM_Switch_p_o( mode )
	 P = Port name
	 O = Mode Group within the ModeSwitchInterface
	 */

	dataRet_u8 =
			SchM_Switch_BswM_MDGP_AUTOSAR_BswM_ModeDeclarationGroups_MDG_ECUM_STATE(
					RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_APP_RUN);

	/* return whether the action was sucessful or not */
	switch (dataRet_u8) {
	case RTE_E_OK:
		action_RetVal_pu8 = E_OK;
		break;
	case RTE_E_LIMIT:
		action_RetVal_pu8 = E_NOT_OK;
		break;
	default:
		action_RetVal_pu8 = BSWM_NO_RET_VALUE;
		break;
	}

	return action_RetVal_pu8;
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_SchMSwitch_1_BswM_AI_BswMSwitchPostRun
 * Description     :       SchMSwitch Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       Std_ReturnType
 * Remarks         :   
 *****************************************************************************************/
static Std_ReturnType BswM_Cfg_AC_SchMSwitch_1_BswM_AI_BswMSwitchPostRun(void) {
	Std_ReturnType dataRet_u8 = BSWM_NO_RET_VALUE;
	Std_ReturnType action_RetVal_pu8 = BSWM_NO_RET_VALUE;

	/*
	 SchM_Switch_p_o( mode )
	 P = Port name
	 O = Mode Group within the ModeSwitchInterface
	 */

	dataRet_u8 =
			SchM_Switch_BswM_MDGP_AUTOSAR_BswM_ModeDeclarationGroups_MDG_ECUM_STATE(
					RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_APP_POST_RUN);

	/* return whether the action was sucessful or not */
	switch (dataRet_u8) {
	case RTE_E_OK:
		action_RetVal_pu8 = E_OK;
		break;
	case RTE_E_LIMIT:
		action_RetVal_pu8 = E_NOT_OK;
		break;
	default:
		action_RetVal_pu8 = BSWM_NO_RET_VALUE;
		break;
	}

	return action_RetVal_pu8;
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_SchMSwitch_2_BswM_AI_BswMSwitchPrepShutdown
 * Description     :       SchMSwitch Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       Std_ReturnType
 * Remarks         :   
 *****************************************************************************************/
static Std_ReturnType BswM_Cfg_AC_SchMSwitch_2_BswM_AI_BswMSwitchPrepShutdown(
		void) {
	Std_ReturnType dataRet_u8 = BSWM_NO_RET_VALUE;
	Std_ReturnType action_RetVal_pu8 = BSWM_NO_RET_VALUE;

	/*
	 SchM_Switch_p_o( mode )
	 P = Port name
	 O = Mode Group within the ModeSwitchInterface
	 */

	dataRet_u8 =
			SchM_Switch_BswM_MDGP_AUTOSAR_BswM_ModeDeclarationGroups_MDG_ECUM_STATE(
					RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_PREP_SHUTDOWN);

	/* return whether the action was sucessful or not */
	switch (dataRet_u8) {
	case RTE_E_OK:
		action_RetVal_pu8 = E_OK;
		break;
	case RTE_E_LIMIT:
		action_RetVal_pu8 = E_NOT_OK;
		break;
	default:
		action_RetVal_pu8 = BSWM_NO_RET_VALUE;
		break;
	}

	return action_RetVal_pu8;
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_SchMSwitch_3_BswM_AI_BswMSwitchRun
 * Description     :       SchMSwitch Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       Std_ReturnType
 * Remarks         :   
 *****************************************************************************************/
static Std_ReturnType BswM_Cfg_AC_SchMSwitch_3_BswM_AI_BswMSwitchRun(void) {
	Std_ReturnType dataRet_u8 = BSWM_NO_RET_VALUE;
	Std_ReturnType action_RetVal_pu8 = BSWM_NO_RET_VALUE;

	/*
	 SchM_Switch_p_o( mode )
	 P = Port name
	 O = Mode Group within the ModeSwitchInterface
	 */

	dataRet_u8 =
			SchM_Switch_BswM_MDGP_AUTOSAR_BswM_ModeDeclarationGroups_MDG_ECUM_STATE(
					RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_RUN);

	/* return whether the action was sucessful or not */
	switch (dataRet_u8) {
	case RTE_E_OK:
		action_RetVal_pu8 = E_OK;
		break;
	case RTE_E_LIMIT:
		action_RetVal_pu8 = E_NOT_OK;
		break;
	default:
		action_RetVal_pu8 = BSWM_NO_RET_VALUE;
		break;
	}

	return action_RetVal_pu8;
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_SchMSwitch_4_BswM_AI_BswMSwitchStartupTwo
 * Description     :       SchMSwitch Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       Std_ReturnType
 * Remarks         :   
 *****************************************************************************************/
static Std_ReturnType BswM_Cfg_AC_SchMSwitch_4_BswM_AI_BswMSwitchStartupTwo(
		void) {
	Std_ReturnType dataRet_u8 = BSWM_NO_RET_VALUE;
	Std_ReturnType action_RetVal_pu8 = BSWM_NO_RET_VALUE;

	/*
	 SchM_Switch_p_o( mode )
	 P = Port name
	 O = Mode Group within the ModeSwitchInterface
	 */

	dataRet_u8 =
			SchM_Switch_BswM_MDGP_AUTOSAR_BswM_ModeDeclarationGroups_MDG_ECUM_STATE(
					RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_STARTUP_TWO);

	/* return whether the action was sucessful or not */
	switch (dataRet_u8) {
	case RTE_E_OK:
		action_RetVal_pu8 = E_OK;
		break;
	case RTE_E_LIMIT:
		action_RetVal_pu8 = E_NOT_OK;
		break;
	default:
		action_RetVal_pu8 = BSWM_NO_RET_VALUE;
		break;
	}

	return action_RetVal_pu8;
}

#define BSWM_STOP_SEC_CODE 
#include "BswM_MemMap.h"            

/***********************************************************************************************************************
 * Array of Functions Pointers
 * # FunctionNames
 ***********************************************************************************************************************/
#define BSWM_START_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"             
static const BswM_Cfg_u8FunPtrv_t BswM_Cfg_AC_BswMSchMSwitch_FunPtr_cafct[BSWM_NO_OF_AC_SCHM_SWITCH] =
		{ &BswM_Cfg_AC_SchMSwitch_0_BswM_AI_BswMSwitchAppRun,
				&BswM_Cfg_AC_SchMSwitch_1_BswM_AI_BswMSwitchPostRun,
				&BswM_Cfg_AC_SchMSwitch_2_BswM_AI_BswMSwitchPrepShutdown,
				&BswM_Cfg_AC_SchMSwitch_3_BswM_AI_BswMSwitchRun,
				&BswM_Cfg_AC_SchMSwitch_4_BswM_AI_BswMSwitchStartupTwo };

#define BSWM_STOP_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"         

/*****************************************************************************************
 * Function name   :       BswM_Cfg_ProcessSchMSwitchFunction
 * Description     :       Determine which SchMSwitch Function to be invoked.
 * Parameter       :       SchMSwitchId
 * Return value    :       Std_ReturnType
 * Remarks         :   
 *****************************************************************************************/
#define BSWM_START_SEC_CODE 
#include "BswM_MemMap.h"
Std_ReturnType BswM_Cfg_ProcessSchMSwitchFunction(uint16 idSchMSwitch_u16) {
	Std_ReturnType RetVal = BSWM_NO_RET_VALUE;

	/* Check if the idSchMSwitch_u16 is within the array boundary */
	if (idSchMSwitch_u16
			< (sizeof(BswM_Cfg_AC_BswMSchMSwitch_FunPtr_cafct)
					/ sizeof(BswM_Cfg_u8FunPtrv_t))) {
		RetVal = BswM_Cfg_AC_BswMSchMSwitch_FunPtr_cafct[idSchMSwitch_u16]();
	} else {
		/* Do Nothing */
	}
	return RetVal;
}
#define BSWM_STOP_SEC_CODE 
#include "BswM_MemMap.h"

/***********************************************************************************************************************
 * UserIDs for UserCallout Functions.
 * # UserID
 ***********************************************************************************************************************/
#define BSWM_START_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"

const BswM_Cfg_AC_UserCalloutType_tst BswM_Cfg_AC_BswMUserCallout_cast[BSWM_NO_OF_AC_USERCALLOUT] =
		{ {
		/* BswM_AI_CanIfInit */
		0 }, {
		/* BswM_AI_CanInit */
		1 }, {
		/* BswM_AI_CanSmInit */
		2 }, {
		/* BswM_AI_CanTpInit */
		3 }, {
		/* BswM_AI_ComInit */
		4 }, {
		/* BswM_AI_ComMDeInit */
		5 }, {
		/* BswM_AI_ComMInit */
		6 }, {
		/* BswM_AI_DcmInit */
		7 }, {
		/* BswM_AI_DemShutdown */
		8 }, {
		/* BswM_AI_Dem_Init */
		9 }, {
		/* BswM_AI_DetInit */
		10 }, {
		/* BswM_AI_FeeInit */
		11 }, {
		/* BswM_AI_FeeRbEndInit */
		12 }, {
		/* BswM_AI_FimInit */
		13 }, {
		/* BswM_AI_FlsInit */
		14 }, {
		/* BswM_AI_GoDown */
		15 }, {
		/* BswM_AI_NvMInit */
		16 }, {
		/* BswM_AI_NvMReadAll */
		17 }, {
		/* BswM_AI_NvMWriteAll */
		18 }, {
		/* BswM_AI_PduRInit */
		19 }, {
		/* BswM_AI_RteStop */
		20 }, {
		/* BswM_AI_RteTimerStart */
		21 }, {
		/* BswM_AI_WdgMInit */
		22 }, {
		/* BswM_AI_XcpInit */
		23 } };

#define BSWM_STOP_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"    

#define BSWM_START_SEC_CODE 
#include "BswM_MemMap.h"                
/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_0_BswM_AI_CanIfInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_0_BswM_AI_CanIfInit(void) {
	CanIf_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_1_BswM_AI_CanInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_1_BswM_AI_CanInit(void) {
	Can_Init (&Can_ConfigData);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_2_BswM_AI_CanSmInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_2_BswM_AI_CanSmInit(void) {
	CanSM_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_3_BswM_AI_CanTpInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_3_BswM_AI_CanTpInit(void) {
	CanTp_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_4_BswM_AI_ComInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_4_BswM_AI_ComInit(void) {
	Com_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_5_BswM_AI_ComMDeInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_5_BswM_AI_ComMDeInit(void) {
	ComM_DeInit();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_6_BswM_AI_ComMInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_6_BswM_AI_ComMInit(void) {
	ComM_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_7_BswM_AI_DcmInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_7_BswM_AI_DcmInit(void) {
	Dcm_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_8_BswM_AI_DemShutdown
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_8_BswM_AI_DemShutdown(void) {
	Dem_Shutdown();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_9_BswM_AI_Dem_Init
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_9_BswM_AI_Dem_Init(void) {
	Dem_Init();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_10_BswM_AI_DetInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_10_BswM_AI_DetInit(void) {
	Det_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_11_BswM_AI_FeeInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_11_BswM_AI_FeeInit(void) {
	Fee_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_12_BswM_AI_FeeRbEndInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_12_BswM_AI_FeeRbEndInit(void) {
	Fee_Rb_EndInit();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_13_BswM_AI_FimInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_13_BswM_AI_FimInit(void) {
	FiM_Init (&FiM_Config);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_14_BswM_AI_FlsInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_14_BswM_AI_FlsInit(void) {
	Fls_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_15_BswM_AI_GoDown
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_15_BswM_AI_GoDown(void) {
	EcuM_GoDown(0);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_16_BswM_AI_NvMInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_16_BswM_AI_NvMInit(void) {
	NvM_Init();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_17_BswM_AI_NvMReadAll
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_17_BswM_AI_NvMReadAll(void) {
	EcuM_User_NvM_ReadAll();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_18_BswM_AI_NvMWriteAll
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_18_BswM_AI_NvMWriteAll(void) {
	IC_BswM_NvM_WriteAll();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_19_BswM_AI_PduRInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_19_BswM_AI_PduRInit(void) {
	PduR_Init (&PduR_Config);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_20_BswM_AI_RteStop
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_20_BswM_AI_RteStop(void) {
	Rte_Stop();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_21_BswM_AI_RteTimerStart
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_21_BswM_AI_RteTimerStart(void) {
	Rte_Start();
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_22_BswM_AI_WdgMInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_22_BswM_AI_WdgMInit(void) {
	WdgM_Init (NULL_PTR);
}

/*****************************************************************************************
 * Function name   :       BswM_Cfg_AC_UserCallout_23_BswM_AI_XcpInit
 * Description     :       UserCallout Function is invoked for Action.
 * Parameter       :       void
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
static void BswM_Cfg_AC_UserCallout_23_BswM_AI_XcpInit(void) {
	Xcp_Init (NULL_PTR);
}

#define BSWM_STOP_SEC_CODE 
#include "BswM_MemMap.h"            

/***********************************************************************************************************************
 * Array of Functions Pointers
 * # FunctionNames
 ***********************************************************************************************************************/
#define BSWM_START_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"             
static const BswM_Cfg_vFunPtrv_t BswM_Cfg_AC_BswMUserCallout_FunPtr_cafct[BSWM_NO_OF_AC_USERCALLOUT] =
		{ &BswM_Cfg_AC_UserCallout_0_BswM_AI_CanIfInit,
				&BswM_Cfg_AC_UserCallout_1_BswM_AI_CanInit,
				&BswM_Cfg_AC_UserCallout_2_BswM_AI_CanSmInit,
				&BswM_Cfg_AC_UserCallout_3_BswM_AI_CanTpInit,
				&BswM_Cfg_AC_UserCallout_4_BswM_AI_ComInit,
				&BswM_Cfg_AC_UserCallout_5_BswM_AI_ComMDeInit,
				&BswM_Cfg_AC_UserCallout_6_BswM_AI_ComMInit,
				&BswM_Cfg_AC_UserCallout_7_BswM_AI_DcmInit,
				&BswM_Cfg_AC_UserCallout_8_BswM_AI_DemShutdown,
				&BswM_Cfg_AC_UserCallout_9_BswM_AI_Dem_Init,
				&BswM_Cfg_AC_UserCallout_10_BswM_AI_DetInit,
				&BswM_Cfg_AC_UserCallout_11_BswM_AI_FeeInit,
				&BswM_Cfg_AC_UserCallout_12_BswM_AI_FeeRbEndInit,
				&BswM_Cfg_AC_UserCallout_13_BswM_AI_FimInit,
				&BswM_Cfg_AC_UserCallout_14_BswM_AI_FlsInit,
				&BswM_Cfg_AC_UserCallout_15_BswM_AI_GoDown,
				&BswM_Cfg_AC_UserCallout_16_BswM_AI_NvMInit,
				&BswM_Cfg_AC_UserCallout_17_BswM_AI_NvMReadAll,
				&BswM_Cfg_AC_UserCallout_18_BswM_AI_NvMWriteAll,
				&BswM_Cfg_AC_UserCallout_19_BswM_AI_PduRInit,
				&BswM_Cfg_AC_UserCallout_20_BswM_AI_RteStop,
				&BswM_Cfg_AC_UserCallout_21_BswM_AI_RteTimerStart,
				&BswM_Cfg_AC_UserCallout_22_BswM_AI_WdgMInit,
				&BswM_Cfg_AC_UserCallout_23_BswM_AI_XcpInit };

#define BSWM_STOP_SEC_CONST_UNSPECIFIED 
#include "BswM_MemMap.h"         

/*****************************************************************************************
 * Function name   :       BswM_Cfg_ProcessUserCalloutFunction
 * Description     :       Determine which UserCallout Function to be invoked.
 * Parameter       :       UserId
 * Return value    :       void
 * Remarks         :   
 *****************************************************************************************/
#define BSWM_START_SEC_CODE 
#include "BswM_MemMap.h"
void BswM_Cfg_ProcessUserCalloutFunction(uint16 idUserCallout_u16) {
	/* Check if the idUserCallout_u16 is within the array boundary */
	if (idUserCallout_u16
			< (sizeof(BswM_Cfg_AC_BswMUserCallout_FunPtr_cafct)
					/ sizeof(BswM_Cfg_vFunPtrv_t))) {
		BswM_Cfg_AC_BswMUserCallout_FunPtr_cafct[idUserCallout_u16]();
	} else {
		/* Do Nothing */
	}
}
#define BSWM_STOP_SEC_CODE 
#include "BswM_MemMap.h"

/**********************************************************************************************************************
 * End of file                                                                                                     
 **********************************************************************************************************************/
