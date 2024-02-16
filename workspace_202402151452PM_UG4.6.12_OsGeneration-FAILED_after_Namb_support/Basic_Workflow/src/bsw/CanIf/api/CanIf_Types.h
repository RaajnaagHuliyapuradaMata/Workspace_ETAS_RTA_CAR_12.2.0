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


#ifndef CANIF_TYPES_H
#define CANIF_TYPES_H

/**************************************************************************************************/
/*              Include Section                                                                   */
/**************************************************************************************************/

/* Include the ComStack definitions */
#include "ComStack_Types.h"                 /*Version check for ComStack_types and CanIf done in CanIf_Priv.h*/

/* Include the Can_GeneralTypes definitions */

#include "Can_GeneralTypes.h"


/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************
 */

/**
 * The PduMode of a channel defines its transmit or receive activity.
 * Communication direction (transmission and/or reception) of the channel can be
 * controlled separately or together by upper layers.
 */
typedef enum
{
    /**
     *
     * = 0
     * Transmit and receive path of the corresponding channel are disabled
     * => no communication mode
     *
     */
    CANIF_OFFLINE = 0,
    /**
     * Transmit path of the corresponding channel is disabled. The receive path is
     * enabled.
     */
    CANIF_TX_OFFLINE = 1,
    /**
     * Transmit path of the corresponding channel is in offline active mode (see
     * SWS_CANIF_00072). The receive path is disabled.
     * This mode requires CanIfTxOfflineActiveSupport = TRUE.
     */
    CANIF_TX_OFFLINE_ACTIVE = 2,
    /**
     *
     * Transmit and receive path of the corresponding channel are enabled
     * => full operation mode
     *
     */
    CANIF_ONLINE = 3,


    /*PduModes for CanTpTxOfflineMode feature*/
    CANIF_TX_TP_ONLINE = 4,
    CANIF_TX_USER_TP_ONLINE = 5


}CanIf_PduModeType;

/**
 * Return value of CAN L-PDU notification status.
 */
typedef enum
{
    /**
     * = 0
     * No transmit or receive event occurred for the requested L-PDU.
     */
    CANIF_NO_NOTIFICATION = 0,
    /**
     * The requested Rx/Tx CAN L-PDU was successfully transmitted or received.
     */
    CANIF_TX_RX_NOTIFICATION
}CanIf_NotifStatusType;




#endif   /* CANIF_TYPES_H */
