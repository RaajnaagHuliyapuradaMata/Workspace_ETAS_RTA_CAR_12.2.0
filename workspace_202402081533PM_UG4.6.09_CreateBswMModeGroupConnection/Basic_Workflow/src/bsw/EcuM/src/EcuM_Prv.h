/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2010, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
    extern variables
 **********************************************************************************************************************
 */

#ifndef ECUM_PRV_H
#define ECUM_PRV_H

/*
 ***************************************************************************************************
 * Preprocessor includes
 ***************************************************************************************************
 */

#include "EcuM_Cfg_Startup.h"
/*
 ***************************************************************************************************
 * Preprocessor Defines
 ***************************************************************************************************
 */

/* Interface API IDs for DET interface*/
#define ECUM_MODULE_ID                  (10U)
#define ECUM_INSTANCE_ID                (0U)
#define ECUM_GETVERINFO_APIID           (0x00U)
#define ECUM_GODOWN_APIID               (0x1fU)
#define ECUM_GODOWNHALTPOLL_APIID       (0x2cU)
#define ECUM_INIT_APIID                 (0x01U)
#define ECUM_START_TWO_APIID            (0x1aU)
#define ECUM_SHUTDOWN_APIID             (0x02U)
#define ECUM_SELECT_SHUT_TAR_APIID      (0x06U)
#define ECUM_GET_SHUT_TAR_APIID         (0x09U)
#define ECUM_SELECT_SHUT_CAUSE_APIID    (0x1bU)
#define ECUM_GET_SHUT_CAUSE_APIID       (0x1cU)
#define ECUM_LAST_SHUT_TAR_APIID        (0x08U)
#define ECUM_GET_PEND_APIID             (0x0dU)
#define ECUM_CLEAR_WKEVENT_APIID        (0x16U)
#define ECUM_GET_VALIDATE_APIID         (0x15U)
#define ECUM_GET_EXPIRED_APIID          (0x19U)
#define ECUM_SEL_BOOT_APIID             (0x12U)
#define ECUM_GET_BOOT_APIID             (0x13U)
#define ECUM_MAIN_FUNC_APIID            (0x18U)
#define ECUM_SET_WKEVENT_APIID          (0x0cU)
#define ECUM_VAL_WKEVENT_APIID          (0x14U)
#define ECUM_GOHALT_APIID               (0x20U)
#define ECUM_GOPOLL_APIID               (0x21U)
#define ECUM_RB_MO_GETACTIVEPBINDEX_APIID   (0x22U)
#define ECUM_RB_MO_INITCHECK_APIID      (0x23U)
#define ECUM_RB_MO_INITCHECKINV_APIID   (0x24U)
#define ECUM_LAST_SHUT_CAUSE_APIID      (0x30U)

#if(ECUM_CFG_MODE_HANDLING == STD_ON)
#define ECUM_SETSTATE_APIID             (0x2BU)
#define ECUM_RB_CURRENTSTATE_APIID             (0x30U)
#define ECUM_REQUESTRUN_APIID           (0x03U)
#define ECUM_RELEASERUN_APIID           (0x04U)
#define ECUM_REQUESTPOST_RUN_APIID      (0x0aU)
#define ECUM_RELEASEPOST_RUN_APIID      (0x0bU)
#define ECUM_KILL_ALL_RUN_REQUESTS_APIID      (0x05U)
#define ECUM_KILL_ALL_POST_RUN_REQUESTS_APIID      (0x2aU)
#endif//ECUM_CFG_MODE_HANDLING

/***************************************************************************************************
 * Macros definitions
 ***************************************************************************************************/
#define ECUM_SLEEP_MAX_U16            ((uint16)0xFF) /*macro for invalid Sleepmode*/
#define ECUM_COREID_DEFAULT_U16       ((uint16)0xFF) /*macro for default CoreId*/
#define ECUM_SHUTDOWNCAUSE_DEFAULT_U8 ((uint8)0x00)  /*macro for default shutdowncause*/
#define ECUM_HASH_FAILURE_U8          ((uint8)0x00)  /*macro for the ram hash failure value*/
#define ECUM_MCU_MODE_ZERO_U8         ((uint8)0x00)  /*macro for passing value to McuSet Mode*/
#define ECUM_TIMEOUT_U16              ((uint16)0x00) /*macro for wakeup timeout*/
#define ECUM_TIMER_INCREMENT_U16      ((uint16)0x01) /*macro for wakeup timer incrementation*/
#define ECUM_TIMER_REDUCTION_U16      ((uint16)0x01) /*macro for wakeup timer reduction*/

/*---------------------------Det error ids--------------------------------------------*/
/*(Non-AR) Error for GetResource api*/
#define ECUM_E_OS_GET_RESOURCE_FAILED                                   (0x05U)

/*(Non-AR) Error for ReleaseResource api*/
#define ECUM_E_OS_RELEASE_RESOURCE_FAILED                               (0x06U)
#define ECUM_E_UNINIT                                                   (0x10U)
#define ECUM_E_SERVICE_DISABLED                                         (0x11U)
#define ECUM_E_NULL_POINTER                                             (0x12U)
#define ECUM_E_INVALID_PAR                                              (0x13U)
/*Error ID for indicating failed read operation of NvM*/
#define ECUM_E_NVM_READ_FAILED                                          (0x1AU)
#define ECUM_E_PARAM_POINTER                                            (0x1BU)



#define ECUM_E_STATE_PAR_OUT_OF_RANGE                                   (0x16U)
#define ECUM_E_UNKNOWN_WAKEUP_SOURCE                                    (0x17U)
/*Error code for RAM hash check fail, during wakeup from sleep mode*/
#define ECUM_E_RAM_CHECK_FAILED                                         (0x19U)
#define ECUM_E_COREREADY_TIMEOUT_EXPIRED                                (0x04U)
/*Error if Shutdown service API's called after GoDown,GoHalt or GoPoll is initiated*/
#define ECUM_E_SHUTDOWN_INFO_UPDATED                                   (0xFEU)
/*Error code for Post build selected pointer, if it is inconsistent*/
#define ECUM_E_CONFIGURATION_DATA_INCONSISTENT                          (0x18U)
/*Error code for post build selected pointer, if it is NULL*/
#define ECUM_E_NULL_PTR_RETURNED                                        (0x30U)
/*Error code for post build selected pointer, if it is invalid. ie.) the pointer which is not generated from EcuM configset*/
#define ECUM_E_INVALID_PTR_RETURNED                                     (0x31U)
/*Error code for any core failed to start during EcuM Startup*/
#define ECUM_E_START_CORE_FAILED                                        (0x32U)
/*Error code for failure during initiation of the write request from the RAM block has failed*/
#define ECUM_E_NVM_WRITEBLOCK_REQUEST_FAILED                            (0x33U)
/*Error code for failure to write the shutdown info to NVRAM.*/
#define ECUM_E_NVM_WRITEBLOCK_ERROR                                     (0x34U)

#if(ECUM_CFG_MODE_HANDLING == STD_ON)
#define ECUM_E_MULTIPLE_RUN_REQUESTS                                    (0x14U)
#define ECUM_E_MISMATCHED_RUN_RELEASE                                   (0x15U)
#define ECUM_RB_E_ALL_RUN_REQUESTS_KILLED                               (0xffU)
#endif//ECUM_CFG_MODE_HANDLING

#if (ECUM_SLEEP_SUPPORT_ENABLE == TRUE)
#define ECUM_E_WAKEUP_TIMEOUT                                           (0x20U)


/*To distinguish between the singlecore and multicore operation*/
#define ECUM_SINGLE_CORE 0x00
#define ECUM_SLAVE_CORE  0x01
#define ECUM_MASTER_CORE 0x02
#endif//ECUM_SLEEP_SUPPORT_ENABLE

/* Phases of EcuM4.0 flex*/
#define ECUM_PRV_PHASE_START_PREOS                                       (0x01U)
#define ECUM_PRV_PHASE_START_POSTOS                                      (0x02U)
#define ECUM_PRV_PHASE_UP                                                (0x03U)
#define ECUM_PRV_PHASE_OFF_PREOS                                         (0x04U)
#define ECUM_PRV_PHASE_OFF_POSTOS                                        (0x05U)
#define ECUM_PRV_PHASE_SLEEP                                             (0x06U)


/*
 *
**************************************************************************************************
* Type Declarations
**************************************************************************************************
*/
#if (ECUM_SLEEP_SUPPORT_ENABLE == TRUE)
typedef enum
{
    ECUM_PRV_NO_SLEEP_E = 0,
    ECUM_PRV_GOPOLL_E =1,
    ECUM_PRV_GOHALT_E=2
} EcuM_Prv_SleepType_Enum_ten;

typedef enum
{
    ECUM_PRV_NORMAL_E = 0,
    ECUM_PRV_RESOURCE_NOT_ACQUIRED_E =1,
    ECUM_PRV_WAKEUP_REQUEST_E=2,
    ECUM_PRV_CHECK_WAKEUP_PENDING_E = 3
} EcuM_Prv_WakeupRestartReason_Enum_ten;

typedef enum
{
    ECUM_PRV_HALT_SEQUENCE_E = 0,
    ECUM_PRV_RESTART_SEQUENCE_E =1,
    ECUM_PRV_CLEAR_SLEEP_E=2,
    ECUM_PRV_POLL_SEQUENCE_E=3
} EcuM_Prv_OperationType_Enum_ten;

typedef enum {
    EXIT_SLEEP_E = 1,
    ENTER_SLEEP_E = 2,
    CONTINUE_WAIT
} EcuM_Prv_SleepStatus_ten;

typedef struct
{
    uint8  CurrentCore;
    uint16 CurrentCoreID;
}EcuM_CurrentCoreInfoType;
#endif//ECUM_SLEEP_SUPPORT_ENABLE
/*
 ***************************************************************************************************
 * Extern declarations
 ***************************************************************************************************/
#define ECUM_START_SEC_VAR_POWER_ON_CLEARED_8
#include "EcuM_MemMap.h"
/* Variable to store the Selected Boot target*/
extern EcuM_BootTargetType EcuM_Prv_dataSelectedBootTarget_u8;
/* Variable which stores the informaition if EcuM_Prv_dataSelectedBootTarget_u8 has been set with EcuM_SelectBootTarget
 * This is needed because ECUM_START_SEC_VAR_POWER_ON_INIT_8 does not exist */
extern uint8 EcuM_Prv_dataSelectedBootTargetInit_u8;

#define ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_8
#include "EcuM_MemMap.h"


#define ECUM_START_SEC_VAR_CLEARED_8
#include "EcuM_MemMap.h"

/*Tells the present Phase of the EcuM*/
extern uint8 EcuM_Prv_dataPresentPhase_u8;
extern uint8 EcuM_Prv_SlaveCoreTimeoutCtr_u8;/*Timeout counter slave core during shutdown synchronization*/


#if( ECUM_CFG_MODE_HANDLING == STD_ON)
/*Variable used to hold the status of Run Request/Release to notify the BswM as per Run handling protocol*/
extern uint8 EcuM_Prv_RunRequested_u8;
/*Variable used to hold the status of Post_Run Request/Release to notify the BswM as per Run handling protocol*/
extern uint8 EcuM_Prv_Post_Run_Requested_u8;

#endif //ECUM_CFG_MODE_HANDLING

#define ECUM_STOP_SEC_VAR_CLEARED_8
#include "EcuM_MemMap.h"

#define ECUM_START_SEC_VAR_INIT_8
#include "EcuM_MemMap.h"
/* Variable to store the selected Shutdown cause*/
extern EcuM_ShutdownCauseType EcuM_Prv_dataSelectedShutdownCause_u8;
#define ECUM_STOP_SEC_VAR_INIT_8
#include "EcuM_MemMap.h"

#define ECUM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"

#if (ECUM_SLEEP_SUPPORT_ENABLE == TRUE)
extern EcuM_Prv_SleepType_Enum_ten EcuM_Prv_SleepReqType_en;
extern EcuM_Prv_WakeupRestartReason_Enum_ten EcuM_Prv_WakeupRestartReason_en;
#endif//ECUM_SLEEP_SUPPORT_ENABLE

#define ECUM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"


#define ECUM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"
/* Variable to store the selected Shutdown target*/
extern EcuM_ShutdownTargetInfoType EcuM_Prv_dataSelectedShutdownTarget_st;

#if (ECUM_SLEEP_SUPPORT_ENABLE == TRUE)
extern EcuM_CurrentCoreInfoType EcuM_Prv_CoreStatus_st;
#endif//ECUM_SLEEP_SUPPORT_ENABLE

#define ECUM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"


#define ECUM_START_SEC_VAR_CLEARED_32
#include "EcuM_MemMap.h"

/*Variable to store the Old Pending Wakeup Events*/
extern EcuM_WakeupSourceType EcuM_Prv_dataOldPendingWakeupEvents_u32;
/* Variable to store the Expired Wakeup Events*/
extern EcuM_WakeupSourceType EcuM_Prv_dataExpiredWakeupEvents_u32;
/* Variable to store the Cleared Wakeup Events*/
extern EcuM_WakeupSourceType EcuM_Prv_dataClrWkpEventsInd_u32;
/* Variable to store the Pending Wakeup Events for BswM indincation*/
extern EcuM_WakeupSourceType EcuM_Prv_dataPndWkpEventsInd_u32;
/* Variable to store the Validated Wakeup Events for BswM indincation*/
extern EcuM_WakeupSourceType EcuM_Prv_dataValWkpEventsInd_u32;
/* Variable to store the Clear Wakeup Events */
extern EcuM_WakeupSourceType EcuM_Prv_dataExpWkpEventsInd_u32;

#define ECUM_STOP_SEC_VAR_CLEARED_32
#include "EcuM_MemMap.h"

#define ECUM_START_SEC_VAR_POWER_ON_CLEARED_32
#include "EcuM_MemMap.h"

/*Wakeup event which has to be validated and action of validated wakeup event to happen are restored even after the reset.*/
/* Variable to store the Pending Wakeup Events */
extern EcuM_WakeupSourceType EcuM_Prv_dataPendingWakeupEvents_u32;

/* Variable to store the Validated Wakeup Events*/
extern EcuM_WakeupSourceType EcuM_Prv_dataValidatedWakeupEvents_u32;

#define ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_32
#include "EcuM_MemMap.h"

#define ECUM_START_SEC_VAR_CLEARED_16
#include "EcuM_MemMap.h"
/* Variable to store the wake up validation Timeout Timer */
extern uint16 EcuM_Prv_WkpSrcValidationTimeoutCtr_u16;
#if (ECUM_SLEEP_SUPPORT_ENABLE == TRUE)
extern EcuM_ShutdownModeType EcuM_Prv_SleepMode_u16;
#endif//ECUM_SLEEP_SUPPORT_ENABLE

#define ECUM_STOP_SEC_VAR_CLEARED_16
#include "EcuM_MemMap.h"

#define ECUM_START_SEC_VAR_CLEARED_BOOLEAN
#include "EcuM_MemMap.h"

/*array used to hold the initalization status of startupTwo*/
extern boolean EcuM_Prv_flgIsStartupTwoCalled_ab[ECUM_CFG_NUM_OS_CORES];
/*EcuM_Init is called or not*/
extern boolean EcuM_Prv_flgIsModuleInitialised_b ;

extern boolean EcuM_Prv_flgCoreReady_ab[ECUM_CFG_NUM_OS_CORES];
/* EcuM_Prv_isEcuMGoDownFailed_b -->to enable or disable shutdown info update.
 * If TRUE updating the shutdown info stack is not possible
If FALSE we are allowed to update the shutdown info stack  --> EcuM_Rb_dataShutdownInfo_st*/
extern boolean EcuM_Prv_isEcuMGoDownFailed_b;
/*Flag to indicate the updatability of shutdown info*/
extern boolean EcuM_Prv_flgShutdownInfoDoNotUpdate_b;
/*To indicate whether shutdown process can be started*/
extern boolean EcuM_Prv_flgGoDown_b;

#if(ECUM_CFG_MODE_HANDLING == STD_ON)
/*Array of flag to maintain the RUN reuqest status of each user */
extern boolean EcuM_Prv_userRUNReqStatus_ab[ECUM_CFG_NUM_FLEX_USERS];

/*Array of flag to maintain the POST_RUN reuqest status of each user */
extern boolean EcuM_Prv_userPostRUNReqStatus_ab[ECUM_CFG_NUM_FLEX_USERS];

#endif//ECUM_CFG_MODE_HANDLING


#if (ECUM_CFG_NVM_BLOCK_CONFIGURED == TRUE)
/*flag to determine whether the previous shutdown information has been copied successfully from NV RAM to EcuM
 * prv variable*/
extern boolean EcuM_Prv_flgNvMReadStatus_b;
#endif//ECUM_CFG_NVM_BLOCK_CONFIGURED

#if (ECUM_SLEEP_SUPPORT_ENABLE == TRUE)
extern boolean EcuM_Prv_flgwakeupRestart_b;
extern boolean EcuM_Prv_flgClearWakeupRestart_b;
extern boolean EcuM_Prv_AllCoreSleepReady_b;
#if(ECUM_CFG_MULTICORE_ENABLED  ==  STD_ON)
extern volatile boolean EcuM_Prv_flgSleepReady_ab[ECUM_CFG_NUM_OS_CORES];
extern volatile boolean EcuM_Prv_RAMCheckPassed_b;
extern volatile boolean EcuM_Prv_PollSlaveCoreContinue_b;
#endif //ECUM_CFG_MULTICORE_ENABLED
#endif//ECUM_SLEEP_SUPPORT_ENABLE
#define ECUM_STOP_SEC_VAR_CLEARED_BOOLEAN
#include "EcuM_MemMap.h"

#if(ECUM_CFG_MODE_HANDLING == STD_ON)
#define ECUM_START_SEC_VAR_INIT_BOOLEAN
#include "EcuM_MemMap.h"

/*Variable used to hold the status of EcuM_KillAllRUNRequests*/
extern boolean EcuM_Prv_KillAllRUNRequests_b;
/*Variable used to hold the status of EcuM_KillAllPostRUNRequests*/
extern boolean EcuM_Prv_KillAllPostRUNRequests_b;

#define ECUM_STOP_SEC_VAR_INIT_BOOLEAN
#include "EcuM_MemMap.h"
#endif//ECUM_CFG_MODE_HANDLING


#define ECUM_START_SEC_VAR_INIT_16
#include "EcuM_MemMap.h"

#if(ECUM_CFG_MODE_HANDLING == STD_ON)
extern uint16 EcuM_Prv_RunMinDuration_u16;
#endif//ECUM_CFG_MODE_HANDLING

#define  ECUM_STOP_SEC_VAR_INIT_16
#include "EcuM_MemMap.h"

#define ECUM_START_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"
/*The ptr to the EcuM whole config pb structure*/
extern const EcuM_ConfigType * EcuM_Rb_adrConfigPtr_st;
/* variable to hold the previous shutdown information */
extern EcuM_ShutdownTargetInfoType EcuM_Prv_dataShutdownInfo_st;
#define ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"


#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"

extern boolean EcuM_Prv_hasCheckWakeupSourceGetHandle_b(
        EcuM_WakeupSourceType datasource_u32);
extern void EcuM_Prv_WakeupIndication(
        EcuM_WakeupSourceType dataPendingWakeupEvents_u32  );
extern void EcuM_Prv_DecValidationCtr( void );
extern void EcuM_Prv_PBPointerConsistencyCheck( void );
extern void EcuM_Prv_SetDefinedMcuWakeupSource( void );
extern void EcuM_Prv_StartOS(void);
extern uint32 EcuM_Prv_ComMWakeupHandling(EcuM_WakeupSourceType dataPendingWakeupEvents);

#if(ECUM_CFG_MODE_HANDLING == STD_ON)
extern boolean EcuM_Prv_IsUserHandlingPossible_b(
        EcuM_UserType * user, uint8 apiid);
#endif//ECUM_CFG_MODE_HANDLING

#if(ECUM_CFG_MULTICORE_ENABLED == STD_ON)
extern void EcuM_Prv_StartSlaveCores(void);
#endif//ECUM_CFG_MULTICORE_ENABLED

#if (ECUM_SLEEP_SUPPORT_ENABLE == TRUE)
extern void EcuM_Prv_GoSleepSequence(uint16 dataSleepModeHandle_u16);
extern void EcuM_Prv_WakeupRestartSequence ( uint16 dataSleepModeHandle_u16 );
extern StatusType EcuM_Prv_GetResource(CoreIdType core_ID);
extern StatusType EcuM_Prv_ReleaseResource(CoreIdType core_ID);
extern boolean EcuM_Prv_CheckWakeupTimerHandling ( void );
#endif// ECUM_SLEEP_SUPPORT_ENABLE
#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"
#endif /*ECUM_PRV_H*/

