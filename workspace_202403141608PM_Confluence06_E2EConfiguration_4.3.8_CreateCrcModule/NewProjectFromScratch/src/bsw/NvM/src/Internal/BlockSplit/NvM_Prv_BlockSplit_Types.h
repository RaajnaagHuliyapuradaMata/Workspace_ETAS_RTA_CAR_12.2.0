/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_BLOCKSPLIT_TYPES_H
#define NVM_PRV_BLOCKSPLIT_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Types.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * Definition of the structure for administration data of a split-block.
 */
typedef struct
{
    /// Pointer to the array with lengths of all sub-blocks
    uint16 const*   SubBlockLengths_pcu16;
    /// Size of the master-block in bytes
    uint16          SizeMaster_uo;
    /// Number on sub-blocks
    uint8           nrSubBlocks_u8;

} NvM_Cfg_BlockSplit_AdminData_tst;

/**
 * Definition of the structure for master-block of a split-block.
 */
typedef struct
{
    /// Pointer to the array with indexes for all sub-blocks
    uint8*              idxSubBlock_pu8;
    /// Pointer to the array with CRCs for all sub-blocks
    NvM_Prv_Crc_tun*    CrcSubBlock_pun;

} NvM_Cfg_BlockSplit_Master_tst;

/**
 * Definition of the structure for configuration data of a split-block.
 */
typedef struct
{
    /// Pointer to the administration data of a split-block
    NvM_Cfg_BlockSplit_AdminData_tst const* BlockSplit_Cfg_pcst;
    /// POinter to the master-block of a split-block
    NvM_Cfg_BlockSplit_Master_tst const*    BlockSplit_Master_pcst;

} NvM_Cfg_BlockSplit_tst;

/* NVM_PRV_BLOCKSPLIT_TYPES_H */
#endif

