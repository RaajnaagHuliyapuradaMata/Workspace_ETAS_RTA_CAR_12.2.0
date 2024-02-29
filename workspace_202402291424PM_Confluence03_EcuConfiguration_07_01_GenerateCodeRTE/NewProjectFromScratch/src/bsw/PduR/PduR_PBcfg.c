

 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: PduR  / AR45.5.0.1                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/


#include "PduR_PBcfg.h"
#include "PduR_UpIf.h"


#include "PduR_LoIf.h"



#include "PduR_Mc.h"
#include "PduR_Gw.h"


#include "PduR_Gw_Cfg.h"
/* Generating PbCfg_c::PduR_UpIfToLo_PBcfg_c::upIf_To_Lo */

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
#if defined(PDUR_CONFIG_SINGLE_IFTX_LO)
#define PduR_comToLo    NULL_PTR
#else
static const PduR_RT_UpToLo PduR_comToLo [] = {
    { CanIfConf_CanIfTxPduCfg_ApplicationECU_TestResult_Tx_Can_Network_CANNODE_0_OUT , (PduR_loTransmitFP)PduR_RF_CanIf_Transmit, (PduR_loCancelTransmitFP)PduR_IH_CancelTransmit }    /*ApplicationECU_TestResult_Tx_Com2PduR_Can_Network_Channel_CAN*/

};
#endif /* PDUR_CONFIG_SINGLE_IFTX_LO */

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
static const PduIdType PduR_ComToLoMapTable[] = 
{ 
           
    /*ApplicationECU_TestResult_Tx_Com2CanIf_Can_Network_Channel_CAN 0 */
    0         
};  

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
/* Generating PbCfg_c::PduR_UpTpToLo_PBcfg_c::upTp_To_Lo */

/* Generating PbCfg_c::PduR_LoIfRxToUp_PBcfg_c::loIfRx_To_Up */
    
        

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
#if defined(PDUR_CONFIG_SINGLE_IFRX)
#define PduR_CanIfRxToUp    NULL_PTR
#else
static const PduR_RT_LoIfRxToUp PduR_CanIfRxToUp[] = {        
    { ComConf_ComIPdu_IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx, (PduR_upIfRxIndicationFP) PduR_RF_Com_RxIndication }    /*TestECU_TestCode_Tx_CanIf2PduR_Can_Network_Channel_CAN*/
 };
#endif  /* PDUR_CONFIG_SINGLE_IFRX */

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
static const PduIdType PduR_CanIfRxToUpMapTable[] = 
{ 
           
    /*TestECU_TestCode_Tx_CanIf2Com_Can_Network_Channel_CAN 0 */
    0         
};

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
/* Generating PbCfg_c::PduR_LoIfDTxToUp_PBcfg_c::loIf_DTxToUp */
    
        

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
#if defined(PDUR_CONFIG_SINGLE_IFTX_UP )
#define PduR_CanIfTxToUp NULL_PTR
#else

static const PduR_RT_LoIfTxToUp PduR_CanIfTxToUp [] = {     
{ ComConf_ComIPdu_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx , (PduR_upIfTxConfirmationFP) PduR_RF_Com_TxConfirmation}     /* Index: 0  SrcPdu: ApplicationECU_TestResult_Tx_Com2PduR_Can_Network_Channel_CAN  DestPdu: ApplicationECU_TestResult_Tx_PduR2CanIf_Can_Network_Channel_CAN*/
};
#endif  /* PDUR_CONFIG_SINGLE_IFTX_UP */

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
static const PduIdType PduR_CanIfTxToUpMapTable[] = 
{  
          
    /*ApplicationECU_TestResult_Tx_PduR2CanIf_Can_Network_Channel_CAN 0 */
    0        
}; 

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
/* Generating PbCfg_c::PduR_LoIfTTxToUp_PBcfg_c::loIf_TTxToUp */
    
/* Generating PbCfg_c::PduR_LoTpRxToUp_PBcfg_c::loTpRx_To_Up */

/* Generating PbCfg_c::PduR_LoTpTxToUp_PBcfg_c::loTpTx_To_Up */
    
/* Generating PbCfg_c::PduR_Mc_UpTpToLo_PBcfg_c::mcUpTp_To_Lo */
/* Generating PbCfg_c::PduR_Mc_UpIfToLo_PBcfg_c::mcUpIf_To_Lo */
            
/* Generating PbCfg_c::PduR_Mc_TpTxToUp_PBcfg_c::xpandMcTpTxToUp */
/* Generating PbCfg_c::PduR_Mc_GwToLo_PBcfg_c::DisplayPduR_mcGwToLo */
/* Generating PbCfg_c::PduR_GwIfTx_PBcfg_c::display_GwIfTx */
/* Generating PbCfg_c::PduR_GwIf_PBcfg_c::display_GwIf */
/* Generating PbCfg_c::PduR_Gw_IfBuf_PBcfg_c::PduR_gw_Buf_If_structure */
/*****   __KW_COMMON   ***********/

/* Generating PbCfg_c::PduR_Rpg_PBcfg_c::display_PduR_RPG*/


#if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)



/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"

static const PduR_RPGInfoType PduR_RPGInfo[] = {

    {
     NULL_PTR,
     NULL_PTR,
     PDUR_RPGID_NULL,
     0,
     0
    },   /* PDUR_RPGID_NULL */

    
};

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"

/* ------------------------------------------------------------------------ */
/* Begin section for constants */


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_8
#else
#define PDUR_START_SEC_CONFIG_DATA_8
#endif

#include "PduR_MemMap.h"

/* Routing enable disbale flag to control routing. */
const boolean PduR_RPG_EnRouting[] =
{
  TRUE, /*PDUR_RPGID_NULL*/
  
};

/* ------------------------------------------------------------------------ */
/* End section for constants */


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_8
#else
#define PDUR_STOP_SEC_CONFIG_DATA_8
#endif
#include "PduR_MemMap.h"

#endif /* #if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0) */


/* Generating PbCfg_c::PduR_Gw_TpBuf_PBcfg_c::PduR_gw_Buf_TP_structure*/
/*****   __KW_COMMON   ***********/


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"


    /* Generating PbCfg_c::PduR_GwTp_PBcfg_c::display_GwTp */

    /* Generating PbCfg_c::PduR_RpgRxTp_PBcfg_c::display_RpgRxTp */
/* Generating PbCfg_c::PduR_PbConfigType_PBcfg_c::PduR_BswLoCfg */

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"

static const PduR_LoIfDConfig PduR_LoIfDCfg[] = {
  {
    PduR_CanIfRxToUp,       /* CanIf */
    PduR_CanIfTxToUp,       /* CanIf */
    PduR_CanIfTxToUpMapTable,
    PduR_CanIfRxToUpMapTable,1,        /* CanIf RxToUp NrEntries*/
    1      /* CanIf TxToUp NrEntries*/
    }
  };

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
/* Generating PbCfg_c::PduR_PbConfigType_PBcfg_c::PduR_BswUpCfg */



/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
static const PduR_UpConfig PduR_UpIfCfg[] = {
  {
    NULL_PTR,       /* mcComToLo */    PduR_comToLo,     /* Com */
    PduR_ComToLoMapTable,        1      /* Com */
    }
  };

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"

/* Generating PbCfg_c::PduR_Cdd_PBcfg_c::PduR_CddCfg */


/* Generating PbCfg_c::PduR_PbConfigType_PBcfg_c::PduR_BswUpToLoRxCfg */




    


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"

const PduR_RPTablesType PduR_RoutingPathTables = {
    (const PduR_loTransmitFuncType * )             PduR_loTransmitTable,
    (const PduR_loCancelReceiveFuncType * )        PduR_loCancelRxTable,
    (const PduR_loCancelTransmitFuncType * )       PduR_loCancelTransmitTable,
    (const PduR_upIfRxIndicationFuncType * )       PduR_upIfRxIndicationTable,
    (const PduR_upIfTxConfirmationFuncType * )     PduR_upIfTxConfirmationTable,
    (const PduR_upTpCopyRxDataFuncType * )         PduR_upTpCopyRxDataTable,
    (const PduR_upTpStartOfReceptionFuncType * )   PduR_upTpStartOfReceptionTable,
    (const PduR_upTpRxIndicationFuncType * )       PduR_upTpRxIndicationTable,
    (const PduR_upTpCopyTxDataFuncType * )         PduR_upTpCopyTxDataTable,
    (const PduR_upTpTxConfirmationFuncType * )     PduR_upTpTxConfirmationTable,
    (const PduR_upIfTriggerTxFuncType * )          PduR_upIfTriggerTxTable

};


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"

    
/*
    These structures are generated by the code generator tool. Respective module's function names are generated
    only if it is present in the PduR_PbCfg.c file in any one of the entries.
*/


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"


const PduR_loTransmitFuncType PduR_loTransmitTable[] =
{
    {&PduR_RF_CanIf_Transmit_Func}
};




const PduR_loCancelReceiveFuncType PduR_loCancelRxTable[] =
{
    {NULL_PTR}
};




const PduR_loCancelTransmitFuncType PduR_loCancelTransmitTable[] =
{
    {&PduR_IH_CancelTransmit_Func}
};



const PduR_upIfRxIndicationFuncType PduR_upIfRxIndicationTable[] =
{
    {&PduR_RF_Com_RxIndication_Func}
};



const PduR_upIfTxConfirmationFuncType PduR_upIfTxConfirmationTable[] =
{
    {&PduR_RF_Com_TxConfirmation_Func}
};



const PduR_upIfTriggerTxFuncType PduR_upIfTriggerTxTable[] =
{
    {NULL_PTR}
};



const PduR_upTpCopyRxDataFuncType PduR_upTpCopyRxDataTable[] =
{
    {NULL_PTR}
};



const PduR_upTpStartOfReceptionFuncType PduR_upTpStartOfReceptionTable[] =
{
    {NULL_PTR}
};



const PduR_upTpRxIndicationFuncType PduR_upTpRxIndicationTable[] =
{
    {NULL_PTR}
};




const PduR_upTpCopyTxDataFuncType PduR_upTpCopyTxDataTable[] =
{
    {NULL_PTR}
};



const PduR_upTpTxConfirmationFuncType PduR_upTpTxConfirmationTable[] =
{
    {NULL_PTR}
};






/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
/* Generating PbCfg_c::PduR_PbConfigType_PBcfg_c::pdur_PBConfigType */

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
const PduR_PBConfigType PduR_GlobalPBConfig = {
  (const PduR_CddConfig *) NULL_PTR,                                          /* PduR_CddCfg */
  (const PduR_LoTpConfig *) NULL_PTR,                                         /* PduR_LoTpCfg */
     (const PduR_LoIfDConfig *) PduR_LoIfDCfg,                                   /* Pointer to Direct lowerlayer If config structure */
     (const PduR_LoIfTTConfig *) NULL_PTR,                                       /* PduR_LoIfTTCfg */
     (const PduR_UpConfig *) PduR_UpIfCfg,                                       /* Pointer to Upperlayer If config structure */
  (const PduR_UpConfig *) NULL_PTR,                                           /* PduR_UpTpCfg */
     (const PduR_MT_UpToLo *) NULL_PTR,                                          /* mcGwToLo */
     (const PduR_MT_LoIfTxToUp *) NULL_PTR,                                      /* McIfRx */
     (const PduR_MT_LoTpTxToUp *) NULL_PTR,                                      /* McTpTx */
     (PduR_MS_LoTpTxToUp *) NULL_PTR,                                            /* PduR_msTpTxToUp*/
     (const PduR_GT_IfTx *) NULL_PTR,                                            /* gwIfTx */
     (const PduR_GT_If *) NULL_PTR,                                              /* gwIf        */
     (const PduR_GT_Tp *) NULL_PTR,                                              /* GwTp */
     (const PduR_RPG_LoTpRxToUp *) NULL_PTR,                                     /* rpgTp */
     (const PduR_RPTablesType * ) &PduR_RoutingPathTables, /* PduR_RoutingPathTables */
#if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)
    (const PduR_GwTp_SessionListType * ) NULL_PTR, /*PduR_TpSession_Dynamic*/
#endif
#if defined(PDUR_MULTICAST_TO_IF_SUPPORT) && (PDUR_MULTICAST_TO_IF_SUPPORT != 0)
     (const PduR_UpIfTxConf_Config * ) PduR_UpIfTxConf_ConfigList,
#endif
#if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)
     (const PduR_RPGInfoType * )        PduR_RPGInfo,        /* RoutingPathGroup ConfigInfo */
     (const boolean * )   PduR_RPG_EnRouting,  /* RoutingControl StatusInfo */
     (boolean * )  PduR_RPG_Status,        /*RAM status for each RPG*/
     (const PduR_RoutingPathGroupIdType *) PduR_RPGIdMappingTab,
     (PduR_RoutingPathGroupIdType)                              0,        /* Number of RPGs.*/
#endif
     (const PduR_UpTpToLoTpRxConfig *) NULL_PTR,                                 /* Pointer to PduR_UpTpToLoTpRxConfig structure for supporting Cancel Receive API */
0,                  /* PDUR_CONFIGURATION_ID */
0, /*Total no of Gw Tp Routing Path*/
0, /*Total no of Gw If Routing path*/
     (PduIdType) 0       /* McTpTx */
};

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"







/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"
const PduR_ConfigType PduR_Config = {
    NULL_PTR,    /* Void pointer initialised with null pointer as PduR_Config will not be used in case of PDUR_VARIANT_PRE_COMPILE */
    NULL_PTR
};


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#else
#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#endif

#include "PduR_MemMap.h"


