/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2024, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Com_Prv.h"
#include "Com_PBcfg_Common.h"
#include "Com_PBcfg_Variant.h"
#include "PduR_Com.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
/* no matching variants found for Com module */


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_xRxIpduVarCfg_tst Com_Prv_xRxIpduVarCfg_acst[] =
{
    /* IP_TestECU_To_ApplicationECU_Can_Network_Channel_CAN_Rx */
    {
        8u,    /* PduLength */
        5   /* fields */
    }
};

const Com_Prv_xTxIpduVarCfg_tst Com_Prv_xTxIpduVarCfg_acst[] =
{
    /* IP_ApplicationECU_To_TestECU_Can_Network_Channel_CAN_Tx */
    {
        8u,    /* PduLength */
        PduRConf_PduRSrcPdu_ApplicationECU_To_TestECU_Com2PduR_Can_Network_Channel_CAN,    /* PduR Tx Handle-ID */
        0   /* fields */
    }
};


#ifdef COM_CALLBACK_RX


/* S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx */
#define Com_RxSAckCbkIdx_S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_cauo (NULL_PTR)

#define Com_RxSInvCbkIdx_S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_cauo (NULL_PTR)

#define Com_RxSToutCbkIdx_S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_cauo (NULL_PTR)


const Com_Prv_xRxSigVarCfg_tst Com_Prv_xRxSigVarCfg_acst[] =
{
    /* S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx */
    {
        Com_RxSAckCbkIdx_S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_cauo, /* ackCbk_pcauo */
        Com_RxSToutCbkIdx_S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_cauo, /* toutCbk_pcauo */
        Com_RxSInvCbkIdx_S_TestECU_To_ApplicationECU_Data_Can_Network_Channel_CAN_Rx_cauo /* invCbk_pcauo */
    }
};



# ifdef COM_RX_SIGNALGROUP
const Com_Prv_xRxSigGrpVarCfg_tst Com_Prv_xRxSigGrpVarCfg_acst[] =
{
    {
        NULL_PTR, /* ackCbk_pcauo */
        NULL_PTR, /* invCbk_pcauo */
        NULL_PTR  /* toutCbk_pcauo */
    }
};
# endif /* end of COM_RX_SIGNALGROUP */

#endif /* end of COM_CALLBACK_RX */


#ifdef COM_CALLBACK_TX
const Com_Prv_xTxSigVarCfg_tst Com_Prv_xTxSigVarCfg_acst[] =
{
    /* S_ApplicationECU_To_TestECU_Data_Can_Network_Channel_CAN_Tx */
    {
        COM_PRV_CBK_INDEX_NULL_PTR, /* idxAckCbk_uo */
        COM_PRV_CBK_INDEX_NULL_PTR, /* idxErrCbk_uo */
        COM_PRV_CBK_INDEX_NULL_PTR  /* idxToutCbk_uo */
    }
};

# ifdef COM_TX_SIGNALGROUP
const Com_Prv_xTxSigGrpVarCfg_tst Com_Prv_xTxSigGrpVarCfg_acst[] =
{
    {
        COM_PRV_CBK_INDEX_NULL_PTR, /* idxAckCbk_uo */
        COM_PRV_CBK_INDEX_NULL_PTR, /* idxErrCbk_uo */
        COM_PRV_CBK_INDEX_NULL_PTR  /* idxToutCbk_uo */
    }
};
# endif /* end of COM_TX_SIGNALGROUP */
#endif /* end of COM_CALLBACK_TX */


static const Com_VariantData_tst Com_Prv_Variant_cst =
{
    &Com_Prv_xRxIpduVarCfg_acst[0],
    &Com_Prv_xTxIpduVarCfg_acst[0]
#ifdef COM_CALLBACK_RX
    ,&Com_Prv_xRxSigVarCfg_acst[0]
# ifdef COM_RX_SIGNALGROUP
    ,&Com_Prv_xRxSigGrpVarCfg_acst[0]
# endif /* end of COM_RX_SIGNALGROUP */
#endif /* end of COM_CALLBACK_RX */
#ifdef COM_CALLBACK_TX
    ,&Com_Prv_xTxSigVarCfg_acst[0]
# ifdef COM_TX_SIGNALGROUP
    ,&Com_Prv_xTxSigGrpVarCfg_acst[0]
# endif /* end of COM_TX_SIGNALGROUP */
#endif /* end of COM_CALLBACK_TX */
};

const Com_ConfigType Com_Config =
{
#ifdef COM_PRV_ENABLECONFIGINTERFACES
    &Com_GlobalConfig_cst,
#endif
    /* MR12 DIR 1.1 VIOLATION: pointer structure types are not exported via com public header, hence it is stored as 
    void pointer and be dereferenced with correct internal type in com initialization. */
    NULL_PTR,
    &Com_Prv_Variant_cst,
    NULL_PTR
};


#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

