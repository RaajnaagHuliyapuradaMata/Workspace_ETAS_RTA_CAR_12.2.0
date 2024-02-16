


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: PduR  / AR45.5.0.1                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/


#ifndef PDUR_CANIF_LO_H
#define PDUR_CANIF_LO_H

#include "PduR_Prv.h"

/* Ensure that the function declarations in this header are located in the code section. */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/* ------------------------------------------------------------------- */

#define PduR_CanIf_upIfRxIndicationFuncId(id)       (PDUR_CANIF_RXIND_BASE[PDUR_CANIF_RXIND_MAP_TAB[(id)]].upRxIndicationID)

/* ------------------------------------------------------------------- */
#if defined(PDUR_CONFIG_SINGLE_IFRX)
#define PduR_iCanIfRxIndicationID(id)        (id)
#define PduR_iCanIfRxIndicationFunc(id)        (PDUR_CONFIG_SINGLE_IFRX(RxIndication))
#else
#define PduR_iCanIfRxIndicationID(id)        (PDUR_CANIF_RXIND_BASE[PDUR_CANIF_RXIND_MAP_TAB[(id)]].upId)
#define PduR_iCanIfRxIndicationFunc(id)        (PduR_upIfRxIndicationTable[(PDUR_CANIF_RXIND_BASE[PDUR_CANIF_RXIND_MAP_TAB[(id)]].upRxIndicationID)].PduR_upIfRxIndicationFunc)

#endif /* PDUR_CONFIG_SINGLE_IFRX */


extern void PduR_dCanIfRxIndication( PduIdType id,
                                                       const PduInfoType * ptr  );



#define PduR_rCanIfRxIndication(id, ptr)      PduR_dCanIfRxIndication((id), (ptr))

/* ------------------------------------------------------------------- */


#if defined(PDUR_CONFIG_SINGLE_IFTX_UP)
#define PduR_iCanIfTxConfirmationID(id)      (id)
#define PduR_iCanIfTxConfirmationFunc(id)      (PDUR_CONFIG_SINGLE_IFTX_UP(TxConfirmation))
#else
#define PduR_iCanIfTxConfirmationID(id)      (PDUR_CANIF_TXCONF_BASE[(id)].upId)
#define PduR_iCanIfTxConfirmationFunc(id)    (PduR_upIfTxConfirmationTable[(PDUR_CANIF_TXCONF_BASE[PDUR_CANIF_TXCONF_MAP_TAB[(id)]].upTxConfirmationID)].PduR_upIfTxConfirmationFunc)

#endif


extern void PduR_dCanIfTxConfirmation( PduIdType id,Std_ReturnType result);



#define PduR_rCanIfTxConfirmation(id,result)         PduR_dCanIfTxConfirmation((id),(result))

/* Anything after this point will not be placed in the code section. */
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"


#endif /* PDUR_CANIF_LO_H */

