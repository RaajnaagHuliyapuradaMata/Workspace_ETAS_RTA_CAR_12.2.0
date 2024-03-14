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

#ifndef RBA_FEEFS1X_PRV_SECTORERASEJOBTYPES_H
#define RBA_FEEFS1X_PRV_SECTORERASEJOBTYPES_H
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
typedef enum
{
    rba_FeeFs1x_SectorEraseJob_erase_stm_idle_e,
    rba_FeeFs1x_SectorEraseJob_erase_stm_findSector2BeErased_e,
    rba_FeeFs1x_SectorEraseJob_erase_stm_erase_e
}rba_FeeFs1x_SectorEraseJob_erase_stm_ten;

typedef struct
{
    rba_FeeFs1x_SectorEraseJob_erase_stm_ten state_en;
    uint8 idxSector2BeErased_u8;
    boolean entry_b;
}rba_FeeFs1x_SectorEraseJob_erase_data_tst;

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

