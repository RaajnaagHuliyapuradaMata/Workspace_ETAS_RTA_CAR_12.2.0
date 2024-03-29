
#ifndef BSWM_PBCFG_H
#define BSWM_PBCFG_H


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "ComStack_Types.h"

#include "BswM_Cfg_General.h"

#include "CanSM_BswM.h"
/*#if (!defined(CANSM_AR_RELEASE_MAJOR_VERSION) || (CANSM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(CANSM_AR_RELEASE_MINOR_VERSION) || (CANSM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif*/

#include "Com.h"
/*#if (!defined(COM_AR_RELEASE_MAJOR_VERSION) || (COM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(COM_AR_RELEASE_MINOR_VERSION) || (COM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif*/

#include "ComM.h"
/*#if (!defined(COMM_AR_RELEASE_MAJOR_VERSION) || (COMM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(COMM_AR_RELEASE_MINOR_VERSION) || (COMM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif*/



#include "EcuM.h"
/*#if (!defined(ECUM_AR_RELEASE_MAJOR_VERSION) || (ECUM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(ECUM_AR_RELEASE_MINOR_VERSION) || (ECUM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif*/









#include "PduR.h"
/*#if (!defined(PDUR_AR_RELEASE_MAJOR_VERSION) || (PDUR_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(PDUR_AR_RELEASE_MINOR_VERSION) || (PDUR_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif*/






/* Os.h inclusion is required if the action BswMCorehaltMode is configured in BswM. If this action is configured, BswM 
will call the Os function ControlIdle().*/ 

#include "BswM_Cfg_MRP.h"
#include "BswM_Cfg_AC.h"
/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
#define BSWM_TOTAL_NO_OF_ACTIONLIST                         4

/*MR12 RULE 5.5 VIOLATION:identifier name depends on the configured pre-defined variants hence the first 31 significant
* characters of one macro may match macro name generated for another config set*/
/* C-Symbols for EcuM for configuration elements which are not unique */ 


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* Type definition for the Logical Expression evaluate function, referred in BswMRule Structure */
typedef void (*BswM_LogicalExpression_tpfct) (boolean *isValidMode_pb, boolean *hasLogExpRes_pb);





/* BswMComMIndication MRP Type Definition 
     * - Mode Init Value present flag
     * - Network Id of referred ComM Channel
     * - Configured/Default Mode Init Value 
     * - RequestProcessing Enum - Immediate/Deferred
     * - Number of rules referring to this MRP
     * - Pointer to array containing indexes of associated rules
*/
typedef struct
{
    boolean                         isModeInitValuePresent_b;
    NetworkHandleType               idNetwork_u8;
    ComM_ModeType                   dataModeInitValue_en;
    BswM_ReqProcessing_ten          dataReqProcessing_en;
    uint16                          nrAssociatedRules_u16;
    const uint16                    *adrRulesRef_pu16;
} BswM_Cfg_MRPComMIndicationType_tst;












/* BswMGenericRequest MRP Type Definition 
     * - Mode Init Value present flag
     * - User Id of configured User
     * - Maximum mode value requestable by configured User
     * - Configured/Default Mode Init Value 
     * - RequestProcessing Enum - Immediate/Deferred
     * - Number of rules referring to this MRP
     * - Pointer to array containing indexes of associated rules
*/
typedef struct
{
    boolean                         isModeInitValuePresent_b;
    BswM_UserType                   idUser_u16;
    BswM_ModeType                   dataModeMax_u16; 
    BswM_ModeType                   dataModeInitValue_u16;
    BswM_ReqProcessing_ten          dataReqProcessing_en;
    uint16                          nrAssociatedRules_u16;
    const uint16                    *adrRulesRef_pu16;
} BswM_Cfg_MRPGenericReqType_tst;















typedef struct
{
    /* MRP Source Structures Array */
    BswM_Cfg_MRPComMIndicationType_tst           dataComM_ast [BSWM_NO_OF_MRP_COMM_INDICATION];                   /* ComM Indication   */
    BswM_Cfg_MRPGenericReqType_tst     dataGenericReq_ast[BSWM_NO_OF_GENREQ_TOTAL];         /* Generic Request + SW-C Mode Notification + SW-C Mode Request + Bsw Mode Notification */
} BswM_Cfg_MRPType_tst;





typedef struct
{
    boolean                 isPduGroupSwitchReinit_b;
    uint8                   nrEnabledPduGroupRef_u8;
    uint8                   nrOfDisabledPduGroupRef_u8;
    const Com_IpduGroupIdType     *adrEnabledPduGroupRefID_pu8;
    const Com_IpduGroupIdType     *adrDisabledPduGroupRefID_pu8;
} BswM_Cfg_AC_PduGroupSwitchType_tst;





/*
 * PostBuild Structure for BswMAction
 * - BswMPduRouterControl
 * - BswMSwitchIPduMode
 * - BswMTriggerIPduSend
 * - BswMLinScheduleSwitch
 * - BswMSdServiceGroupSwitch
 */
typedef struct 
{
    const BswM_Cfg_AC_PduGroupSwitchType_tst             *adrIpduGroupSwitch_pst;
} BswM_Cfg_PBActionType_tst;



/* ActionListItem Type Definition 
     * - AbortOnFail flag 
     * - ActionListItem type enumeration [Action/ActionList]
     * - Pointer to referred Action/ActionList structure
     * - ErrorId reported in the Runtime Error
*/
typedef struct
{
    boolean                             isActionListAbortOnFail_b;
    BswM_ActionListItemType_ten         dataActionListItemType_en;
    const void                          *adrActionListItemRef_pv;
    uint8                                actionListItemErrorId_u8;
} BswM_Cfg_ActionListItemType_tst;

/*
     * - ActionListExecution type enumeration : Triggered/Conditional 
     * - Number of Action List Items 
     * - Pointer to ActionListItem structure 
*/
typedef struct
{
    BswM_ActionListExecutionType_ten        dataActionListExecutionType_en;
    uint8                                   nrActionListItems_u8;
    const BswM_Cfg_ActionListItemType_tst   *adrActionListItem_pst;
    uint16                                  idxActionListnum;
} BswM_Cfg_ActionListType_tst;

/* BswMRule Type Definition 
     * - Rule state type enumeration : True/False/Undefined
     * - Function pointer to logical expression referred by Rule
     * - Pointer to TrueActionList structure
     * - Pointer to FalseActionList structure
*/
typedef struct
{
    BswM_RuleStateType_ten          dataRuleInitState_en;
    BswM_LogicalExpression_tpfct    adrlogExp_pfct;
    const BswM_Cfg_ActionListType_tst     *adrTrueAL_pst;
    const BswM_Cfg_ActionListType_tst     *adrFalseAL_pst;
} BswM_Cfg_RuleType_tst;


/* BswMModeArbitration Type Definition 
     * - Structure of configured RequestPorts
     * - Number of rules configured in the system
     * - Pointer to array of Rules
*/
typedef struct
{
    BswM_Cfg_MRPType_tst        dataModeRequestPorts_st;
    uint16                          nrRules_u16; 
    const BswM_Cfg_RuleType_tst     *adrArbitrationRule_pst;
} BswM_Cfg_ModeArbitrationType_tst;


/* BswMModeControl Type Definition 
     * - Pointer to array of ActionLists
     * - Structure of configured PostBuild Actions
*/
typedef struct
{
    BswM_Cfg_PBActionType_tst               dataAction_st;
    const BswM_Cfg_ActionListType_tst       *adrActionList_pst;
} BswM_Cfg_ModeControlType_tst;

/* BswMConfig Type Definition 
     * - Structure containing ModeArbitration elements 
     * - Structure containing ModeControl elements 
*/
typedef struct
{
    BswM_Cfg_ModeArbitrationType_tst    dataModeArbitration_st;
    BswM_Cfg_ModeControlType_tst        dataModeControl_st;
    Std_VersionInfoType                 dataVersionInfo_st;
} BswM_ConfigType;

#define BSWM_NO_OF_CONFIGSETS 1U

#define BSWM_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#include "BswM_MemMap.h"

extern const BswM_ConfigType * const BswM_Configurations_capcst[BSWM_NO_OF_CONFIGSETS];

#define BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#include "BswM_MemMap.h"



/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

/* C-Structures for EcuM_Init: TPS_ECUC_08011 + Description of EcucPostBuildVariants*/ 
/*MR12 RULE 5.5 VIOLATION:identifier name depends on the configured pre-defined variants hence the first 31 significant
* characters of one macro may match macro name generated for another config set*/

#define BSWM_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED 
#include "BswM_MemMap.h"
 
    extern const BswM_ConfigType BswM_Config;

#define BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED 
#include "BswM_MemMap.h"        



#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/

