


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: PduR  / AR45.5.0.1                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/


#ifndef PDUR_CANIF_H
#define PDUR_CANIF_H

#include "PduR.h"

#include "CanIf.h"
#if(PDUR_ENABLE_INTER_MODULE_CHECKS)
#if (!defined(CANIF_AR_RELEASE_MAJOR_VERSION) || (CANIF_AR_RELEASE_MAJOR_VERSION != PDUR_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(CANIF_AR_RELEASE_MINOR_VERSION) || (CANIF_AR_RELEASE_MINOR_VERSION != PDUR_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif
#endif /* #if(PDUR_ENABLE_INTER_MODULE_CHECKS) */



/* Interface translation layers ------------------------------------- */
#define PduR_RF_CanIf_Transmit_Func   CanIf_Transmit
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"
/* PduR_CanIfRxIndication  */
extern void PduR_CanIfRxIndication( PduIdType id,
                                                   const PduInfoType * ptr );
                         
/* PduR_CanIfTxConfirmation  */
extern void PduR_CanIfTxConfirmation(PduIdType id,Std_ReturnType result);
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* PDUR_CANIF_H    */
