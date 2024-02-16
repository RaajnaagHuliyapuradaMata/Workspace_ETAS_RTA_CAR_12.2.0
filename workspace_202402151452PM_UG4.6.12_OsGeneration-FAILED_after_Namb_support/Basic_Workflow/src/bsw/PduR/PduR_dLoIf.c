


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: PduR  / AR45.5.0.1                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/



#include "PduR_Prv.h"

#include "PduR_Cfg.h"
/* Appropriate header files are included to declare the prototypes
 */
#include "PduR_LoIf.h"


/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_CanIfTxConfirmation - PDU Router CanIfTxConfirmation
 *      This function is called by the CanIf after the PDU has been transmitted on the network.
 *
 * \param           PduIdType Id: ID of  L-PDU that has been transmitted.
 *
 *
 * \retval          None
 * \seealso         PDUR194
 * \usedresources
 **************************************************************************************************
 */
void PduR_CanIfTxConfirmation( PduIdType id,Std_ReturnType result)
{
    PduR_rCanIfTxConfirmation( id,result);

}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */


#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_CanIfRxIndication - PDU Router CanIfRxIndication
 *      This function is called by the CanIf after a L-PDU has been received.
 *
 * \param           PduIdType id
 *                  const uint8 *ptr
 *
 * \retval          None
 * \seealso         PDUR193 and PDUR258
 * \usedresources
 **************************************************************************************************
 */

void PduR_CanIfRxIndication( PduIdType id,
                                                const PduInfoType * ptr)
{
    PduR_rCanIfRxIndication((id), (ptr));
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_dCanIfRxIndication - PDU Router CanIfRxIndication
        Function to be invoked DET enable for PduR_CanIfRxIndication
 *      This function is called by the CanIf after a L-PDU has been received.
 *
 * \param           PduIdType CanIf_RxPduId
 *                  const uint8 *ptr
 *
 * \retval          None
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
void PduR_dCanIfRxIndication( PduIdType id, const PduInfoType * ptr )
{
    PDUR_CHECK_STATE_VOID(PDUR_MODULE_ID_CANIF, PDUR_SID_RXIND)

    PDUR_CHECK_PTR_VOID(PDUR_MODULE_ID_CANIF, PDUR_SID_RXIND, ptr)

    /*Check for Invalid Id and null pointer */
    if ( ( id >= PDUR_NR_VALID_CANIFIFRXTOUP_IDS ) || (PDUR_CANIF_RXIND_MAP_TAB[(id)] == PDUR_INVALID_ID) || ( PduR_iCanIfRxIndicationFunc(id) == NULL_PTR )  )
    {
        PDUR_REPORT_ERROR(PDUR_MODULE_ID_CANIF, PDUR_SID_RXIND, PDUR_E_PDU_ID_INVALID);
        return;
    }
   /*Call the upper layer RxIndication function */
    PduR_iCanIfRxIndicationFunc(id)( PduR_iCanIfRxIndicationID(id), ptr );
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

/* ------------------------------------------------------------------------ */

/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_dCanIfTxConfirmation - PDU Router CanIfTxConfirmation
 *      Function to be invoked DET enable for PduR_CanIfTxConfirmation
 *      This function is called by the CanIf after the PDU has been transmitted on the network.
 *
 * \param           PduIdType CanTxPduId: ID of CAN L-PDU that has been transmitted.
 *
 *
 * \retval          None
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
void PduR_dCanIfTxConfirmation( PduIdType id,Std_ReturnType result)
{
    PDUR_CHECK_STATE_VOID(PDUR_MODULE_ID_CANIF, PDUR_SID_TXCONF)

    /*Check for Invalid Id and null pointer */
    if ( ( id >= PDUR_NR_VALID_CANIFIFTXTOUP_IDS ) || (PDUR_CANIF_TXCONF_MAP_TAB[(id)] == PDUR_INVALID_ID) || ( PduR_iCanIfTxConfirmationFunc(id) == NULL_PTR ) )
    {
        PDUR_REPORT_ERROR(PDUR_MODULE_ID_CANIF, PDUR_SID_TXCONF, PDUR_E_PDU_ID_INVALID);
        return;
    }

    /*Call the upper layer TxConfirmation function */
    PduR_iCanIfTxConfirmationFunc(id)( PduR_iCanIfTxConfirmationID(id),(result) );


}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

