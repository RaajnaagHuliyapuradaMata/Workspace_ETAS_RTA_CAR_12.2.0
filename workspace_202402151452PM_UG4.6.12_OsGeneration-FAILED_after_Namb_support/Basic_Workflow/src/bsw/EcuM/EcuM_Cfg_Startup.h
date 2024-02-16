


#ifndef ECUM_CFG_STARTUP_H
#define ECUM_CFG_STARTUP_H

/**************************************************************************************************/
/* Preprocessor includes                                                                          */
/**************************************************************************************************/
#include "Os.h" /* this is specified by Coding guideline as <ExtModule>.h. required for AppModeType */
// #if (!defined(OS_AR_RELEASE_MAJOR_VERSION) || (OS_AR_RELEASE_MAJOR_VERSION != 4))
    // #error "AUTOSAR major version undefined or mismatched"
// #endif
// #if (!defined(OS_AR_RELEASE_MINOR_VERSION) || (OS_AR_RELEASE_MINOR_VERSION != 2))
    // #error "AUTOSAR minor version undefined or mismatched"
// #endif


#include "Can.h"
#include "CanIf.h"
#include "CanSM.h"
#include "PduR.h"
#include "Com.h"
#include "ComM.h"
#include "BswM.h" /*Required for BswM init pointer used inside EcuM_Cfg_adrModuleInitRef_t*/
/***************************************************************************************************
* Generated type declarations
***************************************************************************************************/

typedef struct
{
            const Can_ConfigType *CanInitConfigPtr_cpst;
            const CanIf_ConfigType *CanIfInitConfigPtr_cpst;
            const CanSM_ConfigType *CanSMInitConfigPtr_cpst;
            const PduR_ConfigType *PduRInitConfigPtr_cpst;
            const Com_ConfigType *ComInitConfigPtr_cpst;
            const ComM_ConfigType *ComMInitConfigPtr_cpst;
    
 
    const BswM_ConfigType *BswM_InitConfigPtr_cpst; /* BswM_Init pointer*/
} EcuM_Cfg_adrModuleInitRef_t;

/* Structure holds the pointers passed to the Module Init Function */
typedef struct
{
    AppModeType DefaultAppMode;
    EcuM_ShutdownTargetInfoType DefaultShutdownTarget; /*Structure to hold the Default Shutdown Target*/
    EcuM_Cfg_adrModuleInitRef_t ModuleInitPtrPB; /*Structure to hold the post build configuration pointer of the modules*/
    const EcuM_Cfg_dataWkupPNCRefStruct_tst *adrWkupPNCRefs; /* Pointer to the PNC references associated with Wakeups */
    uint8 PostBuildHash[ECUM_CFG_LENGTH_OF_HASH];
} EcuM_ConfigType;

#define ECUM_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#include "EcuM_MemMap.h"


extern const EcuM_ConfigType EcuM_Config;


#define ECUM_NO_OF_ECUMCONFIGSETS 1

extern const EcuM_ConfigType * const EcuM_EcuMConfigurations_cpcast[ECUM_NO_OF_ECUMCONFIGSETS];

#define ECUM_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#include "EcuM_MemMap.h"

#endif /*ECUM_CFG_STARTUP_H*/


