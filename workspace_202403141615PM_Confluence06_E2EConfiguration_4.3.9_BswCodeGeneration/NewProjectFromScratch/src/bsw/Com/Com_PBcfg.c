

 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/


/*
 * If COM_DontUseExternalSymbolicNames is defined before including Com_Cfg.h file, then external symbolic names will
 * not be visible.
 * Com_PBcfg.c file should not use external symbolic names.
 * This mechanism is used to prevent the accidental usage of external symbolic names in this file
 * This file should use only internal symbolic name defined in  Com_PBcfg_InternalSymbolicNames.h
 */
#define COM_DontUseExternalSymbolicNames
#include "Com_Prv.h"
#include "PduR_Com.h"
#include "Com_PBcfg_Common.h"
#include "Com_PBcfg_Variant.h"
/*
 * The file Com_PBcfg_InternalSymbolicNames.h defines internal symbolic names
 * These names should be used in the tables generated in this file
 * Regular symbolic names should not be used here
 */
#define COM_PBCFG_INCLUDE_INT_SYM_NAMES
#include "Com_PBcfg_InternalSymbolicNames.h"



/* START: Tx Signal Details  */

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_xTxSigCfg_tst Com_Prv_xTxSigCfg_acst[COM_NUM_TX_SIGNALS] =
{
        
    {  /* S_ApplicationECU_SG_TestResult_Tx_Can_Network_Channel_CAN_Tx */
        &Com_Prv_TxSigFlagS_ApplicationECU_SG_TestResult_Tx_Can_Network_Channel_CAN_Tx_st,
#ifdef COM_TxInvalid
        0uL,        /* DataInvalid_Val*/
#endif
#if !defined(COM_INITVALOPTIMIZATION)
        0uL,             /* Init_Val*/
#endif

        0u, /* Handle Id: ComConf_ComSignal_S_ApplicationECU_SG_TestResult_Tx_Can_Network_Channel_CAN_Tx */

        /*
        {
            TransProp       : 3;    [Bit2-Bit0] = TRIGGERED
            FilterAlgorithm : 4;    [Bit6-Bit3] = NEW_IS_WITHIN
            DataInvalidType : 1;    [Bit7]      = false
            TimeOutEnabled  : 1;    [Bit8]      = false
        } Com_TxSigPropType;
        */
        0x34,                                               /* Transmission Fields */

#ifdef COM_TxSigUpdateBit
        COM_UPDATE_MAX,                                    /*Update bit Position*/
#endif /* #ifdef COM_TxSigUpdateBit */

        0,                                                /*BitPosition*/

        8,/*BitSize*/

#ifdef COM_TxFilters
        0x00,                                                  /*Filter_Index*/
#endif

#ifdef COM_EffectiveSigTOC
        COM_TOC_OLD_VALUE_INV_IDX,
                                            /*OldVal_Index*/
#endif

        (Com_IpduId_tuo)ComIPdu_Internal_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx,             /*Ipdu Reference*/
#ifdef COM_INITVALOPTIMIZATION
        0u,                    /* Init_Val_Index*/
#endif
        /*
        {
            Type:5; [Bit4-Bit0] = SINT8
            Endianess:1; [Bit5] = LITTLE_ENDIAN
            UpdBitConf:1; [Bit6] = false
            Not_Used:1;
        } Com_GeneralType;*/
        0x1                                               /*General*/

    }

};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

/* END: Tx Signal Details  */

/* START: Rx Signal Details  */

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_xRxSigCfg_tst Com_Prv_xRxSigCfg_acst[COM_NUM_RX_SIGNALS] =
{    
        
    {  /* S_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx */
#ifdef COM_RxSignalNotify
        /* Notification Signal part */
        &Rte_COMCbk_S_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx,
#endif
#ifdef COM_RxSigInvalidNotify
        /* Com Invalid Notification */
        NULL_PTR,
#endif
        &Com_Prv_RxSigFlagS_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx_st,
#ifdef COM_RxSigInvalid
       0uL,              /* DataInvalid_Val */
#endif

        0u, /* Handle Id: ComConf_ComSignal_S_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx */

        /*
        {
            DataInvalidType:2; [Bit1-Bit0] = NONE
            FilterAlgorithm:4; [Bit5-Bit2] = COM_NOTCONFIGURED
            DataTimeoutType:2; [Bit7-Bit6] = NONE
            IsGwSignal:1; [Bit8] = false
            unused:7;
        } Com_RxSigPropType;
        */
        0x28,         /* Reception Fields */

#ifdef COM_RxSigUpdateBit
       COM_UPDATE_MAX,                    /*Update bit Position*/
#endif
       0,                         /*BitPosition*/
       0,                /* Signal Buffer Index */
       
      8, /*BitSize*/

#ifdef COM_RxFilters
        COM_MAX_U8_VALUE,                           /*Filter_Index*/
#endif

       (Com_IpduId_tuo)ComIPdu_Internal_IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx,                       /*Ipdu Reference*/
#ifdef COM_SIGNALGATEWAY
        0u, /* idxGwMap */
#endif
        0u,                   /* Init_Val_Index*/
#ifdef COM_PRV_RX_SIG_SUBSTITUTE
        0u,                  /* Substitution_Val_Index */
#endif
        /*
        {
            Type:5; [Bit4-Bit0] = SINT8
            Endianess:1; [Bit5] = LITTLE_ENDIAN
            UpdBitConf:1; [Bit6] = false
            Not_Used:1;
        } Com_GeneralType;
        */
        0x1       /*General*/
    }
    

};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


#ifdef COM_PRV_ENABLECONFIGINTERFACES

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_RxSigConfigType_tst Com_Prv_RxSigCfg_acst[COM_NUM_RX_SIGNALS] =
{
    
        
    {
        /* S_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx */
        &Com_Prv_RxSigS_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx_u8,
        0,            /*Handle Id*/
        /*
        {
            UpdBitConf:1;  = false
            Not_Used:7;
        } Com_RxSigGeneralType;
        */
        0x0       /*General*/
    }
    
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


#endif /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */

/* END: Rx Signal Details  */





#ifdef COM_PRV_ENABLECONFIGINTERFACES

#endif /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */







/* START: TMS Details  */

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

/* IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx has a TMS switch */
static const Com_TransModeInfo_tst Com_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx_TransModeInfo_acst[] =
{
    /* True Mode configuration */
    {
        10, /* TimePeriod */
        
        1, /* TimeOffset */
        
        0, /* RepetitionPeriod */
        
        0,  /* NumRepetitions */
#ifdef COM_MIXEDPHASESHIFT
        COM_TXMODE_PERIODIC, /* Mode */
        COM_FALSE    /* MixedPhaseShift status */
#else
        COM_TXMODE_PERIODIC /* Mode */
#endif /* #ifdef COM_MIXEDPHASESHIFT */
    }
,
    /* False Mode configuration */
    {
        10, /* TimePeriod */
        
        1, /* TimeOffset */
        
        0, /* RepetitionPeriod */
        
        0,  /* NumRepetitions */

#ifdef COM_MIXEDPHASESHIFT
        COM_TXMODE_PERIODIC, /* Mode */
        COM_FALSE    /* MixedPhaseShift status */
#else
        COM_TXMODE_PERIODIC /* Mode */
#endif /* #ifdef COM_MIXEDPHASESHIFT */

    }
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

/* END: TMS Details  */











/* START: Tx IPDU Details  */

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_xTxIpduInfoCfg_tst Com_Prv_xTxIpduCfg_acst[COM_NUM_TX_IPDU] =
{
    {   /*Ipdu: IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx*/

        &Com_dIP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_TxByte[0],              /*Pointer to the Ipdu Buffer*/
        
        &Com_Prv_TxIpduRam_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx_st,         /* pointer to RAM */

        Com_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx_TransModeInfo_acst,
        

        #ifdef COM_TxIPduCallOuts
        /* Ipdu Callout Function*/
        NULL_PTR,
        #endif
        #ifdef COM_TxIPduNotification
        /* Ipdu Notification Function*/

        Com_TxNotify_Wrap_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx,
        #endif

        #ifdef COM_ERRORNOTIFICATION
        /* Error Notification part */

        Com_TxErrNotify_Wrap_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx,
        #endif

        #ifdef COM_TxIPduTimeOutNotify
        /* Timeout Notification part*/

        Com_TxTONotify_Wrap_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx,
        #endif


        #ifdef COM_METADATA_SUPPORT
        NULL_PTR,               /* Pointer to the MetaData Ipdu Buffer */
        #endif

        8                                      /*Size in Bytes*/,

        #ifdef COM_TxIPduTimeOut
        10,               /*Timeout Fact*/
        #endif
        0, /*MinDelay Time factor*/

        1,                    /*No Of Signals present in the IPDU*/
        #ifdef COM_TX_SIGNALGROUP

        0,               /*No of Signal Groups present in the IPDU*/
        #endif

        PduRConf_PduRSrcPdu_ApplicationECU_TestResult_Tx_Com2PduR_Can_Network_Channel_CAN,              /* PduR Id */


        (Com_TxIntSignalId_tuo)ComSignal_Internal_S_ApplicationECU_SG_TestResult_Tx_Can_Network_Channel_CAN_Tx,     /*Index to First Signal within this Ipdu*/
        #ifdef COM_TX_SIGNALGROUP

        0,                            /*This IPDU does not contain any Signal Groups*/
        #endif

        /*
        {
            Signal_Processing:1;          [Bit0] = IMMEDIATE
            TMSCalclation:2;              [Bit2-Bit1] = MODE_VALID
            NoneModeTimeOut:1;            [Bit3] = COM_FALSE
            ClearUpdBit:2                 [Bit5-Bit4] = CLEAR_UPDATEBIT_NOT_APPLICABLE
            FilterEvalReq:1               [Bit6] = true
            IsDynPdu:1;                   [Bit7] = false
            IsGwDestPdu:1;                [Bit8] = false
            IsCalloutFrmTrigTrans:1;      [Bit9] = false
            isLargeDataPdu:1;             [Bit10] = false
            isCancelTransmitSupported:1;  [Bit11] = false
            ipduPartOfIpduGrp:1;          [Bit12] = true
            defaultTMSStatus:1;           [Bit13] = false
            Is_MetaDataPdu:1;             [Bit14] = false
            Not_Used:1;
        }Com_TxIpduFlagType;
        */
        0x1040,  /*Transmission Type*/


#if defined(COM_TxFilters) && defined(COM_IPDU_WITHOUT_IPDUGROUP_EXISTS)
        COM_MAX_TMS_COUNTER,                    /* Default TMS evaluation counter */
#endif
        
#ifdef COM_TX_IPDUCOUNTER
        COM_TXIPDU_CNTR_INV_IDX,      /* Index to TxIPduCounter */
#endif

        /* Com_MainFunctionTx_ComMainFunctionTx() */
        ComMainFunctionTx_Internal_ComMainFunctionTx,

        0xFF               /*Padding Byte*/

    }
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

/* END: Tx IPDU Details  */

/* START : Time out information structure for signals with update-bits */
#ifdef COM_RxSigUpdateTimeout

#endif /* #ifdef COM_RxSigUpdateTimeout */
/* END : Time out information structure for signals with update-bits */

/* START : Time out information structure for signal groups with update-bits */
#ifdef COM_RxSigGrpUpdateTimeout

#endif /* #ifdef COM_RxSigGrpUpdateTimeout */
/* END : Time out information structure for signal groups with update-bits */



/* START: Rx IPDU Details  */

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


const Com_Prv_xRxIpduInfoCfg_tst Com_Prv_xRxIpduCfg_acst[COM_NUM_RX_IPDU] =
{
    {   /*Ipdu: IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx*/

        &Com_dIP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_RxByte[0],              /*Pointer to the Local Ipdu Buffer*/
        
        &Com_Prv_RxIpduRam_IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx_st,         /* pointer to RAM */
        
        #ifdef COM_RxSigUpdateTimeout

        NULL_PTR,                       /* Pointer to timeout information structure for signals with update-bits, within the IPdu */
        #endif /* #ifdef COM_RxSigUpdateTimeout */
        #ifdef COM_RxSigGrpUpdateTimeout

        NULL_PTR,                       /* Pointer to timeout information structure for signal groups with update-bits, within the IPdu */
        #endif /* #ifdef COM_RxSigGrpUpdateTimeout */

        #ifdef COM_RxIPduCallOuts
        /* Ipdu Callout */

        NULL_PTR,
        #endif

        #ifdef COM_RxIPduTimeoutNotify
        /* Timeout Notification part*/

        Com_RxTONotify_Wrap_IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx,
        #endif /* COM_RxIPduTimeoutNotify */

        #ifdef COM_RxIPduNotification
        /* Ipdu Notification Function*/

        NULL_PTR,  /* Rx IPdu notification callback */
        #endif

        #ifdef COM_METADATA_SUPPORT
        NULL_PTR,               /* Pointer to the MetaData Rx Ipdu Buffer */
        #endif

        8                                  /*Size in Bytes*/,

        #ifdef COM_RxIPduTimeout

        0,        /* First time out value after IPDU group start */

        0,              /* Support Rx IPDU Timeout */
        #endif /* #ifdef COM_RxIPduTimeout */

        1,                /*No Of Signals present in the IPDU*/

        #ifdef COM_RX_SIGNALGROUP

        0,           /*No of Signal Groups present in the IPDU*/
        #endif

        (Com_RxIntSignalId_tuo)ComSignal_Internal_S_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx,   /*Index to First Signal within this Ipdu*/
         #ifdef COM_RX_SIGNALGROUP
        0,                            /*This IPDU does not contain any Signal Groups*/
        #endif
#ifdef COM_SIGNALGATEWAY
        0,       /* Number of signals with gateway */
#endif
#ifdef COM_SIGNALGROUPGATEWAY
        0,   /* Number of signal groups with gateway */
#endif

#ifdef COM_RX_IPDUCOUNTER
        COM_RXIPDU_CNTR_INV_IDX,      /* Index to RxIPduCounter */
#endif

        /* Com_MainFunctionRx_ComMainFunctionRx() */
        ComMainFunctionRx_Internal_ComMainFunctionRx,

        /*
        {
            Signal_Processing:1;  [Bit0] = IMMEDIATE
            Notify_Cbk:1;         [Bit1] = true
            IsGwIPdu:1;           [Bit2] = false
            ipduPartOfIpduGrp:1;  [Bit3] = true
            IS_TP_TYPE:1;         [Bit4] = false
            TP_INV_CFG:1;         [Bit5] = false
            Is_MetaDataPdu:1;     [Bit6] = false
            Not_Used:1;
        } Com_RxIpduFlagType;
        */
        0xA                 /* Reception Type */


    }
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

/* END: Rx IPDU Details  */

#ifdef COM_PRV_ENABLECONFIGINTERFACES

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


const Com_RxIpduCfg_tst Com_RxIpduCfg_acst[COM_NUM_RX_IPDU] =
{
    {
        /*Ipdu: IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx*/

        1u,                /*No Of Signals present in the IPDU*/

        ComSignal_Internal_S_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx,   /*Index to First Signal within this Ipdu*/
        #ifdef COM_RX_SIGNALGROUP
        0u,           /*No of Signal Groups present in the IPDU*/

        0u,                            /*This IPDU does not contain any Signal Groups*/
        #endif
    }
};


/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"



/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


const Com_GlobalConfigType_tst Com_GlobalConfig_cst =
{
    (const Com_RxIpduCfg_tst *)               &Com_RxIpduCfg_acst[0],
    (const Com_RxSigConfigType_tst *)         &Com_Prv_RxSigCfg_acst[0],


    NULL_PTR,

    0u,
    0u,
    COM_NUM_RX_IPDU,
    COM_NUM_RX_SIGNALS    /* Total no.of ComRxSignals */
#ifdef COM_RX_SIGNALGROUP
    ,COM_NUM_RX_SIGNALGRP   /* Total no.of ComRxSignalGroups */
#endif
};


/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


#endif       /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */


/* START: IPDU Group Details  */

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_Prv_xIpduGrpInfoCfg_tst Com_Prv_xIpduGrpCfg_acst[2] =
{
    /* "pointer-to-IpduGrpRam"      "Index of First IPdu"               "No of Ipdus" */

    /* ComIPduGroup_Rx */
    { &Com_Prv_IpduGrpRam_ComIPduGroup_Rx_st, 0, 1 },
    /* ComIPduGroup_Tx */
    { &Com_Prv_IpduGrpRam_ComIPduGroup_Tx_st, 1, 1 }

};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

/* END: IPDU Group Details  */

/* Reference to Ipdus belonging to the Ipdu Groups */

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


const Com_IpduId_tuo Com_Prv_xIPduGrp_IpduRefCfg_acuo[2] =
{

    /* ComIPduGroup_Rx */

    ComIPdu_Internal_IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx,
    /* ComIPduGroup_Tx */

    ComIPdu_Internal_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"








#ifdef COM_F_ONEEVERYN

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"
const Com_OneEveryN_tst Com_OneEveryN_Const_acst[1] =
{
    /* Pointer-to-Occurence  Period   Offset  */


    { NULL_PTR, 0, 0 }    /* DummyForMisra */
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif /* #ifdef COM_F_ONEEVERYN */


#if defined (COM_F_MASKEDNEWEQUALSX ) || defined(COM_F_MASKEDNEWDIFFERSX)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_MaskX_tst Com_MaskX_acst[1] =
{
    /*Mask       X*/



    { 0, 0 }    /* DummyForMisra */
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


#endif /* #if defined (COM_F_MASKEDNEWEQUALSX ) || defined(COM_F_MASKEDNEWDIFFERSX) */

#ifdef COM_F_MASKEDNEWDIFFERSOLD

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_FMaskOld_tst Com_FMaskOld_acst[1] =
{
    /* Pointer-to-FOldVal     Mask */
    { NULL_PTR, 0 }    /* DummyForMisra */
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif /* #ifdef COM_F_MASKEDNEWDIFFERSOLD */

#if defined (COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_POSMinMax_tst Com_POSMinMax_acst[1]=
{
    /*  Min      Max */





    { 0, 0 }    /* DummyForMisra */


};


/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif /* #if defined (COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS) */

#if defined (COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG)

/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_NEGMinMax_tst Com_NEGMinMax_acst[1] =
{
    /*  Min      Max */
    /* Signals */
    { 0, 0 }    /* S_ApplicationECU_SG_TestResult_Tx_Can_Network_Channel_CAN_Tx */


};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif /* #if defined (COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG) */

#ifdef COM_ENABLE_MAINFUNCTION_RX


/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_MainFuncRxCfg_tst Com_MainFuncRxCfg_acst[ COM_NUM_OF_RX_MAINFUNCTION ] =
{

    {
        /* name: ComMainFunctionRx */
# if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
        Com_MainRxFifoIdCfgComMainFunctionRx_acu16,
        COM_RX_NUMOFFIFOS_ComMainFunctionRx,
# endif
        0u,  /* Start RxIPdu-Id */
        1u  /* Num of RxIpdus */
    }
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"


#endif /* end of COM_ENABLE_MAINFUNCTION_RX */

#ifdef COM_ENABLE_MAINFUNCTION_TX


/* Begin section for constants */
#define COM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

const Com_MainFuncTxCfg_tst Com_MainFuncTxCfg_acst[ COM_NUM_OF_TX_MAINFUNCTION ] =
{

    {
        /* name: ComMainFunctionTx */
# if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
        Com_MainTxFifoIdCfgComMainFunctionTx_acu16,
        COM_TX_NUMOFFIFOS_ComMainFunctionTx,
# endif
        0u,  /* Start TxIPdu-Id */
        1u  /* Num of TxIpdus */
# ifdef COM_TXSCHEDULED_DIFF_TIMEBASE
        ,10u /* TimeBase in ms */
# endif
    }
};

/* End section for constants */
#define COM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Com_MemMap.h"

#endif /* end of COM_ENABLE_MAINFUNCTION_TX */




