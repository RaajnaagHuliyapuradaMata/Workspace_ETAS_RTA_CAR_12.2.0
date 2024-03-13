/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_BLOCKDATA_H
#define NVM_PRV_BLOCKDATA_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_MemLib_Special.h"

#include "NvM_Prv_Service_Types.h"
#include "NvM_Prv_BlockDescriptor.h"
#include "NvM_Prv_BlockData_Var.h"

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
extern void NvM_Prv_Block_InitializeData(boolean const *isSavedZoneDataLost_pcb);
#if (NVM_RB_AUX_INTERFACE == STD_ON)
extern void NvM_Prv_Block_InitializeDataAux(void);
#endif

extern void NvM_Prv_Block_SetIsNvmEnqueuingForMulti(NvM_Prv_Block_IsNvmEnqueuingForMulti_tpfct IsNvmEnqueuingForMulti_pfct);
extern boolean NvM_Prv_Block_IsNvmEnqueuingForMulti(NvM_BlockIdType idBlock_uo,
                                                    NvM_Prv_Service_InitiationErrors_tuo* Errors_puo);

/*
**********************************************************************************************************************
* Inline functions declarations
**********************************************************************************************************************
*/
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRamCrcRecalcRequired(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRamCrcRecalcOngoing(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsWriteRequired(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsPRamBlockValid(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsChanged(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsTrgWriteAllActive(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRomDataAmbiguous(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsInRom(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsWriteProtected(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsReadDone(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsJobOngoing(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsWriteable(NvM_BlockIdType idBlock_uo,
                                                          uint8 idxDataset_u8,
                                                          NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_Block_GetIdxDataset(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_RequestResultType NvM_Prv_Block_GetRequestResult(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Rb_ErrorInfoDetail_ten NvM_Prv_Block_GetErrorInfoDetail(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRequestPending(NvM_BlockIdType idBlock_uo, uint16 maskService_u16);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsPending(NvM_BlockIdType idBlock_uo,
                                                        NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetIdxData(NvM_BlockIdType idBlock_uo, uint8 idxData_u8);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetRequestResult(NvM_BlockIdType idBlock_uo, NvM_RequestResultType Result_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetErrorInfoDetail(NvM_BlockIdType idBlock_uo,
                                                              NvM_Rb_ErrorInfoDetail_ten errorInfoDetail_en);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetRequest(NvM_BlockIdType idBlock_uo, NvM_Prv_ServiceBit_tuo ServiceBit_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_ClearRequests(NvM_BlockIdType idBlock_uo, uint16 maskRequests_u16);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetState(NvM_BlockIdType idBlock_uo,
                                                    uint16 maskBitsToChange_uo,
                                                    uint16 maskBitsNewValue_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_HasIdConfigChanged(void);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetIdConfig(uint16 idConfigNew_u16);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_InitIdConfigDuringWriteAll(void);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_Block_AppendWriteCounter(NvM_BlockIdType idBlock_uo,
                                                                uint8* Buffer_pu8);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_ExtractWriteCounter(NvM_BlockIdType idBlock_uo,
                                                               uint8 const* Buffer_pu8,
                                                               uint16* SizeInBytes_pu16);

#if (defined(NVM_RB_BLOCK_WRITE_COUNTER) && (STD_ON == NVM_RB_BLOCK_WRITE_COUNTER))
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint32 NvM_Prv_Block_GetWriteCounter(NvM_BlockIdType idBlock_uo);
#endif

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_Block_GetState(NvM_BlockIdType idBlock_uo);

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline functions
 **********************************************************************************************************************
 */
/* Include the actual definitions of the provided inline functions */
#include "NvM_Prv_BlockData_Inl.h"
#include "NvM_Prv_BlockData_WriteCntr_Inl.h"

/* NVM_PRV_BLOCKDATA_H */
#endif
