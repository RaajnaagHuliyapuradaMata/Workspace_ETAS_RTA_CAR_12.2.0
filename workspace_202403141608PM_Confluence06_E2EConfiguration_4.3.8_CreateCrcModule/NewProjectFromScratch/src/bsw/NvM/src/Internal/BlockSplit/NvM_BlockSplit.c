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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM.h"

#include "rba_MemLib.h"

#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_BlockDescriptor.h"
/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

void NvM_Prv_BlockSplit_Initialize(NvM_BlockIdType idBlock_uo)
{
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgBlockSplit_pcst)
    {
        uint8 idxSubBlock_u8;
        uint8 nrSubBlocks_u8 = CfgBlockSplit_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;

        for (idxSubBlock_u8 = 0u; idxSubBlock_u8 < nrSubBlocks_u8; ++idxSubBlock_u8)
        {
            CfgBlockSplit_pcst->BlockSplit_Master_pcst->idxSubBlock_pu8[idxSubBlock_u8] = 0u;
            CfgBlockSplit_pcst->BlockSplit_Master_pcst->CrcSubBlock_pun[idxSubBlock_u8].Crc32_u32 = 0u;
        }
    }
#else
    (void)idBlock_uo;
#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)
}

uint16 NvM_Prv_BlockSplit_GetOffsetToSubBlock(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8)
{
    uint16 OffsetToSubBlock_u16 = 0u;

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgSplitBlocks_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgSplitBlocks_pcst)
    {
        uint8 nrSubBlocks_u8 = CfgSplitBlocks_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;
        uint8 nrSubBlocksOnMedium_u8 = nrSubBlocks_u8 * 2u;

        if (idxDataset_u8 < nrSubBlocksOnMedium_u8)
        {
            // a sub-block sub-block is in use
            uint8 idxSubBlock_u8;
            for (idxSubBlock_u8 = 0u; idxSubBlock_u8 < (idxDataset_u8 % nrSubBlocks_u8); ++idxSubBlock_u8)
            {
                OffsetToSubBlock_u16 +=
                        CfgSplitBlocks_pcst->BlockSplit_Cfg_pcst->SubBlockLengths_pcu16[idxSubBlock_u8];
            }
        }
        else
        {
            OffsetToSubBlock_u16 = NvM_Prv_BlkDesc_GetSize(idBlock_uo);
        }
    }
#else
    (void)idBlock_uo;
    (void)idxDataset_u8;
#endif

    return OffsetToSubBlock_u16;
}

uint16 NvM_Prv_BlockSplit_GetSizeSubBlock(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8)
{
    uint16 SizeSubBlock_u16 = NvM_Prv_BlkDesc_GetSize(idBlock_uo);

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgSplitBlocks_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgSplitBlocks_pcst)
    {
        uint8 nrSubBlocks_u8 = CfgSplitBlocks_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;
        uint8 nrSubBlocksOnMedium_u8 = nrSubBlocks_u8 * 2u;

        if (idxDataset_u8 < nrSubBlocksOnMedium_u8)
        {
            // a sub-block is in use -> return size of this sub-block
            SizeSubBlock_u16 = CfgSplitBlocks_pcst->BlockSplit_Cfg_pcst->SubBlockLengths_pcu16[idxDataset_u8 % nrSubBlocks_u8];
        }
        else
        {
            SizeSubBlock_u16 = CfgSplitBlocks_pcst->BlockSplit_Cfg_pcst->SizeMaster_uo;
        }
    }
#else
    (void) idxDataset_u8;
#endif

    return SizeSubBlock_u16;
}

boolean NvM_Prv_BlockSplit_IsIdxSubBlock(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8)
{
    boolean isIdxSubBlock_b = FALSE;

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgSplitBlocks_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgSplitBlocks_pcst)
    {
        uint8 nrSubBlocks_u8 = CfgSplitBlocks_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;
        uint8 nrSubBlocksOnMedium_u8 = nrSubBlocks_u8 * 2u;

        isIdxSubBlock_b = (idxDataset_u8 < nrSubBlocksOnMedium_u8);
    }
#else
    (void)idBlock_uo;
    (void)idxDataset_u8;
#endif

    return isIdxSubBlock_b;
}

/* MR12 RULE 8.13 VIOLATION: The statements within the function can be disabled via a compiler-switch but the parameter
   is still required to be changeable if the statements within the function are active. */
void NvM_Prv_BlockSplit_SetChangedAllSubBlocks(NvM_BlockIdType idBlock_uo, uint8* Buffer_pu8)
{
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgBlockSplit_pcst)
    {
        // Invalidate all sub-blocks (if current block is not configured as split block then nothing will be done)
        uint8 idxSubBlock_u8;
        uint8 nrSubBlocks_u8                = CfgBlockSplit_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;
        uint16 OffsetToMasterBlock_u16      = NvM_Prv_BlkDesc_GetSize(idBlock_uo);
        uint8* ArrayIdxSubBlockCurrent_pu8  = CfgBlockSplit_pcst->BlockSplit_Master_pcst->idxSubBlock_pu8;
        uint8* ArrayIdxSubBlockNew_pu8      = &Buffer_pu8[OffsetToMasterBlock_u16];

        for (idxSubBlock_u8 = 0u; idxSubBlock_u8 < nrSubBlocks_u8; ++idxSubBlock_u8)
        {
            if (ArrayIdxSubBlockCurrent_pu8[idxSubBlock_u8] == 0u)
            {
                ArrayIdxSubBlockNew_pu8[idxSubBlock_u8] = 1u;
            }
            else
            {
                ArrayIdxSubBlockNew_pu8[idxSubBlock_u8] = 0u;
            }
        }
    }
#else
    (void)idBlock_uo;
    (void)Buffer_pu8;
#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)
}

/* MR12 RULE 8.13 VIOLATION: The statements within the function can be disabled via a compiler-switch but the parameter
   is still required to be changeable if the statements within the function are active. */
boolean NvM_Prv_BlockSplit_UpdateSubBlock(NvM_BlockIdType idBlock_uo,
                                          uint8* idxDataset_pu8,
                                          uint8* Buffer_pu8,
                                          NvM_Prv_Crc_tun const* Crc_pcun)
{
    boolean isSubBlockChanged_b = FALSE;

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgBlockSplit_pcst)
    {
        uint8 nrSubBlocks_u8 = CfgBlockSplit_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;
        uint8 nrDatasets_u8 = 2u * nrSubBlocks_u8;
        // get index of current sub-block in master-block arrays
        // hint: dataset index is not the same as sub-bock index
        uint8 idxSubBlockCurrent_u8 = *idxDataset_pu8 % nrSubBlocks_u8;
        // get offset to the master-block within internal buffer
        uint16 OffsetToMasterBlock_u16 = NvM_Prv_BlkDesc_GetSize(idBlock_uo);

        // get start address of dataset-index-array within new master-block within internal buffer
        // hint: master-block is always located directly behind user data in internal buffer and
        //       dataset-index-array is always located at the beginning of the master-block
        uint8* idxSubBlock_pu8 = Buffer_pu8 + OffsetToMasterBlock_u16;

        // get start address of CRC for current sub-block in CRC-array within new master-block within internal buffer
        // hint: CRC-array is always located directly behind dataset-index-array in the master-block
        uint8* CrcSubBlock_pu8 = idxSubBlock_pu8 + nrSubBlocks_u8 + (idxSubBlockCurrent_u8 * sizeof(uint32));

        // set CRC of current sub-block in new master-block
        rba_MemLib_MemCopy((uint8 const*)&Crc_pcun->Crc32_u32, CrcSubBlock_pu8, sizeof(uint32));

        if (Crc_pcun->Crc32_u32 != CfgBlockSplit_pcst->BlockSplit_Master_pcst->CrcSubBlock_pun[idxSubBlockCurrent_u8].Crc32_u32)
        {
            // data in current sub-block has been changed
            isSubBlockChanged_b = TRUE;
            // changed data shall be written into the partner-dataset-index of the current sub-block
            idxSubBlock_pu8[idxSubBlockCurrent_u8] = ((*idxDataset_pu8 + nrSubBlocks_u8) % nrDatasets_u8) / nrSubBlocks_u8;
            *idxDataset_pu8 = (*idxDataset_pu8 + nrSubBlocks_u8) % nrDatasets_u8;
        }
        else
        {
            // current sub-block has not been changed by the user, i.e. CRC is still the same
            // -> no write required, dataset-index remains
            idxSubBlock_pu8[idxSubBlockCurrent_u8] = (*idxDataset_pu8 % nrDatasets_u8) / nrSubBlocks_u8;
        }
    }
#else
    (void)idBlock_uo;
    (void)idxDataset_pu8;
    (void)Buffer_pu8;
    (void)Crc_pcun;
#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)

    return isSubBlockChanged_b;
}

boolean NvM_Prv_BlockSplit_IsSubBlockConsistent(NvM_BlockIdType idBlock_uo,
                                                uint8 idxDataset_u8,
                                                uint8 const* IntBuffer_pcu8,
                                                NvM_Prv_Crc_tun const* Crc_pcun)
{
    boolean isSubBlockConsistent = FALSE;

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgBlockSplit_pcst)
    {
        uint8 nrSubBlocks_u8 = CfgBlockSplit_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;
        uint8 idxSubBlockCurrent_u8 = idxDataset_u8 % nrSubBlocks_u8;

        // get offset to the master-block within internal buffer
        uint16 OffsetToMasterBlock_u16 = NvM_Prv_BlkDesc_GetSize(idBlock_uo);

        // get start address of dataset-index-array within new master-block within internal buffer
        // hint: master-block is always located directly behind user data in internal buffer and
        //       dataset-index-array is always located at the beginning of the master-block
        uint8 const* idxSubBlock_pcu8 = &IntBuffer_pcu8[OffsetToMasterBlock_u16];

        // get start address of CRC for current sub-block in CRC-array within new master-block within internal buffer
        // hint: CRC-array is always located directly behind dataset-index-array in the master-block
        uint8 const* CrcSubBlock_pcu8 = idxSubBlock_pcu8 + nrSubBlocks_u8 + (idxSubBlockCurrent_u8 * sizeof(uint32));

        isSubBlockConsistent = rba_MemLib_MemCompareBfr(CrcSubBlock_pcu8,
                                                        (uint8 const*)&Crc_pcun->Crc32_u32,
                                                        sizeof(uint32));
    }
#else
    (void)idBlock_uo;
    (void)idxDataset_u8;
    (void)IntBuffer_pcu8;
    (void)Crc_pcun;
#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)

    return isSubBlockConsistent;
}

void NvM_Prv_BlockSplit_UpdateMasterBlock(NvM_BlockIdType idBlock_uo, uint8 const* Buffer_pcu8)
{
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgBlockSplit_pcst)
    {
        uint8* ArrayIdxSubBlock_pu8 = CfgBlockSplit_pcst->BlockSplit_Master_pcst->idxSubBlock_pu8;
        uint8* ArrayCrcSubBlock_pu8 = (uint8*)CfgBlockSplit_pcst->BlockSplit_Master_pcst->CrcSubBlock_pun;
        uint16 SizeUserData_u16     = NvM_Prv_BlkDesc_GetSize(idBlock_uo);
        uint8 nrSubBlocks_u8        = CfgBlockSplit_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;

        // copy updated and written master-block from internal buffer to the configured master-block
        // master-block contains:
        // - array with dataset indexes for each sub-block
        rba_MemLib_MemCopy(Buffer_pcu8 + SizeUserData_u16, ArrayIdxSubBlock_pu8, nrSubBlocks_u8);
        // - array with CRCs for each sub-block
        rba_MemLib_MemCopy(Buffer_pcu8 + SizeUserData_u16 + nrSubBlocks_u8,
                           ArrayCrcSubBlock_pu8,
                           nrSubBlocks_u8 * sizeof(NvM_Prv_Crc_tun));
    }
#else
    (void)idBlock_uo;
    (void)Buffer_pcu8;
#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)
}

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
uint8 NvM_Prv_BlockSplit_GetIdxSubBlockForInvalidation(uint8 idxSubBlock_u8,
                                                       uint8 nrSubBlocks_u8,
                                                       uint8 const* ArrayIdxSubBlockCurrent_pcu8,
                                                       uint8 const* ArrayIdxSubBlockNew_pcu8)
{
    while (   (idxSubBlock_u8 < nrSubBlocks_u8)
           && (ArrayIdxSubBlockCurrent_pcu8[idxSubBlock_u8] == ArrayIdxSubBlockNew_pcu8[idxSubBlock_u8]))
    {
        ++idxSubBlock_u8;
    }

    return idxSubBlock_u8;
}
#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

