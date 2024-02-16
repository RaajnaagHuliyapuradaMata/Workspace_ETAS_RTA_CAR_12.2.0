


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/

#if !defined(COM_PBCFG_COMMON_H)
#define COM_PBCFG_COMMON_H

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

#define COM_UPDATE_MAX      COM_MAX_U8_VALUE    /* max ipdu size */



/* In all the variants */
# define COM_NUM_OF_TXIPDU_IN_ALL_VARIANTS              1u
# define COM_NUM_OF_RXIPDU_IN_ALL_VARIANTS              1u

# define COM_NUM_OF_TXSIG_IN_ALL_VARIANTS               1u
# define COM_NUM_OF_RXSIG_IN_ALL_VARIANTS               1u

# define COM_NUM_OF_TXSIGGRP_IN_ALL_VARIANTS            0u
# define COM_NUM_OF_RXSIGGRP_IN_ALL_VARIANTS            0u

# define COM_NUM_OF_TXGRPSIG_IN_ALL_VARIANTS            0u
# define COM_NUM_OF_RXGRPSIG_IN_ALL_VARIANTS            0u

# define COM_NUM_OF_IPDUGRP_IN_ALL_VARIANTS             2u

# define COM_TXGRPSIGNAL_ID_MAX_OUT_BOUND      (COM_NUM_OF_TXSIG_IN_ALL_VARIANTS + COM_NUM_OF_TXGRPSIG_IN_ALL_VARIANTS)
# define COM_RXGRPSIGNAL_ID_MAX_OUT_BOUND      (COM_NUM_OF_RXSIG_IN_ALL_VARIANTS + COM_NUM_OF_RXGRPSIG_IN_ALL_VARIANTS)

# define COM_TXSIGNALGRP_ID_MAX_OUT_BOUND      (COM_NUM_OF_TXSIG_IN_ALL_VARIANTS + COM_NUM_OF_TXSIGGRP_IN_ALL_VARIANTS)
# define COM_RXSIGNALGRP_ID_MAX_OUT_BOUND      (COM_NUM_OF_RXSIG_IN_ALL_VARIANTS + COM_NUM_OF_RXSIGGRP_IN_ALL_VARIANTS)




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

/* START: I-PDU Buffers */

    /* Tx-Ipdu Local Buffers */

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
extern uint8 Com_dIP_ApplicationECU_To_TestECU_Can_Network_Channel_CAN_TxByte[];

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
    /* Rx-Ipdu Local Buffers */

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
extern uint8 Com_dIP_TestECU_To_ApplicationECU_Can_Network_Channel_CAN_RxByte[];

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"

/* END: I-PDU Buffers */

#ifdef COM_PRV_ENABLECONFIGINTERFACES

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
extern uint8 Com_Prv_RxSigS_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_u8;

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
#endif /* end of COM_PRV_ENABLECONFIGINTERFACES */


/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
extern Com_RxSignalFlagType_tst Com_Prv_RxSigFlagS_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
#ifdef COM_RX_SIGNALGROUP
#endif /* end of #ifdef COM_RX_SIGNALGROUP */

/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
extern Com_TxSignalFlagType_tst Com_Prv_TxSigFlagS_ApplicationECU_To_TestECU_Data_Can_Network_Channel_CAN_Tx_st;

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
extern Com_RxIpduRamData_tst Com_Prv_RxIpduRam_IP_TestECU_To_ApplicationECU_Can_Network_Channel_CAN_Rx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
extern Com_TxIpduRamData_tst Com_Prv_TxIpduRam_IP_ApplicationECU_To_TestECU_Can_Network_Channel_CAN_Tx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"


/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
extern Com_Prv_xIpduGrpRam_tst Com_Prv_IpduGrpRam_ComIPduGroup_Tx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"



/* Begin section for RAM variables of struct/enum/pointer type */
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"
extern Com_Prv_xIpduGrpRam_tst Com_Prv_IpduGrpRam_ComIPduGroup_Rx_st;

/* End section for RAM variables of struct/enum/pointer type */
#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

#ifdef COM_F_MASKEDNEWDIFFERSOLD

#endif

#ifdef COM_F_ONEEVERYN



#endif



/*Start: Signal Buffer -----> uint8/sint8/boolean/uint8[n]*/

/* Begin section for RAM variables of uint8 type */
#define COM_START_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
extern uint8 Com_RxSigBuf_ComMainFunctionRx_au8[];

/* End section for RAM variables of uint8 type */
#define COM_STOP_SEC_VAR_CLEARED_8
#include "Com_MemMap.h"
/*End: Signal Buffer -----> uint8/sint8/boolean/uint8[n]*/





#if defined (COM_TXSIG_INT64 ) || defined(COM_RXSIG_INT64) || defined(COM_TXGRPSIG_INT64) || defined(COM_RXGRPSIG_INT64)

/* Begin section for 64-bit constants */
#define COM_START_SEC_CONFIG_DATA_64
#include "Com_MemMap.h"

extern const uint64 Com_Int64SigValues_acu64[];

/* End section for 64-bit constants */
#define COM_STOP_SEC_CONFIG_DATA_64
#include "Com_MemMap.h"
#endif


/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


#if defined (COM_TXSIG_FLOAT64SUPP ) || defined(COM_TXGRPSIG_FLOAT64SUPP) || defined(COM_RXGRPSIG_FLOAT64SUPP) || defined(COM_RXSIG_FLOAT64SUPP)
extern const float64 Com_Float64SigValues_acf64[];
#endif

extern const Com_ByteArraySig_tst Com_ByteArraySigValues_acst[];

/* Set of Rx-buffers generated for each ComMainFunctionRx */
extern const Com_Prv_xRxRamBuf_tst                          Com_Prv_xRxRamBuf_acst[];

/* Set of Tx RAM generated for each ComMainFunctionTx */
#if defined(COM_TX_SIGNALGROUP) || defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) || \
    defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
extern const Com_Prv_xTxRamBuf_tst                    Com_Prv_xTxRamBuf_acst[];
#endif /* end of #if defined(COM_TX_SIGNALGROUP) || defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) \
    || defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */


/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"



/* Begin section for 32-bit constants */
#define COM_START_SEC_CONFIG_DATA_32
#include "Com_MemMap.h"

extern const uint32                                         Com_Prv_Int32SigValues_acu32[];


/* End section for 32-bit constants */
#define COM_STOP_SEC_CONFIG_DATA_32
#include "Com_MemMap.h"





#endif   /* end of COM_PBCFG_COMMON_H */


