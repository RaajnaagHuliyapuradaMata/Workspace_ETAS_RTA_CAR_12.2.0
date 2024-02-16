


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/

#if !defined(COM_CFG_H)
#define COM_CFG_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Com_Cfg_SymbolicNames.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
/**
 * @ingroup COM_CFG_H
 *
 * TRACE[SWS_Com_00442]
 * Com module id(according to AUTOSAR BSW module list)
 */
#define COM_MODULE_ID                                           50u
/**
 * @ingroup COM_CFG_H
 *
 * TRACE[BSW_VersionInfo_002]: according to RTA-BSW coding guidelines
 * Vendor id(vendor identification number for ETAS)
 */
#define COM_VENDOR_ID                                           6u

/* Version of AUTOSAR Com specification */
/**
 * @ingroup COM_CFG_H
 *
 * Major Version of AUTOSAR Com specification
 */
#define COM_AR_RELEASE_MAJOR_VERSION                            4u
/**
 * @ingroup COM_CFG_H
 *
 * Minor Version of AUTOSAR Com specification
 */
#define COM_AR_RELEASE_MINOR_VERSION                            5u
/**
 * @ingroup COM_CFG_H
 *
 * Revision Version of AUTOSAR Com specification
 */
#define COM_AR_RELEASE_REVISION_VERSION                         0u

/* Version of AUTOSAR Com implementation */
/**
 * @ingroup COM_CFG_H
 *
 * Major Version of AUTOSAR Com implementation
 */
#define COM_SW_MAJOR_VERSION                                    5u
/**
 * @ingroup COM_CFG_H
 *
 * Minor Version of AUTOSAR Com implementation
 */
#define COM_SW_MINOR_VERSION                                    0u
/**
 * @ingroup COM_CFG_H
 *
 * Patch Version of AUTOSAR Com implementation
 */
#define COM_SW_PATCH_VERSION                                    0u

#define COM_ECUC_RB_RTE_IN_USE                                  STD_ON

/**
 * @ingroup COM_CFG_H
 *
 * To be used only internal to this module, other modules shall not use this macro
 */
#define COM_ENABLE_INTER_MODULE_CHECKS                          (FALSE)


/* #define COM_PRV_ENABLECONFIGINTERFACES*/

/* SIGNAL GROUP switches */

/* #define COM_TX_SIGNALGROUP */

/* #define COM_RX_SIGNALGROUP */

#define COM_RXTIMER_TICKS_TO_STOP_IPDU_DM                       65534u

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/
/* Start: Type Declarations*/

/* TRACE[SWS_Com_00820]: The AUTOSAR COM module's signal object identifier. */
typedef uint16 Com_SignalIdType;

/* TRACE[SWS_Com_00821]: The AUTOSAR COM module's signal group object identifier. */
typedef uint16 Com_SignalGroupIdType;

/* TRACE[SWS_Com_00822]: The AUTOSAR COM module's I-PDU group object identifier. */
typedef uint16 Com_IpduGroupIdType;

/* TRACE[SWS_Com_00819]: Status value returned by the API service Com_GetStatus() */
typedef enum
{
    COM_UNINIT,     /* This status shall have the value 0. */
    COM_INIT
} Com_StatusType;

#ifdef COM_PRV_ENABLECONFIGINTERFACES
typedef struct
{
    uint16                                          numOfSig_u16;
    uint16                                          idRxSig_u16;
#ifdef COM_RX_SIGNALGROUP
    uint16                                          numOfSigGrp_u16;
    uint16                                          idFirstRxSigGrp_u16;
#endif
} Com_RxIpduCfg_tst;

typedef struct
{
    uint8 *                                         rxSigRAMFields_pu8;
    uint16                                          handleId_u16;
    uint8                                           general_u8;
} Com_RxSigConfigType_tst;

typedef struct
{
    uint8 *                                         rxSigGrpRAMFields_pu8;
    uint16                                          handleId_u16;
    uint8                                           rxSignalGrpFields_u8;
} Com_RxSigGrpConfigType_tst;

typedef struct
{
    const Com_RxIpduCfg_tst *                       rxIpduCfg_pcst;
    const Com_RxSigConfigType_tst *                 rxSigCfg_pcst;
    const Com_RxSigGrpConfigType_tst *              rxSigGrpCfg_pcst;
    uint16                                          numOfRxSigWithUpdtBit_u16;
    uint16                                          numOfRxSigGrpWithUpdtBit_u16;
    uint16                                          numOfRxIPdus_u16;
    uint16                                          numOfRxSig_u16;
#ifdef COM_RX_SIGNALGROUP
    uint16                                          numOfRxSigGrp_u16;
#endif
} Com_GlobalConfigType_tst;

#endif              /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */

/* TRACE[SWS_Com_00825]: Type of the data structure containing the initialization data for COM. */
typedef struct
{
#ifdef COM_PRV_ENABLECONFIGINTERFACES
    const Com_GlobalConfigType_tst *                globalConfigData_pcst;
#endif
    const void *                                    configData_pcv;
    const void *                                    variantData_pcv;
    const void *                                    commonData_pcv;

} Com_ConfigType;

/*  END: Type Declaration  */

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#ifdef COM_PRV_ENABLECONFIGINTERFACES
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

extern const Com_GlobalConfigType_tst *                      Com_GlobalConfig_pcst;

#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
#endif                   /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */

/**
 * TRACE[SWS_BSW_00210] Exclusion of MainFunction and BswModuleClientServerEntrys from the Implementation header
 * The module header files shall not include the prototype declarations of MainFunctions and
 * BswModuleClientServerEntrys that are expected to be invoked by the RTE/BswScheduler.
 * --> But in RTA-BSW BSW, if RTE layer is not present, then it shall be provided from module header.
 */
#if ( COM_ECUC_RB_RTE_IN_USE == STD_OFF )

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

/* Rx-MainFunction declarations */

extern void Com_MainFunctionRx_ComMainFunctionRx( void );

/* Tx-MainFunction declarations */

extern void Com_MainFunctionTx_ComMainFunctionTx( void );

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #if ( COM_ECUC_RB_RTE_IN_USE == STD_OFF ) */

#endif /* end of COM_CFG_H */


