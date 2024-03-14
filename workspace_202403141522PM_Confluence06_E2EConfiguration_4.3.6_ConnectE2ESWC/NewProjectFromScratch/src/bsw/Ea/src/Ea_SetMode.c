/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2015, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Ea.h"

#if (EA_PRESENT == 1)
#include "Ea_Prv.h"
#include "Eep.h"

#if(EA_DEV_ERROR_DETECT != FALSE) || (EA_DEV_RUNTIME_ERROR_DETECT != FALSE)
#include "Det.h"
#endif

/* *********************************************************************************************************************
 * Req:                 EA085
 *
 * Service name:        EA_SetMode
 * Syntax:
 *                      void EA_SetMode( MemIf_ModeType Mode )
 * Service ID[hex]:     0x01
 * Sync/Async:          Synchronous
 * Reentrancy:          Non Reentrant
 * Parameters (in):     Mode = Desired mode for the underlying flash driver
 * Parameters (inout):  None
 * Parameters (out):    None
 * Return value:        None
 * Description:         Service to call the Ea_SetMode function of the underlying flash driver.
 * ****************************************************************************************************************** */
/* [EA150]:
 * The function Ea_SetMode shall be enabled / disabled via the pre-compile time parameter EaSetModeSupported such that
 * the function is completely removed from the code if it is disabled. */
#if (EA_SET_MODE_SUPPORTED != FALSE)

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
MemIf_ModeType Ea_Mode;
#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

void Ea_SetMode(MemIf_ModeType Mode)
{
    /* [EA129]:
     * If development error detection for the module EA is enabled: the function Ea_SetMode shall check that the module
     * has been initialized. If this check fails, the function Ea_SetMode shall raise the development error
     * EA_E_UNINIT and return without performing the requested function. */

    if (Ea_Rb_CheckInit(EA_SID_SETMODE) != E_OK)
    {
        /* Do nothing */
    }
    else
    {
        if (Ea_GlobModuleState_st != MEMIF_BUSY)
        {
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG != TRUE)
            /* Forced Migration check not necessary since it's a special scenario.
               Forced Migrationfeature not present in EaFs1. Prodcuctline conveyed they
               don't have a usecase for switching to Fast Mode immediately after a new software
               flashing.Below DET is internal to detect the scenario and to discuss with productline
               for further Action. In Ea module there is no impact but in Eep module there is a impact
               due to EA_E_RB_SETMODE_MIG_CONFLICT. */
            if((EA_RB_MIGRATION_RUNNING == Ea_Rb_MigrationStatus()) && (MEMIF_MODE_FAST == Mode))
            {
#if(EA_DEV_ERROR_DETECT != FALSE)
            /* Report error to DET about development error */
                (void)Det_ReportError(EA_MODULE_ID, EA_INSTANCE_ID, EA_SID_SETMODE, EA_E_RB_SETMODE_MIG_CONFLICT);
#endif/* EA_DEV_ERROR_DETECT */
            }
#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG */
            /* [EA020]:
             * If supported by the underlying hardware and device driver, the function EA_SetMode shall call the function
             * Ea_SetMode of the underlying flash driver with the given Mode parameter.
             *
             * Example:
             * During normal operation of an ECU the EA module and underlying device driver shall use as few (runtime)
             * resources as possible, therefore the flash driver is switched to slow mode. During startup and especially
             * during shutdown it might be desirable to read / write the NV memory blocks as fast as possible, therefore the
             * EA and the underlying device driver could be switched into fast mode.*/
#if((defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS)) && (EA_AR_RELEASE_MINOR_VERSION >= 5))
            rba_EaFs2_Prv_SetModeRequest(Mode);       /*SetMode Asynchronous */
#else
            Eep_SetMode(Mode);      /* SetMode Synchronous */
            Ea_Mode= Mode;
#endif /* EA_AR_RELEASE_MINOR_VERSION */
        }
        else
        {
#if(EA_DEV_RUNTIME_ERROR_DETECT != FALSE)
            /* Report error to DET about runtime error */
            (void)Det_ReportRuntimeError(EA_MODULE_ID, EA_INSTANCE_ID, EA_SID_SETMODE, EA_E_BUSY);
#endif
        }
    }
}
#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"
#endif /* #if (EA_SET_MODE_SUPPORTED != FALSE) */
#endif /* EA_PRESENT */
