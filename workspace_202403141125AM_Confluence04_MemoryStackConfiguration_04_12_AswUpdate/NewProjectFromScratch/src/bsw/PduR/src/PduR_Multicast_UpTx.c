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
/* Support function to handle multicast from PduR_ComTransmit */


#include "PduR_Prv.h"
#include "PduR_Mc.h"
#include "PduR_Gw.h"

#if defined(PDUR_DCM_SUPPORT) && (PDUR_DCM_SUPPORT != 0)

#include "PduR_Dcm.h"
#include "PduR_Dcm_Up.h"
#endif

#if defined(PDUR_DLT_SUPPORT) && (PDUR_DLT_SUPPORT != 0)
#include "PduR_Dlt.h"
#include "PduR_Dlt_Up.h"
#endif

#if defined(PDUR_COM_SUPPORT) && (PDUR_COM_SUPPORT != 0)

#include "PduR_Com.h"
#include "PduR_Com_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_Com_Transmit_Func

 *      Support function to handle multicast from PduR_ComTransmit
 *
 * \param           PduIdType id      :multicast ID to be transmitted.
 *                  const PduInfoType * info:Pointer to a structure with PDU related data that shall be transmitted:
 *                                         data length and pointer to buffer
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_Com_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_COM_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* #if defined(PDUR_COM_SUPPORT) && (PDUR_COM_SUPPORT != 0) */


/* Only enabled if Multicast to Tp support is enabled */
/* This macro has to be generated in PduR_Cfg.h if TP Multicast is required */
#if defined(PDUR_MULTICAST_TO_TP_SUPPORT) && (PDUR_MULTICAST_TO_TP_SUPPORT != 0)


#if defined(PDUR_DCM_SUPPORT) && (PDUR_DCM_SUPPORT != 0)
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_Dcm_Transmit_Func
 *      Support function to handle multicast from PduR_DcmTransmit
 *
 * \param           PduIdType id      :multicast ID to be transmitted.
 *                  const PduInfoType * info:Pointer to a structure with PDU related data that shall be transmitted:
 *                                         data length and pointer to buffer
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_Dcm_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_DCM_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* #if defined(PDUR_DCM_SUPPORT) && (PDUR_DCM_SUPPORT != 0) */



#if defined(PDUR_DLT_SUPPORT) && (PDUR_DLT_SUPPORT != 0)
/* ------------------------------------------------------------------------ */
/* Begin section for code */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_Dlt_Transmit_Func
 *      Support function to handle multicast from PduR_DltTransmit
 *
 * \param           PduIdType id      :multicast ID to be transmitted.
 *                  const PduInfoType * info:Pointer to a structure with PDU related data that shall be transmitted:
 *                                         data length and pointer to buffer
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_Dlt_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_DLT_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"


#endif /* #if defined(PDUR_DLT_SUPPORT) && (PDUR_DLT_SUPPORT != 0) */


#endif /* PDUR_MULTICAST_TO_TP_SUPPORT */


/* ------------------------------------------------------------------------ */
/* Begin section for code */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GwMcTx_Transmit_Func - Support function to handle multicast from a gateway operation

 *      Support function to handle multicast from upper layers to lower layers
 *
 * \param           PduIdType id            :ID of I-PDU to be transmitted.
 *                  PduInfoType ** ptr:Pointer to pointer to PduInfoStructure containing SDU data pointer and
 *                                             SDU length of a transmit buffer.
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_GwMcTx_Transmit_Func(PduIdType id, const PduInfoType * info)
{
    return PduR_MF_UpToLo(PDUR_GW_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"


#if defined(PDUR_IPDUM_SUPPORT) && (PDUR_IPDUM_SUPPORT != 0)

#include "PduR_IpduM.h"
#include "PduR_IpduM_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_Ipdum_Transmit

 *        This function is called by the PDU-Router to request a transmission.
 *
 * \param           PduIdType gwayId  :ID of I-PDU to be transmitted.
 *                  const PduInfoType * ptr: A pointer to a structure with I-PDU related data that
 *                  shall be transmitted: data length and pointer  to I-SDU buffer
 *
 *
 * \retval          E_OK Transmit request is accepted
 *                  E_NOT_OK Transmit request is not accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_IpduM_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_IPDUM_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_IPDUM_SUPPORT */


#if defined(PDUR_SECOC_SUPPORT) && (PDUR_SECOC_SUPPORT != 0)

#include "PduR_SecOC.h"
#include "PduR_SecOC_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_SecOC_Transmit

 *        This function is called by the PDU-Router to request a transmission.
 *
 * \param           PduIdType gwayId  :ID of I-PDU to be transmitted.
 *                  const PduInfoType * ptr: A pointer to a structure with I-PDU related data that
 *                  shall be transmitted: data length and pointer  to I-SDU buffer
 *
 *
 * \retval          E_OK Transmit request is accepted
 *                  E_NOT_OK Transmit request is not accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_SecOC_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_SECOC_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_SECOC_SUPPORT */

#if defined(PDUR_LDCOM_SUPPORT) && (PDUR_LDCOM_SUPPORT != 0)

#include "PduR_LdCom.h"
#include "PduR_LdCom_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_LdCom_Transmit

 *        This function is called by the PDU-Router to request a transmission.
 *
 * \param           PduIdType gwayId  :ID of I-PDU to be transmitted.
 *                  const PduInfoType * ptr: A pointer to a structure with I-PDU related data that
 *                  shall be transmitted: data length and pointer  to I-SDU buffer
 *
 *
 * \retval          E_OK Transmit request is accepted
 *                  E_NOT_OK Transmit request is not accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_LdCom_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_LDCOM_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_LDCOM_SUPPORT */

#if defined(PDUR_J1939DCM_SUPPORT) && (PDUR_J1939DCM_SUPPORT != 0)

#include "PduR_J1939Dcm.h"
#include "PduR_J1939Dcm_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_J1939Dcm_Transmit

 *        This function is called by the PDU-Router to request a transmission.
 *
 * \param           PduIdType gwayId  :ID of I-PDU to be transmitted.
 *                  const PduInfoType * ptr: A pointer to a structure with I-PDU related data that
 *                  shall be transmitted: data length and pointer  to I-SDU buffer
 *
 *
 * \retval          E_OK Transmit request is accepted
 *                  E_NOT_OK Transmit request is not accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_J1939Dcm_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_J1939DCM_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_J1939DCM_SUPPORT */

#if defined(PDUR_SD_SUPPORT) && (PDUR_SD_SUPPORT != 0)

#include "PduR_Sd.h"
#include "PduR_Sd_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_Sd_Transmit_Func

 *      Support function to handle multicast from PduR_ComTransmit
 *
 * \param           PduIdType id      :multicast ID to be transmitted.
 *                  const PduInfoType * info:Pointer to a structure with PDU related data that shall be transmitted:
 *                                         data length and pointer to buffer
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_Sd_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_SD_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* #if defined(PDUR_SD_SUPPORT) && (PDUR_SD_SUPPORT != 0) */

#if defined(PDUR_RBA_EBDATACLIENT_SUPPORT) && (PDUR_RBA_EBDATACLIENT_SUPPORT != 0)

#include "PduR_rba_EbDataClient.h"
#include "PduR_rba_EbDataClient_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_rba_EbDataClient_Transmit_Func

 *      Support function to handle multicast from PduR_ComTransmit
 *
 * \param           PduIdType id      :multicast ID to be transmitted.
 *                  const PduInfoType * info:Pointer to a structure with PDU related data that shall be transmitted:
 *                                         data length and pointer to buffer
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_rba_EbDataClient_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_RBA_EBDATACLIENT_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* #if defined(PDUR_RBA_EBDATACLIENT_SUPPORT) && (PDUR_RBA_EBDATACLIENT_SUPPORT != 0) */

#if defined(PDUR_IDSM_SUPPORT) && (PDUR_IDSM_SUPPORT != 0)

#include "PduR_IdsM.h"
#include "PduR_IdsM_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_MF_IdsM_Transmit_Func

 *      Support function to handle multicast from PduR_ComTransmit
 *
 * \param           PduIdType id      :multicast ID to be transmitted.
 *                  const PduInfoType * info:Pointer to a structure with PDU related data that shall be transmitted:
 *                                         data length and pointer to buffer
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_MF_IdsM_Transmit_Func( PduIdType id, const PduInfoType * info )
{
    return PduR_MF_UpToLo(PDUR_IDSM_MC_TX_BASE, id, info);
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* #if defined(PDUR_IDSM_SUPPORT) && (PDUR_IDSM_SUPPORT != 0) */
