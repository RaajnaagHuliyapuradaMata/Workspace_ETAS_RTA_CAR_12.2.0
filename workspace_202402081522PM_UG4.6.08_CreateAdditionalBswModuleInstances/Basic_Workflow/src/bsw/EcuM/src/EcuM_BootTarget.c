/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2010, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/**************************************************************************************************/
/* Preprocessor includes                                                                          */
/**************************************************************************************************/

#include "EcuM.h" /*BSW_HeaderInc_002*/


#include "EcuM_Cfg_SchM.h"

/*Extern module Headers*/
#if(ECUM_DEV_ERROR_DETECT == STD_ON)/*Det included when Det is enabled in EcuM*/
#include "Det.h"
// #if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != 4))
// #error "AUTOSAR major version undefined or mismatched"
// #endif
// #if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != 2))
// #error "AUTOSAR minor version undefined or mismatched"
// #endif
#endif /* ECUM_DEV_ERROR_DETECT */

/*EcuM Private headers*/
#include "EcuM_Prv.h"

#define BOOT_TARGET_NOT_INITIALIZED 0
#define BOOT_TARGET_INITIALIZED 1

/***********************************************************************************
 Function name  :   Std_ReturnType EcuM_GetBootTarget(
                    EcuM_BootTargetType * target )
 Description    :   Returns the current Boot Target
 Parameter in   :   None
 Parameter out  :   target
 return         :   Std_ReturnType
 ************************************************************************************/
#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"
Std_ReturnType EcuM_GetBootTarget( EcuM_BootTargetType * target )
{

    Std_ReturnType return_value_u8;

    if (target == NULL_PTR)
    {
#if(ECUM_DEV_ERROR_DETECT == STD_ON)
        /**DET raised if the in parameter is a NULL PTR */
        (void)Det_ReportError(ECUM_MODULE_ID, ECUM_INSTANCE_ID, ECUM_GET_BOOT_APIID, ECUM_E_NULL_POINTER);
#endif /* ECUM_DEV_ERROR_DETECT */
        return_value_u8 =  E_NOT_OK;
    }

    else
    {
        SchM_Enter_EcuM(); /**Enter Critical Section*/
        /**Update the selected shutdown target */

        /* Check if Boot target has been set by EcuM_SelectBootTarget before */
        if (EcuM_Prv_dataSelectedBootTargetInit_u8 == BOOT_TARGET_NOT_INITIALIZED)
        {
            /* Return default Boot target */
            *target = ECUM_BOOT_TARGET_OEM_BOOTLOADER;
        }
        else
        {
            /* Return Boot target selected by EcuM_SelectBootTarget  */
            *target = EcuM_Prv_dataSelectedBootTarget_u8;
        }

        SchM_Exit_EcuM();  /**Leave critical section*/
        return_value_u8 =  E_OK;
    }

    return return_value_u8;
}

/***********************************************************************************
 Function name  :   Std_ReturnType EcuM_SelectBootTarget( EcuM_BootTargetType target )
 Description    :   Selects the current Boot Target
 Parameter in   :   None
 Parameter out  :   target
 return         :   Std_ReturnType
 ************************************************************************************/

Std_ReturnType EcuM_SelectBootTarget( EcuM_BootTargetType target )
{

    Std_ReturnType return_value_u8;

    if((target!=ECUM_BOOT_TARGET_APP)&&(target!=ECUM_BOOT_TARGET_OEM_BOOTLOADER)&&(target!=ECUM_BOOT_TARGET_SYS_BOOTLOADER))
    {
#if(ECUM_DEV_ERROR_DETECT == STD_ON)
        /**DET raised for wrong parameter */
        (void)Det_ReportError(ECUM_MODULE_ID, ECUM_INSTANCE_ID, ECUM_SEL_BOOT_APIID , ECUM_E_INVALID_PAR);
#endif /* ECUM_DEV_ERROR_DETECT */
        return_value_u8 = E_NOT_OK;
    }
    else
    {
        SchM_Enter_EcuM(); /**Enter Critical Sectiont*/
        /**The shutdown target is passed to the selected state*/

        EcuM_Prv_dataSelectedBootTarget_u8 = target;

        /* Set information that Boot target has been initialized by EcuM_SelectBootTarget */
        EcuM_Prv_dataSelectedBootTargetInit_u8 = BOOT_TARGET_INITIALIZED;

        SchM_Exit_EcuM();  /**Leave critical section*/
        return_value_u8 = E_OK;
    }

    return return_value_u8;

}

#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"
