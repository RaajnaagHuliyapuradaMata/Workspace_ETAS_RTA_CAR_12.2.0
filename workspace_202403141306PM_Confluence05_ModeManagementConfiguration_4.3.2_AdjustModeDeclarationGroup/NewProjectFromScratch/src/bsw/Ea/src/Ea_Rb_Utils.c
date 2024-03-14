/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
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
#include "Ea.h"

#if (EA_PRESENT == 1)
#include "Ea_Prv.h"


#define EA_START_SEC_CODE
#include "Ea_MemMap.h"



/**
 *********************************************************************
 * Ea_Rb_MainFunctionAndDependency(): Mainfunction and dependencies of EA
 *
 * This function invokes Ea MainFunction and its dependencies
 *
 *********************************************************************
 */
void Ea_Rb_MainFunctionAndDependency(void)
{
    Ea_MainFunction();
    Eep_MainFunction();

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))
    Ea_Rb_EepScheduleDependency();
#else
    Ea_Rb_EaFs1_EepScheduleDependency();
#endif
    return;
}


#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"
#endif /* EA_PRESENT */
