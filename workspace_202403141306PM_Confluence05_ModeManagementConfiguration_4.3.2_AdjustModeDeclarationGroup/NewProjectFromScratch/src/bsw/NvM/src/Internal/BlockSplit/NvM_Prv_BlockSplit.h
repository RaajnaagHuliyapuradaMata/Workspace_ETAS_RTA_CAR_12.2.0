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
#ifndef NVM_PRV_BLOCKSPLIT_H
#define NVM_PRV_BLOCKSPLIT_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_BlockSplit_Types.h"
#include "NvM_Prv_JobStateMachine_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_BlockSplit_Read_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_BlockSplit_Write_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_BlockSplit_Invalidate_GetData(void);

extern void NvM_Prv_BlockSplit_Initialize(NvM_BlockIdType idBlock_uo);
extern uint16 NvM_Prv_BlockSplit_GetOffsetToSubBlock(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8);
extern uint16 NvM_Prv_BlockSplit_GetSizeSubBlock(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8);
extern boolean NvM_Prv_BlockSplit_IsIdxSubBlock(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8);
extern void NvM_Prv_BlockSplit_SetChangedAllSubBlocks(NvM_BlockIdType idBlock_uo, uint8* Buffer_pu8);
extern boolean NvM_Prv_BlockSplit_UpdateSubBlock(NvM_BlockIdType idBlock_uo,
                                                 uint8* idxDataset_pu8,
                                                 uint8* Buffer_pu8,
                                                 NvM_Prv_Crc_tun const* Crc_pcun);

extern boolean NvM_Prv_BlockSplit_IsSubBlockConsistent(NvM_BlockIdType idBlock_uo,
                                                       uint8 idxDataset_u8,
                                                       uint8 const* IntBuffer_pcu8,
                                                       NvM_Prv_Crc_tun const* Crc_pcun);
extern void NvM_Prv_BlockSplit_UpdateMasterBlock(NvM_BlockIdType idBlock_uo, uint8 const* Buffer_pcu8);

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
extern uint8 NvM_Prv_BlockSplit_GetIdxSubBlockForInvalidation(uint8 idxSubBlock_u8,
                                                              uint8 nrSubBlocks_u8,
                                                              uint8 const* ArrayIdxSubBlockCurrent_pcu8,
                                                              uint8 const* ArrayIdxSubBlockNew_pcu8);
#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_BLOCKSPLIT_H */
#endif
