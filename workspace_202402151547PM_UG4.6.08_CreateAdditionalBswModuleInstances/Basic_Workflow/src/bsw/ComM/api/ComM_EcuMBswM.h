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
\defgroup COMM_ECUMBSWM_H    COMM - AUTOSAR interfaces Upper Layer
*/
#ifndef COMM_ECUMBSWM_H
#define COMM_ECUMBSWM_H

#include "ComStack_Types.h"
#include "ComM_Cfg.h"

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/*
 ***************************************************************************************************
 * Extern declarations
 ***************************************************************************************************
 */

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

/**
 * @ingroup COMM_ECUMBSWM_H
 *
 * This function indicates the Notification of a wake up on the corresponding channel. \n
 *
 *  @param  In:      Channel- This parameter indicates Channel needed for wakeup indication   \n
 *
 *  @return None \n
 */
extern void ComM_EcuM_WakeUpIndication(NetworkHandleType Channel);


/**
 * @ingroup COMM_ECUMBSWM_H
 *
 * This function indicates the Notification of a wake up on the corresponding channel. \n
 *
 *  @param  In:      PNCid- This parameter indicates PNC needed for wakeup indication   \n
 *
 *  @return None \n
 */
extern void ComM_EcuM_PNCWakeUpIndication(PNCHandleType PNCid);

/**
 * @ingroup COMM_ECUMBSWM_H
 *
 * EcuM or BswM shall indicate to ComM when communication is allowed. \n
 *
 *  @param  In:      Channel- This parameter indicates Channel needed for CommunicationAllowed   \n
 *  @param  In:      Allowed- TRUE: Communication is allowed \n
 *                            FALSE: Communication is not allowed \n
 *
 *  @return None \n
 */
extern void ComM_CommunicationAllowed(NetworkHandleType Channel, boolean Allowed);

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif /*COMM_ECUMBSWM_H*/

