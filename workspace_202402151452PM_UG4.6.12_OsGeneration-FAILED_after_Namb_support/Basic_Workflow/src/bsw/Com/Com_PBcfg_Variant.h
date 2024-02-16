


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/


#if !defined(COM_PBCFG_VARIANT_H)
#define COM_PBCFG_VARIANT_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
/* Start variant defines : __KW_COMMON */


#define COM_NUM_RX_SIGNALS                  1u
#define COM_NUM_TX_SIGNALS                  1u


#define COM_NUM_TX_SIGNALGRP                0u

#define COM_NUM_TX_GRPSIGNALS               0u


#define COM_NUM_RX_SIGNALGRP                0u

#define COM_NUM_RX_GRPSIGNALS               0u

#define COM_NUM_TX_IPDU                     1u
#define COM_NUM_RX_IPDU                     1u


#ifdef COM_F_ONEEVERYN
#define COM_NUM_OF_SIG_GRPSIG_WITH_ONE_EVERY_N 0u
#endif /* #ifdef COM_F_ONEEVERYN */


#define COM_NUM_TOTAL_IPDU_GRP              2u

#define COM_NUM_TOTAL_TX_IPDU_GRP           1u

#define COM_NUM_TOTAL_RX_IPDU_GRP           1u

/* End variant defines : __KW_COMMON */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

/* Start variant declarations : __KW_COMMON */


/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)

extern const Com_Prv_xGwMapSigCfg_tst                       Com_Prv_xGwMapSigCfg_acst[];
extern const Com_Prv_xGwMapSigFifoCfg_tst                   Com_Prv_xGwMapSigFifoCfg_acst[];
extern const Com_Prv_xGwMapSigIdCfg_tst                     Com_Prv_xGwMapSigIdCfg_acst[];

#endif /* end of (COM_SIGNALGATEWAY) || (COM_SIGNALGROUPGATEWAY) */

/* Begin section for constants */
/* Signal Details */
extern const Com_Prv_xTxSigCfg_tst                          Com_Prv_xTxSigCfg_acst[];

/* Signal Details */
extern const Com_Prv_xRxSigCfg_tst                          Com_Prv_xRxSigCfg_acst[];
#ifdef COM_PRV_ENABLECONFIGINTERFACES
extern const Com_RxSigConfigType_tst                        Com_Prv_RxSigCfg_acst[];
#endif

/* Tx ipdu Details */
extern const Com_Prv_xTxIpduInfoCfg_tst                     Com_Prv_xTxIpduCfg_acst[];

/* Rx ipdu Details */
extern const Com_Prv_xRxIpduInfoCfg_tst                     Com_Prv_xRxIpduCfg_acst[];
#ifdef COM_PRV_ENABLECONFIGINTERFACES
extern const Com_RxIpduCfg_tst                              Com_RxIpduCfg_acst[];
#endif

#ifdef COM_TX_IPDUCOUNTER

/* Tx ipdu counter Details */
extern const Com_Prv_xTxIpduCntrCfg_tst                     Com_Prv_xTxIpduCntrCfg_acst[];
#endif /* #ifdef COM_TX_IPDUCOUNTER */

#ifdef COM_RX_IPDUCOUNTER

/* Rx ipdu counter Details */
extern const Com_Prv_xRxIpduCntrCfg_tst                     Com_Prv_xRxIpduCntrCfg_acst[];
#endif /* #ifdef COM_RX_IPDUCOUNTER */

/* Ipdu Group details */
extern const Com_Prv_xIpduGrpInfoCfg_tst                    Com_Prv_xIpduGrpCfg_acst[];
#ifdef COM_PRV_ENABLECONFIGINTERFACES
extern const Com_GlobalConfigType_tst                       Com_GlobalConfig_cst;
#endif

/* Internal config structure */
#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)
extern const Com_ConfigData_tst                             Com_Prv_Config_cst;
#endif

/* Array of Ipdu's placed in the order they occur in Ipdu groups */
extern const Com_IpduId_tuo                                 Com_Prv_xIPduGrp_IpduRefCfg_acuo[];

#ifdef COM_TX_SIGNALGROUP

/* Array of internal Tx signal group configuration */
extern const Com_Prv_xTxSigGrpCfg_tst                       Com_Prv_xTxSigGrpCfg_acst[];

#ifdef COM_TX_SIGNALGROUP_ARRAY

/* Array of internal Tx signal group Array configuration information */
extern const Com_Prv_xTxSigGrpArrayCfg_tst                  Com_Prv_xTxSigGrpArrayCfg_acst[];
#endif /* #ifdef COM_TX_SIGNALGROUP_ARRAY */

#endif /* #ifdef COM_TX_SIGNALGROUP */

#ifdef COM_RX_SIGNALGROUP

/* Array of internal Rx signal group configuration */
extern const Com_Prv_xRxSigGrpCfg_tst                       Com_Prv_xRxSigGrpCfg_acst[];

#ifdef COM_RX_SIGNALGROUP_ARRAY

/* Array of internal Tx signal group Array configuration information */
extern const Com_Prv_xRxSigGrpArrayCfg_tst                  Com_Prv_xRxSigGrpArrayCfg_acst[];
#endif /* #ifdef COM_RX_SIGNALGROUP_ARRAY */

#endif /* #ifdef COM_RX_SIGNALGROUP */

#ifdef COM_PRV_ENABLECONFIGINTERFACES
extern const Com_RxSigGrpConfigType_tst                     Com_Prv_RxSigGrpCfg_acst[];
#endif

/* Structure for GroupSignals */
#ifdef COM_TX_SIGNALGROUP

/* Array of Tx  group signal flags */
extern const Com_Prv_xTxGrpSigCfg_tst                       Com_Prv_xTxGrpSigCfg_acst[];
#endif

#ifdef COM_RX_SIGNALGROUP

/* Array of Rx group signal flags */
extern const Com_Prv_xRxGrpSigCfg_tst                       Com_Prv_xRxGrpSigCfg_acst[];
#endif

/* COM multiple MainFunction configuration for multi-task processing */
#ifdef COM_ENABLE_MAINFUNCTION_RX
extern const Com_MainFuncRxCfg_tst                          Com_MainFuncRxCfg_acst[];
#endif
#ifdef COM_ENABLE_MAINFUNCTION_TX
extern const Com_MainFuncTxCfg_tst                          Com_MainFuncTxCfg_acst[];
#endif


/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


/* Filter types */
#if defined (COM_F_MASKEDNEWEQUALSX) || defined(COM_F_MASKEDNEWDIFFERSX)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"
extern  const Com_MaskX_tst Com_MaskX_acst[];

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif

#ifdef COM_F_MASKEDNEWDIFFERSOLD

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"
extern const Com_FMaskOld_tst Com_FMaskOld_acst[];

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif

#if defined(COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"
extern const Com_POSMinMax_tst Com_POSMinMax_acst[];

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif

#if defined(COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"
extern const Com_NEGMinMax_tst Com_NEGMinMax_acst[];

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif

#ifdef COM_F_ONEEVERYN

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"
extern const Com_OneEveryN_tst Com_OneEveryN_Const_acst[];

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


#endif

/* End variant declarations : __KW_COMMON */


/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

extern const Com_Prv_xRxIpduVarCfg_tst Com_Prv_xRxIpduVarCfg_acst[];
extern const Com_Prv_xTxIpduVarCfg_tst Com_Prv_xTxIpduVarCfg_acst[];

#ifdef COM_CALLBACK_RX
extern const Com_Prv_xRxSigVarCfg_tst Com_Prv_xRxSigVarCfg_acst[];
# ifdef COM_RX_SIGNALGROUP
extern const Com_Prv_xRxSigGrpVarCfg_tst Com_Prv_xRxSigGrpVarCfg_acst[];
# endif /* end of COM_RX_SIGNALGROUP */
#endif /* end of COM_CALLBACK_RX */

#ifdef COM_CALLBACK_TX
extern const Com_Prv_xTxSigVarCfg_tst Com_Prv_xTxSigVarCfg_acst[];
# ifdef COM_TX_SIGNALGROUP
extern const Com_Prv_xTxSigGrpVarCfg_tst Com_Prv_xTxSigGrpVarCfg_acst[];
# endif /* end of COM_TX_SIGNALGROUP */
#endif /* end of COM_CALLBACK_TX */


/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"



#endif  /* end of COM_PBCFG_VARIANT_H */


