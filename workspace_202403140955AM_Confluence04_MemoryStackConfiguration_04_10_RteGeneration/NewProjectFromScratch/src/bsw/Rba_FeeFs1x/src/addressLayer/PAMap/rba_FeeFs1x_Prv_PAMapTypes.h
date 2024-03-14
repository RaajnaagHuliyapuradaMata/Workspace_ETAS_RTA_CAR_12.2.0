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

#ifndef RBA_FEEFS1X_PRV_PAMAPTYPES_H
#define RBA_FEEFS1X_PRV_PAMAPTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#define RBA_FEEFS1X_PAMAP_DETECTWRPAGE_CHUNKSIZE (16uL)
#define RBA_FEEFS1X_PAMAP_FILLPATTERN_SIZE (16uL)

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

typedef enum
{
    rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_prepare_e,
    rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_determLowerLimit_e,
    rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_areaBlChk_e,
    rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_pageBlkChk_e,
    rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e,
    rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_error_e
}rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_ten;

typedef struct
{
    uint32 upperLimit_u32;
    uint32 lowerLimit_u32;
    uint32 currentChunkPage_u32;
    uint32 chunkSize_u32;
    rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_ten state_en;
    uint8 activeSectorIdx_u8;
    boolean areaBlank_b;
    boolean entry_b;
} rba_FeeFs1x_PAMap_detectCurrentWrPage_tst;


typedef enum
{
    rba_FeeFs1x_PAMap_fillSectorEnd_stm_idle_e,
    rba_FeeFs1x_PAMap_fillSectorEnd_stm_write_e
}rba_FeeFs1x_PAMap_fillSectorEnd_ten;

typedef struct
{
    uint32 logStartWritePage_u32;
    uint32 logEndWritePage_u32;
    rba_FeeFs1x_PAMap_fillSectorEnd_ten state_en;
    boolean entry_b;
} rba_FeeFs1x_PAMap_fillSectorEnd_tst;

typedef enum
{
    rba_FeeFs1x_PAMap_prepareSectorStates_stm_idle_e,
    rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrFull_e,
    rba_FeeFs1x_PAMap_prepareSectorStates_stm_erase_e,
    rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrUsed_e,
    rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrStart_e
}rba_FeeFs1x_PAMap_prepareSectorStates_stm_ten;

typedef struct
{
    uint32 nextBlockStartPage_u32;
    rba_FeeFs1x_PAMap_prepareSectorStates_stm_ten state_en;
    uint8 logSectIdxNextBlock_u8;
    boolean isReerasing_b;
    boolean entry_b;
}rba_FeeFs1x_PAMap_prepareSectorStates_data_tst;

typedef struct
{
    uint32 currWrPage_u32;
}rba_FeeFs1x_PAMap_data_tst;

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

