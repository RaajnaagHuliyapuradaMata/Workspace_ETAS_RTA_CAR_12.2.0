/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef RBA_FEEFS1X_PRV_TRIGGERREORGJOBTYPES_H
#define RBA_FEEFS1X_PRV_TRIGGERREORGJOBTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
// state machine for force trigger reorg
typedef enum
{
    rba_FeeFs1x_TriggerReorg_stm_idle_e,
    rba_FeeFs1x_TriggerReorg_stm_reorg_e,
    rba_FeeFs1x_TriggerReorg_stm_erase_e
}rba_FeeFs1x_TriggerReorg_ten;

typedef struct
{
    rba_FeeFs1x_TriggerReorg_ten  state_en;
    boolean                       entry_b;
}rba_FeeFs1x_TriggerReorg_tst;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */


#endif

