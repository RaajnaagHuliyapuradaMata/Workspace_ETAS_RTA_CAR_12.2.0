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

#ifndef RBA_FEEFS1X_PRV_CFG_H
#define RBA_FEEFS1X_PRV_CFG_H

/* ******************************************************************************************************************
   *************************************************** Includes *****************************************************
   ****************************************************************************************************************** */

#include "Std_Types.h"
#include "Fee_Prv_ConfigTypes.h" // to get structure type for Flash properties and block properties table

/* ******************************************************************************************************************
   ******************************************* Macros for code optimization *****************************************
   ****************************************************************************************************************** */

/* Pre-processor switch to enable or disable the API to maintain the redundant blocks. */
#define RBA_FEEFS1X_PRV_CFG_MAINTAIN                         (FALSE)

/* Pre-processor switch to enable or disable forced sector reorganisations. */
#define RBA_FEEFS1X_PRV_CFG_TRIGGER_REORG                    (FALSE)

/* Pre-processor switch to enable or disable an additional block specific reorganization for unknown
 *                  blocks with an active survival setting ("FeeRbSurvival") in the medium. */
#define RBA_FEEFS1X_PRV_CFG_SURVIVAL_EVAL                    (FALSE)

/* Pre-processor switch to enable or disable an additional reorganization for any unknown block in the medium (boot usage) */
#define RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_REORG              (FALSE)

/* Pre-processor switch to enable or disable free space API */
#define RBA_FEEFS1X_PRV_CFG_GET_NR_FREE_BYTES                (FALSE)

/* Fee NoFallback feature active for any of the configured blocks */
#define RBA_FEEFS1X_PRV_CFG_NO_FALLBACK_ACTIVE               (FALSE)

/* Chunk wise unknown block write operation */
#define RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_WRITE              (FALSE)

/* Chunk wise unknown survival block write operation */
#define RBA_FEEFS1X_PRV_CFG_UNKNOWN_SURVIVAL_BLOCK_WRITE     (FALSE)

/* Pre-processor switch to enable or disable Detailed Block Info API */
#define RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API            (FALSE)

/* Pre-processor switch to enable or disable get sector state API */
#define RBA_FEEFS1X_PRV_CFG_GET_SECT_STATE_API               (FALSE)

/* Pre-processor switch to cancel ongoing job */
#define RBA_FEEFS1X_PRV_CFG_CANCEL                           (FALSE)

/* ******************************************************************************************************************
   ******************** Status bit position macros (constant for all instances of a file system) ********************
   ****************************************************************************************************************** */

#define RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS               (0x0001u)
#define RBA_FEEFS1X_PRV_CFG_INVALIDATE_BITS                  (0x0008u)
#define RBA_FEEFS1X_PRV_CFG_SURVIVAL_BITS                    (0x0010u)
#define RBA_FEEFS1X_PRV_CFG_LEGACY_ROB_BITS                  (0x0020u)
#define RBA_FEEFS1X_PRV_CFG_NOFALLBACK_BITS                  (0x0040u)
#define RBA_FEEFS1X_PRV_CFG_IGNORE_BITS                      (0x0080u)
#define RBA_FEEFS1X_PRV_CFG_ROBUSTNESS_BITS                  (0x0800u)

/* ******************************************************************************************************************
   **************************************** Common macros for all file systems **************************************
   ****************************************************************************************************************** */

/* The size in bytes to which logical blocks shall be aligned. */
#define RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE                   (8u)

/* Number of FeeBlocks */
#define RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS                     (3u)

#define RBA_FEEFS1X_PRV_CFG_MED_ALLOW_DIRECT                 (FALSE)

/* ******************************************************************************************************************
   ******************************************* Generate FLASH configuration *****************************************
   ****************************************************************************************************************** */

#define RBA_FEEFS1X_PRV_CFG_PHYS_SEC_START0                  (0x00000000u)
#define RBA_FEEFS1X_PRV_CFG_PHYS_SEC_END0                    (0x000003FFu)
#define RBA_FEEFS1X_PRV_CFG_PHYS_SEC_START1                  (0x00000400u)
#define RBA_FEEFS1X_PRV_CFG_PHYS_SEC_END1                    (0x000007FFu)
#define RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE         (2u)
/* Relative Addressing */
#define RBA_FEEFS1X_PRV_CFG_FLS_BASE_ADR                     (0x0u)

/* Erase Pattern */
#define RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT               (0u)
#define RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_16BIT              ((((uint32)RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT) << 8u) | ((uint32)RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT))

#define RBA_FEEFS1X_PRV_CFG_PHY_PAGE_LEN                     (8u)

/* ******************************************************************************************************************
   ********************************************** Fs specific defines ***********************************************
   ****************************************************************************************************************** */

/* Header Recheck - necessary for flash types with undefined erase pattern like Renesas RH850 */
#define RBA_FEEFS1X_PRV_CFG_HEADER_RECHECK_ACTIVE            (FALSE)

/* Number of unknown blocks that can be transfered/checked in one iteration of unknown block sector reorganisation (Global After burner feature) */
#define RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLK_CACHE_ARRAY_SIZE     (0u)

#define RBA_FEEFS1X_PRV_CFG_SSR_THRESHOLD_PAGE               (115uL)
#define RBA_FEEFS1X_PRV_CFG_HSR_THRESHOLD_PAGE               (115uL)
#define RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR            (116u)
#define RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS                   (0u)
#define RBA_FEEFS1X_PRV_CFG_SIMULATION_TEST_ACTIVE           (FALSE)


/* ******************************************************************************************************************
   *************************************** Fs specific external declarations ****************************************
   ****************************************************************************************************************** */

/* Properties of flash sectors */
#define FEE_START_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"
extern const Fee_Rb_FlashProp_tst rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast[RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE];
#define FEE_STOP_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"

#define FEE_START_SEC_VAR_INIT_UNSPECIFIED
#include "Fee_MemMap.h"
extern Fee_Rb_BlockPropertiesType_tst rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast[RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS];
#define FEE_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Fee_MemMap.h"

#endif /* RBA_FEEFS1X_PRV_CFG_H */
