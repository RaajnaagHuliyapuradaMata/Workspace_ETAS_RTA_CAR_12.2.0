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


/**
 *********************************************************************
 * MemIf_Rb_GetDetailedBlkInfo(): Check the detailed error information of particular block
 *
 * This function will return detailed error information of particular block
 *
 * \param   BlockNumber
 * \return  MemIf_Rb_DetailedBlkInfo_ten
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INIT_E                   Block was never read inconsitent - result not available
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E   MEMIF_BLOCK_INCONSISTENT: Valid block (header) was not found on the medium
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_DATA_E      MEMIF_BLOCK_INCONSISTENT: Data CRC of block is wrong
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_LENGTH_E is not supported. In Ea, if a greater length is requested, the excessive buffer is filled with erase pattern.
 *
 *********************************************************************
 */
#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

MemIf_Rb_DetailedBlkInfo_ten Ea_Rb_GetDetailedBlkInfo(uint16 blockNumber_u16)
{

#if((defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS)))
    return rba_EaFs2_Prv_GetDetailedBlkInfo(blockNumber_u16);
#else
    (void)blockNumber_u16;
    return MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E;
#endif
}

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"
#endif /* EA_PRESENT */
