/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
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
#include "Com_Prv_Inl.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/
#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
# ifdef COM_ENABLE_MAINFUNCTION_RX
LOCAL_INLINE void Com_Prv_InitRxFifo(Com_MainFunc_tuo idRxMainFunc_uo);
# endif
# ifdef COM_ENABLE_MAINFUNCTION_TX
LOCAL_INLINE void Com_Prv_InitTxFifo(Com_MainFunc_tuo idTxMainFunc_uo);
# endif
#endif /* end of GATEWAY */

LOCAL_INLINE void Com_Prv_Init_Receive(void);

LOCAL_INLINE void Com_Prv_Init_RxSignal(PduIdType idIpdu_uo);

#ifdef COM_RX_SIGNALGROUP
LOCAL_INLINE void Com_Prv_Init_RxSignalGroup(PduIdType idIpdu_uo);
#endif

LOCAL_INLINE void Com_Prv_Init_Send(void);

LOCAL_INLINE void Com_Prv_Init_TxSignal(PduIdType idIpdu_uo);

#ifdef COM_TX_SIGNALGROUP
LOCAL_INLINE void Com_Prv_Init_TxSignalGroup(PduIdType idIpdu_uo);

LOCAL_INLINE void Com_Prv_Init_GroupSignal(
                                    Com_TxIpduCfg_tpcst     txIpduConstPtr_pcst,
                                    Com_TxSigGrpCfg_tpcst   txSigGrpConstPtr_pcst
                                          );
#endif /* COM_TX_SIGNALGROUP */

#ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS
LOCAL_INLINE void Com_Prv_Init_RxIpduWoGroupRef(PduIdType idIpdu_uo);
LOCAL_INLINE void Com_Prv_Init_TxIpduWoGroupRef(PduIdType idIpdu_uo);
#endif /* COM_IPDU_WITHOUT_IPDUGROUP_EXISTS */

#if defined(COM_METADATA_SUPPORT)
LOCAL_INLINE void Com_Prv_Init_RxIpduMetaData(PduIdType idIpdu_uo);
#endif

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/
#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

Com_StatusType  Com_InitStatus_en;

#ifdef COM_PRV_ENABLECONFIGINTERFACES
const Com_GlobalConfigType_tst *      Com_GlobalConfig_pcst;
#endif

#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)

/* Note: pointer variable should always be under Pre-Compile MemMap section,
 * or else it can lead to RAM corruption */
const Com_ConfigData_tst *      Com_Prv_Config_pcst;
const Com_CommonData_tst *      Com_Prv_Common_pcst;
const Com_VariantData_tst *     Com_Prv_Variant_pcst;

#endif /* end of if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE) */

#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

/*
 **********************************************************************************************************************
 Function name    : Com_Init
 Description      : Service for initializing COM module
 Parameter        : config -> Pointer to Com module's configuration data
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Init(const Com_ConfigType * config_pcst)
{

    Com_IpduGrpCfg_tpcst        ipduGrpConstPtr_pcst;
    Com_IpduGrpRam_tpst         ipduGrpRam_pst;
    uint16_least                index_qu16;

#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)

    if (config_pcst == NULL_PTR)
    {
        /* Repot to DET in case input configuration pointer is not valid */
        COM_DET_REPORT_ERROR(COMServiceId_Init, COM_E_INIT_FAILED);
        return;
    }
    else
    {

        /* MR12 RULE 11.5 VIOLATION: On assignment, target pointer will reference to the memory address, and there are
        * no side effects. Hence suppressed the warning. */
        Com_Prv_Config_pcst = (const Com_ConfigData_tst * )config_pcst->configData_pcv;
        Com_Prv_Common_pcst = (const Com_CommonData_tst * )config_pcst->commonData_pcv;
        Com_Prv_Variant_pcst = (const Com_VariantData_tst * )config_pcst->variantData_pcv;

#ifdef COM_PRV_ENABLECONFIGINTERFACES
        Com_GlobalConfig_pcst = config_pcst->globalConfigData_pcst;
#endif
    }
#else
    (void)config_pcst;
#ifdef COM_PRV_ENABLECONFIGINTERFACES
    Com_GlobalConfig_pcst = &Com_GlobalConfig_cst;
#endif
#endif /* (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE) */

    ipduGrpConstPtr_pcst = COM_GET_IPDUGRP_CONSTDATA(0);

    for (index_qu16 = COM_ZERO; index_qu16 < COM_GET_NUM_TOTAL_IPDU_GRP; index_qu16++)
    {
        ipduGrpRam_pst = ipduGrpConstPtr_pcst->ipduGroupRam_pst;

        if (ipduGrpRam_pst != NULL_PTR)
        {
            /* clear IPduGroup status flag */
            ipduGrpRam_pst->ipduGroupStatus_b = COM_STOP;

#if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout)
            /* reset the Dealine Monitoring */
            ipduGrpRam_pst->ipduGrpDMStatus_b = COM_STOP;
#endif
        }
        ipduGrpConstPtr_pcst++;
    }

    /* FC_VariationPoint_START */
#if (COM_COMMON_TX_PERIOD == STD_ON)
    /* Reset common IPDU Tx period feature, if active */
    Com_ReducedPeriod_u16 = COM_ZERO;
#endif /* COM_COMMON_TX_PERIOD */
    /* FC_VariationPoint_END */

#ifdef COM_F_ONEEVERYN
    /* Initialize Occurence to 0 */
    for (index_qu16 = COM_ZERO; index_qu16 < COM_GET_NUM_SIG_GRPSIG_WITH_ONE_EVERY_N_FILTER; index_qu16++)
    {
        (*(COM_GET_ONEEVERYN_CONST( index_qu16 ).occurrence_pu32)) = COM_ZERO;
    }
#endif /* #ifdef COM_F_ONEEVERYN */

#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
# ifdef COM_ENABLE_MAINFUNCTION_RX
    for (index_qu16 = 0u; index_qu16 < COM_NUM_OF_RX_MAINFUNCTION; ++index_qu16)
    {
        Com_Prv_InitRxFifo((Com_MainFunc_tuo) index_qu16);
    }
# endif /* end of COM_ENABLE_MAINFUNCTION_RX */

# ifdef COM_ENABLE_MAINFUNCTION_TX
    for (index_qu16 = 0u; index_qu16 < COM_NUM_OF_TX_MAINFUNCTION; ++index_qu16)
    {
        Com_Prv_InitTxFifo((Com_MainFunc_tuo) index_qu16);
    }
# endif /* end of COM_ENABLE_MAINFUNCTION_TX */
#endif /* end of defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */

    /* Initialize configured Receive Signals, Signal Groups */
    Com_Prv_Init_Receive();

    /* Initialize configured transmission Signals, Signal Groups */
    Com_Prv_Init_Send();

    /* Set Com module initialization status as complete. */
    Com_InitStatus_en = COM_INIT;
}


#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
# ifdef COM_ENABLE_MAINFUNCTION_RX
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_InitRxFifo
 Description      : Service to initialize Rx FIFO
 Parameter        : idRxMainFunc_uo - internal MainFunctionRx ID
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_InitRxFifo(Com_MainFunc_tuo idRxMainFunc_uo)
{
    const Com_MainFuncRxCfg_tst * mainFuncRxCfg_pcst;
    const uint16 * idFifo_pcu16;
    uint16_least idxFifo_u16;

    mainFuncRxCfg_pcst = COM_GET_MAINFUNCRX_CFG(idRxMainFunc_uo);
    idFifo_pcu16 = mainFuncRxCfg_pcst->idFifo_pcau16;

    for (idxFifo_u16 = mainFuncRxCfg_pcst->numOfFifo_u16; idxFifo_u16 > 0u; --idxFifo_u16)
    {
        (void)rba_BswSrv_FifoInitPush((rba_BswSrv_FifoId_t)*idFifo_pcu16);
        ++idFifo_pcu16;
    }
}
# endif /* end of COM_ENABLE_MAINFUNCTION_RX */


# ifdef COM_ENABLE_MAINFUNCTION_TX
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_InitTxFifo
 Description      : Service to initialize Tx FIFO
 Parameter        : idTxMainFunc_uo - internal MainFunctionTx ID
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_InitTxFifo(Com_MainFunc_tuo idTxMainFunc_uo)
{
    const Com_MainFuncTxCfg_tst * mainFuncTxCfg_pcst;
    const uint16 * idFifo_pcu16;
    uint16_least idxFifo_u16;

    mainFuncTxCfg_pcst = COM_GET_MAINFUNCTX_CFG(idTxMainFunc_uo);
    idFifo_pcu16 = mainFuncTxCfg_pcst->idFifo_pcau16;

    for (idxFifo_u16 = mainFuncTxCfg_pcst->numOfFifo_u16; idxFifo_u16 > 0u; --idxFifo_u16)
    {
        (void)rba_BswSrv_FifoInitPop((rba_BswSrv_FifoId_t)*idFifo_pcu16);
        ++idFifo_pcu16;
    }
}
# endif /* end of COM_ENABLE_MAINFUNCTION_TX */
#endif /* end of defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_Receive
 Description      : Service to initialize all configured signals, signalGroup at the receiver side
 Parameter        : None
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_Init_Receive(void)
{
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    uint16_least                idxIdIpdu_qu16;

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(0);

    for (idxIdIpdu_qu16 = COM_ZERO; idxIdIpdu_qu16 < COM_GET_NUM_RX_IPDU; idxIdIpdu_qu16++)
    {
        rxIpduRamPtr_pst = rxIpduConstPtr_pcst->ipduRam_pst;
        /* Initial value for all the RxIpdus should be set for IpduControl Via Rba_Nds_EcuVariant feature */
#ifdef COM_RXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT
        rxIpduRamPtr_pst->isIpduStartedViaNds_b = COM_TRUE;
#endif

        rxIpduRamPtr_pst->rxFlags_u16      = COM_ZERO;
        rxIpduRamPtr_pst->rxIPduLength_uo = COM_ZERO;

#if defined(COM_RxIPduTimeout) && defined(COM_RX_DUAL_CYCLE_SUPPPORT)
        rxIpduRamPtr_pst->overrideTimeout_u16 = COM_ZERO;
#endif

#ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS
        if (Com_GetValue(RXIPDU,_PDUWITHPDUGRP,rxIpduConstPtr_pcst->rxIPduFields_u8 ) == COM_FALSE)
        {
            /* Initialize Ipdu which does not belongs to any IpduGroup */
            Com_Prv_Init_RxIpduWoGroupRef((PduIdType)idxIdIpdu_qu16);
        }
        else
#endif /* #ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS */
        {
            rxIpduRamPtr_pst->cntrStartedIpduGrp_u8  = COM_ZERO;

# if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout)
            rxIpduRamPtr_pst->cntrStartedIpduGrpDM_u8 = COM_ZERO;
# endif
        }

        /* Call function to initialize configured reception signals */
        Com_Prv_Init_RxSignal((PduIdType)idxIdIpdu_qu16);

#ifdef COM_RX_SIGNALGROUP
        /* Call function to initialize configured reception signalgroup */
        Com_Prv_Init_RxSignalGroup((PduIdType)idxIdIpdu_qu16);
#endif  /* #ifdef COM_RX_SIGNALGROUP */

#if defined(COM_METADATA_SUPPORT)
        if (Com_GetValue(RXIPDU,_IS_METADATAPDU,rxIpduConstPtr_pcst->rxIPduFields_u8))
        {
            Com_Prv_Init_RxIpduMetaData((PduIdType)idxIdIpdu_qu16);
        }
#endif

        rxIpduConstPtr_pcst++;
    }
    /* End: Reset all Rx Falgs */
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_RxSignal
 Description      : Service to initialize all configured signal at the receiver side
 Parameter        : idIpdu_uo  -> Rx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_Init_RxSignal(PduIdType idIpdu_uo)
{
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    Com_RxSigCfg_tpcst          rxSigConstPtr_pcst;
#ifdef COM_PRV_ENABLECONFIGINTERFACES
    const Com_RxSigConfigType_tst *    rxSigCfgInterface_pcst;
#endif
    uint16_least                idxSig_qu16;
    Com_SigMax_tuo              rxSigVal_uo;

    rxIpduConstPtr_pcst     = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);

    rxSigConstPtr_pcst      = COM_GET_RXSIG_CONSTDATA(rxIpduConstPtr_pcst->idRxSig_uo);

#ifdef COM_PRV_ENABLECONFIGINTERFACES
    rxSigCfgInterface_pcst  = &(Com_GlobalConfig_pcst->rxSigCfg_pcst[rxIpduConstPtr_pcst->idRxSig_uo]);
#endif

    /* AUTOSAR COM shall initialize all signals on receiver side with the configured init values */
    for (idxSig_qu16 = rxIpduConstPtr_pcst->numOfSig_u16; idxSig_qu16 != COM_ZERO; idxSig_qu16--)
    {
        /* Reset all the flags in the rxSigRAMFields */
        rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8 = COM_ZERO;

#ifdef COM_PRV_ENABLECONFIGINTERFACES
        *(rxSigCfgInterface_pcst->rxSigRAMFields_pu8) = COM_ZERO;
#endif

        /*
        * TRACE[SWS_Com_00098] The AUTOSAR COM module shall initialize each signal of n-bit sized signal type on
        * sender and receiver side with the lower n-bits of its configuration parameter ComSignalInitValue.
        */
        rxSigVal_uo = Com_Prv_RxReplaceSignal(rxSigConstPtr_pcst, rxIpduConstPtr_pcst->idMainFunc_uo, COM_REPLACE);

#if defined(COM_RxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD)

        /* Check the filter algorithm */
        if (Com_GetValue(RXSIG,_FILTRALG,rxSigConstPtr_pcst->rxSignalFields_u16) == COM_MASKED_NEW_DIFFERS_MASKED_OLD)
        {
            /* TRACE[SWS_Com_00603] The AUTOSAR COM module shall set the old_value of the filtering mechanisms for
            * each signal to the ComSignalInitValue during start-up. */
            COM_GET_F_OLDVALUE(rxSigConstPtr_pcst->idxFilter_uo) = (uint32)rxSigVal_uo;
        }
#endif /* #if defined(COM_RxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD) */

        /* conditionally used variable(s) is type-cast to 'void' to avoid warnings,
         * they are intentionally not placed under compiler switch statement,
         * so that these lines are always available for compilation*/
        (void)rxSigVal_uo;

        rxSigConstPtr_pcst++;
#ifdef COM_PRV_ENABLECONFIGINTERFACES
        rxSigCfgInterface_pcst++;
#endif
    }
}


#ifdef COM_RX_SIGNALGROUP

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_RxSignalGroup
 Description      : Service to initialize all configured signal group at the receiver side
 Parameter        : idIpdu_uo  -> Rx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_Init_RxSignalGroup(PduIdType idIpdu_uo)
{
    Com_RxIpduCfg_tpcst             rxIpduConstPtr_pcst;
    Com_RxSigGrpCfg_tpcst           rxSigGrpConstPtr_pcst;
#ifdef COM_PRV_ENABLECONFIGINTERFACES
    const Com_RxSigGrpConfigType_tst * rxSigGrpCfgInterface_pcst;
#endif
    uint16_least                    idxSigGrp_qu16;
    Com_MainFunc_tuo                idRxMainFunc_uo;

    rxIpduConstPtr_pcst     = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);
    rxSigGrpConstPtr_pcst   = COM_GET_RXSIGGRP_CONSTDATA(rxIpduConstPtr_pcst->idFirstRxSigGrp_uo);
    idRxMainFunc_uo         = rxIpduConstPtr_pcst->idMainFunc_uo;

#ifdef COM_PRV_ENABLECONFIGINTERFACES
    rxSigGrpCfgInterface_pcst = &(Com_GlobalConfig_pcst->rxSigGrpCfg_pcst[rxIpduConstPtr_pcst->idFirstRxSigGrp_uo]);
#endif

    /* AUTOSAR COM shall initialize all Rx-SignalGroup Array(if any) and Secondary GroupSignal Buffers
    * on receiver side with the configured init values */
    for (idxSigGrp_qu16 = rxIpduConstPtr_pcst->numOfSigGrp_u16; idxSigGrp_qu16 != COM_ZERO; idxSigGrp_qu16--)
    {
        /**
        * TRACE[SWS_Com_00098] The AUTOSAR COM module shall initialize each signal of n-bit sized signal type on
        * sender and receiver side with the lower n-bits of its configuration parameter ComSignalInitValue.
        */
        Com_Prv_RxReplaceSigGrp( rxSigGrpConstPtr_pcst, idRxMainFunc_uo, COM_REPLACE );

        rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8 = COM_ZERO;

#ifdef COM_PRV_ENABLECONFIGINTERFACES
        *(rxSigGrpCfgInterface_pcst->rxSigGrpRAMFields_pu8) = COM_ZERO;
#endif

        rxSigGrpConstPtr_pcst++;
#ifdef COM_PRV_ENABLECONFIGINTERFACES
        rxSigGrpCfgInterface_pcst++;
#endif
    }
}

#endif /* #ifdef COM_RX_SIGNALGROUP */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_Send
 Description      : Service to initialize all configured signals, signalGroup at the sender side
 Parameter        : None
 Return value     : None
 **********************************************************************************************************************
*/

LOCAL_INLINE void Com_Prv_Init_Send(void)
{
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    uint16_least                idxIdIpdu_qu16;

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(0);

    for (idxIdIpdu_qu16 = COM_ZERO; idxIdIpdu_qu16 < COM_GET_NUM_TX_IPDU; idxIdIpdu_qu16++)
    {
        txIpduRamPtr_pst = txIpduConstPtr_pcst->ipduRam_pst;

        /*Reset all the Tx Flags*/
        txIpduRamPtr_pst ->txFlags_u16      = COM_ZERO;

        /* Initial value for all the TxIpdus should be set for IpduControl Via Rba_Nds_EcuVariant feature */
#ifdef COM_TXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT
        txIpduRamPtr_pst->isIpduStartedViaNds_b = COM_TRUE;
#endif

        /**
        * Dynamic length is initialized in Signal/GroupSignal function accordingly,
        * hence those respective RAM members are not intialized here.
        * i.e., dynLength_u16, dynTxGrpSigLength_u16
        */
#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
        txIpduRamPtr_pst->dynTxGrpSigLength_u16 = COM_ZERO;
#endif
        txIpduRamPtr_pst->currentTxModePtr_pcst  = txIpduConstPtr_pcst->tMConstPtr_pcst;

        /**
        * TRACE[SWS_Com_00015] The AUTOSAR COM module shall fill not used areas within an
        * I-PDU with a value determined by configuration parameter ComTxIPduUnusedAreasDefault.
        * TRACE[SWS_Com_00217] The AUTOSAR COM module shall initialize each I-PDU during
        * execution of Com_Init, firstly byte wise with the ComTxIPduUnusedAreasDefault value.
        */
        rba_BswSrv_MemSet8(txIpduConstPtr_pcst->buffPtr_pu8, txIpduConstPtr_pcst->paddingByte_u8,
                COM_GET_TXIPDU_SIZE(idxIdIpdu_qu16));

        txIpduRamPtr_pst->cntrStartedIpduGrp_u8 = COM_ZERO;

        /* Set the TxIPdu-Mode to most probable case */
        Com_SetRamValue(TXIPDU,_TXMODESTATE,txIpduRamPtr_pst->transMode_u8,COM_TRUE);

        Com_SetRamValue(TXIPDU,_LATESTMODE,txIpduRamPtr_pst->transMode_u8,COM_TXMODE_NONE);
        /* FC_VariationPoint_START */
#ifdef COM_MIXEDPHASESHIFT
        /* Disable MixedPhaseShift by default */
        Com_SetRamValue(TXIPDU,_MIXEDPHASESHIFT,txIpduRamPtr_pst->transMode_u8,COM_FALSE);
#endif /* #ifdef COM_MIXEDPHASESHIFT */
        /* FC_VariationPoint_END */

        /*Reset all other Tx Global variables including Min Delays*/
#ifdef COM_TxIPduTimeOut
        txIpduRamPtr_pst->cntrTxTimeout_u16         = COM_ZERO;
#endif /* #ifdef COM_TxIPduTimeOut */
#ifdef COM_TxFilters
        txIpduRamPtr_pst->cntrTrueTMC_u16           = COM_ZERO;
#endif /* #ifdef COM_TxFilters */
        txIpduRamPtr_pst->cntrRepetitions_u8        = COM_ZERO;

        txIpduRamPtr_pst->cntrMinDelayTime_u16      = COM_ZERO; /*Minimum Delay Tick for the Tx IPDU's */

        /* Call function to initialize configured transmission signals */
        Com_Prv_Init_TxSignal((PduIdType)idxIdIpdu_qu16);

#ifdef COM_TX_SIGNALGROUP
        /* Call function to initialize configured transmission signalgroup */
        Com_Prv_Init_TxSignalGroup((PduIdType)idxIdIpdu_qu16);
#endif

#ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS
        /* Start: Reset all Tx Flags */
        if (Com_GetValue(TXIPDU,_PDUWITHPDUGRP,txIpduConstPtr_pcst->txIPduFields_u16) == COM_FALSE)
        {
            /* Call function to initialize Ipdu which does not belongs to any IpduGroup */
            Com_Prv_Init_TxIpduWoGroupRef((PduIdType) idxIdIpdu_qu16);
        }
#endif

#ifdef COM_TX_IPDUCOUNTER
        /* Check if IPduCounter is configured */
        if (txIpduConstPtr_pcst->idxTxIpduCntr_uo != COM_TXIPDU_CNTR_INV_IDX)
        {
            /* TRACE[SWS_Com_00687] The AUTOSAR COM module shall set the I-PDU counter to 0, after the I-PDU was
            * initialized by Com_Init or reinitialized by Com_IpduGroupStart. */
            Com_Prv_TxIpduInitializeCounter( (PduIdType)idxIdIpdu_qu16 );
        }
        else
        {
            /* do nothing: counter is not configured */
        }
#endif /* #ifdef COM_TX_IPDUCOUNTER */

        txIpduConstPtr_pcst++;

    } /* end of for loop */
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_TxSignal
 Description      : Service to initialize all configured signals at the sender side
 Parameter        : idIpdu_uo -> Tx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_Init_TxSignal(PduIdType idIpdu_uo)
{

    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    Com_TxSigCfg_tpcst          txSigConstPtr_pcst;
    Com_SigMax_tuo              txSigNewVal_uo;
    uint16_least                idxSig_qu16;
    PduLengthType               byteNo_uo;
#if (defined(COM_TxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD)) || defined(COM_EffectiveSigTOC)
    uint16                      constByteValue_u16;
#endif
#ifdef COM_EffectiveSigTOC
    uint16                      txSigTp_u16;
    Com_MainFunc_tuo            idTxMainFunc_uo;
#endif
    uint8                       constByteValue_u8;
    uint8                       type_u8;

    txIpduConstPtr_pcst      = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);

#ifdef COM_EffectiveSigTOC
    idTxMainFunc_uo          = txIpduConstPtr_pcst->idMainFunc_uo;
#endif

    /* AUTOSAR COM shall initialize all signals on sender side with
    the configured init values */
    txSigConstPtr_pcst       = COM_GET_TXSIG_CONSTDATA(txIpduConstPtr_pcst->idTxSig_uo);

    for (idxSig_qu16 = txIpduConstPtr_pcst->numOfSig_u16; idxSig_qu16 != COM_ZERO; idxSig_qu16--)
    {
        txSigConstPtr_pcst->sigRAM_pst->txSigRAMFields_u8 = COM_ZERO;

        constByteValue_u8    = txSigConstPtr_pcst->general_u8;

#if (defined(COM_TxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD)) || defined(COM_EffectiveSigTOC)
        constByteValue_u16   = txSigConstPtr_pcst->txSignalFields_u16;
#endif

        type_u8              = Com_GetValue(GEN,_TYPE,constByteValue_u8);

        /* Sign extension to be performed on Init value
        * But when a signed value is assigned to a uint32 variable,it is sign extended by default*/
        txSigNewVal_uo       = Com_Prv_GetTxSigInitValue(txSigConstPtr_pcst);

        /**
        * TRACE[SWS_Com_00217] The AUTOSAR COM module shall initialize each I-PDU during
        * execution of Com_Init, firstly byte wise with the
        * ComTxIPduUnusedAreasDefault value and then bit wise according to initial values
        * (ComSignalInitValue) of the contained signals and the update-bits
        */
        if (type_u8 == COM_UINT8_N)
        {
            byteNo_uo = (txSigConstPtr_pcst->bitPos_uo >> 3u);

            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". As below mem-copy is
            * optimized operation to copy data, so, type cast warning is ignored here */
            COM_MEMCOPY((txIpduConstPtr_pcst->buffPtr_pu8 + byteNo_uo),
                         COM_GET_BYTE_ARRAY_SIG_VAL(txSigNewVal_uo).sigByteArray_pau8,
                         txSigConstPtr_pcst->bitSize_uo);

        }
        else
#ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT
        if (type_u8 == COM_UINT8_DYN)
        {
            PduLengthType dynLength_uo;
            dynLength_uo = COM_GET_BYTE_ARRAY_SIG_VAL(txSigNewVal_uo).sigLength_uo;

            byteNo_uo = (txSigConstPtr_pcst->bitPos_uo >> 3u);

            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". As below mem-copy is
            * optimized operation to copy data, so, type cast warning is ignored here */
            COM_MEMCOPY((txIpduConstPtr_pcst->buffPtr_pu8 + byteNo_uo),
                         COM_GET_BYTE_ARRAY_SIG_VAL(txSigNewVal_uo).sigByteArray_pau8,
                         dynLength_uo);

            txIpduConstPtr_pcst->ipduRam_pst->dynLength_u16 = (uint16)dynLength_uo;
        }
        else
#endif /* end of COM_TX_DYNAMIC_SIGNAL_SUPPORT */
        {
            uint8 sigEndianess_u8;

            sigEndianess_u8 = Com_GetValue(GEN,_ENDIANESS,constByteValue_u8);

#ifdef COM_TXSIG_FLOAT64SUPP
            if (type_u8 == COM_FLOAT64)
            {
                Com_Prv_PackFloatSignal( sigEndianess_u8, txSigConstPtr_pcst->bitPos_uo,
                                         Com_getfloat64SigValue(txSigNewVal_uo), txIpduConstPtr_pcst->buffPtr_pu8 );
            }
            else
#endif
            {
#ifdef COM_TXSIG_INT64
                if (COM_SIG_IS_INT64_TYPE(type_u8))
                {
                    txSigNewVal_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(txSigNewVal_uo));
                }
                else
#endif /* end of COM_TXSIG_INT64 */
                {
#ifdef  COM_INITVALOPTIMIZATION
                    txSigNewVal_uo = COM_GET_INT32_SIG_VALUE(txSigNewVal_uo);
#endif
                }

                Com_Prv_PackSignal( sigEndianess_u8, txSigConstPtr_pcst->bitPos_uo, txSigConstPtr_pcst->bitSize_uo,
                                    txSigNewVal_uo, txIpduConstPtr_pcst->buffPtr_pu8 );

#if (defined(COM_TxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD))
                if (Com_GetValue(TXSIG,_FILTRALG,constByteValue_u16) == (uint16)COM_MASKED_NEW_DIFFERS_MASKED_OLD)
                {
                    /* TRACE[SWS_Com_00603] The AUTOSAR COM module shall set the old_value of the filtering
                    * mechanisms for each signal to the ComSignalInitValue during start-up. */
                    COM_GET_F_OLDVALUE(txSigConstPtr_pcst->idxFilter_uo) = (uint32)txSigNewVal_uo;
                }
#endif /* #if (defined(COM_TxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD)) */

#ifdef COM_EffectiveSigTOC
                /* Get Signal Transfer Property */
                txSigTp_u16 = Com_GetValue(TXSIG,_TP,constByteValue_u16);

                /* Assign Init value as Old Value, if Signal's Transfer property is TriggeredOnChange or
                * TriggeredOnChangeWithoutRepetition */
                if (COM_SIG_TP_IS_TRIGGERED_ON_CHANGE(txSigTp_u16) &&
                   (txSigConstPtr_pcst->idxOldValue_uo != COM_TOC_OLD_VALUE_INV_IDX))
                {
                    COM_GET_OLDVALTOC(idTxMainFunc_uo, txSigConstPtr_pcst->idxOldValue_uo) =
                            (Com_OldValTOC_tuo)txSigNewVal_uo;
                }
#endif /* #ifdef COM_EffectiveSigTOC */
            }
        }

#ifdef COM_TxSigUpdateBit

        /* is update bit configured */
        if (Com_GetValue(GEN,_UPDBITCONF,constByteValue_u8))
        {
            /*Find the Byte No*/
            byteNo_uo = (PduLengthType)(txSigConstPtr_pcst->updateBitPos_uo >> 3u);

            /* TRACE[SWS_Com_00117] The AUTOSAR COM module shall clear all update-bits during initialization. */
            Com_ClearUpdateBitValue(txIpduConstPtr_pcst, txSigConstPtr_pcst->updateBitPos_uo, byteNo_uo)
        }
#endif/*COM_TxSigUpdateBit*/

        txSigConstPtr_pcst++;
    }
}


#ifdef COM_TX_SIGNALGROUP
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_TxSignalGroup
 Description      : Service to initialize all configured signal Group at the sender side
 Parameter        : idIpdu_uo  -> Tx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_Init_TxSignalGroup(PduIdType idIpdu_uo)
{
    Com_TxIpduCfg_tpcst             txIpduConstPtr_pcst;
    Com_TxSigGrpCfg_tpcst           txSigGrpConstPtr_pcst;
    uint16_least                    idxIdSigGrp_qu16;
    uint16_least                    idFirstSigGrp_qu16;
    uint16_least                    idMaxSigGrp_qu16;
#ifdef COM_TxSigGrpUpdateBit
    PduLengthType                   byteNo_uo;
#endif

    txIpduConstPtr_pcst   = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);

    idFirstSigGrp_qu16    = txIpduConstPtr_pcst->idFirstTxSigGrp_uo;
    idMaxSigGrp_qu16      = idFirstSigGrp_qu16 + txIpduConstPtr_pcst->numOfSigGroups_u16;

    txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(idFirstSigGrp_qu16);

    for (idxIdSigGrp_qu16 = idFirstSigGrp_qu16; idxIdSigGrp_qu16 < idMaxSigGrp_qu16; idxIdSigGrp_qu16++)
    {
        /* Call function to initialize configured transmission group signals */
        Com_Prv_Init_GroupSignal(txIpduConstPtr_pcst, txSigGrpConstPtr_pcst);

#if defined(COM_EffectiveSigGrpTOC) || defined (COM_SIGNALGROUPGATEWAY)
        txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8 = COM_ZERO;
#endif

        /**
        * TRACE[SWS_Com_00217] The AUTOSAR COM module shall initialize each I-PDU during
        * execution of Com_Init, firstly byte wise with the
        * ComTxIPduUnusedAreasDefault value and then bit wise according to initial values
        * (ComSignalInitValue) of the contained signals and the update-bits
        */
#ifdef COM_TX_SIGNALGROUP_ARRAY
        /* Check if SignalGroupArray is enabled to copy the data to IPDU buffer else, do nothing
        * GroupSignal's SignalInitValues are already updated into the IPduBuffer in
        * Com_Prv_Init_GroupSignal() */
        if (Com_GetValue(TXSIGGRP,_ARRAYACCESS,txSigGrpConstPtr_pcst->txSignalGrpFields_u8) == COM_FALSE)
#endif /* #ifdef COM_TX_SIGNALGROUP_ARRAY */
        {
            /* Copy the data which is present in Shadow buffer to IPDU buffer */
            Com_Prv_ShadowBuffToIpduBuff((Com_SignalGroupIdType)idxIdSigGrp_qu16);
        }

#ifdef COM_TxSigGrpUpdateBit

        /* is update bit configured */
        if (Com_GetValue(TXSIGGRP,_UPDBITCONF,txSigGrpConstPtr_pcst->txSignalGrpFields_u8))
        {
            /*Find the Byte No*/
            byteNo_uo = (PduLengthType)(txSigGrpConstPtr_pcst->updateBitPos_uo >> 3u);

            /* TRACE[SWS_Com_00117] The AUTOSAR COM module shall clear all update-bits during initialization. */
            Com_ClearUpdateBitValue(txIpduConstPtr_pcst, txSigGrpConstPtr_pcst->updateBitPos_uo, byteNo_uo)
        }
#endif  /*COM_TxSigGrpUpdateBit*/
        txSigGrpConstPtr_pcst++;
    }
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_GroupSignal
 Description      : Service to initialize all configured Groupsignal at the sender side
 Parameter        : txIpduConstPtr_pcst      -> Address to Tx IPdu
                    txSigGrpConstPtr_pcst    -> Address to SignalGroup
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_Init_GroupSignal(Com_TxIpduCfg_tpcst      txIpduConstPtr_pcst,
                                           Com_TxSigGrpCfg_tpcst    txSigGrpConstPtr_pcst)
{
    /* Variable declaration */
    Com_TxGrpSigCfg_tpcst           txGrpSigConstPtr_pcst;
    Com_SigMax_tuo                  txGrpSigNewVal_uo;
    uint16_least                    idxIdGrpSig_qu16;
    uint16_least                    idMaxGrpSig_qu16;
    uint16                          constByteValue_u16;
    uint8                           type_u8;
    Com_MainFunc_tuo                idTxMainFunc_uo;
#ifdef COM_TX_SIGNALGROUP_ARRAY
    boolean                         isSigGrpArrayAccess_b;
    isSigGrpArrayAccess_b = Com_GetValue(TXSIGGRP,_ARRAYACCESS,txSigGrpConstPtr_pcst->txSignalGrpFields_u8);
#endif /* #ifdef COM_TX_SIGNALGROUP_ARRAY */

    idTxMainFunc_uo     = txIpduConstPtr_pcst->idMainFunc_uo;

    idxIdGrpSig_qu16    = txSigGrpConstPtr_pcst->idFirstGrpSig_uo;
    idMaxGrpSig_qu16    = idxIdGrpSig_qu16 + txSigGrpConstPtr_pcst->numOfGrpSig_uo;

    txGrpSigConstPtr_pcst    = COM_GET_TXGRPSIG_CONSTDATA(idxIdGrpSig_qu16);

    for ( ; idxIdGrpSig_qu16 < idMaxGrpSig_qu16; idxIdGrpSig_qu16++)
    {
        /* Shadowbuffer of a signal Group(group signal buffers) on sender-side shall be initialized*/
        /* Sign extension to be performed on Init valu, when a signed value is assigned to a uint32 variable,
        * it is sign extended by default*/
        txGrpSigNewVal_uo = Com_Prv_GetTxGrpSigInitValue(txGrpSigConstPtr_pcst);

        /* Get the value of the ComSignaltype bit field */
        constByteValue_u16   = txGrpSigConstPtr_pcst->txGrpSigFields_u8;
        /* Get type of the signal */
        type_u8 = (uint8) Com_GetValue(TXGRPSIG,_TYPE,constByteValue_u16);

#ifdef COM_TX_SIGNALGROUP_ARRAY
        if ( isSigGrpArrayAccess_b )
        {
            /* COM_UINT8_DYN is not handled here, as it cannot be configured in Tx-ComSignalGroup-with-ArrayAccess */

            if (type_u8 == COM_UINT8_N)
            {
                PduLengthType startByteNo_uo;

                startByteNo_uo = ( PduLengthType )( txGrpSigConstPtr_pcst->bitPos_uo >> 3u );

                /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". As below mem-copy
                *  is optimized operation to copy data, so, type cast warning is ignored here */
                COM_MEMCOPY((txIpduConstPtr_pcst->buffPtr_pu8 + startByteNo_uo),
                             COM_GET_BYTE_ARRAY_SIG_VAL(txGrpSigNewVal_uo).sigByteArray_pau8,
                             txGrpSigConstPtr_pcst->bitSize_uo
                           );

            }
            else
            {
                uint8 sigEndianess_u8;

                sigEndianess_u8 = Com_GetValue(TXGRPSIG,_ENDIANESS,constByteValue_u16);

#ifdef COM_TXGRPSIG_FLOAT64SUPP
                if (type_u8 == COM_FLOAT64)
                {
                    Com_Prv_PackFloatSignal( sigEndianess_u8, txGrpSigConstPtr_pcst->bitPos_uo,
                                             Com_getfloat64SigValue(txGrpSigNewVal_uo),
                                             txIpduConstPtr_pcst->buffPtr_pu8 );
                }
                else
#endif /* #ifdef COM_TXGRPSIG_FLOAT64SUPP */
                {
#ifdef COM_TXGRPSIG_INT64
                    if (COM_SIG_IS_INT64_TYPE(type_u8))
                    {
                        txGrpSigNewVal_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(txGrpSigNewVal_uo));
                    }
                    else
#endif /* end of COM_TXGRPSIG_INT64 */
                    {
#ifdef  COM_INITVALOPTIMIZATION
                        txGrpSigNewVal_uo = COM_GET_INT32_SIG_VALUE(txGrpSigNewVal_uo);
#endif
                    }

                    Com_Prv_PackSignal( sigEndianess_u8, txGrpSigConstPtr_pcst->bitPos_uo,
                                        txGrpSigConstPtr_pcst->bitSize_uo, txGrpSigNewVal_uo,
                                        txIpduConstPtr_pcst->buffPtr_pu8 );
                }
            }
        }
        else
#endif /* #ifdef COM_TX_SIGNALGROUP_ARRAY */
        {
            switch (type_u8)
            {
                case COM_UINT8_N:
                {
                    /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void".
                    * As below mem-copy is optimized operation to copy data, so, type cast warning is ignored here */
                    COM_MEMCOPY(
                        &COM_GET_TXGRPSIGTYPEU8_BUFF(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo),
                        COM_GET_BYTE_ARRAY_SIG_VAL(txGrpSigNewVal_uo).sigByteArray_pau8,
                        txGrpSigConstPtr_pcst->bitSize_uo
                               );

                }
                break;
#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
                case COM_UINT8_DYN:
                {
                    PduLengthType dynLength_uo;
                    dynLength_uo = COM_GET_BYTE_ARRAY_SIG_VAL(txGrpSigNewVal_uo).sigLength_uo;

                    /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void".
                    * As below mem-copy is optimized operation to copy data, so, type cast warning is ignored here */
                    COM_MEMCOPY(
                        &COM_GET_TXGRPSIGTYPEUINT8DYN_BUFF(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo),
                        COM_GET_BYTE_ARRAY_SIG_VAL(txGrpSigNewVal_uo).sigByteArray_pau8, dynLength_uo
                               );

                    txIpduConstPtr_pcst->ipduRam_pst->dynTxGrpSigLength_u16 = (uint16)dynLength_uo;
                }
                break;
#endif /* end of COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */
#ifdef COM_TXGRPSIG_FLOAT64SUPP
                case COM_FLOAT64:
                {
                    /* FLOAT64 signal */
                    /* For float64 signals, TxGrpSig_NewVal holds index to float array holding init values.*/
                    COM_GET_TXGRPSIGTYPEFLOAT64_BUFF(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo) =
                                                                             Com_getfloat64SigValue(txGrpSigNewVal_uo);
                }
                break;
#endif /* #ifdef COM_TXGRPSIG_FLOAT64SUPP */
                default:
                {
#ifdef COM_TXGRPSIG_INT64
                    if (COM_SIG_IS_INT64_TYPE(type_u8))
                    {
                        txGrpSigNewVal_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(txGrpSigNewVal_uo));
                    }
                    else
#endif /* end of COM_TXGRPSIG_INT64 */
                    {
#ifdef  COM_INITVALOPTIMIZATION
                        txGrpSigNewVal_uo = COM_GET_INT32_SIG_VALUE(txGrpSigNewVal_uo);
#endif
                    }

                    Com_Prv_CopyTxGroupSignalData( (Com_SignalIdType)(idxIdGrpSig_qu16),
                                                    txGrpSigNewVal_uo, idTxMainFunc_uo );
                }
                break;
            }
        }

#if (defined(COM_TxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD))
        if (txGrpSigConstPtr_pcst->filterAlgo_u8 == COM_MASKED_NEW_DIFFERS_MASKED_OLD)
        {
            /* TRACE[SWS_Com_00603] The AUTOSAR COM module shall set the old_value of the filtering mechanisms for
            * each signal to the ComSignalInitValue during start-up. */
            COM_GET_F_OLDVALUE(txGrpSigConstPtr_pcst->idxFilter_uo) = (uint32)txGrpSigNewVal_uo;
        }
#endif /* #if (defined(COM_TxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD)) */

#ifdef COM_EffectiveSigGrpTOC
        /* Get transfer property of the group signal */
        if (Com_GetValue(TXGRPSIG,_IS_TOC,constByteValue_u16) &&
           (txGrpSigConstPtr_pcst->idxOldValue_uo != COM_TOC_OLD_VALUE_INV_IDX))
        {
            COM_GET_OLDVALTOC(idTxMainFunc_uo, (txGrpSigConstPtr_pcst->idxOldValue_uo)) =
                    (Com_OldValTOC_tuo)txGrpSigNewVal_uo;
        }
#endif /* #ifdef COM_EffectiveSigGrpTOC */

        txGrpSigConstPtr_pcst++;
    }
}
#endif  /* end of COM_TX_SIGNALGROUP */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_RxIpduWoGroupRef
 Description      : Service for initializing COM module for the reception
 Parameter        : idIpdu_uo -> Rx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
#ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS
LOCAL_INLINE void Com_Prv_Init_RxIpduWoGroupRef(PduIdType idIpdu_uo)
{
    Com_RxIpduRam_tpst        rxIpduRamPtr_pst;

    rxIpduRamPtr_pst = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst;

    /* TRACE[SWS_Com_00840] If an I-PDU is not assigned to any I-PDU group, the AUTOSAR COM shall start this I-PDU
    * within Com_Init, such an I-PDU can never be stopped */
    rxIpduRamPtr_pst->cntrStartedIpduGrp_u8 = COM_ONE;

    Com_SetRamValue(RXIPDU,_PDUSTATUS,rxIpduRamPtr_pst->rxFlags_u16,COM_START);

#if defined(COM_RxIPduTimeout) || defined(COM_RxSigUpdateTimeout) || defined(COM_RxSigGrpUpdateTimeout)
    /* TRACE[SWS_Com_00224] If the reception deadline monitoring state of an I-PDU is enabled(if an I-PDU is not
    * assigned to any I-PDU group), the AUTOSAR COM module shall set the reception deadline monitoring timer for the
    * included signals and signal groups to the configured ComFirstTimeout value */
    if (Com_Prv_EnableRxDeadlineMonitoring((Com_IpduId_tuo)idIpdu_uo))
    {
        /* TRACE[SWS_Com_00772] The reception deadline monitoring of an I PDU is enabled,
        * if an I-PDU is not assigned to any I-PDU group */
        rxIpduRamPtr_pst->cntrStartedIpduGrpDM_u8 = COM_ONE;

        Com_SetRamValue(RXIPDU,_DMSTATUS,rxIpduRamPtr_pst->rxFlags_u16,COM_START);
    }
    else
    {
        rxIpduRamPtr_pst->cntrStartedIpduGrpDM_u8 = COM_ZERO;
    }

#endif /* #if defined(COM_RxIPduTimeout) || defined(COM_RxSigUpdateTimeout) || defined(COM_RxSigGrpUpdateTimeout) */

#ifdef COM_RX_IPDUCOUNTER
    /* Set the flag until first successful reception of an Rx IPDU with counter */
    Com_SetRamValue(RXIPDU,_IS_FIRST_RECEPTION,rxIpduRamPtr_pst->rxFlags_u16,COM_TRUE);
#endif
}
#endif /* #ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_TxIpduWoGroupRef
 Description      : Service for initializing COM module for the transmission
 Parameter        : idIpdu_uo -> Tx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
#ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS
LOCAL_INLINE void Com_Prv_Init_TxIpduWoGroupRef(PduIdType idIpdu_uo)
{
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    uint8                       latestTransMode_u8;
    boolean                     defaultTMSStatus_b;

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);

    txIpduRamPtr_pst    = txIpduConstPtr_pcst->ipduRam_pst;

    /* Fetch the default TMS status which is evaluated during the code generation time
    * for the ComIpdu's which are not part of any ComIpduGroup  */
    defaultTMSStatus_b = Com_GetValue(TXIPDU,_DEFAULTTMSSTATUS,txIpduConstPtr_pcst->txIPduFields_u16);

    /* TRACE[SWS_Com_00840] If an I-PDU is not assigned to any I-PDU group, the I-PDU shall be initially started
    * (and never been stopped) */
    txIpduRamPtr_pst->cntrStartedIpduGrp_u8 = COM_ONE;

    Com_SetRamValue(TXIPDU,_PDUSTATUS,txIpduRamPtr_pst->txFlags_u16,COM_START);

    /* TRACE[SWS_Com_00877]: If an I-PDU is not part of any I-PDU Group, it is started during the initialization of
    * COM. Its starting transmission mode shall be evaluated according to the
    * ComSignalInitValue of the signals contributing to its TMS.
    * Set the Tx Mode state as DefaultTMSStatus. */
    Com_SetRamValue(TXIPDU,_TXMODESTATE,txIpduRamPtr_pst->transMode_u8,defaultTMSStatus_b);
    /* Set accordingly the TxModeMode as per DefaultTMSStatus */
    Com_Prv_SetCurrentTxModePtr(txIpduRamPtr_pst,txIpduConstPtr_pcst,defaultTMSStatus_b);
    /* Set the latest mode as per current mode */
    latestTransMode_u8 = txIpduRamPtr_pst->currentTxModePtr_pcst->mode_u8;
    Com_SetRamValue(TXIPDU,_LATESTMODE,txIpduRamPtr_pst->transMode_u8,latestTransMode_u8);

#ifdef COM_TxFilters
    if (txIpduConstPtr_pcst->TxTMSEvaluateCounter != COM_MAX_TMS_COUNTER)
    {
        /* Load TMS evaluation counter for I-PDU without ComIPduGroupRef */
        txIpduRamPtr_pst->cntrTrueTMC_u16 = txIpduConstPtr_pcst->TxTMSEvaluateCounter;
    }
#endif

    if (COM_TX_MODE_IS_CYCLIC(latestTransMode_u8))
    {
        /* I.e Load the TxTimeOffset for cyclic type Pdu's(PERIODIC or MIXED) */
        txIpduRamPtr_pst->cntrTimePeriod_u16 = COM_GET_TXIPDU_TIMEOFFSET(idIpdu_uo,
                                                                         txIpduRamPtr_pst->currentTxModePtr_pcst);
#ifdef COM_MIXEDPHASESHIFT
        /* Set the MixedPhaseShift Mode */
        Com_SetRamValue(TXIPDU,_MIXEDPHASESHIFT,txIpduRamPtr_pst->transMode_u8,
                        txIpduRamPtr_pst->currentTxModePtr_pcst->mixedPhaseShift_b);
#endif /* #ifdef COM_MIXEDPHASESHIFT */
    }
}
#endif /* #ifdef COM_IPDU_WITHOUT_IPDUGROUP_EXISTS */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_Init_RxIpduMetaData
 Description      : Service to initialize metadata buffer
 Parameter        : idIpdu_uo  -> Rx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
#if defined(COM_METADATA_SUPPORT)

LOCAL_INLINE void Com_Prv_Init_RxIpduMetaData(PduIdType idIpdu_uo)
{
    rba_BswSrv_MemSet8(COM_GET_RXMETADATA(idIpdu_uo)->rxMetaDataValue_pau8, COM_ZERO,
                                      COM_GET_RXMETADATA(idIpdu_uo)->rxMetaDataLength_u8);
}
#endif

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

