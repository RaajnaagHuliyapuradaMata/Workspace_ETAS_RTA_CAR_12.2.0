/*******************************************************************************
 * Autogenerated by MCALgen for MCU V3.0.51.35438: Mcu_Cfg.h
 *******************************************************************************/
#ifndef MCU_CFG_H_
#define MCU_CFG_H_

#define MCU_DEFAULT_CONFIG ((const Mcu_ConfigType*) 0)
#define MCU_DEV_ERROR_DETECT STD_OFF /* [$MCU 100]  */
#define MCU_NO_PLL  TRUE
#define MCU_INIT_CLOCK STD_ON
#define MCU_PERFORM_RESET_API STD_ON
#define MCU_GET_RAM_STATE_API STD_OFF
#define MCU_VERSION_INFO_API STD_OFF
#define DEFAULT_RESET_RAW_VALUE (0)
#define UNINIT_RESET_RAW_VALUE (0xFFFF)


#define McuConf_McuClockSettingConfig_McuClockSettingConfig (1U)



#define McuConf_McuModeSettingConf_McuModeSettingConf (1U)



#define McuConf_McuResetReasonConf_McuResetReasonConf (0U)


extern const Mcu_ConfigType Mcu_ConfigData;

#endif /* MCU_CFG_H_ */


