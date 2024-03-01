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


#ifdef COM_FILTERS
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
 Function name    : Com_Prv_Filter
 Description      : Service used for filtering the signal depending on the conditions
 Parameter        : newVal_u32              -> New value for filtering
                  : algorithm_u8            -> Filter type
                  : idxFilter_uo            -> Filter Index
                  : notFromIpduGrpStart_b   -> indicates if the call is from IpduGroup start functions
 Return value     : COM_TRUE/COM_FALSE
 **********************************************************************************************************************
*/
boolean Com_Prv_Filter(
                uint32              newVal_u32,
                uint8               algorithm_u8,
                Com_Filter_tuo      idxFilter_uo,
                boolean             notFromIpduGrpStart_b
                  )
{
    boolean     state_b;

    /* conditionally used variable(s) is type-cast to 'void' to avoid warnings,
     * they are intentionally not placed under compiler switch statement,
     * so that these lines are always available for compilation*/

    (void)newVal_u32;
    (void)idxFilter_uo;
    (void)notFromIpduGrpStart_b;

    /* If algorithm is COM_ALWAYS, State is initialized to true, otherwise with false */
    state_b = (algorithm_u8 == COM_ALWAYS);

    switch (algorithm_u8)
    {
#ifdef COM_F_MASKEDNEWEQUALSX
        case COM_MASKED_NEW_EQUALS_X:
        {
            state_b = ((newVal_u32 & COM_GET_MASK_X(idxFilter_uo).mask_u32) == COM_GET_MASK_X(idxFilter_uo).xval_u32);
        }
        break;
#endif /* #ifdef COM_F_MASKEDNEWEQUALSX */

#ifdef COM_F_MASKEDNEWDIFFERSX
        case COM_MASKED_NEW_DIFFERS_X:
        {
            state_b = ((newVal_u32 & COM_GET_MASK_X(idxFilter_uo).mask_u32) != COM_GET_MASK_X(idxFilter_uo).xval_u32);
        }
        break;
#endif /* #ifdef COM_F_MASKEDNEWDIFFERSX */

#ifdef COM_F_MASKEDNEWDIFFERSOLD
        case COM_MASKED_NEW_DIFFERS_MASKED_OLD:
        {
            const Com_FMaskOld_tst * maskOldCfg_pcst;

            maskOldCfg_pcst = &COM_GET_FMASKOLD(idxFilter_uo);

            state_b = ((newVal_u32 & maskOldCfg_pcst->mask_u32) !=
                       ((*maskOldCfg_pcst->fOldVal_pu32) & maskOldCfg_pcst->mask_u32));
            if (state_b)
            {
                /* TRACE[SWS_Com_00302] If the AUTOSAR COM module evaluates a filter for a signal to true, (value is
                * not filtered out) then the AUTOSAR COM module shall place the value of that signal into old_value */
                *maskOldCfg_pcst->fOldVal_pu32 = newVal_u32;
            }
        }
        break;
#endif /* COM_F_MASKEDNEWDIFFERSOLD */

#ifdef COM_F_NEWISWITHIN_POS
        case COM_NEW_IS_WITHIN_POS:
        {
            state_b = ((COM_GET_POSMINMAX(idxFilter_uo).min_u32 <= newVal_u32) &&
                       (newVal_u32 <= COM_GET_POSMINMAX(idxFilter_uo).max_u32));
        }
        break;
#endif /* #ifdef COM_F_NEWISWITHIN_POS */

#ifdef COM_F_NEWISWITHIN_NEG
        case COM_NEW_IS_WITHIN_NEG:
        {
            state_b = ((COM_GET_NEGMINMAX(idxFilter_uo).min_s32 <= (sint32)newVal_u32) &&
                     ((sint32)newVal_u32 <= COM_GET_NEGMINMAX(idxFilter_uo).max_s32));
        }
        break;
#endif /* #ifdef COM_F_NEWISWITHIN_NEG */

#ifdef COM_F_NEWISOUTSIDE_POS
        case COM_NEW_IS_OUTSIDE_POS:
        {
            state_b = ((newVal_u32 < COM_GET_POSMINMAX(idxFilter_uo).min_u32) ||
                     (newVal_u32 > COM_GET_POSMINMAX(idxFilter_uo).max_u32));
        }
        break;
#endif /* #ifdef COM_F_NEWISOUTSIDE_POS */

#ifdef COM_F_NEWISOUTSIDE_NEG
        case COM_NEW_IS_OUTSIDE_NEG:
        {
            state_b = (((sint32)newVal_u32 < COM_GET_NEGMINMAX(idxFilter_uo).min_s32) ||
                     ((sint32)newVal_u32 > COM_GET_NEGMINMAX(idxFilter_uo).max_s32));
        }
        break;
#endif /* #ifdef COM_F_NEWISOUTSIDE_NEG */

#ifdef COM_F_ONEEVERYN
        case COM_ONE_EVERY_N:
        {
            const Com_OneEveryN_tst * oneEveryN_pcst;

            oneEveryN_pcst = &COM_GET_ONEEVERYN_CONST(idxFilter_uo);

            /* TRACE[SWS_Com_00231] In the case of ComFilterAlgorithm is configured to ONE_EVERY_N,
            * the AUTOSAR COM module shall
            * - set FILTER to true, when OCCURRENCE == OFFSET
            * - increment OCCURRENCE after filter processing
            * - set OCCURRENCE to zero when OCCURRENCE == PERIOD
            */
            state_b = ((*(oneEveryN_pcst->occurrence_pu32)) == oneEveryN_pcst->offset_u32);

            /* if call is not from Com_Init/Com_IpduGroupStart, occurrence value has to be incremented by one. */
            if (notFromIpduGrpStart_b)
            {
                (*(oneEveryN_pcst->occurrence_pu32))++;
            }
            if (*(oneEveryN_pcst->occurrence_pu32) == oneEveryN_pcst->period_u32)
            {
                (*(oneEveryN_pcst->occurrence_pu32)) = COM_ZERO;
            }
        }
        break;
#endif /* #ifdef COM_F_ONEEVERYN */

        default:
        {
            /* Default case covers COM_NEVER algorithm, hence there is no explicit case for COM_NEVER */
        }
        break;
    }

    return state_b;
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #ifdef COM_FILTERS */

