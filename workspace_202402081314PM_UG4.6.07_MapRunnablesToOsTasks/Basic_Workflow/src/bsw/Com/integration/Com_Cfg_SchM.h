/*
* *********************************************************************************************************************
* This is a template file.
* It defines integration functions which are necessary to be implemented.
* The integrator must complete the templates before deploying software containing functions defined in this file.
* Once templates have been completed, the integrator should delete the "removal part"
* Note: The integrator is responsible for updates made to this file.
*
* To suppress following error message, define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT
* during compiler invocation (e.g. -DNOT_READY_FOR_TESTING_OR_DEPLOYMENT)
* The suppression of the error only allows the user to proceed with the building phase.
* *********************************************************************************************************************
*/

/*---- Removal start ----*/
/* Hint for the integrator: remove everything between Removal start and Removal end after proper implementation */

#ifndef NOT_READY_FOR_TESTING_OR_DEPLOYMENT
# error The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#else
# warning The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#endif /* NOT_READY_FOR_TESTING_OR_DEPLOYMENT */
/*---- Removal end ----*/

#ifndef COM_CFG_SCHM_H
#define COM_CFG_SCHM_H

#include "SchM.h"

#if ( COM_ECUC_RB_RTE_IN_USE == STD_ON )
/* If the RTE code generation is Involved please include the below file */
#include "SchM_Com.h"
#endif /* #if ( COM_ECUC_RB_RTE_IN_USE == STD_ON ) */

#define SchM_Enter_Com_RxIpduProtArea(FUNCTIONNAME) SchM_Enter_Com_RxIpduProtArea_##FUNCTIONNAME()
#define SchM_Exit_Com_RxIpduProtArea(FUNCTIONNAME) SchM_Exit_Com_RxIpduProtArea_##FUNCTIONNAME()

#define SchM_Enter_Com_TxIpduProtArea(FUNCTIONNAME) SchM_Enter_Com_TxIpduProtArea_##FUNCTIONNAME()
#define SchM_Exit_Com_TxIpduProtArea(FUNCTIONNAME) SchM_Exit_Com_TxIpduProtArea_##FUNCTIONNAME()

#define SchM_Enter_Com_RxSigBuff(FUNCTIONNAME) SchM_Enter_Com_RxSigBuff_##FUNCTIONNAME()
#define SchM_Exit_Com_RxSigBuff(FUNCTIONNAME) SchM_Exit_Com_RxSigBuff_##FUNCTIONNAME()

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_INVALIDATESIGNAL(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_INVALIDATESIGNAL(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_MAINFUNCTIONTX(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_MAINFUNCTIONTX(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDDYNSIGNAL(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDDYNSIGNAL(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDIPDU_DATA(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDIPDU_DATA(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDIPDU_TXFLAGS(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDIPDU_TXFLAGS(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDSIGNAL(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDSIGNAL(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDSIGNALGRP(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDSIGNALGRP(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TRIGGERTRANSMIT(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TRIGGERTRANSMIT(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TRIGGERIPDUSENDWITHMETADATA(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TRIGGERIPDUSENDWITHMETADATA(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SIGTXCHANGEMODE(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SIGTXCHANGEMODE(void);

#if (defined(COM_TxFilters) && defined(COM_TX_SIGNALGROUP))
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SIGGRPTXCHANGEMODE(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SIGGRPTXCHANGEMODE(void);
#endif

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TXCONFIRMATION(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TXCONFIRMATION(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SEND_DYNGROUPSIGNAL(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SEND_DYNGROUPSIGNAL(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_IPDUGROUPSTART(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_IPDUGROUPSTART(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SWITCHTXIPDU(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SWITCHTXIPDU(void);

#ifdef COM_METADATA_SUPPORT
LOCAL_INLINE void SchM_Enter_Com_RxIpduProtArea_RECEIVE_METADATA(void);
LOCAL_INLINE void SchM_Exit_Com_RxIpduProtArea_RECEIVE_METADATA(void);

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TRANSMIT_METADATA(void);
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TRANSMIT_METADATA(void);
#endif

#ifdef COM_TX_SIGNALGROUP
LOCAL_INLINE void SchM_Enter_Com_TxGrpSigBuffer(void);
LOCAL_INLINE void SchM_Exit_Com_TxGrpSigBuffer(void);
#endif /* #ifdef COM_TX_SIGNALGROUP */

LOCAL_INLINE void SchM_Enter_Com_RxPduBuffer(void);
LOCAL_INLINE void SchM_Exit_Com_RxPduBuffer(void);

#ifdef COM_RX_SIGNALGROUP
LOCAL_INLINE void SchM_Enter_Com_RxGrpSigSecBuff (void);
LOCAL_INLINE void SchM_Exit_Com_RxGrpSigSecBuff (void);
#endif /* #ifdef COM_RX_SIGNALGROUP */

#if defined( COM_RX_SIGNALGROUP_ARRAY )
LOCAL_INLINE void SchM_Enter_Com_RxSigGrpArrayBuff( void );
LOCAL_INLINE void SchM_Exit_Com_RxSigGrpArrayBuff( void );
#endif /* #if defined( COM_RX_SIGNALGROUP_ARRAY ) */

#ifdef COM_RX_SIGNALGROUP
LOCAL_INLINE void SchM_Enter_Com_RxGrpSigBuff(void);
LOCAL_INLINE void SchM_Exit_Com_RxGrpSigBuff (void);
#endif /* #ifdef COM_RX_SIGNALGROUP */

LOCAL_INLINE void SchM_Enter_Com_RxSigBuff_MAINFUNCTIONRX (void);
LOCAL_INLINE void SchM_Exit_Com_RxSigBuff_MAINFUNCTIONRX (void);

LOCAL_INLINE void SchM_Enter_Com_RxSigBuff_RECEIVESIGNAL (void);
LOCAL_INLINE void SchM_Exit_Com_RxSigBuff_RECEIVESIGNAL  (void);

LOCAL_INLINE void SchM_Enter_Com_RxSigBuff_RXINDICATION (void);
LOCAL_INLINE void SchM_Exit_Com_RxSigBuff_RXINDICATION (void);

#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
LOCAL_INLINE void SchM_Enter_Com_RxSigDynBuff (void);
LOCAL_INLINE void SchM_Exit_Com_RxSigDynBuff (void);
#endif /* #ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT */

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
LOCAL_INLINE void SchM_Enter_Com_RxGrpSigDynBuff (void);
LOCAL_INLINE void SchM_Exit_Com_RxGrpSigDynBuff (void);
#endif /* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

#ifdef COM_RxSigUpdateTimeout
LOCAL_INLINE void SchM_Enter_Com_RxSigToTicks (void);
LOCAL_INLINE void SchM_Exit_Com_RxSigToTicks (void);
#endif /* #ifdef COM_RxSigUpdateTimeout */

#ifdef COM_RxSigGrpUpdateTimeout
LOCAL_INLINE void SchM_Enter_Com_RxSigGrpToTicks(void);
LOCAL_INLINE void SchM_Exit_Com_RxSigGrpToTicks(void);
#endif /*#ifdef COM_RxSigGrpUpdateTimeout*/

#ifdef COM_PDUBASEDLOCKSENABLED

LOCAL_INLINE void SchM_Enter_Com_RxIPduProcess(void);
LOCAL_INLINE void SchM_Exit_Com_RxIPduProcess(void);

LOCAL_INLINE void SchM_Enter_Com_RxIndication(void);
LOCAL_INLINE void SchM_Exit_Com_RxIndication(void);

LOCAL_INLINE void SchM_Enter_Com_TxIPdu(void);
LOCAL_INLINE void SchM_Exit_Com_TxIPdu(void);
#endif /* #ifdef COM_PDUBASEDLOCKSENABLED */

/* NOTE to Integrator : These below Exclusive areas can be defined depending on the project scenario,
 */

/* NOTE on Exclusive area INVALIDATESIGNAL:
Resource:
The signal value is being updated in the Tx Ipdu buffer
Scenario:
The below are the possibilites of shared resource access(Inconsistent Tx Ipdu data)
If any other signal of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_SendDynSignal
 *   a. If a dynamic signal update is called for a signal from the same Ipdu
 * 2) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger-
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the stack
 * 3) Com_SendSignal
 *   a. If a signal update is called for a signal from the same Ipdu
 * 4) Com_SendSignalGroup/Com_SendSignalGroupArray
 *   a. If a signal group update is called for a signal group from the same Ipdu
 * Read Operation :
 * 1) Com_TriggerTransmit
 *   a.The data of the Ipdu could be read here
 */

LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_INVALIDATESIGNAL (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_INVALIDATESIGNAL (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area MAINFUNCTIONTX:
Resource:
The set of Tx Timers and Tx flags are updated
Scenario:
The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 2) Com_Prv_SigTxChangeMode/Com_Prv_SigGrpTxChangeMode
 *   a. a set of tx timers and Tx Flags are updated
 * 3) Com_TxConfirmation
 *   a. a set of tx timers and Tx Flags are updated
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_MAINFUNCTIONTX(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_MAINFUNCTIONTX(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}


/* NOTE on Exclusive area SENDDYNSIGNAL:
Resource:
The Data is updated into the Tx Ipdu buffer for a Dynamic signal
Scenario:
The below are the possibilites of shared resource access(Inconsistent Tx Ipdu data)
If any other signal/Signal group of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_InvalidateSignal
 *   a. If a signal update is called for a signal from the same Ipdu with invalid value
 * 2) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 3) Com_SendSignal
 *   a. If a signal update is called for a signal from the same Ipdu
 * 4) Com_SendSignalGroup/Com_SendSignalGroupArray
 *   a. If a signal group update is called for a signal group from the same Ipdu
 * Read Operation :
 * 1) Com_TriggerTransmit
 *   a.The data of the Ipdu could be read here
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDDYNSIGNAL(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDDYNSIGNAL(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area SEND_DYNGROUPSIGNAL:
Resource:
The Data is updated into the Tx Dynamic group signal shadow buffer
Scenario:
The below are the possibilites of shared resource access(Inconsistent Tx dynamic GroupSignal data)
If same GroupSignal is being updated by any of the below API's
 * Write Operation:
 * 2) Com_SendDynSignal
 *   a. If same GroupSignal update is called
 * 1) Com_InvalidateSignal
 *   a. If same GroupSignal update is called with invalid value
 * Read Operation :
 * 1) Com_SendSignalGroup
 *   a. Tx GroupSignal data from shadow buffer is copied into its assciated Tx IPDU buffer
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SEND_DYNGROUPSIGNAL(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SEND_DYNGROUPSIGNAL(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

/* NOTE on Exclusive area SENDIPDU_DATA:
Resource:
TxIPdu buffer
The set of Tx Timers and Tx flags are updated

(a. The Data is updated into the Tx Ipdu buffer via Ipdu callout
 b. IpduCounter value is updated into the Tx Ipdu buffer
 c. Default metadata is updated into the Tx Ipdu buffer
 d. Data is read by the other ComStack modules)
Scenario:
The below are the possibilites of shared resource access(Inconsistent Tx Ipdu data)
If any other signal/Signal group of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_InvalidateSignal
 *   a. If a signal update is called for a signal from the same Ipdu with invalid value
 * 2) Com_SendSignal
 *   a. If a signal update is called for a signal from the same Ipdu
 * 3) Com_SendDynSignal
 *   a. If a dynamic signal update is called for a signal from the same Ipdu
 * 4) Com_SendSignalGroup/Com_SendSignalGroupArray
 *   a. If a signal group update is called for a signal group from the same Ipdu
 * 5) Com_MainFunctionTx:
 *   a. If a gateway signal or signal group update is triggered for a signal or signal group from the same Ipdu
 * Read Operation :
 * 6) Com_TriggerTransmit
 *   a.The data of the Ipdu could be read here
 * 7) Com_Prv_SigTxChangeMode/Com_Prv_SigGrpTxChangeMode
 *   a. The data of signal group buffers is read here
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDIPDU_DATA(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDIPDU_DATA(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area SENDIPDU_TXFLAGS:
Resource:
The set of Tx Timers and Tx flags are updated
Scenario:
The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_InternalMainFunctionTx
 *   a. The Tx Ipdu data and Tx Flags are updated
 * 2) Com_Prv_SigTxChangeMode/Com_Prv_SigGrpTxChangeMode
 *   a. a set of tx timers and Tx Flags are updated when called from
 *   Com_SendSignal/Com_SendSignalGroup/Com_SendSignalGroupArray
 * 3) Com_TxConfirmation
 *   a. a set of tx timers and Tx Flags are updated
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDIPDU_TXFLAGS(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDIPDU_TXFLAGS(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area SENDSIGNAL:
Resource:
The Data is updated into the Tx Ipdu buffer for a signal
Scenario:
The below are the possibilites of shared resource access(Inconsistent Tx Ipdu data)
If any other signal/Signal group of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_InvalidateSignal
 *   a. If a signal update is called for a signal from the same Ipdu with invalid value
 * 2) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 3) Com_SendDynSignal
 *   a. If a dynamic signal update is called for a signal from the same Ipdu
 * 4) Com_SendSignalGroup/Com_SendSignalGroupArray
 *   a. If a signal group update is called for a signal group from the same Ipdu
 * 5) Com_MainFunctionTx:
 *   a. If a gateway signal or signal group update is triggered for a signal or signal group from the same Ipdu
 * Read Operation :
 * 1) Com_TriggerTransmit
 *   a.The data of the Ipdu could be read here
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDSIGNAL(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDSIGNAL(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
/* NOTE on Exclusive area SENDSIGNALGRP:
Resource:
The Data is updated into the Tx Ipdu buffer for a signal group from a shadow buffer
Scenario:
The below are the possibilites of shared resource access (Inconsistent Tx Ipdu data)
If any other signal/Signal group of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_InvalidateSignal
 *   a. If a signal update is called for a signal from the same Ipdu with invalid value
 * 2) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 3) Com_SendDynSignal
 *   a. If a dynamic signal update is called for a signal from the same Ipdu
 * 4) Com_SendSignal
 *   a. If a signal update is called for a signal  from the same Ipdu
 * 5) Com_MainFunctionTx:
 *   a. If a gateway signal or signal group update is triggered for a signal or signal group from the same Ipdu
 * 6) Com_SendSignalGroupArray:
 *   a. If a SignalGroup update is called for a SignalGroup from the same IPdu
 * Read Operation :
 * 1) Com_TriggerTransmit
 *   a.The data of the Ipdu could be read here
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SENDSIGNALGRP(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SENDSIGNALGRP(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
/* NOTE on Exclusive area TRIGGERTRANSMIT:
Resource:
The Data is being read from the Tx Ipdu buffer
Dynamic length is being read.
Scenario:
The below are the possibilites of shared resource access (Inconsistent read of Tx Ipdu data)
If any other signal/Signal group of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_InvalidateSignal
 *   a. If a signal update is called for a signal from the same Ipdu with invalid value
 * 2) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 3) Com_SendDynSignal
 *   a. If a dynamic signal update is called for a signal from the same Ipdu
 * 4) Com_SendSignal
 *   a. If a signal update is called for a signal  from the same Ipdu
 * 5) Com_MainFunctionTx:
 *   a. If a gateway signal or signal group update is triggered for a signal or signal group from the same Ipdu
 * 6) Com_SendSignalGroup/Com_SendSignalGroupArray
 *   a. If a signal group data update is called for a signal group  from the same Ipdu
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TRIGGERTRANSMIT(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TRIGGERTRANSMIT(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area TRIGGERIPDUSENDWITHMETADATA:
 * Resource:
 * The Data is updated into the Tx Ipdu buffer for a signal
 * Scenario:
 * The below are the possibilites of shared resource access(Inconsistent Tx Ipdu data)
 * If any other signal/Signal group of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_InvalidateSignal
 *   a. If a signal update is called for a signal from the same Ipdu with invalid value
 * 2) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 3) Com_SendSignal
 *   a. If a signal update is called for a signal  from the same Ipdu
 * 4) Com_SendDynSignal
 *   a. If a dynamic signal update is called for a signal from the same Ipdu
 * 5) Com_SendSignalGroup/Com_SendSignalGroupArray
 *   a. If a signal group update is called for a signal group from the same Ipdu
 * 6) Com_MainFunctionTx:
 *   a. If a gateway signal or signal group update is triggered for a signal or signal group from the same Ipdu
 * 7) Com_TriggerIPDUSendWithMetaData:
 *      Tx IPdu Buffer can be updated in this API
 *
 * Read Operation :
 * 1) Com_TriggerTransmit
 *   a.The data of the Ipdu could be read here
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TRIGGERIPDUSENDWITHMETADATA(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TRIGGERIPDUSENDWITHMETADATA(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area SIGTXCHANGEMODE:
Resource:
The set of Tx Timers and Tx flags are updates
Scenario:
The below are the possibilites of shared resource access(Inconsistent read of Tx Ipdu data)
If any other signal of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 2) Com_InternalMainFunctionTx
 *   a. a set of tx timers and Tx Flags are updated
 * 3) Com_TxConfirmation
 *   a. a set of tx timers and Tx Flags are updated
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SIGTXCHANGEMODE(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SIGTXCHANGEMODE(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

#if (defined(COM_TxFilters) && defined(COM_TX_SIGNALGROUP))
/* NOTE on Exclusive area SIGGRPTXCHANGEMODE:
Resource:
The set of Tx group signal flags are updated and read operation of Tx group signal shadow buffers
Scenario:
The below are the possibilites of shared resource access(Inconsistent read of Tx Ipdu data)
If any other signalgroup of this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_MainFunctionTx:
 *   a. If a gateway signal group update is triggered for a signal group from the same Ipdu, this could result in inconsistent read here
 * 2)Com_SendSignalGroup/Com_SendSignalGroupArray:
 *   a. If a signal group data update is triggered for a signal group from the same Ipdu
 * 3) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 4) Com_InternalMainFunctionTx
 *   a. a set of tx timers and Tx Flags are updated
 * 5) Com_TxConfirmation
 *   a. a set of tx timers and Tx Flags are updated
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SIGGRPTXCHANGEMODE(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SIGGRPTXCHANGEMODE(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #if (defined(COM_TxFilters) && defined(COM_TX_SIGNALGROUP)) */

/* NOTE on Exclusive area TXCONFIRMATION:
Resource:
The set of Tx Timers and Tx flags are updated
Scenario:
The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 2) Com_Prv_SigTxChangeMode/Com_Prv_SigGrpTxChangeMode
 *   a. a set of tx timers and Tx Flags are updated when called from Com_SendSignal/Com_SendSignalGroup/Com_SendSignalGroupArray
 * 3) Com_InternalMainFunctionTx
 *   a. a set of tx timers and Tx Flags are updated
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TXCONFIRMATION(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TXCONFIRMATION(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}


/* NOTE on Exclusive area IPDUGROUPSTART:
 * Resource:
 * Tx Ipdu data buffer
 * Scenario:
 * Lock is applicable only in case Signal/SignalGroup data is updated while TxIpdu is stopped.
 * The below are the possibilites of shared resource access(Inconsistent Tx Ipdu data)
 * If any other signal/Signal group of a this particular Ipdu is being updated by any of the below API's
 * Write Operation:
 * 1) Com_InvalidateSignal
 *   a. If a signal update is called for a signal from the same Ipdu with invalid value
 * 2) Com_SendSignal
 *   a. If a signal update is called for a signal  from the same Ipdu
 * 3) Com_SendDynSignal
 *   a. If a dynamic signal update is called for a signal from the same Ipdu
 * 4) Com_InvalidateSignalGroup
 *   a. If a signal group is invalidated for the same Ipdu
 * 5) Com_SendSignalGroup/Com_SendSignalGroupArray
 *   a. If a signal group update is called for a signal group from the same Ipdu
 *
 * Read Operation :
 * 1) Com_TriggerTransmit
 *   a.The data of the Ipdu could be read here(even for stopped TxIpdu)
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_IPDUGROUPSTART(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_IPDUGROUPSTART(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}


/* NOTE on Exclusive area SWITCHTXIPDU:
 * Resource:
 * Tx Ipdu timers and flags
 * Scenario:
 * The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_Prv_SendIpdu
 *   a. If the transmission is initiated for the Ipdu from Com_InternalMainFunctionTx/Event trigger then
 *      a1. Callout can update Tx Ipdu
 *      a2. Metadata can be updated
 *      a3. IpduCounter value can be updated.
 *   b. The Tx Ipdu data can be updated by other modules along the communication stack
 * 2) Com_Prv_SigTxChangeMode/Com_Prv_SigGrpTxChangeMode
 *   a. a set of tx timers and Tx Flags are updated when called from Com_SendSignal/Com_SendSignalGroup/Com_SendSignalGroupArray
 * 3) Com_InternalMainFunctionTx
 *   a. a set of tx timers and Tx Flags are updated
 * 4) Com_TxConfirmation or Com_TpTxConfirmation
 *   a. a set of Tx Flags are updated
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_SWITCHTXIPDU(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_SWITCHTXIPDU(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

#ifdef COM_METADATA_SUPPORT

/* NOTE on Exclusive area RECEIVE_METADATA:
 * Resource:
 * Rx metadata Ipdu
 * Scenario:
 * The below are the possibilites of shared resource access
 * Read Operation:
 * 1) Com_ReceiveSignalWithMetadata
 *   a. During reception of signal with metadata value
 * 2) Com_ReceiveDynSignalWithMetadata
 *   a. During reception of dynamic signal or dynamic groupsignal with metadata value
 * 3) Com_ReceiveSignalGroupWithMetaData
 *   a. During reception of signal group with metadata value
 * 4) Com_ReceiveSignalGroupArrayWithMetaData
 *   a. During reception of signal group array with metadata value
 */
LOCAL_INLINE void SchM_Enter_Com_RxIpduProtArea_RECEIVE_METADATA(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_RxIpduProtArea_RECEIVE_METADATA(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area TRANSMIT_METADATA:
 * Resource:
 * Tx metadata Ipdu
 * Scenario:
 * The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_SendSignalWithMetaData
 *   a. During transmission of signal with metadata value
 * 2) Com_SendDynSignalWithMetaData
 *   a. During transmission of dynamic signal or dynamic groupsignal with metadata value
 * 3) Com_SendSignalGroupWithMetaData
 *   a. During transmission of signal group with metadata value
 * 4) Com_SendSignalGroupArrayWithMetaData
 *   a. During transmission of signal group array with metadata value
 */
LOCAL_INLINE void SchM_Enter_Com_TxIpduProtArea_TRANSMIT_METADATA(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIpduProtArea_TRANSMIT_METADATA(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area RECEIVE_METADATA:
 * Resource:
 * Rx metadata Ipdu
 * Scenario:
 * The below are the possibilites of shared resource access
 * Read Operation:
 * 1) Com_RxIndication.c
 *   a. During reception of I-PDU
 */
LOCAL_INLINE void SchM_Enter_Com_RxIPdu_RECEIVE_METADATA(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_RxIPdu_RECEIVE_METADATA(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

/* NOTE on Exclusive area TRANSMIT_METADATA:
 * Resource:
 * Tx metadata Ipdu
 * Scenario:
 * The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_SendIPdu
 *   a. During transmission of I-PDU
 */
LOCAL_INLINE void SchM_Enter_Com_TxIPdu_TRANSMIT_METADATA(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_TxIPdu_TRANSMIT_METADATA(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}

#endif

#ifdef COM_TX_SIGNALGROUP
/* NOTE on Exclusive area of GRPSIGNALBUFFER: Can be generated
 * The Group signal shadow buffer is read/written as below
 * Write Operation:
 * 1) Com_InvalidateSignal
 * 2) Com_SendSignal
 * Read Operation :
 * 1) Com_SendSignalGroup
 * 2) Com_IpduGroupStart to start Ipdu group.
 */

LOCAL_INLINE void SchM_Enter_Com_TxGrpSigBuffer(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_TxGrpSigBuffer(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #ifdef COM_TX_SIGNALGROUP */

/* NOTE on Exclusive area of RXIPDUBUFFER:
 * Critical section: RxIPdu-buffer, RxIPdu-Flags
 * Scenario:
 * Write operation:
 *  a. Com_RxIndication:             RxIPdu-buffer and RxIPdu-Flags
 *  b. Com_InternalMainFunctionRx:   RxIPdu-Flags
 * Read operation:
 *  a. Com_InternalMainFunctionRx:   RxIPdu-buffer and RxIPdu-Flags
 *
 * If there is a possibility that the above mentioned APIs can interrupt each other,
 * then this exclusive area needs to be enabled.
 */
LOCAL_INLINE void SchM_Enter_Com_RxPduBuffer(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_RxPduBuffer(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}


#ifdef COM_RxSigUpdateTimeout
/* NOTE on Exclusive area of Rx-Signal with UpdateBit
 * The signal buffer is read/written as below
 * Write Operation:
 * 1) Com_EnableReceptionDM
 * 2) Com_InternalMainFunctionRx
 * 3) Com_RxIndication
 * Read Operation :
 * 1) Com_EnableReceptionDM
 * 2) Com_InternalMainFunctionRx
 * 3) Com_RxIndication
 * */
LOCAL_INLINE void SchM_Enter_Com_RxSigToTicks (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxSigToTicks (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #ifdef COM_RxSigUpdateTimeout */

#ifdef COM_RxSigGrpUpdateTimeout
/* NOTE on Exclusive area of SigUpdatebitTimer: Can be generated
 * The Group signal shadow buffer is read/written as below
 * Write Operation:
 * 1) Com_EnableReceptionDM
 * 2) Com_InternalMainFunctionRx
 * 3) Com_RxIndication
 * Read Operation :
 * 1) Com_EnableReceptionDM
 * 2) Com_InternalMainFunctionRx
 * 3) Com_RxIndication
 * */
LOCAL_INLINE void SchM_Enter_Com_RxSigGrpToTicks (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxSigGrpToTicks (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /*#ifdef COM_RxSigGrpUpdateTimeout */

/* NOTE on Exclusive area MAINFUNCTIONRX:
Resource:
1. The Rx signal buffer is updated here in case of timeout
2. Rx Timeout operation consistency is to be ensured using this lock
3. Rx signal buffer is read for gatewaying
Scenario:
The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_RxIndication
 *   a. Signal data is updated per signal into the signal buffers
 *   b. Signal data is updated for float signals
 *    Read Operation:
 * 2) Com_ReceiveSignal
 *   a. Signal data is read from the signal buffers.
 */
LOCAL_INLINE void SchM_Enter_Com_RxSigBuff_MAINFUNCTIONRX (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}
LOCAL_INLINE void SchM_Exit_Com_RxSigBuff_MAINFUNCTIONRX (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}


/* NOTE on Exclusive area RXINDICATION:
Resource:
1. The Rx signal buffer is updated here in case of Ipdu reception
Scenario:
The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_InternalMainFunctionRx
 *   a. Signal data is updated per signal after timeout
 *    Read Operation:
 * 2) Com_ReceiveSignal
 *   a. Signal data is read from the signal buffers.
 */
LOCAL_INLINE void SchM_Enter_Com_RxSigBuff_RXINDICATION (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxSigBuff_RXINDICATION (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}


/* NOTE on Exclusive area RECEIVESIGNAL:
Resource:
1. The Rx signal buffer is read here after Ipdu reception
Scenario:
The below are the possibilites of shared resource access
 * Write Operation:
 * 1) Com_InternalMainFunctionRx
 *   a. Signal data is updated per signal after timeout
 * 2) Com_RxIndication
 *   a. Signal data is updated per signal into the signal buffers
 *   b. Signal data is updated for float signals
 */
LOCAL_INLINE void SchM_Enter_Com_RxSigBuff_RECEIVESIGNAL (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxSigBuff_RECEIVESIGNAL  (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}


#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
/* NOTE on Exclusive area of Rx Signal buffer: Can be generated
 * The application buffer is updated here, and the protection is intended for Rx dynamic signal buffer
 * Write Operation:
 * 1) Com_RxIndication
 * Read Operation :
 * 1) Com_ReceiveSignal
 */
LOCAL_INLINE void SchM_Enter_Com_RxSigDynBuff (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxSigDynBuff (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT */

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
/* NOTE on Exclusive area of Rx Group Signal buffer: Can be generated
 * The application buffer is updated here, and the protection is intended for Rx dynamic group signal buffer
 * Write Operation:
 * 1) Com_RxIndication
 * Read Operation :
 * 1) Com_ReceiveDynSignal
 */
LOCAL_INLINE void SchM_Enter_Com_RxGrpSigDynBuff (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxGrpSigDynBuff (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

#ifdef COM_RX_SIGNALGROUP
/* NOTE on Exclusive area of RXGRPSIGNALSECONDBUFFER: Can be generated
 * The Group signal shadow buffer is read/written as below
 * Write Operation:
 * 1) Com_RxIndication
 * 2) Com_InternalMainFunctionRx
 * Read Operation :
 * 1) Com_ReceiveSignalGroup
 * 2) Com_MainFunctionRx - SignalGroup Gateway from Rx GroupSignal buffer to FIFO
 */
/* As the lock is around the loop for all the group signals the Interrupt locking time can vary*/
LOCAL_INLINE void SchM_Enter_Com_RxGrpSigSecBuff (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxGrpSigSecBuff(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #ifdef COM_RX_SIGNALGROUP */

#if defined( COM_RX_SIGNALGROUP_ARRAY )
/* NOTE on Exclusive area of RX SignalGroup Array Buffer: Can be generated
 * The SignalGroup Array buffer is read/written as below
 * Write Operation:
 *  1) Com_RxIndication
 *  2) Com_InternalMainFunctionRx
 * Read Operation :
 *  1) Com_ReceiveSignalGroupArray
 */
/* As the lock is around the loop for all the group signals the Interrupt locking time can vary*/
LOCAL_INLINE void SchM_Enter_Com_RxSigGrpArrayBuff( void )
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxSigGrpArrayBuff( void )
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #if defined( COM_RX_SIGNALGROUP_ARRAY ) */

#ifdef COM_RX_SIGNALGROUP
/* NOTE on Exclusive area of Rx group Signal buffer: Can be generated
 * The application buffer is updated here, and the protection is intended for Rx group signal buffer
 * Write Operation:
 * 1) Com_ReceiveSignalGroup
 * Read Operation :
 * 1) Com_ReceiveSignal
 *
 * Use case scenario: Use below locks only in case, Com_ReceiveSignalGroup & Com_ReceiveSignal api's
 * can interrupt each other. If Com_ReceiveSignalGroup & Com_ReceiveSignal api's are called from the
 * same task context, then below locks can be left empty.
 */
/* As the lock is around the loop for all the group signals the Interrupt locking time can vary*/
LOCAL_INLINE void SchM_Enter_Com_RxGrpSigBuff (void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxGrpSigBuff (void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
#endif /* #ifdef COM_RX_SIGNALGROUP */


/**********************************************************************************************************************
 * Pdu based locks : Below locks are applicable only in case 'ComEnablePduBasedLocks' parameter is enabled and
 * use these locks to avoid multiple short locks
 *********************************************************************************************************************/

/* This lock shall be enabled in Com_InternalMainFunctionRx()
 * Which protects the complete Rx Ipdu processing and avoids multiple short locks:
 * The below locks can be disabled and a common Pdu(SchM_Enter_Com_RxIPduProcess) lock can be enabled
 * SchM_Enter_Com_RxPduBuffer()
 * SchM_Enter_Com_RxSigBuff_MAINFUNCTIONRX()
 * SchM_Enter_Com_RxSigToTicks()
 * SchM_Enter_Com_RxSigGrpToTicks()
 */
# ifdef COM_PDUBASEDLOCKSENABLED
LOCAL_INLINE void SchM_Enter_Com_RxIPduProcess(void)
{
     /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxIPduProcess(void)
{
     /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
# endif /* #ifdef COM_PDUBASEDLOCKSENABLED */


/* This lock shall be enabled in Com_RxIndication()
 * Which protects the complete Rx Ipdu processing and avoids multiple short locks:
 * The below locks can be disabled and a common Pdu (SchM_Enter_Com_RxIndication) lock can be enabled
 * SchM_Enter_Com_RxPduBuffer()
 * SchM_Enter_Com_RxSigToTicks()
 * SchM_Enter_Com_RxSigBuff_RXINDICATION()
 * SchM_Enter_Com_RxSigDynBuff()
 * SchM_Enter_Com_RxSigGrpToTicks()
 * SchM_Enter_Com_RxSigGrpArrayBuff()
 * SchM_Enter_Com_RxGrpSigSecBuff()
 */
# ifdef COM_PDUBASEDLOCKSENABLED
LOCAL_INLINE void SchM_Enter_Com_RxIndication(void)
{
     /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_RxIndication(void)
{
     /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
# endif /* #ifdef COM_PDUBASEDLOCKSENABLED */


/* This lock shall be enabled in Com_Prv_SendIpdu()
 * Which protects the complete Tx Ipdu processing and avoids multiple short locks:
 * The below locks can be disabled and a common Pdu (SchM_Enter_Com_TxIPdu) lock can be enabled
 * SchM_Enter_Com_TxIpduProtArea()
 */
# ifdef COM_PDUBASEDLOCKSENABLED
LOCAL_INLINE void SchM_Enter_Com_TxIPdu(void)
{
    /*The integrator shall place his code here which would disable interrupt or acquire lock*/
}

LOCAL_INLINE void SchM_Exit_Com_TxIPdu(void)
{
    /*The integrator shall place his code here which would re-enable interrupt or release lock */
}
# endif /* #ifdef COM_PDUBASEDLOCKSENABLED */


#endif /* COM_CFG_SCHM_H */

