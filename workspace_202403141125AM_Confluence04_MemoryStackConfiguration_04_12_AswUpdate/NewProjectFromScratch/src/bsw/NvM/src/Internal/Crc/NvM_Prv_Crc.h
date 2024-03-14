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
#ifndef NVM_PRV_CRC_H
#define NVM_PRV_CRC_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Types.h"
#include "NvM_Prv_Crc_Types.h"
#include "NvM_Prv_BlockDescriptor.h"
#include "NvM_Prv_JobStateMachine_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_Crc_Calculate_GetData(void);

extern void NvM_Prv_Crc_Initialize(void);
extern void NvM_Prv_Crc_Calculate(NvM_BlockIdType idBlock_uo,
                                  uint16 nrBytesToCalculate_u16,
                                  NvM_Prv_Job_CrcCalculation_tst* dataCrcCalculation_pst);
extern boolean NvM_Prv_Crc_CheckRamBlockCrc(NvM_BlockIdType idBlock_uo, NvM_Prv_Crc_tun const* Crc_pcun);
extern void NvM_Prv_Crc_SetRamBlockCrc(NvM_BlockIdType idBlock_uo, NvM_Prv_Crc_tun const* Crc_pcun);
extern boolean NvM_Prv_Crc_IsRamBlockCrcRecalcRequired(NvM_BlockIdType* idBlock_puo);
extern boolean NvM_Prv_Crc_CheckCrc(NvM_BlockIdType idBlock_uo,
                                    uint8 const* intBuf_pcu8,
                                    uint16 const* nrUsedBytes_pcu16,
                                    NvM_Prv_Crc_tun const* Crc_pcun);
extern void NvM_Prv_Crc_AppendCrc(NvM_BlockIdType idBlock_uo,
                                  uint8* intBuf_pu8,
                                  uint16* nrUsedBytes_pu16,
                                  NvM_Prv_Crc_tun const* Crc_pcun);

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
extern void NvM_Prv_Crc_ResetAllRamBlockCrcs(void);
extern void NvM_Prv_Crc_ResetRamBlockCrc(NvM_BlockIdType idBlock_uo);
extern void NvM_Prv_Crc_GetRamBlockCrc(NvM_BlockIdType idBlock_uo, NvM_Prv_Crc_tun* Crc_pun);
extern NvM_BlockIdType NvM_Prv_Crc_GetIdRamBlockCrcRecalc(void);
extern void NvM_Prv_Crc_InjectNvCrcError(uint8* crcByte_pu8);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"


/* NVM_PRV_CRC_H */
#endif

