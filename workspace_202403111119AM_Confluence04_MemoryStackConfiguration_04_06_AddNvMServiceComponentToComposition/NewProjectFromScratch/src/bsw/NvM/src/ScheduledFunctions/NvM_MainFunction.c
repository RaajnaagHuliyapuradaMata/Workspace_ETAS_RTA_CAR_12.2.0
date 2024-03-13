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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "NvM_Prv_HideRteApi.h"

#include "NvM_Cfg_SchM.h"

#include "NvM_Prv.h"
#include "NvM_Prv_MemIf.h"
#include "NvM_Prv_JobSuspension.h"
#include "NvM_Prv_ErrorDetection.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
#include "NvM_Prv_JobQueue.h"
#endif
/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
/**
 * Function-like macro: LIST_ENTRY_ID_JOB_FOR_SERVICE_BIT
 *
 * This macro generates an entry for a job ID in a list of all service bits provided by NvM.
 * s.a. macro LIST_SERVICE_BIT to generate a list for all service bits provided by NvM
 *
 * Paramater:
 * Unused: unused
 * JobName: name of the job for which a job ID will be generated.
 */
#define LIST_ENTRY_ID_JOB_FOR_SERVICE_BIT(Unused, JobName)     NvM_Prv_idJob_##JobName##_e,

/**
 *  Protection mechanism configurable via NvM_Cfg_SchM.h, enabled by default
 *
 *  TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3265]
 *  The re-entrant protection mechanism shall be provided as configurable option
 */
#ifndef NVM_RB_USE_PROTECTION
#define NVM_RB_USE_PROTECTION   (TRUE)
#endif


#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

static NvM_Prv_idJob_ten const NvM_Prv_MapServiceBitToIdJob_caen[NvM_Prv_ServiceBit_nr_e] =
{
    /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
    LIST_SERVICE_BIT(LIST_ENTRY_ID_JOB_FOR_SERVICE_BIT)
};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define NVM_START_SEC_VAR_CLEARED_8
#include "NvM_MemMap.h"

/*
 * Flag to hold the main function access status
 */
static volatile boolean NvM_Prv_flgUsedSema_b;

#define NVM_STOP_SEC_VAR_CLEARED_8
#include "NvM_MemMap.h"

#define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/**
 * Global NvM state information which is not block-specific.
 */
NvM_Prv_MainStates_tst NvM_Prv_Main_st;

#define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"


#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"
/*
 **********************************************************************************************************************
 * Static Declaration
 **********************************************************************************************************************
 */
LOCAL_INLINE void NvM_Prv_MainFunctionIntern(void);
LOCAL_INLINE boolean NvM_Prv_TryToGetLock(void);
LOCAL_INLINE void NvM_Prv_ReleaseLock(void);

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
void NvM_Prv_InitializeMainStates(void)
{
    // Preset NvM state to Idle
    NvM_Prv_Main_st.Activity_rAMwM_en = NVM_PRV_ACTIVITY_IDLE;
    // Preset ID of the active queue to a value which signals that no queue is active
    NvM_Prv_Main_st.idQueueActive_uo = NvM_Prv_idQueue_nrQueues_e;
    // Preset ID of the active service to a value which signals that no service is active
    NvM_Prv_Main_st.idServiceActive_uo = NVM_SERVICE_ID_INIT;
    // Preset ReadAllDone status to FALSE.
    NvM_Prv_Main_st.isReadAllDone_b = FALSE;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    // Preset high speed mode as not active and not required
    NvM_Prv_Main_st.isHighSpeedModeActive_b = FALSE;
    NvM_Prv_Main_st.isHighSpeedModeRequired_b = FALSE;
#endif

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
    NvM_Prv_Main_st.isInitVectorAvailable_b = FALSE;
#endif
}

void NvM_Prv_UninitializeMainStates(void)
{
    // Preset NvM state to NotInit to make sure that NvM will not accept any requests
    NvM_Prv_Main_st.Activity_rAMwM_en = NVM_PRV_ACTIVITY_NOT_INIT;
    // Preset ID of the active queue to a value which signals that no queue is active
    NvM_Prv_Main_st.idQueueActive_uo = NvM_Prv_idQueue_nrQueues_e;
    // Preset ID of the active service to a value which signals that no service is active
    NvM_Prv_Main_st.idServiceActive_uo = NVM_SERVICE_ID_INIT;
    // Preset ReadAllDone status to FALSE.
    NvM_Prv_Main_st.isReadAllDone_b = FALSE;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    // Preset high speed mode as not active and not required
    NvM_Prv_Main_st.isHighSpeedModeActive_b = FALSE;
    NvM_Prv_Main_st.isHighSpeedModeRequired_b = FALSE;
#endif

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
    NvM_Prv_Main_st.isInitVectorAvailable_b = FALSE;
#endif
}

boolean NvM_Prv_IsNvmInitialized(void)
{
    return (NVM_PRV_ACTIVITY_NOT_INIT != NvM_Prv_Main_st.Activity_rAMwM_en);
}

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))

void NvM_Prv_RequireHighSpeedMode(boolean isHighSpeedModeRequired_b)
{
    NvM_Prv_Main_st.isHighSpeedModeRequired_b = isHighSpeedModeRequired_b;
}

void NvM_Prv_SetHighSpeedMode(boolean stHighSpeedMode_b)
{
    NvM_Prv_Main_st.isHighSpeedModeActive_b = stHighSpeedMode_b;
    NvM_Prv_Main_st.isHighSpeedModeRequired_b = FALSE;
}

boolean NvM_Prv_IsHighSpeedModeActive(void)
{
    return NvM_Prv_Main_st.isHighSpeedModeActive_b;
}

boolean NvM_Prv_IsHighSpeedModeRequired(void)
{
    return NvM_Prv_Main_st.isHighSpeedModeRequired_b;
}

#endif

/**
 * \brief
 * This NvM private function returns whether the NvM is currently idle.
 *
 * \detailed
 * The state idle means that NvM is neither processing user requests nor executing internal tasks
 * like RAM block CRC re-calculation.
 *
 * \return
 * - TRUE = NvM is idle
 * - FALSE = NvM is busy
 */
boolean NvM_Prv_IsNvmIdle(void)
{
    return (NVM_PRV_ACTIVITY_IDLE == NvM_Prv_Main_st.Activity_rAMwM_en);
}

/**
 * \brief
 * This NvM private function returns whether the recalculation of RAM block CRCs is currently ongoing.
 *
 * \return
 * - TRUE = NvM is re-calculating RAM block CRCs
 * - FALSE = NvM is not re-calculating RAM block CRCs
 */
boolean NvM_Prv_IsCrcRecalcActive(void)
{
    return (NVM_PRV_ACTIVITY_RAM_BLOCK_CRC == NvM_Prv_Main_st.Activity_rAMwM_en);
}

boolean NvM_Prv_IsMultiActive(void)
{
    return (NvM_Prv_idQueue_Multi_e == NvM_Prv_Main_st.idQueueActive_uo);
}

NvM_Prv_idService_tuo NvM_Prv_GetActiveService(void)
{
    return NvM_Prv_Main_st.idServiceActive_uo;
}

NvM_Prv_idQueue_tuo NvM_Prv_GetActiveQueue(void)
{
    return NvM_Prv_Main_st.idQueueActive_uo;
}

void NvM_Prv_UpdateActiveServiceData(NvM_Prv_idService_tuo idService_uo, NvM_Prv_idQueue_tuo idReqQueue_uo)
{
    NvM_Prv_Main_st.idServiceActive_uo = idService_uo;
    NvM_Prv_Main_st.idQueueActive_uo = idReqQueue_uo;
}

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
/**
 * \brief
 * This private function returns status of the initialization vector to be used for encryption.
 *
 * \return
 * - TRUE = initialization vector available -> encryption possible
 * - FALSE = initialization vector not available -> no encryption possible
 */
boolean NvM_Prv_IsInitVectorAvailable(void)
{
    return NvM_Prv_Main_st.isInitVectorAvailable_b;
}

/**
 * \brief
 * This private function sets the status of the initialization vector to be used for encryption.
 *
 * \param [in] isInitVectorAvalable_b
 * New initialization vector status
 */
void NvM_Prv_SetInitVectorStatus(boolean isInitVectorAvalable_b)
{
    NvM_Prv_Main_st.isInitVectorAvailable_b = isInitVectorAvalable_b;
}

/**
 * \brief
 * This private function returns pointer to the initialization vector which can be used for encryption.
 *
 * \return
 * Pointer to the initialization vector
 */
uint8* NvM_Prv_GetInitVectorCryptoKey(void)
{
    return ((uint8*)&NvM_Prv_Main_st.InitVectorCryptoKey_auo);
}

/**
 * \brief
 * This private function increments current initialization vector to get new value.
 * \details
 * The NvM invokes this function if new initialization vector is required.
 * New initialization vector is required for each encryption of user data.
 */
void NvM_Prv_IncrementInitVector(void)
{
    uint32 i = 0u;

    do
    {
        ++(NvM_Prv_Main_st.InitVectorCryptoKey_auo[i]);
        ++i;

    } while ((i < NVM_PRV_LENGTH_RANDOM_NR) && (0u == ((uint8*)&NvM_Prv_Main_st.InitVectorCryptoKey_auo)[i - 1u]));
}
#endif

NvM_Prv_idJob_ten NvM_Prv_GetJobId(NvM_Prv_ServiceBit_tuo ServiceBit_uo)
{
    if (ServiceBit_uo >= NvM_Prv_ServiceBit_nr_e)
    {
        ServiceBit_uo = NvM_Prv_ServiceBit_Unspecified_e;
    }

    return NvM_Prv_MapServiceBitToIdJob_caen[ServiceBit_uo];
}

/**
 * \brief
 * This NvM private function returns whether a single block processing is independant on ReadAll or not.
 *
 * If the NoSingleBlockBeforeReadAll feature is not activated then Single block processing is independant on ReadAll.
 *
 * \return
 * - TRUE:  Single block processing is independant on ReadAll (NoSingleBlockBeforeReadAll feature is not activated).
 *          Or
 *          Single block processing is dependant on ReadAll (NoSingleBlockBeforeReadAll feature is activated) and
 *          a ReadAll request was executed.
 * - FALSE: Single block processing is dependant on ReadAll (NoSingleBlockBeforeReadAll feature is activated) and
 *          no ReadAll request is executed.
 */
boolean NvM_Prv_IsSingleBlockProcessingIndependentOnReadAll(void)
{
#if (defined(NVM_RB_NO_SINGLE_BLOCK_BEFORE_READALL) && (NVM_RB_NO_SINGLE_BLOCK_BEFORE_READALL == STD_ON))
    return NvM_Prv_Main_st.isReadAllDone_b;
#else
    return TRUE;
#endif
}

/**
 * \brief
 * This NvM private function sets the global NvM state information isReadAllDone to TRUE if the Service calling this
 * function is ReadAll or InitAfterReadAll.
 * \idService_uo
 * ID of the serivice calling this setter function.
 */
void NvM_Prv_SetReadAllDone(NvM_Prv_idService_tuo idService_uo)
{
    if ((NVM_SERVICE_ID_READ_ALL == idService_uo) ||
        (NVM_SERVICE_ID_RB_INIT_AFTER_READ_ALL == idService_uo))
    {
        NvM_Prv_Main_st.isReadAllDone_b = TRUE;
    }
}

/**
 * \brief
 * The internal main function does the processing of NvM jobs.
 *
 */
LOCAL_INLINE void NvM_Prv_MainFunctionIntern(void)
{
    // TRACE[NVM464] Service for performing the processing of the NvM jobs
    // TRACE[NVM256] Same as NVM464
    // TRACE[NVM324] No assumption is made about the invocation timing relative to other BSW main processing functions

    // TRACE[NVM257] Check for proper initialization before doing any real job processing
    // Note: This is done in a strict way here, thereby trying to catch general RAM corruption e.g. by wild pointers
    if ((NvM_Prv_Main_st.Activity_rAMwM_en != NVM_PRV_ACTIVITY_NOT_INIT) &&
        (NvM_Prv_Main_st.Activity_rAMwM_en != NVM_PRV_ACTIVITY_IDLE) &&
        (NvM_Prv_Main_st.Activity_rAMwM_en != NVM_PRV_ACTIVITY_BUSY) &&
        (NvM_Prv_Main_st.Activity_rAMwM_en != NVM_PRV_ACTIVITY_RAM_BLOCK_CRC))
    {
        // Disallow any further job processing
        NvM_Prv_Main_st.Activity_rAMwM_en = NVM_PRV_ACTIVITY_NOT_INIT;
    }

    // If NvM is not initialized
    // -> refuse to do anything, report error to Det if this functionality is enabled
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_MAIN_FUNCTION, 0u))
    {
#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
        NvM_Prv_JobSuspension_PrepareHighSpeedMode(NvM_Prv_Main_st.isHighSpeedModeRequired_b);
#endif
        NvM_Prv_JobSuspension_Handle();

        if (NvM_Prv_MainFunctionArbitrate(&NvM_Prv_Main_st))
        {
            NvM_Prv_MainFunctionJobHandling();
            NvM_Prv_MainFunctionResultEval(&NvM_Prv_Main_st);
        }
        // TRACE[NVM349] If no further job processing is possible, immediately return
    }
}

/*
 * \brief The function check the global flag for re-entrant invocation and reports the re-entrant error if detected
 *
 * \return
 * - TRUE = Lock available and accquired, proceed for execution
 * - FALSE = Lock not available, prevent re-entrant execution
 */
LOCAL_INLINE boolean NvM_Prv_TryToGetLock(void)
{
    boolean isLockAvailable_b = FALSE;

#if (NVM_RB_USE_PROTECTION != FALSE)
    // Disable interrupts
    SchM_Enter_NvM_Main();
#endif

    // Is the Main function not invoked yet, then reserve it and allow the execution
    if (!NvM_Prv_flgUsedSema_b)
    {
        NvM_Prv_flgUsedSema_b = TRUE;
        isLockAvailable_b = TRUE;
    }

#if (NVM_RB_USE_PROTECTION != FALSE)
    // Enable interrupts
    SchM_Exit_NvM_Main();
#endif

    if (!isLockAvailable_b)
    {
        // Report re-entrant error
        NvM_Prv_ErrorDetection_ReportReentrantError(NVM_SERVICE_ID_MAIN_FUNCTION);
    }

#if (NVM_RB_USE_PROTECTION == FALSE)
    // Protection disabled, only detection by default
    isLockAvailable_b = TRUE;
#endif

    return isLockAvailable_b;
}

/**
 * \brief Reset the flag having reentrant information
 *
 */
LOCAL_INLINE void NvM_Prv_ReleaseLock(void)
{
    // Reset the access flag
    NvM_Prv_flgUsedSema_b = FALSE;
    return;
}

/**
 * \brief
 * MainFunction of NvM, does the processing of NvM jobs.
 *
 */
void NvM_MainFunction(void)
{
    // Local flag to store the lock accuring status
    boolean flgUsed_b = FALSE;

    // Check for re-entrant invocation of MainFunction
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3264] - NvM shall provide a mechanism to prevent re-entrant execution.
    flgUsed_b = NvM_Prv_TryToGetLock();

    if (flgUsed_b)
    {
        // Internal processing of NvM Mainfunction
        NvM_Prv_MainFunctionIntern();

        // Release the lock
        NvM_Prv_ReleaseLock();
    }

    return;
}

/**
 * \brief
 * MainFunction and Dependency of NvM.
 * API helpful during startup and shutdown phase
 *
 */
void NvM_Rb_MainFunctionAndDependency(void)
{
    NvM_MainFunction();
    NvM_Prv_MemIf_MainFunctionAndDependency();
    return;
}

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

NvM_Prv_Activities_ten NvM_Prv_Main_GetCurrentState(void)
{
    return NvM_Prv_Main_st.Activity_rAMwM_en;
}

/**
 * \brief
 * Setter function for the re-entrant lock status flag
 *
 */
void NvM_Prv_SetSemaphoreFlag(boolean flg_b)
{
    NvM_Prv_flgUsedSema_b = flg_b;
    return;
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

