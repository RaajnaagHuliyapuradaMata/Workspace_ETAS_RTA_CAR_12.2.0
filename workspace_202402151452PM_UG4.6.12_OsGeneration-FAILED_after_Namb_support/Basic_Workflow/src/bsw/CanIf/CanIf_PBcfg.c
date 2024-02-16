



/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: CanIf / AR45.5.0.1                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 * 
 

 </VersionHead>*/


/******************************************************************************/
/*                                  Include Section                                */
/******************************************************************************/
/* CanIf Private header */
#include "CanIf_Prv.h"



/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */









/*======================================================================================================================
 *                                VARIANT:  PRE_COMPILE
 *======================================================================================================================
*/







/*====================================================================================================================*/
/*
 *                                  CANIF CONTROLLER CONFIG STRUCTURE             
 *                                     
 * Structure contains following members:
 *{
 *#if(CANIF_PUBLIC_TXBUFFERING ==STD_ON) 
 *  (0) BufferIdPtr,
 *  (1) TotalBufferCount,
 *  (2) TxPduIdPtr,
 *  (3) TotalTxPduCount,
 *#endif
 *  (4) CtrlId,
 *  (5) CtrlCanCtrlRef,
 *  (6) CtrlWakeupSupport,
 *#if (CANIF_PUBLIC_PN_SUPPORT == STD_ON)
 *  (7) PnCtrlEn            
 *#endif
 * #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON
 * CanDrvIndx
 * #endif
 *}
*/


#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"

static const CanIf_Cfg_CtrlConfig_tst CanIf_CtrlGen_a[]=
{

/*0:Can_Network_CANNODE_0 */
{ /*(4)*/0,   /*(5)*/CanConf_CanController_Can_Network_CANNODE_0,   /*(6)*/FALSE  }
};





#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"



#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"


static const CanIf_Cfg_HthConfig_tst CanIf_HthGen_a[]=
{
/*,  *CanIf_CtrlConfigPtr,  CanObjectId,   CanHandleType */

/*0:Can_Network_CANNODE_0_Tx_Std_MailBox_1*/
{/*(0)*/ &CanIf_CtrlGen_a[CanIf_Ctrl_CustId_Can_Network_CANNODE_0],
/*(1)*/ CanConf_CanHardwareObject_Can_Network_CANNODE_0_Tx_Std_MailBox_1 
}



};


#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"




/*                                  CANIF TXBUFFER CONFIG STRUCTURE       

 * Structure contains following members:
 *{
 * (0)*CanIf_HthConfigPtr
 *#if (CANIF_PUBLIC_TXBUFFERING == STD_ON)
 * (1)DataBuf
 * (2)CanIdBuf
 * (3)CanIfBufferId;
 * (4)CanIfBufferSize
 * (5)CanIfBufferMaxDataLength 
 *#endif
 *}
 */     

#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"
                                            
static const CanIf_Cfg_TxBufferConfig_tst CanIf_TxBufferGen_a[]=
{

/*0:Bu_Can_Network_CANNODE_0_Tx_Std_MailBox_1*/
{/*(0)*/&CanIf_HthGen_a[CanIf_Hth_CustId_Can_Network_CANNODE_0_Tx_Std_MailBox_1]}
};

#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"




/*                                  CANIF TXPDU CONFIG STRUCTURE       
 * Structure contains following members:
 *{
 * (0)*CanIf_TxBufferConfigPt
 *#if(CANIF_METADATA_SUPPORT == STD_ON)
 * (1)TxPduCanIdMask
 * (2)MetaDataLength          
 *#endif
 * (3)TxPduId
 * (4)TxPduTargetPduId       
 * (5)TxPduType                
 * (6)TxPduCanIdType
 * (7)TxPduTxUserUL
 * (8)Function pointer to TxPduTxUserULName
 * (9)TxPduCanId
 *
 *#if(CANIF_RB_NODE_CALIBRATION == STD_ON)
 *(10) getTxPduCanId
 *#endif
 *
 *#if (CANIF_PUBLIC_PN_SUPPORT == STD_ON)
 * (11)TxPduPnFilterPdu
 *#endif
 *
 *#if (CANIF_READTXPDU_NOTIFY_STATUS_API == STD_ON)
 * (12)TxPduReadNotifyStatus
 *#endif
 *
 *#if(CANIF_TRIGGERTRANSMIT_SUPPORT== STD_ON)
 * (13)Function pointer to UserTriggerTransmitName
 * (14)TxPduTriggerTransmit
 *#endif
 *
 *#if(CANIF_RB_NODE_CALIBRATION == STD_ON)
 *(15)getTxPduDlc
 *#endif
 *
 *(16)TxTruncEnabled
 *(17)TxPduLength
 *}
 */  

#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"
                              

static const CanIf_Cfg_TxPduConfig_tst CanIf_TxPduGen_a[]=
{

/*0:ApplicationECU_To_TestECU_Can_Network_CANNODE_0_OUT*/

{ /*CanIf_TxBufferConfigPtr*/&CanIf_TxBufferGen_a[CanIf_Buffer_CustId_Bu_Can_Network_CANNODE_0_Tx_Std_MailBox_1],    /*TxPduId*/CanIfConf_CanIfTxPduCfg_ApplicationECU_To_TestECU_Can_Network_CANNODE_0_OUT, /*TxPduTargetPduId*/ 0 , 
/*TxPduType*/CANIF_STATIC, /*TxPduCanIdType*/STANDARD_CAN,   /*TxPduTxUserUL*/PDUR,      /*UserTxConfirmation*/&PduR_CanIfTxConfirmation, /*TxPduCanId*/ 0xA   , /*TxPduReadNotifyStatus*/ FALSE    ,/*TxTruncEnabled*/ FALSE,/*TxPduLength*/8u}
};


#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"
/*=====================================================================================================================
 *=====================================================================================================================
 *=====================================================================================================================
 */











#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"

static const CanIf_Cfg_Hrhtype_tst CanIf_Prv_HrhConfig_tacst[1] =
{
          /*HrhInfo_e,    PduIdx_t,  NumRxPdus_u32,  HrhRangeMask_b,  ControllerId_u8 */
    
/*0: Can_Network_CANNODE_0_Rx_Std_MailBox_1*/
    {CANIF_PRV_FULL_E,       0u,             1u,            FALSE,           0u           }
};
#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"







#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"
static const CanIf_Cfg_RxPduType_tst CanIf_Prv_RxPduConfig_tacst[1]=
{   
   /*RxPduReadNotifyReadDataStatus_u8       IndexForUL_u8   CanIdtype_u8    RxPduDlc_u8      RxPduCanId      Hrhref_t    RxPduTargetId_t 	*/
    
    /*0:TestECU_To_ApplicationECU_Can_Network_CANNODE_0_IN*/
    {
        CANIF_READ_NOTIFSTATUS,     6u,           0x30u,            8u,         20u,       0,         PduRConf_PduRSrcPdu_TestECU_To_ApplicationECU_CanIf2PduR_Can_Network_Channel_CAN            }
};
#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"
















/* Array for mapping Hoh Id(CanObjectId) and Hrh */
#define CANIF_START_SEC_CONST_16
#include "CanIf_MemMap.h"
static const uint16 CanIf_CFG_HrhIdMapping_au16[] =
{/*0:Can_Network_CANNODE_0_Rx_Std_MailBox_1*/
0,
/*1:Can_Network_CANNODE_0_Tx_Std_MailBox_1*/
CANIF_INVALID_ID};
#define CANIF_STOP_SEC_CONST_16
#include "CanIf_MemMap.h"

/* Array for mapping CanIfRxpduId accross the variant */
#define CANIF_START_SEC_CONST_16
#include "CanIf_MemMap.h"
static const uint16 CanIf_CFG_RxPduIdMapping_au16[] =
{
/*0:TestECU_To_ApplicationECU_Can_Network_CANNODE_0_IN*/
0};
#define CANIF_STOP_SEC_CONST_16
#include "CanIf_MemMap.h"





/* CANIF callback configuration */

#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"
static const CanIf_CtrlDrvCfgStruct CanIf_DrvCtrlGen[]=
{
/*0:Can*/
    {0,
    
	
   
    &Can_Integn_SetControllerMode, 
    
    &Can_Integn_Write, 
	
	
    &Can_Integn_GetControllerMode,
	 
	
    
    NULL_PTR,
    
	
    
    NULL_PTR,
    
	
    
    NULL_PTR,
    
    
    
    NULL_PTR
    

}


};



#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"







/* array of function pointer which provies Callback name to the UL*/
#define CANIF_START_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"
static const CanIf_RxCbk_Prototype CanIf_Prv_ULName_ta__fct[] =
{
    {NULL_PTR},
    {NULL_PTR},
    {NULL_PTR},
    {NULL_PTR},
    {NULL_PTR},
    {NULL_PTR},
    {&PduR_CanIfRxIndication},
    {NULL_PTR},
    {NULL_PTR},
    
};
#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"










/* CANIF callback configuration */


#define CANIF_START_SEC_CONST_UNSPECIFIED

#include "CanIf_MemMap.h"
const CanIf_CallbackFuncType CanIf_Callback =
{
   
    /*5: User_ControllerBusOff */
    &CanSM_ControllerBusOff,
    
    /* 6:User_ControllerModeIndication */
    &CanSM_ControllerModeIndication,
    /*7:User_ControllerErrorPassive*/
};
#define CANIF_STOP_SEC_CONST_UNSPECIFIED
#include "CanIf_MemMap.h"









/*This mapping table is generated for finding invalid TxPduIs passed via CanIf APIs in Post-Build.
 *Size of the array is total number of Tx PDUs across the variants. Each element is the index of Tx Pdu config structure.
 * If a TxPdu is not present in this variant, an invalid value 0xFFFF is generated in the particular position.
 */
 
 #define  CANIF_START_SEC_CONST_16
#include "CanIf_MemMap.h"



static const uint16 CanIf_TxPduId_MappingTable[] = 
{
      
/*ApplicationECU_To_TestECU_Can_Network_CANNODE_0_OUT*/
0 
};  







#define  CANIF_STOP_SEC_CONST_16
#include "CanIf_MemMap.h"


/*This mapping table is generated for finding invalid CtrlIds passed via CanIf APIs in Post-Build.
 *Size of the array is total number of controllers across the variants. Each element is the index of Controller config structure.
 * If a CtrlId is not present in this variant, an invalid value 0xFF is generated in the particular position.
 */

#define  CANIF_START_SEC_CONST_8
#include "CanIf_MemMap.h"



static const uint8 CanIf_CtrlId_MappingTable[] = 
{

/*Can_Network_CANNODE_0*/
0 
};  






#define  CANIF_STOP_SEC_CONST_8
#include "CanIf_MemMap.h"





/*Configuration structure for __KW_COMMON*/

#define CANIF_START_SEC_CONFIG_DATA_UNSPECIFIED

#include "CanIf_MemMap.h"
const CanIf_ConfigType CanIf_Config =
{
    
    /* HrhConfig_pcst */
    CanIf_Prv_HrhConfig_tacst,
    /* RxPduConfig_pcst */
    CanIf_Prv_RxPduConfig_tacst,
   /* NumCanRxPduId_t */
   1u,
   /*NumCanCtrl_u8*/
   1,
   /*NumCddRxPdus_t*/
   
   0,
   /*RxPduIdTable_Ptr*/
   &CanIf_CFG_RxPduIdMapping_au16[0],
   /*HrhPduIdTable_Ptr*/
   &CanIf_CFG_HrhIdMapping_au16[0],
   /*CfgSetIndex_u8*/
   0,
   &CanIf_TxPduGen_a[0u],             /*CanIf_TxPduConfigPtr*/
   &CanIf_TxBufferGen_a[0u],          /*CanIf_TxBufferConfigPtr*/
   &CanIf_CtrlGen_a[0u],              /*CanIf_CtrlConfigPtr*/
   
   NULL_PTR,
   
   1,      /*NumOfTxPdus*/
   1,        /*NumOfTxBuffers*/  
   &CanIf_TxPduId_MappingTable[0],      /*TxPduIdTable_Ptr*/
   &CanIf_CtrlId_MappingTable[0],
   /*RxAutosarUL_Ptr*/
   &CanIf_Prv_ULName_ta__fct[0],
   /*CanCtrlDrvCont_Ptr*/
   &CanIf_DrvCtrlGen[0]  
};

#define CANIF_STOP_SEC_CONFIG_DATA_UNSPECIFIED

#include "CanIf_MemMap.h"







