


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/

#if !defined(COM_CFG_INTERNAL_H)
#define COM_CFG_INTERNAL_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Com_Cfg_Cbk.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* definition of supported module variant  */

#define COM_VARIANT_PRE_COMPILE                   0

#define COM_VARIANT_LINK_TIME                     1

#define COM_VARIANT_POSTBUILD_LOADABLE            2

#define COM_VARIANT_POSTBUILD_SELECTABLE          3

/* Macro to hold configured module variant */
#define COM_CONFIGURATION_VARIANT                 COM_VARIANT_PRE_COMPILE

/* Maximum value for 8bits */
#define COM_MAX_U8_VALUE                        (0xFFu)

/* Maximum value for 16bits */
#define COM_MAX_U16_VALUE                       (0xFFFFu)

/* Maximum value for 32bits */
#define COM_MAX_U32_VALUE                       (0xFFFFFFFFuL)

/* Maximum value for 64bits */
#define COM_MAX_U64_VALUE                       (0xFFFFFFFFFFFFFFFFuLL)

/* Maximum value for TMS evaluate counter */
#define COM_MAX_TMS_COUNTER                     (0xFFFFu)



/********************* TRANSFER PROPERTY  switches **********************/
/************************************************************************/

#define COM_SigTriggered

/*#define COM_SigTriggeredWithoutRepetition*/


/*#define COM_SigTriggeredOnChange*/


/*#define COM_SigTriggeredOnChangeWithoutRepetition*/


/*#define COM_SigGrpTriggered*/


/*#define COM_SigGrpTriggeredWithoutRepetition*/


/*#define COM_SigGrpTriggeredOnChange*/



/*#define COM_SigGrpTriggeredOnChangeWithoutRepetition*/

/* Switch is enabled when signal transfer propery is TriggeredOnChange or TriggeredOnChangeWithoutRepetition*/
#if defined(COM_SigTriggeredOnChange) || defined(COM_SigTriggeredOnChangeWithoutRepetition)
#define COM_EffectiveSigTOC
#endif

/* Switch is enabled when signal group transfer propery is Triggered_On_Change or Triggered_On_Change_Without_Repetition */
#if defined(COM_SigGrpTriggeredOnChange) || defined(COM_SigGrpTriggeredOnChangeWithoutRepetition)
#define COM_EffectiveSigGrpTOC
#endif

/************************************************************************/
/************************ UPDATE_BIT switches ***************************/
/************************************************************************/

/* #define COM_RxSigUpdateBit */

/* #define COM_TxSigUpdateBit */

/* #define COM_RxSigGrpUpdateBit */

/* #define COM_TxSigGrpUpdateBit */

#if (defined(COM_RxSigUpdateBit) || defined(COM_TxSigUpdateBit) || defined(COM_RxSigGrpUpdateBit) || defined(COM_TxSigGrpUpdateBit))
# define COM_UPDATEBIT
#endif
/************************************************************************/
/************************************************************************/

/************************ RX TIME OUT switches **************************/
/************************************************************************/

/* #define COM_RxSigUpdateTimeout */

/* #define COM_RxSigGrpUpdateTimeout */


/* #define COM_RxIPduTimeout */

/* #define COM_PRV_RX_SIG_SUBSTITUTE */

/* #define COM_PRV_RX_SIGGRP_SUBSTITUTE */
/************************************************************************/

/************************ TX TIME OUT switches **************************/
/************************************************************************/

#define COM_TxIPduTimeOut

/************************************************************************/
/************************************************************************/

/********************* RX NOTIFICATION switches *************************/
/************************************************************************/

#define COM_RxSignalNotify

/* #define COM_RxIPduNotification */

/* #define COM_RxSignalGrpNotify */
/************************************************************************/
/************************************************************************/


/********************* FILTER related switches **************************/
/************************************************************************/

/* #define COM_RxFilters */

#define COM_TxFilters

#if (defined(COM_RxFilters) || defined(COM_TxFilters))
#define COM_FILTERS
#endif


/* #define COM_F_MASKEDNEWEQUALSX */

/* #define COM_F_MASKEDNEWDIFFERSX */

/* #define COM_F_MASKEDNEWDIFFERSOLD */

/* #define COM_F_NEWISWITHIN_POS */

#define COM_F_NEWISWITHIN_NEG

/* #define COM_F_NEWISOUTSIDE_POS */

/* #define COM_F_NEWISOUTSIDE_NEG */

/* #define COM_F_ONEEVERYN */
/************************************************************************/
/************************************************************************/

/* #define COM_IPDU_WITHOUT_IPDUGROUP_EXISTS */

/*********************** RX INVALID switches ****************************/
/************************************************************************/

/* #define COM_RxSigInvalid */

/* #define COM_RxSigInvalidNotify */

/* #define COM_RxSigGrpInvalid */

/* #define COM_RxSigGrpInvalidNotify */

/************************************************************************/
/************************************************************************/

/*********************** TX INVALID switches ****************************/
/************************************************************************/
/* #define COM_TxInvalid */

/* #define COM_TxGrpSigInvalid */
/************************************************************************/
/************************************************************************/

/********************* UINT64/SINT64 switches ***************************/
/* #define COM_TXSIG_INT64 */

/* #define COM_RXSIG_INT64 */

/* #define COM_TXGRPSIG_INT64 */

/* #define COM_RXGRPSIG_INT64 */
/************************************************************************/

/********************* DYNAMIC SIGNAL switches **********************/
/************************************************************************/
/* #define COM_TX_DYNAMIC_SIGNAL_SUPPORT */

/* #define COM_RX_DYNAMIC_SIGNAL_SUPPORT */

/* #define COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/* #define COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/********************* FLOAT64 switches **********************/
/************************************************************************/
/* #define COM_TXGRPSIG_FLOAT64SUPP */

/* #define COM_TXSIG_FLOAT64SUPP */

/* #define COM_RXSIG_FLOAT64SUPP */

/* #define COM_RXGRPSIG_FLOAT64SUPP */

/* #define COM_FLOAT32SUPP */
/* #define COM_GRPSIGFLOAT32SUPP */

/************************** Misc FEATURES *******************************/
/************************************************************************/
/* #define COM_RxIPduDeferredProcessing */

/*#define COM_MIXEDPHASESHIFT*/


/* #define COM_RxIPduCallOuts */

/* #define COM_TxIPduCallOuts */

/* #define COM_SIGNALGATEWAY */

/* #define COM_SIGNALGROUPGATEWAY */

/* #define COM_TX_TP_IPDUTYPE */

/* #define COM_RX_TP_IPDUTYPE */


/* #define COM_TX_SIGNALGROUP_ARRAY */


/* #define COM_RX_SIGNALGROUP_ARRAY */

/* #define COM_RX_IPDUCOUNTER */

/* #define COM_TX_IPDUCOUNTER */

/* START: Miscellaneous Macros */

/* Mask used in the Code to identify the maximum unsigned long index of an array */
#define COM_MAX_U32_INDEX                       (COM_MAX_U32_VALUE)

/* Maximum number of bits supported in Com */
#define COM_SIG_MAX_NO_OF_BITS                  (32u)

/* Maximum signal value */
#define COM_SIG_MAX_DATA                        (COM_MAX_U32_VALUE)

/* END: Miscellaneous Macros */

/************************************************************************/
/************************************************************************/



#define COM_VERSION_INFO_API                STD_OFF

#define COM_CONFIGURATION_USE_DET           STD_OFF

/* TRACE[Rule BSW_ErrorHandling_002] BSW Strategy for Error Handling */
#define COM_PRV_ERROR_HANDLING             (STD_ON)


#define COM_COMMON_TX_PERIOD                STD_OFF
#define COM_ENABLE_JITTERFLITER             STD_OFF
/* #define COM_ENABLE_READRXIPDULENGTH */

#define COM_ARSRVLIB_SUPPORT

/* #define COM_PROVIDE_IPDU_STATUS */

/* #define COM_MULTICORE_SUPPORT */

/* #define COM_TXSCHEDULED_DIFF_TIMEBASE */

#define COM_ENABLE_MDT_FOR_CYCLIC_TRANSMISSION   STD_ON

/* #define COM_METADATA_SUPPORT */

#define COM_RETRY_FAILED_TX_REQUESTS             STD_OFF

/* #define COM_CANCELTRANSMITSUPPORT */

#define  COM_TXPDU_DEFERREDPROCESSING

/* #define COM_PDUBASEDLOCKSENABLED */

/* #define COM_TXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT */

/* #define COM_RXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT */

/* #define COM_COPY_IMMEDIATE_RXIPDU */

#define COM_ENABLE_PERIODIC_FRAME_SYNC           STD_OFF

/* #define COM_RX_DUAL_CYCLE_SUPPPORT */


/* #define COM_INITVALOPTIMIZATION*/

/* #define COM_TX_IPDUCONTROL_VIA_CALIBRATION */

/* #define COM_RX_IPDUCONTROL_VIA_CALIBRATION */

/* #define COM_TXPDU_TIMEPERIOD_VIA_CALIBRATION */

/* #define COM_RXPDU_FIRSTTIMEOUT_VIA_CALIBRATION */

/* #define COM_RXPDU_TIMEOUT_VIA_CALIBRATION */

/* #define COM_TXPDU_TIMEOFFSET_VIA_CALIBRATION */

/* #define COM_TXPDU_NUMOFREPETITION_VIA_CALIBRATION */



#define COM_RXTIMER_MAX                         65535u

/* Maximum Length of the MetaData Supported by Com Module */
# define COM_MAX_METADATA_LENGTH_IN_BYTES       4u

/* Number of ComMainFunctionRx(s) */
#define COM_NUM_OF_RX_MAINFUNCTION          1u

/* Number of ComMainFunctionTx(s) */
#define COM_NUM_OF_TX_MAINFUNCTION          1u

/* macro is generated in the idxTxIpduCntr_uo that are not configured with counter */
#define COM_TXIPDU_CNTR_INV_IDX             COM_MAX_U8_VALUE

/* macro is generated in the idxRxIpduCntr_uo that are not configured with counter */
#define COM_RXIPDU_CNTR_INV_IDX             COM_MAX_U8_VALUE

/* macro is generated in the idxOldValue_uo that are not configured with old value */
#define COM_TOC_OLD_VALUE_INV_IDX           COM_MAX_U8_VALUE

#if (defined(COM_NUM_OF_RX_MAINFUNCTION) && (COM_NUM_OF_RX_MAINFUNCTION > 0)) && \
(defined(COM_RxIPduDeferredProcessing) || \
defined(COM_RxIPduTimeout) || defined(COM_RxSigUpdateTimeout) || defined(COM_RxSigGrpUpdateTimeout) || \
defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY))

# define COM_ENABLE_MAINFUNCTION_RX

#endif  /* end of enable-MainFunctionRx */

#if (defined(COM_NUM_OF_TX_MAINFUNCTION) && (COM_NUM_OF_TX_MAINFUNCTION > 0))

# define COM_ENABLE_MAINFUNCTION_TX

#endif  /* end of enable-MainFunctionTx */

#ifdef COM_RX_TP_IPDUTYPE
# if defined(COM_RxSigInvalid) || defined(COM_RxSigGrpInvalid)

#  define COM_RXIPDU_TP_INVALIDATION

# endif
#endif

#if defined (COM_TXSIG_FLOAT64SUPP ) || defined(COM_TXGRPSIG_FLOAT64SUPP) || defined(COM_RXGRPSIG_FLOAT64SUPP) || \
defined(COM_RXSIG_FLOAT64SUPP)
#define Com_getfloat64SigValue(val)    (COM_GET_FLOAT_ARRAY_VALUE(val))
#endif

/* Begin section for mapping ComMainFunctionRx names to internal IDs
 * Scope of Rx-MainFunction macros are strictly restricted internal to Com. */

# define    ComMainFunctionRx_Internal_ComMainFunctionRx          0


/* Begin section for mapping ComMainFunctionTx names to internal IDs
 * Scope of Tx-MainFunction macros are strictly restricted internal to Com. */

# define    ComMainFunctionTx_Internal_ComMainFunctionTx          0


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/


/* Filter type */
#ifdef COM_F_MASKEDNEWDIFFERSOLD
typedef struct
{
    uint32 *fOldVal_pu32;
    uint32  mask_u32;
} Com_FMaskOld_tst;
#endif

#if defined(COM_F_MASKEDNEWEQUALSX) || defined(COM_F_MASKEDNEWDIFFERSX)
    typedef struct
    {
        uint32 mask_u32;
        uint32 xval_u32;
    } Com_MaskX_tst;
#endif
#if defined(COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS)
    typedef struct
    {
        uint32 min_u32;
        uint32 max_u32;
    } Com_POSMinMax_tst;
#endif
#if defined(COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG)
    typedef struct
    {
        sint32 min_s32;
        sint32 max_s32;
    } Com_NEGMinMax_tst;
#endif

#ifdef COM_F_ONEEVERYN
    typedef struct
    {
        uint32 * occurrence_pu32;
        uint32 period_u32;
        uint32 offset_u32;
    } Com_OneEveryN_tst;
#endif


typedef uint8    Com_NoOfTxGrpSignal_tuo;

typedef uint8    Com_NoOfRxGrpSignal_tuo;

#ifdef COM_TX_SIGNALGROUP
typedef uint8    Com_TxIntGrpSignalId_tuo;
#endif

#ifdef COM_RX_SIGNALGROUP
typedef uint8    Com_RxIntGrpSignalId_tuo;
#endif


typedef uint8    Com_TxIntSignalId_tuo;
typedef uint8    Com_RxIntSignalId_tuo;

#ifdef COM_TX_SIGNALGROUP
typedef uint8  Com_TxIntSignalGroupId_tuo;
#endif

#ifdef COM_RX_SIGNALGROUP
typedef uint8  Com_RxIntSignalGroupId_tuo;
#endif



typedef uint8  Com_IpduId_tuo;

typedef uint8  Com_GrpSignalId_tuo;

typedef uint8 Com_idxSVal_tuo;

#if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC)
typedef uint8   Com_OldVal_tuo;
typedef uint32 Com_OldValTOC_tuo;
#endif

typedef uint8   Com_Bitsize_tuo;
typedef uint8 Com_Bitposition_tuo;
#if defined(COM_FILTERS)
typedef uint8   Com_Filter_tuo;
#endif


typedef uint8 Com_SigBuffIndex_tuo;



typedef uint8 Com_RxGrpSigBuffIndex_tuo;


typedef uint8 Com_TxGrpSigBuffIndex_tuo;

#ifdef COM_TX_SIGNALGROUP_ARRAY


    /* Com_TxSigGrpArrayIndex_tuo - is used as a typedef for SigGrpArray_Index member in
     * TxSignalGroup Configuration structure. Its typedef is generated based on the number of Tx-SignalGroup's
     * configured with the Array Access */
    typedef uint8 Com_TxSigGrpArrayIndex_tuo;

# endif /* #ifdef COM_TX_SIGNALGROUP_ARRAY */

# ifdef COM_RX_SIGNALGROUP_ARRAY

    /* Com_RxSigGrpArrayIndex_tuo - is used as a typedef for SigGrpArray_Index member in
     * RxSignalGroup Configuration structure. Its typedef is generated based on the number of Rx-SignalGroup's
     * configured with the Array Access */
    typedef uint8 Com_RxSigGrpArrayIndex_tuo;

    /* Com_RxSigGrpBuffIndex_tuo - is used as a typedef for RxSigGrpBuf_Index in RxSignalGroup Array
     * configuration structure. Its typedef is generated based on the total number of bytes of all the RxSignalGroup's
     * configured with Array Access */
    typedef uint8 Com_RxSigGrpBuffIndex_tuo;

# endif /* #ifdef COM_RX_SIGNALGROUP_ARRAY */


typedef uint32 Com_SigMax_tuo;

#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
typedef uint8 Com_IdxGwMap_tuo;
#endif

#ifdef COM_METADATA_SUPPORT

/**
 * @ingroup COM_TYPES_H
 * This typedef structure is data type for MetaData information\n
 *
 * typedef struct\n
 * {\n
 *   uint8 * rxMetaDataValue_pau8;\n
 *   uint8   rxMetaDataLength_u8;\n
 * } Com_RxMetaDataInfo_tst;
 */
typedef struct
{
    uint8 *     rxMetaDataValue_pau8;
    uint8       rxMetaDataLength_u8;
} Com_RxMetaDataInfo_tst;

/**
 * @ingroup COM_TYPES_H
 * This type defines a pointer type that points to the current IPdu MetaData Information.\n
 *
 * typedef const Com_RxMetaDataInfo_tst * Com_RxMetaData_tpcst;\n
 */
typedef const Com_RxMetaDataInfo_tst * Com_RxMetaData_tpcst;

/**
 * @ingroup COM_TYPES_H
 * This typedef structure is data type for MetaData information\n
 *
 * typedef struct\n
 * {\n
 *   uint8 * txMetaDataValue_pau8;\n
 *   uint8   txMetaDataDefaultValue_au8[COM_MAX_METADATA_LENGTH_IN_BYTES];\n
 *   uint8   txMetaDataLength_u8;\n
 * } Com_TxMetaDataInfo_tst;
 */
typedef struct
{
    uint8 *     txMetaDataValue_pau8;
    uint8       txMetaDataDefaultValue_au8[COM_MAX_METADATA_LENGTH_IN_BYTES];
    uint8       txMetaDataLength_u8;
} Com_TxMetaDataInfo_tst;

/**
 * @ingroup COM_TYPES_H
 * This type defines a pointer type that points to the current IPdu MetaData Information.\n
 *
 * typedef const Com_TxMetaDataInfo_tst * Com_TxMetaData_tpcst;\n
 */
typedef const Com_TxMetaDataInfo_tst * Com_TxMetaData_tpcst;

#endif /* #ifdef COM_METADATA_SUPPORT */

typedef uint8  Com_MainFunc_tuo;

typedef uint16 Com_NumOfIpdus_tuo;

typedef uint16 Com_TimeBase_tuo;

#ifdef COM_TX_IPDUCOUNTER

    typedef uint8  Com_TxCntrIdx_tuo;
#endif

#ifdef COM_RX_IPDUCOUNTER

    typedef uint8  Com_RxCntrIdx_tuo;
#endif


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#endif   /* COM_CFG_INTERNAL_H */


