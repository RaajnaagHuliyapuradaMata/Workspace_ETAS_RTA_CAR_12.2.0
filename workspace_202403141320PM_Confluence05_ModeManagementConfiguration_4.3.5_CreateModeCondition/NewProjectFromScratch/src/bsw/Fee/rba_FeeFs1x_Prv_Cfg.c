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
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "Fee_Prv_ConfigTypes.h"

/*
 **********************************************************************************************************************
 * Generated Tables
 **********************************************************************************************************************
 */

/* Properties of flash sectors */
#define FEE_START_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"
const Fee_Rb_FlashProp_tst rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast[RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE] = {
     { RBA_FEEFS1X_PRV_CFG_PHYS_SEC_START0, RBA_FEEFS1X_PRV_CFG_PHYS_SEC_END0 },
     { RBA_FEEFS1X_PRV_CFG_PHYS_SEC_START1, RBA_FEEFS1X_PRV_CFG_PHYS_SEC_END1 }
};
#define FEE_STOP_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"

#define FEE_START_SEC_VAR_INIT_UNSPECIFIED
#include "Fee_MemMap.h"
Fee_Rb_BlockPropertiesType_tst rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast[RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS] = {
    { 0xA6C9u, 0x0100u, 0x0002u },    /*   (0)     NvM_NativeBlock_2 */
    { 0xB740u, 0x0100u, 0x0002u },    /*   (1)     NvM_NativeBlock_3 */
    { 0xF2F7u, 0x0100u, 0x0004u }     /*   (2)     ECUM_CFG_NVM_BLOCK */
};
#define FEE_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Generated Fs-specifc Variables
 **********************************************************************************************************************
 */

