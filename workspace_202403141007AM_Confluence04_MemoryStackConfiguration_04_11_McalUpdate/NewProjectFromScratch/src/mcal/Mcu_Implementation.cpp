/*******************************************************************************
 * Autogenerated by MCALgen for MCU V3.0.51.35438: Mcu_Implementation.cpp
 *******************************************************************************/

extern "C" {
  #include "Mcu.h"
  #include "Mcu_Cfg.h"
  #include "EcuM.h"
#if (MCU_DEV_ERROR_DETECT == STD_ON)
  #include "Det.h"
#endif
  #include "Mcu_Implementation.h"
}
#include "Mcu_Device.hpp"

/*******************************************************************************
 *                         Global Variables
 ******************************************************************************/
static const Mcu_ConfigType* Mcu_CurrentConfigData;
static Mcu_ResetType mcu_reset_reason;

/******************************************************************************/
Std_ReturnType Mcu_DETError(uint32 instanceId, uint32 apiId, uint32 errorId, boolean fromAction) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)  /* [$MCU 101]  [$MCU 13]  [$MCU 100]  */
  if (!fromAction) {
    Det_ReportError(MCU_MODULE_ID, instanceId, apiId, errorId); /* [$MCU 163] [$MCU21 163] [$MCU 16]  */
  }
  Mcu_ReportDETError(instanceId, apiId, errorId);
#endif
  return E_NOT_OK;  /* [$MCU 17] [$MCU21 17] */
}


/*******************************************************************************
*                         API Provided to Mcu.c
 ******************************************************************************/
extern "C" {

// -----------------------------------------------------------------------------
// Initializes the MCU
void Mcu_Init_helper(const Mcu_ConfigType *configData, boolean fromAction) {
  /*[$MCU 127] */
  if (configData != &Mcu_ConfigData && configData != MCU_DEFAULT_CONFIG) { /* [$MCU 126] [$MCU21 126] */
    Mcu_DETError(0U, MCU_INIT_API_ID, MCU_E_PARAM_CONFIG, fromAction); /* [$MCU 18]  */
    return;
  }
  /*[$MCU 26] [$MCU21 26]*/
  Mcu_CurrentConfigData =  &Mcu_ConfigData;
  if (Mcu_CurrentConfigData->reset_reason < 3) {
    mcu_reset_reason = Mcu_CurrentConfigData->reset_reason;
  } else {
    mcu_reset_reason = MCU_POWER_ON_RESET; /* [$MCU 5] [$MCU21 5] */
  }
}

// -----------------------------------------------------------------------------
Std_ReturnType Mcu_InitRamSection_helper(Mcu_RamSectionType RamSection, boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR) {
    return Mcu_DETError(0U, MCU_INITRAMSECTION_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 136] [$MCU21 136] */
  }
  return E_OK;
}

// -----------------------------------------------------------------------------
Std_ReturnType Mcu_InitClock_helper(Mcu_ClockType ClockSetting,boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR) {
    return Mcu_DETError(0U, MCU_INITCLOCK_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 139] [$MCU21 139] [$MCU 125] [$MCU21 125]*/
  }
  return E_OK;
}

// -----------------------------------------------------------------------------
void Mcu_DistributePllClock_helper(boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR) {
    Mcu_DETError(0U, MCU_DISTRIBUTEPLLCLOCK_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 125] [$MCU21 125] */
  }
}

// -----------------------------------------------------------------------------
Mcu_PllStatusType Mcu_GetPllStatus_helper(boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR) { /* [$MCU 132] [$MCU21 132] */
    Mcu_DETError(0U, MCU_GETPLLSTATUS_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 125] [$MCU21 125] */
  }
  /* Always return MCU_PLL_STATUS_UNDEFINED as pll is  not supported */
  return MCU_PLL_STATUS_UNDEFINED; /* [$MCU 230] [$MCU21 230] [$MCU 8] [$MCU21 8] [$MCU 206] [$MCU21 206] */
}

// -----------------------------------------------------------------------------
Mcu_ResetType Mcu_GetResetReason_helper(boolean fromAction) { /* [$MCU 52] [$MCU21 52] */
  if (Mcu_CurrentConfigData == NULL_PTR) {
    Mcu_DETError(0U, MCU_GETRESETREASON_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 125] [$MCU21 125] */
    return MCU_RESET_UNDEFINED; /* [$MCU 133] [$MCU21 133] */
  }
  return mcu_reset_reason;
}

// -----------------------------------------------------------------------------
Mcu_RawResetType Mcu_GetResetRawValue_helper(boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR) {
    Mcu_DETError(0U, MCU_GETRESETRAWVALUE_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 125] [$MCU21 125] */
    /* return value is implementation specific */
    return UNINIT_RESET_RAW_VALUE; /* [$MCU 135] [$MCU21 135] */
  }
  return ((Mcu_RawResetType)mcu_reset_reason); /* [$MCU 6] [$MCU21 6] */
}

// -----------------------------------------------------------------------------
void Mcu_PerformReset_helper(boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR) {
    Mcu_DETError(0U, MCU_PERFORMRESET_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 125] [$MCU21 125] */
    return; /* [$MCU 145] [$MCU21 145] */
  }
  if (Mcu_CurrentConfigData->reset_reason < 3) {
    mcu_reset_reason = (Mcu_ResetType) Mcu_CurrentConfigData->reset_reason; /* [$MCU 144] [$MCU21 144] */
    Mcu_Raise_ResetReason_event(mcu_reset_reason);
  }
}

// -----------------------------------------------------------------------------
void Mcu_SetMode_helper(Mcu_ModeType McuMode, boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR){
    Mcu_DETError(0U, MCU_SETMODE_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 125] [$MCU21 125] */
    return; /* [$MCU 148] [$MCU21 148] */
  }
}

// -----------------------------------------------------------------------------
void Mcu_GetVersionInfo_helper(Std_VersionInfoType* VersionInfo, boolean fromAction) {
  if (VersionInfo == NULL_PTR) {
    Mcu_DETError(0U, MCU_GETVERSIONINFO_API_ID, MCU_E_PARAM_POINTER, fromAction); /* [$MCU 204] */
    return;
  }
  /* [$MCU 103]  */
  VersionInfo->vendorID = MCU_VENDOR_ID;
  VersionInfo->moduleID = MCU_MODULE_ID;
  VersionInfo->sw_major_version = MCU_SW_MAJOR_VERSION;
  VersionInfo->sw_minor_version = MCU_SW_MINOR_VERSION;
  VersionInfo->sw_patch_version = MCU_SW_PATCH_VERSION;

}

// -----------------------------------------------------------------------------
Mcu_RamStateType Mcu_GetRamState_helper(boolean fromAction) {
  if (Mcu_CurrentConfigData == NULL_PTR) {
    Mcu_DETError(0U, MCU_GETRAMSTATE_API_ID, MCU_E_UNINIT, fromAction); /* [$MCU 125] [$MCU21 125] */
  }
  /* Ram not supported */
  return MCU_RAMSTATE_INVALID; /* [$MCU 208] [$MCU21 208] */
}

// -----------------------------------------------------------------------------
void Mcu_InjectReset_helper(Mcu_ResetType reset) {
  if (Mcu_CurrentConfigData == NULL_PTR) {
      return;
  }
  mcu_reset_reason = reset;
  Mcu_Raise_ResetReason_event(mcu_reset_reason); /* [$MCU 1016] */
}

// -----------------------------------------------------------------------------
const Mcu_ConfigType* Mcu_GetInternalConfig(void) { return (Mcu_ConfigType *) Mcu_CurrentConfigData; }
// -----------------------------------------------------------------------------
}

