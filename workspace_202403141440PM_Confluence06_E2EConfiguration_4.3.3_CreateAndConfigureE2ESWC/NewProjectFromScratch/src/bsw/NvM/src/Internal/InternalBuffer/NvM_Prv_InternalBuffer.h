/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_INTERNALBUFFER_H
#define NVM_PRV_INTERNALBUFFER_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv.h"
#include "NvM_Prv_Job_Types.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

// RAM mirror: Used in explicit synchronization and for Nv CRC
#if (defined(NVM_PRV_HAS_RAM_MIRROR) && (NVM_PRV_HAS_RAM_MIRROR == STD_ON))

# if (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_OFF)

#  define NVM_START_SEC_VAR_SECURED_CACHED_RAM_ALIGNED_CLEARED_UNSPECIFIED
#  include "NvM_MemMap.h"

extern uint8 NvM_Prv_RamMirror_au8[NVM_PRV_NR_PARALLEL_JOB * (NVM_PRV_SIZE_RAM_MIRROR + NVM_PRV_CFG_SIZE_CRYPTO_BUFFER)];

#  define NVM_STOP_SEC_VAR_SECURED_CACHED_RAM_ALIGNED_CLEARED_UNSPECIFIED
#  include "NvM_MemMap.h"

# elif (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON)

#  define NVM_START_SEC_VAR_SECURED_CLEARED_UNSPECIFIED
#  include "NvM_MemMap.h"

extern uint8 *NvM_Prv_RamMirror_au8;

#  define NVM_STOP_SEC_VAR_SECURED_CLEARED_UNSPECIFIED
#  include "NvM_MemMap.h"

#  define NVM_START_SEC_VAR_SECURED_CLEARED_32
#  include "NvM_MemMap.h"

extern uint32 NvM_Prv_RntRamMirrorSize_u32;

#  define NVM_STOP_SEC_VAR_SECURED_CLEARED_32
#  include "NvM_MemMap.h"

# endif

#endif

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern uint8* NvM_Prv_InternalBuffer_GetPtr(void);

extern uint32 NvM_Prv_InternalBuffer_GetSize(void);

extern uint16* NvM_Prv_InternalBuffer_GetPtrToUsedSize(void);

extern NvM_Prv_JobResult_ten NvM_Prv_InternalBuffer_CopyData(NvM_BlockIdType idBlock_uo,
                                                             NvM_Prv_ptrRamBlock_tun userData_un,
                                                             uint8* IntBuffer_pu8,
                                                             boolean CopyIntoBuf_b);

#if ((NVM_PRV_HAS_RAM_MIRROR == STD_ON) && (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON))

extern void   NvM_Prv_InitRuntimeRamMirror(uint8* RamMirror_pu8, uint32 RuntimeRamMirrorSize_u32);

#endif

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
extern void NvM_Prv_InternalBuffer_ResetData(void);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_INTERNALBUFFER_H */
#endif

