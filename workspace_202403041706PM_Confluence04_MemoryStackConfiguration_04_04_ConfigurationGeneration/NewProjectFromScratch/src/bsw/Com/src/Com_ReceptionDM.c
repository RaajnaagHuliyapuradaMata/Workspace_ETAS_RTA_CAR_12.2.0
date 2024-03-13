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

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Com_Prv.h"
#include "Com_Prv_Inl.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/
#ifdef COM_RxIPduTimeout

LOCAL_INLINE uint16 Com_Prv_GetRxIpduFirstTimeout(Com_IpduId_tuo idIpdu_uo);

#endif

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

#if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout)

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_EnableRxDeadlineMonitoring
 Description      : Function to enable the Rx DeadlineMonitoring.
 Parameter        : idIpdu_uo -> Id of the Rx ipdu
 Return value     : returns true if Rx Deadline monitoring is to be enabled for the given Rx IPDU
 **********************************************************************************************************************
*/
boolean Com_Prv_EnableRxDeadlineMonitoring(Com_IpduId_tuo idIpdu_uo)
{
    Com_RxIpduCfg_tpcst                 rxIpduConstPtr_pcst;
#ifdef COM_RxIPduTimeout
    Com_RxIpduRam_tpst                  rxIpduRamPtr_pst;
#endif
#if (defined(COM_RxSigUpdateTimeout) || defined(COM_RxSigGrpUpdateTimeout))
    const Com_SignalTimeoutInfo_tst *    sigTimeOutConstPtr_pcst;
    uint16 *                             sigTimeOutRamPtr_pu16;
    uint16_least                         index_qu16;
    uint16_least                         numOfSigWithUpdateBit_qu16;
#endif
    boolean                              isRxIpduDmReq_b;

    isRxIpduDmReq_b     = COM_FALSE;

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);

#ifdef COM_RxIPduTimeout
    rxIpduRamPtr_pst    = rxIpduConstPtr_pcst->ipduRam_pst;

    /* Rx Deadline monitoring is required only if ComTimeout(non-zero) is configured, independent of FirstTimeout */
    if (rxIpduConstPtr_pcst->timeout_u16 != COM_ZERO)
    {
        isRxIpduDmReq_b = COM_TRUE;
    }

    /* If First timeout is not configured , load the timer with specific value ie COM_RXTIMER_MAX(0xFFFF).
    * If First timeout is configured then load the actual value
    */
    rxIpduRamPtr_pst->cntrRxTimeout_u16 = Com_Prv_GetRxIpduFirstTimeout(idIpdu_uo);

#endif /* #ifdef COM_RxIPduTimeout */

#ifdef COM_RxSigUpdateTimeout
    /* If this IPDU has signals with update-bits , initialise the pointers.
    * Separate deadline monitoring should be performed for signals with update-bit
    */
    if (rxIpduConstPtr_pcst->signalTimeoutRef_pcst != NULL_PTR)
    {
        isRxIpduDmReq_b = COM_TRUE;

        /* Get the number of signals with update-bit & configured timeout, within this IPDU */
        numOfSigWithUpdateBit_qu16   = rxIpduConstPtr_pcst->signalTimeoutRef_pcst->numWithUpdateBit_u8;
        /* Initialise pointer to Com_SignalTimeoutInfo */
        sigTimeOutConstPtr_pcst     = rxIpduConstPtr_pcst->signalTimeoutRef_pcst->timeoutInfoRef_pcst;
        /* Initialise pointer to signal RAM timer */
        sigTimeOutRamPtr_pu16       = rxIpduConstPtr_pcst->signalTimeoutRef_pcst->timeoutTicks_pu16;

        SchM_Enter_Com_RxSigToTicks();

        for (index_qu16 = numOfSigWithUpdateBit_qu16; index_qu16 != COM_ZERO; index_qu16--)
        {
            /* If FirstTimeOutFactor is not configured then load timer with COM_RXTIMER_MAX(0xFFFF)
            * If FirstTimeOutFactor is configured then load the ComFirstTimeout value
            */
            (*sigTimeOutRamPtr_pu16) = (sigTimeOutConstPtr_pcst->firstTimeout_u16 == COM_ZERO) ?
                    COM_RXTIMER_MAX : (sigTimeOutConstPtr_pcst->firstTimeout_u16 + COM_ONE);

            /* Increment pointers to refer next signal with update-bit(within this IPDU) */
            sigTimeOutConstPtr_pcst++;
            sigTimeOutRamPtr_pu16++;

        }/*End of for loop*/

        SchM_Exit_Com_RxSigToTicks();
    }
#endif /* #ifdef COM_RxSigUpdateTimeout */

#ifdef COM_RxSigGrpUpdateTimeout
    /* If this IPDU has signal groups with update-bits , initialise the pointers.
    * Separate deadline monitoring should be performed for signal groups with update-bit
    */
    if (rxIpduConstPtr_pcst->signalGrpTimeoutRef_pcst != NULL_PTR)
    {
        isRxIpduDmReq_b = COM_TRUE;

        /* Get the number of signals with update-bit & configured timeout, within this IPDU */
        numOfSigWithUpdateBit_qu16   = rxIpduConstPtr_pcst->signalGrpTimeoutRef_pcst->numWithUpdateBit_u8;
        /* Initialise pointer to Com_SignalGrpTimeoutInfo */
        sigTimeOutConstPtr_pcst     = rxIpduConstPtr_pcst->signalGrpTimeoutRef_pcst->timeoutInfoRef_pcst;
        /* Initialise pointer to signalGroup timer */
        sigTimeOutRamPtr_pu16       = rxIpduConstPtr_pcst->signalGrpTimeoutRef_pcst->timeoutTicks_pu16;

        SchM_Enter_Com_RxSigGrpToTicks();

        for (index_qu16 = numOfSigWithUpdateBit_qu16; index_qu16 != COM_ZERO; index_qu16--)
        {
            /* If FirstTimeOutFactor is not configured then load timer with COM_RXTIMER_MAX(0xFFFF)
            * If FirstTimeOutFactor is configured then load the ComFirstTimeout value
            */
            (*sigTimeOutRamPtr_pu16) = (sigTimeOutConstPtr_pcst->firstTimeout_u16 == COM_ZERO) ?
                    COM_RXTIMER_MAX : (sigTimeOutConstPtr_pcst->firstTimeout_u16 + COM_ONE);

            /* Increment pointers to refer next signal group with update-bit(within this IPDU)
            * If no next signal group with update-bit is present in the IPDU, then the if() check fails and
            * the code inside if() block is not executed.
            */
            sigTimeOutConstPtr_pcst++;
            sigTimeOutRamPtr_pu16++;
        }/*End of for loop*/

        SchM_Exit_Com_RxSigGrpToTicks();
    }
#endif /* #ifdef COM_RxSigGrpUpdateTimeout */

    return isRxIpduDmReq_b;
}

#endif /* #if defined (COM_RxIPduTimeout) || defined (COM_RxSigUpdateTimeout) || defined (COM_RxSigGrpUpdateTimeout) */


#ifdef COM_RxIPduTimeout
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_GetRxIpduFirstTimeout
 Description      : Internal API to get RX IPDU first timeout
 Parameter        : idIpdu_uo -> ID of the Rx IPDU (internal function, so internal ID is passed in PB variant)
 Return value     : Rx IPDU "first timeout" configured/calibrated value
 **********************************************************************************************************************
*/
LOCAL_INLINE uint16 Com_Prv_GetRxIpduFirstTimeout(Com_IpduId_tuo idIpdu_uo)
{
    uint16 firstTimeout_u16;

#if !defined(COM_RXPDU_FIRSTTIMEOUT_VIA_CALIBRATION)
    Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst;

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);

    firstTimeout_u16 = rxIpduConstPtr_pcst->firstTimeout_u16;
#else
    firstTimeout_u16 = Com_Prv_GetRxIPduFirstTimeoutCalibration((PduIdType) idIpdu_uo);
#endif
    firstTimeout_u16 = (firstTimeout_u16 == COM_ZERO) ? COM_RXTIMER_MAX : (firstTimeout_u16 + COM_ONE);

    return firstTimeout_u16;
}
#endif /* #ifdef COM_RxIPduTimeout */


#ifdef COM_RX_DUAL_CYCLE_SUPPPORT
/*
 **********************************************************************************************************************
 Function name    : Com_SetRxIPduTimeoutTicks
 Description      : To update new(dual-cycle-time) IPDU based reception deadline value
 Parameter        : idRxPdu_uo       - Id of the Rx IPDU
                  : timeoutTicks_u16 - Rx IPDU deadline value in 'ticks', i.e., multiple of referred MainFunctionRx
 Return value     : E_OK     - new deadline will be considered from next call to MainFunctionRx
                  : E_NOT_OK - new deadline value is not considered.
 **********************************************************************************************************************
*/
Std_ReturnType Com_SetRxIPduTimeoutTicks(PduIdType idRxPdu_uo, uint16 timeoutTicks_u16)
{

# ifdef COM_RxIPduTimeout

    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;
    Std_ReturnType              retValue_u8;

    retValue_u8 = E_NOT_OK;

#  if(COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SetRxIPduTimeoutTicks, COM_E_UNINIT);
    }
    else if (timeoutTicks_u16 > COM_RXTIMER_TICKS_TO_STOP_IPDU_DM)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SetRxIPduTimeoutTicks, COM_E_INVALID_VALUE);
    }
    else if (!Com_Prv_IsValidRxIpduId(idRxPdu_uo))
    {
        COM_DET_REPORT_ERROR(COMServiceId_SetRxIPduTimeoutTicks, COM_E_PARAM);
    }
    else
#  endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idRxPdu_uo              = COM_GET_RX_IPDU_ID(idRxPdu_uo);

        rxIpduConstPtr_pcst     = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);
        rxIpduRamPtr_pst        = rxIpduConstPtr_pcst->ipduRam_pst;

        /* is IPDU ComTimeout configured */
        if (rxIpduConstPtr_pcst->timeout_u16 != COM_ZERO)
        {
            retValue_u8 = E_OK;

            /* does new tick-value differs from previously updated ticks */
            if (rxIpduRamPtr_pst->overrideTimeout_u16 != timeoutTicks_u16)
            {
                rxIpduRamPtr_pst->overrideTimeout_u16 = timeoutTicks_u16;

                /* locks are acquired to protect 'RxFlags' */
                SchM_Enter_Com_RxPduBuffer();

                Com_SetRamValue(RXIPDU,_RELOAD_DM_TIMER, rxIpduRamPtr_pst->rxFlags_u16, COM_TRUE);

                SchM_Exit_Com_RxPduBuffer();
            }
        }
    }

    return retValue_u8;
# else

    (void)idRxPdu_uo;
    (void)timeoutTicks_u16;

    return E_NOT_OK;

# endif /* end of #ifdef COM_RxIPduTimeout */

}

#endif /* end of COM_RX_DUAL_CYCLE_SUPPPORT */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

