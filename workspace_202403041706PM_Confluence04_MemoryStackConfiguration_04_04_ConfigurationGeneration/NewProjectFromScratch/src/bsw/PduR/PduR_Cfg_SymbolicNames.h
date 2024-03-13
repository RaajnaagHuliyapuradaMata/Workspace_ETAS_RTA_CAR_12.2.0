

 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: PduR  / AR45.5.0.1                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/

#ifndef PDUR_CFG_SYMBOLICNAMES_H
#define PDUR_CFG_SYMBOLICNAMES_H

/* Note: Module variant generation is done here, specifically to make below macros available on the inclusion of 
 * PduR_memmap.h header file by other modules without PduR_Cfg.h inclusion */


#define PDUR_VARIANT_PRE_COMPILE    (0)

#define PDUR_VARIANT_POSTBUILD_LOADABLE    (1)

#define PDUR_VARIANT_POSTBUILD_SELECTABLE   (2)

#if !defined(PDUR_CONFIGURATION_VARIANT)
#define PDUR_CONFIGURATION_VARIANT    PDUR_VARIANT_PRE_COMPILE
#endif /* PDUR_CONFIGURATION_VARIANT */




#define PduRConf_PduRSrcPdu_TestECU_TestCode_Tx_CanIf2PduR_Can_Network_Channel_CAN    0
#define PduRConf_PduRSrcPdu_ApplicationECU_TestResult_Tx_Com2PduR_Can_Network_Channel_CAN    0
#define PduRConf_PduRDestPdu_TestECU_TestCode_Tx_PduR2Com_Can_Network_Channel_CAN  0
#define PduRConf_PduRDestPdu_ApplicationECU_TestResult_Tx_PduR2CanIf_Can_Network_Channel_CAN  0




#endif /* PDUR_CFG_SYMBOLICNAMES_H */
