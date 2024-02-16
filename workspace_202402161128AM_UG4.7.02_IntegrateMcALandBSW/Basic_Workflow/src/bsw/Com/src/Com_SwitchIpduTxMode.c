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

/*
 **********************************************************************************************************************
 Function name    : Com_SwitchIpduTxMode
 Description      : The service sets the transmission mode of the I-PDU referenced by PduId to Mode
 Parameter        : idPdu_uo,mode_b
 Return value     : None
 **********************************************************************************************************************
*/
void Com_SwitchIpduTxMode(PduIdType idPdu_uo, boolean mode_b)
{

#if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SwitchIpduTxMode, COM_E_UNINIT);
    }
    else if (!Com_Prv_IsValidTxIpduId(idPdu_uo))
    {
        COM_DET_REPORT_ERROR(COMServiceId_SwitchIpduTxMode, COM_E_PARAM);
    }
    else
#endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then idPdu_uo which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idPdu_uo = COM_GET_TX_IPDU_ID(idPdu_uo);

        if (Com_Prv_CheckTxIPduStatus(idPdu_uo))
        {
            Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
            Com_TxIpduRam_tpst          txIpduRamPtr_pst;

            txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idPdu_uo);
            txIpduRamPtr_pst    = txIpduConstPtr_pcst->ipduRam_pst;

            /* Check if requested mode is already active */
            if (Com_GetRamValue(TXIPDU,_TXMODESTATE,txIpduRamPtr_pst->transMode_u8) != mode_b)
            {

                SchM_Enter_Com_TxIpduProtArea(SWITCHTXIPDU);

                /* Update current Tx-Mode state with the requested Mode */
                Com_SetRamValue(TXIPDU,_TXMODESTATE,txIpduRamPtr_pst->transMode_u8,mode_b);

                Com_Prv_SetCurrentTxModePtr(txIpduRamPtr_pst,txIpduConstPtr_pcst,mode_b);

                Com_SetRamValue(TXIPDU,_LATESTMODE,txIpduRamPtr_pst->transMode_u8,
                                                   txIpduRamPtr_pst->currentTxModePtr_pcst->mode_u8);

                /* FC_VariationPoint_START */
# ifdef COM_MIXEDPHASESHIFT
                /* Initialise MixedPhaseShift state, when IPdu tx mode is calculated in Com_IPduGroupStart() */
                Com_SetRamValue(TXIPDU,_MIXEDPHASESHIFT,txIpduRamPtr_pst->transMode_u8,
                        txIpduRamPtr_pst->currentTxModePtr_pcst->mixedPhaseShift_b);
# endif /* #ifdef COM_MIXEDPHASESHIFT */
                /* FC_VariationPoint_END */

                SchM_Exit_Com_TxIpduProtArea(SWITCHTXIPDU);

                /* TRACE The new transmission mode is immediately effective (within the next main function at
                the latest), however, ongoing TP transmission will be finished. In this case if the new
                transmission mode would necessitate a new transmission then
                COM_E_SKIPPED_TRANSMISSION runtime error will be reported. */
#ifdef COM_TX_TP_IPDUTYPE
                /* Proceed only if large data transmission is not in progress */
                /* For the change in  the mode to periodic and mixed the Runtime Error is reported
                * from MainfunctionTx */
                /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
                * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
                * system behaviour. Hence suppressed. */
                if ((Com_GetRamValue(TXIPDU,_LARGEDATAINPROG,txIpduRamPtr_pst->txFlags_u16) == COM_TRUE) &&
                        (Com_GetRamValue(TXIPDU,_LATESTMODE,txIpduRamPtr_pst->transMode_u8) == COM_TXMODE_DIRECT))
                {

                    COM_DET_REPORT_RUNTIME_ERROR(COMServiceId_SwitchIpduTxMode, COM_E_SKIPPED_TRANSMISSION)

                }
                else
#endif
                {
                    /* All unused flags are reset here */
                    Com_SendIpduInfo_tst sendIpduFlag_st = COM_CLEAR_SENDIPDU_FLAGS;

                    sendIpduFlag_st.isEventTrig_u1     = COM_SET;

                    sendIpduFlag_st.isModeChangd_u1    = COM_SET;

                    sendIpduFlag_st.isSwtIpduTxMode_u1 = COM_SET;
                    /* as the signal transfer property information is not known.
                    * It is assumed, for transmission from this APi,the signal transfer property is Pending */

                    /* Time out monitoring cannot be performed for event part */

                    /* If a change of the TMS causes a change of the transmission mode for one I-PDU,
                    * the timer for the cycle time of the periodic and the Mixed Tx Mode shall be restarted,
                    * will be done in Com_Prv_SendIpdu */

                    Com_Prv_SendIpdu(idPdu_uo, sendIpduFlag_st);
                }
            }
            else
            {
                /* In case the requested transmission mode was already active for this I-PDU,
                * the call will have no effect. */
            }
        }
    }
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

