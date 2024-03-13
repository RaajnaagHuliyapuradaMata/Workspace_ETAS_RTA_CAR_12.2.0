/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
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
#include "NvM_Prv.h"
/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#if (NVM_RB_RAM_INIT_CHECK == NVM_RB_RAM_INIT_CHECK_QUICK)

# define NVM_START_SEC_VAR_SAVED_ZONE_32
# include "NvM_MemMap.h"

/// The marker which signals whether the NvM data in the saved zone RAM has been lost after a reset.
static uint32 NvM_Prv_SavedZoneMarker_u32;

# define NVM_STOP_SEC_VAR_SAVED_ZONE_32
# include "NvM_MemMap.h"


#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

static NvM_Prv_SavedZoneMarker_tcpu32 NvM_Prv_SavedZoneMarker_acpu32[NVM_RB_RAM_INIT_CHECK_NR_SAVEDZONES] = 
{
    &NvM_Prv_SavedZoneMarker_u32,
};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"
/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This function sets the current value of the saved zone marker with index idxSavedZone_u8.
 *
 * \param MarkerSavedZone_u32
 * New value for the saved zone marker
 * \param idxSavedZone_u8
 * Arrayindex of the saved zone marker
 */
void NvM_Prv_SetMarkerSavedZoneId(uint8 idxSavedZone_u8, uint32 MarkerSavedZone_u32)
{
    *(NvM_Prv_SavedZoneMarker_acpu32[idxSavedZone_u8]) = MarkerSavedZone_u32;
}

/**
 * \brief
 * This function returns the current value of the saved zone marker with index idxSavedZone_u8.
 *
 * \return
 * Current value of the saved zone marker
 * \param idxSavedZone_u8
 * Arrayindex of the saved zone marker
 */
uint32 NvM_Prv_GetMarkerSavedZoneId(uint8 idxSavedZone_u8)
{
    return *(NvM_Prv_SavedZoneMarker_acpu32[idxSavedZone_u8]);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif
