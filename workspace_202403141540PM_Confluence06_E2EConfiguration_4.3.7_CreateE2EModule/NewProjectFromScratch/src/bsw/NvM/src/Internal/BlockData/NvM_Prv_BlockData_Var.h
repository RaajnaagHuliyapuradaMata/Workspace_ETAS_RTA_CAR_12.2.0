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
#ifndef NVM_PRV_BLOCKDATA_VAR_H
#define NVM_PRV_BLOCKDATA_VAR_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
//#include "rba_MemLib_Special.h"

//#include "NvM_Prv_Service_Types.h"
//#include "NvM_Prv_BlockDescriptor.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
// Declaration of variables containing administrative data for all NVRAM blocks.
// ------------------------------------------------------------------------------------------------
// TRACE[NVM135] Administrative block is private
#define NVM_START_SEC_VAR_SAVED_ZONE_16
#include "NvM_MemMap.h"
/**
 * Declaration of the array containing attribute/status information of all NVRAM blocks.
 */
extern uint16 NvM_Prv_stBlock_auo[NVM_CFG_NR_ALL_BLOCKS];

#define NVM_STOP_SEC_VAR_SAVED_ZONE_16
#include "NvM_MemMap.h"

/************************************************/

#define NVM_START_SEC_VAR_SAVED_ZONE_32
#include "NvM_MemMap.h"

#if (defined(NVM_RB_BLOCK_WRITE_COUNTER) && (STD_ON == NVM_RB_BLOCK_WRITE_COUNTER))
/**
 * Declaration of the array containing block write counters.
 *
 * Block write counters are only available if enabled by configuration, see NVM_RB_BLOCK_WRITE_COUNTER.
 */
extern uint32 NvM_Prv_cntrWrite_auo[NVM_CFG_NR_ALL_BLOCKS];
#endif

#define NVM_STOP_SEC_VAR_SAVED_ZONE_32
#include "NvM_MemMap.h"

/************************************************/

#define NVM_START_SEC_VAR_CLEARED_16
#include "NvM_MemMap.h"
/**
 * Declaration of the array containing all requests pending on each NVRAM block.
 */
extern uint16 NvM_Prv_stRequests_au16[NVM_PRV_NR_BLOCKS_ADMIN];

#define NVM_STOP_SEC_VAR_CLEARED_16
#include "NvM_MemMap.h"

/************************************************/

#define NVM_START_SEC_VAR_CLEARED_8
#include "NvM_MemMap.h"
/**
 * Declaration of the array containing results of the last asynchronous request for all NVRAM blocks.
 */
extern NvM_RequestResultType NvM_Prv_stRequestResult_au8[NVM_PRV_NR_BLOCKS_ADMIN];
#if (NVM_RB_ERROR_INFO_DETAIL == STD_ON)
/**
 * Declaration of the array containing detailed error information of the last read request for all NVRAM blocks.
 */
extern NvM_Rb_ErrorInfoDetail_ten NvM_Prv_stDetailedErrorInfo_aen[NVM_PRV_NR_BLOCKS_ADMIN];
#endif
/**
 * Declaration of the array containing indexes for all NVRAM block.
 */
extern uint8 NvM_Prv_idxDataSet_au8[NVM_PRV_NR_BLOCKS_ADMIN];

#define NVM_STOP_SEC_VAR_CLEARED_8
#include "NvM_MemMap.h"

/************************************************/

# define NVM_START_SEC_VAR_SAVED_ZONE_16
# include "NvM_MemMap.h"
/**
 * Declaration of the permanent RAM block for the configuration ID.
 */
extern uint16 NvM_Prv_idConfigStored_u16;

# define NVM_STOP_SEC_VAR_SAVED_ZONE_16
# include "NvM_MemMap.h"

/* NVM_PRV_BLOCKDATA_VAR_H */
#endif
