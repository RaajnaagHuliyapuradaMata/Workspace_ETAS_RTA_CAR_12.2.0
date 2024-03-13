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
#include "rba_MemLib_Special.h"
#include "NvM.h"
#include "NvM_Prv_BlockDescriptor.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define NVM_START_SEC_VAR_SAVED_ZONE_8
#include "NvM_MemMap.h"

// Arrays with indexes for used sub-blocks in non-volatile memory for all blocks configured to use block splitting
#define NVM_STOP_SEC_VAR_SAVED_ZONE_8
#include "NvM_MemMap.h"

#define NVM_START_SEC_VAR_SAVED_ZONE_32
#include "NvM_MemMap.h"

//-------------------------------------------------------------------------------------------------
// Arrays with sub-block CRCs for all blocks configured to use block splitting
//-------------------------------------------------------------------------------------------------
#define NVM_STOP_SEC_VAR_SAVED_ZONE_32
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_16
#include "NvM_MemMap.h"

//-------------------------------------------------------------------------------------------------
// Arrays with configured sub-block lengths for all blocks configured to use block splitting
//-------------------------------------------------------------------------------------------------
#define NVM_STOP_SEC_CONST_16
#include "NvM_MemMap.h"

#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

//-------------------------------------------------------------------------------------------------
// Arrays with master blocks for all blocks configured to use block splitting
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Administrative data for all blocks configured to use block splitting
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Configuration data for all blocks configured to use block splitting
//-------------------------------------------------------------------------------------------------

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

