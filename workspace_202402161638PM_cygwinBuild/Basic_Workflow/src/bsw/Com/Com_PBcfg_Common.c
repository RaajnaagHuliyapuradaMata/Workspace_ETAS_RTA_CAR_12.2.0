


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
/*
 * If COM_DontUseExternalSymbolicNames is defined before including Com_Cfg.h file, then external symbolic names will
 * not be visible.
 * Com_PBcfg.c file should not use external symbolic names.
 * This mechanism is used to prevent the accidental usage of external symbolic names in this file
 * This file should use only internal symbolic name defined in  Com_PBcfg_InternalSymbolicNames.h
 */
#define COM_DontUseExternalSymbolicNames
#include "Com_Prv.h"
#include "Com_PBcfg_Common.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

/* START: I-PDU Buffers */

    /* Tx-Ipdu Local Buffers */

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
    uint8 Com_dIP_ApplicationECU_To_TestECU_Can_Network_Channel_CAN_TxByte[8];

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
    /* Rx-Ipdu Local Buffers */

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
    uint8 Com_dIP_TestECU_To_ApplicationECU_Can_Network_Channel_CAN_RxByte[8];

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"

/* END: I-PDU Buffers */

#ifdef COM_PRV_ENABLECONFIGINTERFACES

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
    uint8 Com_Prv_RxSigS_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_u8;

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
#endif /* end of COM_PRV_ENABLECONFIGINTERFACES */


/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
    Com_RxSignalFlagType_tst Com_Prv_RxSigFlagS_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
#ifdef COM_RX_SIGNALGROUP
#endif /* end of #ifdef COM_RX_SIGNALGROUP */

/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
    Com_TxSignalFlagType_tst Com_Prv_TxSigFlagS_ApplicationECU_To_TestECU_Data_Can_Network_Channel_CAN_Tx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
#ifdef COM_TX_SIGNALGROUP
#if defined(COM_EffectiveSigGrpTOC) || defined(COM_SIGNALGROUPGATEWAY)
#endif /* #if defined(COM_EffectiveSigGrpTOC) || defined(COM_SIGNALGROUPGATEWAY) */
#endif /* end of COM_TX_SIGNALGROUP */

/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
Com_RxIpduRamData_tst Com_Prv_RxIpduRam_IP_TestECU_To_ApplicationECU_Can_Network_Channel_CAN_Rx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
Com_TxIpduRamData_tst Com_Prv_TxIpduRam_IP_ApplicationECU_To_TestECU_Can_Network_Channel_CAN_Tx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"


/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
Com_Prv_xIpduGrpRam_tst Com_Prv_IpduGrpRam_ComIPduGroup_Tx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"



/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
Com_Prv_xIpduGrpRam_tst Com_Prv_IpduGrpRam_ComIPduGroup_Rx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

#ifdef COM_F_MASKEDNEWDIFFERSOLD

#endif

#ifdef COM_F_ONEEVERYN



#endif

#if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC)


/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
static Com_OldValTOC_tuo Com_OldValueTOC_ComMainFunctionTx_auo[ 1 ];

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

#endif /* end of #if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) */

#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)

#define Com_Prv_GwTxSigBuf_ComMainFunctionTx_au8 (NULL_PTR)

#endif /* end of #if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */

#if defined(COM_RxIPduDeferredProcessing) || defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
/* Deferred buffer is used for deferred IPDU signal-processing or
to locally copy rx-signal data in case of signal gateway,
hence buffer is enabled in both the above use-cases */

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
/* Buffer to hold data for deferred signal processing */
static uint8 Com_RxDeferredBuf_ComMainFunctionRx_au8[ 8 ];

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
#endif /* end of (COM_RxIPduDeferredProcessing) || (COM_SIGNALGATEWAY) || (COM_SIGNALGROUPGATEWAY) */





/* Signal Buffers */
/* Type: UINT8, SINT8, UINT8_N, BOOLEAN */

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"

uint8 Com_RxSigBuf_ComMainFunctionRx_au8[1];

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"


/* Type: UINT16, SINT16 */
#define Com_RxSigBuf_ComMainFunctionRx_au16          ( NULL_PTR )

/* Type: UINT32, SINT32, FLOAT32 */
#define Com_RxSigBuf_ComMainFunctionRx_au32          ( NULL_PTR )

/* Type: UINT64, SINT64 */
#ifdef COM_RXSIG_INT64
#define Com_RxSigBuf_ComMainFunctionRx_au64          ( NULL_PTR )
#endif /* #ifdef COM_RXSIG_INT64 */


#ifdef COM_RXSIG_FLOAT64SUPP
#define Com_RxSigBuf_ComMainFunctionRx_af64          ( NULL_PTR )
#endif /* #ifdef COM_RXSIG_FLOAT64SUPP */

/* Type: UINT8_DYN */
#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
#define Com_RxSigBuf_ComMainFunctionRx_dyn_au8          ( NULL_PTR )
#endif


#ifdef COM_RX_SIGNALGROUP_ARRAY
#define Com_RxSigGrpArrayBuf_ComMainFunctionRx_au8  ( NULL_PTR )
#endif /* #ifdef COM_RX_SIGNALGROUP_ARRAY */

#ifdef COM_RX_SIGNALGROUP
/* Rx Group Signal Buffers */
/* The function Com_ReceiveSignalGroup() is not interrupt safe, hence there is need to
 * to generate second buffer for all the group signals
 */
/* Type: UINT8, SINT8, UINT8_N, BOOLEAN */
#define Com_RxGrpSigBuf_ComMainFunctionRx_au8             ( NULL_PTR )
#define Com_SecondRxGrpSigBuf_ComMainFunctionRx_au8       ( NULL_PTR )

/* Type: UINT16, SINT16 */
#define Com_RxGrpSigBuf_ComMainFunctionRx_au16            ( NULL_PTR )
#define Com_SecondRxGrpSigBuf_ComMainFunctionRx_au16      ( NULL_PTR )

/* Type: UINT32, SINT32 */
#define Com_RxGrpSigBuf_ComMainFunctionRx_au32            ( NULL_PTR )
#define Com_SecondRxGrpSigBuf_ComMainFunctionRx_au32      ( NULL_PTR )

/* Type: UINT64, SINT64 */
#ifdef COM_RXGRPSIG_INT64
#define Com_RxGrpSigBuf_ComMainFunctionRx_au64            ( NULL_PTR )
#define Com_SecondRxGrpSigBuf_ComMainFunctionRx_au64      ( NULL_PTR )
#endif /* #ifdef COM_RXGRPSIG_INT64 */

#ifdef COM_RXGRPSIG_FLOAT64SUPP
#define Com_RxGrpSigBuf_ComMainFunctionRx_af64            ( NULL_PTR )
#define Com_SecondRxGrpSigBuf_ComMainFunctionRx_af64      ( NULL_PTR )
#endif /* #ifdef COM_RXGRPSIG_FLOAT64SUPP */

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
#define Com_RxGrpSigBuf_ComMainFunctionRx_dyn_au8         ( NULL_PTR )
#define Com_SecondRxGrpSigBuf_ComMainFunctionRx_dyn_au8   ( NULL_PTR )
#endif /* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

#endif /* #ifdef COM_RX_SIGNALGROUP */


#ifdef COM_TX_SIGNALGROUP


/* Tx Group Signal Buffers  */
/* Type: UINT8, SINT8, UINT8_N, BOOLEAN */
#define Com_TxGrpSigBuf_ComMainFunctionTx_au8          ( NULL_PTR )

/* Type: UINT16, SINT16 */
#define Com_TxGrpSigBuf_ComMainFunctionTx_au16         ( NULL_PTR )

/* Type: UINT32, SINT32, FLOAT32 */
#define Com_TxGrpSigBuf_ComMainFunctionTx_au32         ( NULL_PTR )

/* Type: UINT64, SINT64 */
#ifdef COM_TXGRPSIG_INT64
#define Com_TxGrpSigBuf_ComMainFunctionTx_au64         ( NULL_PTR )
#endif /* #ifdef COM_TXGRPSIG_INT64 */


/* Type: FLOAT64 */
#ifdef COM_TXGRPSIG_FLOAT64SUPP
#define Com_TxGrpSigBuf_ComMainFunctionTx_af64         ( NULL_PTR )
#endif /* #ifdef COM_TXGRPSIG_FLOAT64SUPP */


/* Type: UINT8_DYN */
#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
#define Com_TxGrpSigBuf_ComMainFunctionTx_dyn_au8         ( NULL_PTR )
#endif /* #ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */



#endif /* end of #ifdef COM_TX_SIGNALGROUP */



/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/



#if defined (COM_TXSIG_FLOAT64SUPP ) || defined(COM_TXGRPSIG_FLOAT64SUPP) || defined(COM_RXGRPSIG_FLOAT64SUPP) || defined(COM_RXSIG_FLOAT64SUPP)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const float64 Com_Float64SigValues_acf64[ 1u ] =
{

    0.0 /* Index : 0 */
    
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif




#if defined (COM_TXSIG_INT64 ) || defined(COM_RXSIG_INT64) || defined(COM_TXGRPSIG_INT64) || defined(COM_RXGRPSIG_INT64)

/* Begin section for 64-bit constants */
#define COM_START_SEC_CONFIG_DATA_64
#include "Com_MemMap.h"

const uint64 Com_Int64SigValues_acu64[ 1u ] =
{

    0uLL /* Index : 0, raw-value: 0 */
    
};

/* End section for 64-bit constants */
#define COM_STOP_SEC_CONFIG_DATA_64
#include "Com_MemMap.h"

#endif /* end of _INT64 */




/* Begin section for 32-bit constants */
#define COM_START_SEC_CONFIG_DATA_32
#include "Com_MemMap.h"

const uint32 Com_Prv_Int32SigValues_acu32[ 1u ] =
{

    0uL /* Index : 0, raw-value: 0 */
    
};

/* End section for 32-bit constants */
#define COM_STOP_SEC_CONFIG_DATA_32
#include "Com_MemMap.h"




/* Begin section for 8-bit constants */
#define COM_START_SEC_CONFIG_DATA_8
#include "Com_MemMap.h"

/* This array shall be used as INIT value by UINT8_N signals that are not configured with ComSignalInitValue */
static const uint8 Com_ByteArraySigValue_Index0_au8[ 1 ] = 
{
    0u
};


/* End section for 8-bit constants */
#define COM_STOP_SEC_CONFIG_DATA_8
#include "Com_MemMap.h"



/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_ByteArraySig_tst Com_ByteArraySigValues_acst[] =
{
     {&Com_ByteArraySigValue_Index0_au8[0], 0u}
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"





/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_xRxRamBuf_tst Com_Prv_xRxRamBuf_acst[ 1u ] =
{

    {
        (uint8 * )              Com_RxSigBuf_ComMainFunctionRx_au8

#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
        ,(uint8 * )             Com_RxSigBuf_ComMainFunctionRx_dyn_au8
#endif

        ,(uint16 * )            Com_RxSigBuf_ComMainFunctionRx_au16

        ,(uint32 * )            Com_RxSigBuf_ComMainFunctionRx_au32

#ifdef COM_RXSIG_INT64
        ,(uint64 * )            Com_RxSigBuf_ComMainFunctionRx_au64
#endif

#ifdef COM_RXSIG_FLOAT64SUPP
        ,(float64 * )           Com_RxSigBuf_ComMainFunctionRx_af64
#endif

#ifdef COM_RX_SIGNALGROUP
        ,(uint8 * )             Com_RxGrpSigBuf_ComMainFunctionRx_au8
        ,(uint8 * )             Com_SecondRxGrpSigBuf_ComMainFunctionRx_au8

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
        ,(uint8 * )             Com_RxGrpSigBuf_ComMainFunctionRx_dyn_au8
        ,(uint8 * )             Com_SecondRxGrpSigBuf_ComMainFunctionRx_dyn_au8
#endif

        ,(uint16 * )            Com_RxGrpSigBuf_ComMainFunctionRx_au16
        ,(uint16 * )            Com_SecondRxGrpSigBuf_ComMainFunctionRx_au16

        ,(uint32 * )            Com_RxGrpSigBuf_ComMainFunctionRx_au32
        ,(uint32 * )            Com_SecondRxGrpSigBuf_ComMainFunctionRx_au32

#ifdef COM_RXGRPSIG_INT64
        ,(uint64 * )            Com_RxGrpSigBuf_ComMainFunctionRx_au64
        ,(uint64 * )            Com_SecondRxGrpSigBuf_ComMainFunctionRx_au64
#endif

#ifdef COM_RXGRPSIG_FLOAT64SUPP
        ,(float64 * )           Com_RxGrpSigBuf_ComMainFunctionRx_af64
        ,(float64 * )           Com_SecondRxGrpSigBuf_ComMainFunctionRx_af64
#endif

#ifdef COM_RX_SIGNALGROUP_ARRAY
        ,(uint8 * )             Com_RxSigGrpArrayBuf_ComMainFunctionRx_au8
#endif

#endif /* #ifdef COM_RX_SIGNALGROUP */

#if defined(COM_RxIPduDeferredProcessing) || defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
        ,(uint8 * )             Com_RxDeferredBuf_ComMainFunctionRx_au8
#endif

#ifdef COM_METADATA_SUPPORT
        ,(uint8 * )             Com_RxMetaDataDeferredBuf_ComMainFunctionRx_au8
#endif
    }

};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"




#if defined(COM_TX_SIGNALGROUP) || defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) || \
    defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_xTxRamBuf_tst Com_Prv_xTxRamBuf_acst[ 1u ] =
{

    {
#ifdef COM_TX_SIGNALGROUP
        (uint8 * )              Com_TxGrpSigBuf_ComMainFunctionTx_au8

#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
        ,(uint8 * )             Com_TxGrpSigBuf_ComMainFunctionTx_dyn_au8
#endif

        ,(uint16 * )            Com_TxGrpSigBuf_ComMainFunctionTx_au16

        ,(uint32 * )            Com_TxGrpSigBuf_ComMainFunctionTx_au32

#ifdef COM_TXGRPSIG_INT64
        ,(uint64 * )            Com_TxGrpSigBuf_ComMainFunctionTx_au64
#endif

#ifdef COM_TXGRPSIG_FLOAT64SUPP
        ,(float64 * )           Com_TxGrpSigBuf_ComMainFunctionTx_af64
#endif
#endif/* #ifdef COM_TX_SIGNALGROUP */

#if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC)
#ifdef COM_TX_SIGNALGROUP
        ,   /* all the features are required */
#endif
        (Com_OldValTOC_tuo * )  Com_OldValueTOC_ComMainFunctionTx_auo
#endif /* end of #if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) */

#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
#if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) || defined(COM_TX_SIGNALGROUP)
        ,   /* all the features are required */
#endif
        (uint8 * )              Com_Prv_GwTxSigBuf_ComMainFunctionTx_au8
#endif /* end of #if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */
    }

};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif /* end of #if defined(COM_TX_SIGNALGROUP) || defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) \
    || defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */








