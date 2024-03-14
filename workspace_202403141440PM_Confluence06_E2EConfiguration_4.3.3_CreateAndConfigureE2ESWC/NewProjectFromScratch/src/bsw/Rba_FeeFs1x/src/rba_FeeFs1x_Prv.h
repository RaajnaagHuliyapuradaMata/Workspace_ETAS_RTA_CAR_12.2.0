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

#ifndef RBA_FEEFS1X_PRV_H
#define RBA_FEEFS1X_PRV_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

# if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "rba_FeeFs1x_Prv_Cfg.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
#define RBA_FEEFS1X_SECTORMARKER_PREAMBLE                   (0xCAFEu)
#define RBA_FEEFS1X_MAXUINT32                               (0xFFFFFFFFuL)
#define RBA_FEEFS1X_MAXUINT16                               (0xFFFFu)
#define RBA_FEEFS1X_MAXUINT8                                (0xFFu)

#define RBA_FEEFS1X_PRV_BLK_HDR_SIZE                        (14u)   // Size of header
#define RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR     ((RBA_FEEFS1X_PRV_BLK_HDR_SIZE/RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE) + 1u)
#define RBA_FEEFS1X_PRV_NR_OF_MARKERS                       (12u)   // Nr of markers in the header area
#define RBA_FEEFS1X_PRV_SECTOR_HEADER_SIZE                  (RBA_FEEFS1X_PRV_NR_OF_MARKERS * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE)
#define RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES              (3u) // Maximum number of retries for read, write and compare operations in case of failures
/* Size of the data to be PREFETCHED in the Medium buffer. Setting it to max value means that the complete Medium buffer
* will be loaded. Medium unit is already robust against when the value passed is more then the actual medium buffer size.
* The Medium will load only the data that can fit into its buffer. Since Fee1x does not know the buffer size,
* it sends this max value when ever it wants to reload the complete buffer. The buffer size is not fixed and configurable
* and can be sometimes 1024 bytes, 2048 bytes or 10k bytes. basically, the amount of RAM applicaiton can allocate. */
#  ifndef RBA_FEEFS1X_PRV_MAX_BUFFER_SIZE_TO_BE_PREFETCHED
#define RBA_FEEFS1X_PRV_MAX_BUFFER_SIZE_TO_BE_PREFETCHED     (RBA_FEEFS1X_MAXUINT32)
#  endif

#  if (2u < RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE)
#define RBA_FEEFS1X_PRV_IS_MULTI_SECTOR_EMULATION (TRUE)
#  else
#define RBA_FEEFS1X_PRV_IS_MULTI_SECTOR_EMULATION (FALSE)
#  endif

/* Below macro is used to convert a static variable to global scope for component testing. The scope is changed only
 * for testing purpose and not part of the final hex. Also, only the variables that are currently needed for component testing
 * are being remapped and not all variables*/
#  if(defined(RBA_FEEFS1X_PRV_CFG_SIMULATION_TEST_ACTIVE) &&  (TRUE == RBA_FEEFS1X_PRV_CFG_SIMULATION_TEST_ACTIVE))
#define RBA_FEEFS1X_VAR_SCOPE(var) extern var; var;
#  else
#define RBA_FEEFS1X_VAR_SCOPE(var) static var;
#  endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

typedef enum
{
    rba_FeeFs1x_Pending_e,
    rba_FeeFs1x_JobOK_e,
    rba_FeeFs1x_JobFailed_e,
    rba_FeeFs1x_ErrorInternal_e,
    rba_FeeFs1x_ErrorExternal_e
} rba_FeeFs1x_RetVal_ten;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Private inline functions
 **********************************************************************************************************************
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isMaintainActive(void);
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isTriggerReorgActive(void);
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isSurvivalActive(void);
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isReorgUnknown(void);
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isNoFallbackActive(void);
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isBlankChkRequired(void);
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isMultiSectorEmulation(void);

/**
 * \brief   Is the maintain functionality to maintain the redundant blocks active
 *
 * \retval  FALSE   Maintain feature inactive
 * \retval  TRUE    Maintain feature active
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isMaintainActive(void)
{
#  if defined(RBA_FEEFS1X_PRV_CFG_MAINTAIN)
    return (RBA_FEEFS1X_PRV_CFG_MAINTAIN);
#  else
    return (FALSE);
#  endif
}

/**
 * \brief   Is the forced sector reorganisation active or inactive
 *
 * \retval  FALSE   Trigger Reorg feature inactive
 * \retval  TRUE    Trigger Reorg feature active
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isTriggerReorgActive(void)
{
#  if defined(RBA_FEEFS1X_PRV_CFG_TRIGGER_REORG)
    return (RBA_FEEFS1X_PRV_CFG_TRIGGER_REORG);
#  else
    return (FALSE);
#  endif
}

/**
 * \brief   Is the survival feature active or not
 *
 * \retval  FALSE   Survival reorg is disabled
 * \retval  TRUE    Survival reorg is activated
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isSurvivalActive(void)
{
#  if defined(RBA_FEEFS1X_PRV_CFG_SURVIVAL_EVAL)
    return (RBA_FEEFS1X_PRV_CFG_SURVIVAL_EVAL);
#  else
    return (FALSE);
#  endif
}

/**
 * \brief   Shall unknown blocks be reorganized or not by default (CB-Mode yes or no)
 *
 * \retval  FALSE   ASW-Mode:   Do throw away unknown blocks during reorganization (if not survival)
 * \retval  TRUE    CB-Mode:    Do not throw away unknown blocks during reorganization
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isReorgUnknown(void)
{
#  if defined(RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_REORG)
    return (RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_REORG);
#  else
    return (FALSE);
#  endif
}

/**
 * \brief   Is the NoFallback feature active or inactive
 *
 * \retval  FALSE   NoFallback feature inactive
 * \retval  TRUE    NoFallback feature active
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isNoFallbackActive(void)
{
#  if defined(RBA_FEEFS1X_PRV_CFG_NO_FALLBACK_ACTIVE)
    return (RBA_FEEFS1X_PRV_CFG_NO_FALLBACK_ACTIVE);
#  else
    return (FALSE);
#  endif
}

/**
 * \brief   Is blank check required to verify if the page is programmed or erased
 *
 * \retval  FALSE   blank check not required
 * \retval  TRUE    blank check required
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isBlankChkRequired(void)
{
#  if defined(RBA_FEEFS1X_PRV_CFG_HEADER_RECHECK_ACTIVE)
    return (RBA_FEEFS1X_PRV_CFG_HEADER_RECHECK_ACTIVE);
#  else
    return (FALSE);
#  endif
}

/**
 * \brief   Is multi sector handling needed?
 *
 * \retval  FALSE   Two sector emuation -> Multi sector handling not needed
 * \retval  TRUE    More then two logical sector -> multi sector handling needed
*/
LOCAL_INLINE boolean rba_FeeFs1x_Prv_isMultiSectorEmulation(void)
{
    return (RBA_FEEFS1X_PRV_IS_MULTI_SECTOR_EMULATION);
}

# endif /* (defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED)) */

#endif

