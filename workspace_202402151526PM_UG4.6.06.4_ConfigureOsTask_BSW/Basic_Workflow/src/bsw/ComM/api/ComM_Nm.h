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
\defgroup COMM_NM_H    COMM - AUTOSAR interfaces Upper Layer
*/

/* EasyCASE - */
#ifndef COMM_NM_H
#define COMM_NM_H
/* EasyCASE ( 106
   Includes */

#include "ComM_Cfg.h"
#include "ComStack_Types.h"

/* EasyCASE ) */
/* EasyCASE ( 162
   #Defines */
/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/* EasyCASE ) */
/* EasyCASE ( 168
   Type Definitions */
/* EasyCASE ) */
/* EasyCASE ( 38
   Compatibility Check */
/* EasyCASE ) */
/* EasyCASE ( 167
   Extern Declarations */
/*
 ***************************************************************************************************
 * Extern declarations
 ***************************************************************************************************
 */
/* EasyCASE ( 274
   Code */
/* EasyCASE ( 269
   ComM_Nm_NetworkStartIndication */

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

/**
 * @ingroup COMM_NM_H
 *
 * Indication that a NM-message has been received in the Bus Sleep Mode,what indicates that some nodes in the network\n
 * have already entered the Network Mode. \n
 *
 *  @param  In:      Channel -Handle to identify a communication channel \n
 *
 *  @return None \n
 */
extern void ComM_Nm_NetworkStartIndication(NetworkHandleType Channel);
/* EasyCASE ) */
/* EasyCASE ( 270
   ComM_Nm_BusSleepMode */
/**
 * @ingroup COMM_NM_H
 *
 * Notification that the network management has entered Bus-Sleep Mode. This callback function should perform a \n
 * transition of the hardware and transceiver to bus-sleep mode. \n
 *
 *  @param  In:      Channel -Handle to identify a communication channel\n
 *
 *  @return None \n
 */
extern void ComM_Nm_BusSleepMode(NetworkHandleType Channel);
/* EasyCASE ) */
/* EasyCASE ( 271
   ComM_Nm_NetworkMode */
/**
 * @ingroup COMM_NM_H
 *
 * Notification that the network management has entered Network Mode. \n
 *
 *  @param  In:      Channel- Handle to identify a communication channel\n
 *
 *  @return None \n
 */
extern void ComM_Nm_NetworkMode(NetworkHandleType Channel);
/* EasyCASE ) */
/* EasyCASE ( 272
   ComM_Nm_RestartIndication */

/**
 * @ingroup COMM_NM_H
 *
 * If NmIf has started to shut down the coordinated busses, AND not all coordinated busses have indicated
 * bus sleep state, \n
 * AND on at least on one of the coordinated busses NM is restarted, THEN the NM Interface shall call the
 * callback function \n
 * ComM_Nm_RestartIndication with the nmNetworkHandle of the channels which have already indicated bus sleep state. \n
 *
 *  @param  In:      Channel -Handle to identify a communication channel\n
 *
 *  @return None \n
 */
extern void ComM_Nm_RestartIndication(NetworkHandleType Channel);
/* EasyCASE ) */
/* EasyCASE ( 273
   ComM_Nm_PrepareBusSleepMode */
/**
 * @ingroup COMM_NM_H
 *
 * Notification that the network management has entered Prepare Bus-Sleep Mode. Reentrancy: Reentrant
 *                                                                              (but not for the same NM-Channel) \n
 *
 *  @param  In:      Channel- Handle to identify a communication channel\n
 *
 *  @return None \n
 */
extern void ComM_Nm_PrepareBusSleepMode(NetworkHandleType Channel);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ) */
/**
 * @ingroup COMM_NM_H
 *
 * Indicates that NM message transmission has failed \n
 *
 *  @param  In:      Channel- Handle to identify a communication channel\n
 *
 *  @return None \n
 */
extern void ComM_Nm_TransmissionFailure(NetworkHandleType Channel_tu8);

/**
 * @ingroup COMM_NM_H
 *
 * Indicates that NM-Timeout time has expired when Network is still not released \n
 *
 *  @param  In:      Channel- Handle to identify a communication channel \n
 *
 *  @return None \n
 */

extern void ComM_Nm_NetworkTimeoutException(NetworkHandleType Channel_tu8);


/**
 * @ingroup COMM_NM_H
 *
 * Indicates that Request for Synchronization Shutdown is received \n
 *
 *  @param  In:      Channel- Handle to identify a communication channel \n
 *
 *  @return None \n
 */
extern void ComM_Nm_ForwardSynchronizedPncShutdown(NetworkHandleType Channel_u8);


#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif /*COMM_NM_H*/
/* EasyCASE ) */

