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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "NvM_Prv_HideRteApi.h"

#include "NvM_Prv.h"
#include "NvM_Prv_MemIf.h"
#include "NvM_Prv_ErrorDetection.h"
/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This public function enters the high-speed-mode if possible.
 * \details
 * In high-speed-mode the NvM handles only user requests with immediate priority.
 * I.e. NvM suspends all other user requests as long as high-speed-mode is active.
 *
 *  The NvM enters the high-speed-mode only if following conditions are true:
 *  - NvM is initialized (see NvM_Prv_ErrorDetection_IsNvmInitialized)
 *  - NvM has entered the high-speed-mode for all configured memory devices (see NvM_Prv_MemIf_EnterHighSpeedMode)
 *
 * \attention
 * If NvM cannot enter the high-speed-mode for a memory device then NvM exits the high-speed-mode
 * for all memory devices already in high-speed-mode.
 *
 * This function succeds only if all configured memory devices are in high-speed-mode.
 * If it is not possible to enter the high-speed-mode for at least one memory device then this function fails.
 *
 * \return
 * - E_OK     = NvM AND all configured memory devices are in high-speed-mode
 * - E_NOT_OK = NvM AND all configured memory devices are NOT in high-speed-mode
 */
Std_ReturnType NvM_Rb_EnterHighSpeedMode(void)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3456] Service to enter the high-speed-mode
    Std_ReturnType stEnterHighSpeedMode_uo = E_NOT_OK;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3469] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_RB_ENTER_HIGH_SPEED_MODE, 0u))
    {
        stEnterHighSpeedMode_uo = NvM_Prv_MemIf_EnterHighSpeedMode();

        NvM_Prv_RequireHighSpeedMode(E_OK == stEnterHighSpeedMode_uo);
    }
#endif

    return stEnterHighSpeedMode_uo;
}

/**
 * \brief
 * This public function exits the high-speed-mode.
 * \details
 *  The NvM exits the high-speed-mode only if following conditions are true:
 *  - NvM is initialized (see NvM_Prv_ErrorDetection_IsNvmInitialized)
 *  - NvM has entered the high-speed-mode before
 *
 * \return
 * - E_OK     = NvM AND all configured memory devices are NOT in high-speed-mode anymore
 * - E_NOT_OK = It is not possible to exit high-speed-mode because it was not active before
 */
Std_ReturnType NvM_Rb_ExitHighSpeedMode(void)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3458] Service to exit the high-speed-mode
    Std_ReturnType stExitHighSpeedMode_uo = E_NOT_OK;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3470] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_RB_EXIT_HIGH_SPEED_MODE, 0u))
    {
        if (NvM_Prv_IsHighSpeedModeActive())
        {
            stExitHighSpeedMode_uo = NvM_Prv_MemIf_ExitHighSpeedMode();
            NvM_Prv_SetHighSpeedMode(FALSE);
        }
    }
#endif

    return stExitHighSpeedMode_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

