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

/**
\defgroup COMM_TYPES_H    COMM - AUTOSAR interfaces Upper Layer
*/
#ifndef COMM_TYPES_H
#define COMM_TYPES_H

#include "ComStack_Types.h"

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/**
 * @ingroup COMM_TYPES_H
 *
 * Macro used to define passive ComMPncGatewayType.
 */
typedef enum
{
    COMM_GATEWAY_TYPE_ACTIVE,
    COMM_GATEWAY_TYPE_PASSIVE,
    COMM_GATEWAY_TYPE_NONE
}ComM_PncGatewayType_ten;

/**
 * @ingroup COMM_TYPES_H
 *
 * Macro used to define PNC Co-ordinator Type.
 */
typedef enum
{
    COMM_TOP_LEVEL_PNC,
    COMM_INTERMEDIATE_PNC,
    COMM_SUBORDINATE_PNC
}ComM_PncCoordinatorType_ten;

/**
 * @ingroup COMM_TYPES_H
 *
 * Macro used to define ComM transmission.
 */
#define COMM_TX                             0
/**
 * @ingroup COMM_TYPES_H
 *
 * Macro used to define ComM reception.
 */
#define COMM_RX                             1
/**
 * @ingroup COMM_TYPES_H
 *
 * Macro used to define ComM EIRA
 */
#define COMM_EIRA                           0
/**
 * @ingroup COMM_TYPES_H
 *
 * Macro used to define ComM ERA
 */
#define COMM_ERA                            1

#ifndef COMM_NO_COMMUNICATION
#define COMM_NO_COMMUNICATION               (0u)
#endif
#ifndef COMM_SILENT_COMMUNICATION
#define COMM_SILENT_COMMUNICATION           (1u)
#endif
#ifndef COMM_FULL_COMMUNICATION
#define COMM_FULL_COMMUNICATION             (2u)
#endif
#ifndef COMM_FULL_COMMUNICATION_WITH_WAKEUP_REQUEST
#define COMM_FULL_COMMUNICATION_WITH_WAKEUP_REQUEST             (3u)
#endif
/*--------------------------------------------------------------*/
/*   Initialization status of ComM                              */
/*--------------------------------------------------------------*/
/**
 * @ingroup COMM_TYPES_H
 *  This enum is used to define the initialization status of ComM           \n
typedef enum
{
    COMM_UNINIT,        The COM Manager is not initialized or not usable.   \n
    COMM_INIT           The COM Manager is initialized and usable.          \n
} ComM_InitStatusType;
 */
typedef enum
{
    COMM_UNINIT,
    COMM_INIT
} ComM_InitStatusType;

/* ComM_BusType_ten */
/**
 * @ingroup COMM_TYPES_H
 *  This enum is used to define the bus types of ComM   \n
typedef enum
{
    COMM_BUS_TYPE_CAN,          CAN bus type        \n
    COMM_BUS_TYPE_FR,           FlexRay bus type    \n
    COMM_BUS_TYPE_INTERNAL,     Internal bus type   \n
    COMM_BUS_TYPE_LIN,          LIN bus type        \n
    COMM_BUS_TYPE_ETH           Ethernet bus type   \n
} ComM_BusType_ten;
 */
typedef enum
{
    COMM_BUS_TYPE_CAN,
    COMM_BUS_TYPE_ETH,
    COMM_BUS_TYPE_FR,
    COMM_BUS_TYPE_INTERNAL,
    COMM_BUS_TYPE_LIN,
    COMM_BUS_TYPE_CDD
} ComM_BusType_ten;

/* ComM_NMVariantType_ten */
   /**
 * @ingroup COMM_TYPES_H
 *  This enum is used to define the variant types of ComM                               \n
typedef enum
{
    FULL,           NM available (default).                                                 \n
    LIGHT,          NM not available but functionality to shut down a channel is available. \n
    NONE,           No NM available.                                                        \n
    PASSIVE,        NM running in passive mode available.                                   \n
    SLAVE_ACTIVE,   NM not available but synchronized by its master.                        \n
    SLAVE_PASSIVE   NM not available but synchronized by its master.                        \n
} ComM_NMVariantType_ten;
 */
typedef enum
{
    FULL,
    LIGHT,
    NONE,
    PASSIVE,
    SLAVE_ACTIVE,
    SLAVE_PASSIVE
} ComM_NMVariantType_ten;

   /* Type Definitions */
   /* ComM_StateType */

/**
 * @ingroup COMM_TYPES_H
 *  This enum is used to define the state types of ComM \n
typedef enum
{
    COMM_NO_COM_NO_PENDING_REQUEST ,    no pending request in no communication mode     \n
    COMM_NO_COM_REQUEST_PENDING,        request pending in no communication mode        \n
    COMM_FULL_COM_NETWORK_REQUESTED,    network requested in full communication mode    \n
    COMM_FULL_COM_READY_SLEEP,          full communication ready sleep state            \n
    COMM_SILENT_COM                     silent communication state                      \n
} ComM_StateType;
 */
typedef enum
{
    COMM_NO_COM_NO_PENDING_REQUEST,
    COMM_NO_COM_REQUEST_PENDING,
    COMM_FULL_COM_NETWORK_REQUESTED,
    COMM_FULL_COM_READY_SLEEP,
    COMM_SILENT_COM
} ComM_StateType;



/**
 * @ingroup COMM_TYPES_H
 *  This enum is used to define the PNC mode types of ComM  \n
typedef enum
{
    COMM_PNC_REQUESTED,                      PNC is requested by a local ComM user      \n
    COMM_PNC_READY_SLEEP,                    PNC is requested by a remote ComM user     \n
    COMM_PNC_PREPARE_SLEEP,                  PNC is active with no deadline monitoring  \n
    COMM_PNC_NO_COMMUNICATION,               PNC does not communicate                   \n
    COMM_PNC_REQUESTED_WITH_WAKEUP_REQUEST   PNC is requested by a local ComM user      \n
} ComM_PncModeType;
 */
typedef enum
{
    COMM_PNC_REQUESTED,
    COMM_PNC_READY_SLEEP,
    COMM_PNC_PREPARE_SLEEP,
    COMM_PNC_NO_COMMUNICATION,
    COMM_PNC_REQUESTED_WITH_WAKEUP_REQUEST
} ComM_PncModeType;

/*
 ***************************************************************************************************
 * Extern declarations
 ***************************************************************************************************
 */
#endif /*COMM_TYPES_H*/

