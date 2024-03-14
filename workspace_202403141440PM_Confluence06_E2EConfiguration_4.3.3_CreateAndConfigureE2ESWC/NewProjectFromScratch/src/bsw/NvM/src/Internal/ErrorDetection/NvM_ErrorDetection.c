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
#include "NvM.h"

#include "NvM_Prv.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"

#if (NVM_DEV_ERROR_DETECT == STD_ON)
// TRACE[NVM556] NvM module shall include Det.h
# include "Det.h"
// TRACE[NVM089] Check version compatibility of included header files
# if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != NVM_AR_RELEASE_MAJOR_VERSION))
#  error "AUTOSAR major version undefined or mismatched"
# endif
#endif

#if (NVM_CRYPTO_USED == STD_ON)
# include "NvM_Prv_Crypto_Types.h"
#endif

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define NVM_START_SEC_VAR_CLEARED_8
#include "NvM_MemMap.h"

// Variable saving blocks' errors (useful for debugging) and provided for external via API
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2811] public block specific production error array
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2812] public summary variable of all occured production errors (index #0)
uint8 NvM_Rb_stBlockErrors_au8[NVM_CFG_NR_ALL_BLOCKS];

// Flag to hold the debug information, Re-entrant MainFunction
static volatile boolean NvM_Prv_dbgReentrantFunction_b;

#define NVM_STOP_SEC_VAR_CLEARED_8
#include "NvM_MemMap.h"

#define NVM_START_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "NvM_MemMap.h"

// ID of the most previous error reported via Det_ReportError within NvM (initialized to NVM_PRV_NO_DET_ERROR)
NvM_Prv_idDetError_tuo NvM_Prv_idLastDetError_uo;
// ID of the service which triggered the last DET error (initialized to NVM_SERVICE_ID_INIT)
NvM_Prv_idService_tuo NvM_Prv_idServiceLastDetError_uo;
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3113] Private variable to store block ID used in NvM service
//                                               which has caused the last DET error
NvM_BlockIdType NvM_Prv_idBlockLastDetError_uo;

#define NVM_STOP_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * NvM-private code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"
/**
 * This function returns the block ID used in NvM service which has caused the last DET error.
 *
 * Typical use case for this function is to be used as callback within the DET.
 * If NvM service which has caused the last DET error does not use any block or no DET error has occured yet,
 * then this function returns 0.
 *
 * \param[global in] NvM_Prv_idBlockCausingLastDetError_uo
 * Global variable to store block ID used in NvM service which has caused the last DET error
 *
 * \return
 * Block identifier used in NvM service which has caused the last DET error.
 */
NvM_BlockIdType NvM_Rb_GetBlockIdCausingLastDetError(void)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3113] Service to get the block ID used in NvM service
    //                                               which has caused the last DET error
    return (NvM_Prv_idBlockLastDetError_uo);
}

/**
 * This function returns the block errors.
 *
 * Getter function to provide it for the API.
 *
 * \param[in] idBlock_uo
 * ID of the block for which production error is reported. *
 *
 * \return
 * Variable saving blocks' errors.
 */
NvM_Rb_BlockErrors_tuo NvM_Prv_ErrorDetection_GetBlockErrors(NvM_BlockIdType idBlock_uo)
{
    return(NvM_Rb_stBlockErrors_au8[idBlock_uo]);
}

/**
 * \brief
 * This private function reports the passed error to the DET together with the NvM service which has detected
 * this error.
 * \details
 * Additionally this function stores the reported error and the NvM service to the NvM private variables.
 *
 * \param[in] idService_uo
 * Id of the service which reports an error to the DET.
 * \param[in] idError_uo
 * Id of the error which shall be reported to the DET.
  * \param[in] idBlock_uo
 * ID of the block for which production error is reported.
 */
// TRACE[NVM191]
// Development errors shall be reported to the Det_ReportError service
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3113]
// NvM private function to set block ID used in NvM service which has caused the last DET error
static void NvM_Prv_ReportDetError(NvM_Prv_idService_tuo idService_uo,
                                   NvM_Prv_idDetError_tuo idError_uo,
                                   NvM_BlockIdType idBlock_uo)
{
    NvM_Prv_idServiceLastDetError_uo = idService_uo;
    NvM_Prv_idLastDetError_uo = idError_uo;
    NvM_Prv_idBlockLastDetError_uo = idBlock_uo;

    // TRACE[NVM188] If the NvMDevErrorDetect switch is enabled API parameter checking is enabled
#if (NVM_DEV_ERROR_DETECT == STD_ON)
    (void)Det_ReportError(NVM_MODULE_ID, NVM_INSTANCE_ID, idService_uo, idError_uo);
#endif
}

/**
 * \brief
 * This NvM private function initializes all data which will be reported to the DET.
 * \details
 * The reported data will be initialized only if data in the saved zone is lost.
 *
 * \param[in] isSavedZoneDataLost_b
 * Status information of the data in the saved zone RAM:
 * - true = data in the saved zone RAM is lost after the reset
 * - false = data in the saved zone RAM is unchanged after the reset
 */
void NvM_Prv_ErrorDetection_InitializeDetError(boolean isSavedZoneDataLost_b)
{
    if (isSavedZoneDataLost_b)
    {
        NvM_Prv_idServiceLastDetError_uo = NVM_SERVICE_ID_INIT;
        NvM_Prv_idLastDetError_uo = NVM_PRV_NO_DET_ERROR;
        NvM_Prv_idBlockLastDetError_uo = 0u;
    }
}

/**
 * \brief
 * This NvM private function initializes all block specific production errors.
 */
void NvM_Prv_ErrorDetection_InitializeProductionErrors(void)
{
    NvM_BlockIdType idBlock_uo;
    // Variable saving blocks' errors (useful for debugging)
    for (idBlock_uo = 0u; idBlock_uo < NVM_CFG_NR_ALL_BLOCKS; ++idBlock_uo)
    {
        NvM_Rb_stBlockErrors_au8[idBlock_uo] = 0u; // No errors
    }
}

/**
 * \brief
 * This NVM private function sets a production error.
 * \details
 * In cases this function is called with enabled scheduler lock the run-time of 3 can be used:
 * - 1 for function call
 * - 2 for internal copy operations
 *
 * \param[in] idBlock_uo
 * ID of the block for which production error is reported.
 * \param[in] MaskErrorBit_u8
 * Bit mask of the production error to be set.
 * \param[out] NvM_Rb_stBlockErrors_au8
 * Global array with all block specific production error.
 */
void NvM_Prv_ErrorDetection_SetProductionError(NvM_BlockIdType idBlock_uo, uint8 MaskErrorBit_u8)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2811] Report block specific production error
    NvM_Rb_stBlockErrors_au8[idBlock_uo] |= MaskErrorBit_u8;
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2812] Set reported production error in the overall summary
    NvM_Rb_stBlockErrors_au8[0] |= MaskErrorBit_u8;
}

/**
 * \brief
 * This NvM private function checks the passed service bit and reports the DET error NVM_E_SINGLE_BLOCK_SERVICE_ID
 * if this bit is invalid.
 *
 * \param[in] idService_uo
 * ID of the service which checks the service bit. This service ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * Block ID used in NvM service causing last DET error.
 * \param[in] ServiceBit_uo
 * Service bit to be checked for validness.
 *
 * \return
 * Validness of the passed block ID:
 * - TRUE = service bit is valid
 * - FALSE = service bit is invalid
 */
boolean NvM_Prv_ErrorDetection_IsServiceBitValid(NvM_Prv_idService_tuo idService_uo,
                                                 NvM_BlockIdType idBlock_uo,
                                                 NvM_Prv_ServiceBit_tuo ServiceBit_uo)
{
    boolean isServiceBitValid_b = (ServiceBit_uo < NvM_Prv_ServiceBit_Unspecified_e);

    if (!isServiceBitValid_b)
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_SINGLE_BLOCK_SERVICE_ID, idBlock_uo);
    }
    return isServiceBitValid_b;
}

boolean NvM_Prv_ErrorDetection_IsJobIdValid(NvM_Prv_idService_tuo idService_uo,
                                            NvM_BlockIdType idBlock_uo,
                                            NvM_Prv_idJob_ten idJob_en)
{
    boolean isJobIdValid_b = (idJob_en < NvM_Prv_idJob_Invalid_e);

    if (!isJobIdValid_b)
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_PRV_JOB_ID, idBlock_uo);
    }
    return isJobIdValid_b;
}

boolean NvM_Prv_ErrorDetection_IsJobResultMemIfPlausible(NvM_Prv_idService_tuo idService_uo,
                                                         NvM_BlockIdType idBlock_uo,
                                                         NvM_Prv_JobResult_ten JobResult_en)
{
    boolean isJobResultMemIfPlausible_b = ((JobResult_en == NvM_Prv_JobResult_Succeeded_e) ||
                                           (JobResult_en == NvM_Prv_JobResult_Succeeded_MemIfSkipped_e) ||
                                           (JobResult_en == NvM_Prv_JobResult_Failed_e) ||
                                           (JobResult_en == NvM_Prv_JobResult_Skipped_e) ||
                                           (JobResult_en == NvM_Prv_JobResult_BlockDefault_e) ||
                                           (JobResult_en == NvM_Prv_JobResult_BlockInvalidated_e) ||
                                           (JobResult_en == NvM_Prv_JobResult_BlockInconsistent_e));
    if (!isJobResultMemIfPlausible_b)
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_MEMIF_JOB_RESULT, idBlock_uo);
    }
    return isJobResultMemIfPlausible_b;
}

/**
 * \brief
 * This NvM private function checks whether the RAM mirror used for explicit synchronization is big enough
 * for the passed block and reports the DET error NVM_E_RAM_MIRROR_SIZE if RAM mirror is too small.
 *
 * \param[in] idService_uo
 * ID of the service which checks whether the block size is valid. This ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * ID of the block for which the size will be validated.
 * \param[in] SizeRamMirror_u32
 * Size of the RAM mirror used for explicit synchronization
 *
 * \return
 * - TRUE = RAM mirror is big enough
 * - FALSE = RAM mirror is too small
 */
boolean NvM_Prv_ErrorDetection_IsBlockSizeValidForExplicitSync(NvM_Prv_idService_tuo idService_uo,
                                                               NvM_BlockIdType idBlock_uo,
                                                               uint32 SizeRamMirror_u32)
{
    boolean isBlockSizeValid_b = (NvM_Prv_BlkDesc_GetSize(idBlock_uo) <= SizeRamMirror_u32);

    if (!isBlockSizeValid_b)
    {
        // block size greater than size of the RAM mirror used for explicit synchronization
        NvM_Prv_ReportDetError(idService_uo, NVM_E_RAM_MIRROR_SIZE, idBlock_uo);
    }

    return isBlockSizeValid_b;
}

/**
 * \brief
 * This NvM private function checks whether the passed pointer is valid and reports the required DET error
 * if NULL pointer is passed.
 *
 * \param[in] idService_uo
 * ID of the service which checks whether the data index pointer is valid.
 * This ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * ID of the block for which the data index pointer will be validated.
 * \param[in] idDetError_uo
 * DET error to be reported if NULL pointer is passed
 * \param[in] ptr_pcun
 * Pointer to be validated.
 *
 * \return
 * - TRUE = pointer is valid
 * - FALSE = pointer is invalid
 */
boolean NvM_Prv_ErrorDetection_IsPtrValid(NvM_Prv_idService_tuo idService_uo,
                                          NvM_BlockIdType idBlock_uo,
                                          NvM_Prv_idDetError_tuo idDetError_uo,
                                          NvM_Prv_ErrorDetection_Ptr_tun const* ptr_pcun)
{
    boolean isPtrValid_b = (NULL_PTR != ptr_pcun->ptrToCheck_pv);

    if (!isPtrValid_b)
    {
        NvM_Prv_ReportDetError(idService_uo, idDetError_uo, idBlock_uo);
    }

    return isPtrValid_b;
}

/**
 * \brief
 * This private function checks the initialization of the NvM and reports the DET error NVM_E_NOT_INITIALIZED
 * when the NvM is not yet initialized.
 *
 * \param[in] idService_uo
 * ID of the service which checks the NvM initialization. This ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * Block ID used in NvM service causing last DET error.
 *
 * \return
 * Initialization status of the NvM:
 * - TRUE = NvM initialized
 * - FALSE = NvM not initialized
 */
boolean NvM_Prv_ErrorDetection_IsNvmInitialized(NvM_Prv_idService_tuo idService_uo,
                                                NvM_BlockIdType idBlock_uo)
{
    boolean isNvmInitialized_b = NvM_Prv_IsNvmInitialized();

    if (!isNvmInitialized_b)
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_NOT_INITIALIZED, idBlock_uo);
    }

    return (isNvmInitialized_b);
}

/**
 * \brief
 * This NvM private function checks the passed block ID and reports the DET error NVM_E_PARAM_BLOCK_ID
 * if this ID is invalid.
 *
 * \param[in] idService_uo
 * ID of the service which checks the block ID. This service ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * Block ID to be checked for validness.
 * \param[in] isMultiBlockAllowed_b
 * Flag to define whether the multi-block ID is valid ID or not
 *
 * \return
 * Validness of the passed block ID:
 * - TRUE = block ID is valid
 * - FALSE = block ID is invalid
 */
boolean NvM_Prv_ErrorDetection_IsBlockIdValid(NvM_Prv_idService_tuo idService_uo,
                                              NvM_BlockIdType idBlock_uo,
                                              boolean isMultiBlockAllowed_b)
{
    boolean isBlockIdValid_b;
    NvM_BlockIdType idBlockLowest_uo;

    if (isMultiBlockAllowed_b)
    {
        idBlockLowest_uo = NVM_PRV_MULTI_BLOCK;
    }
    else
    {
        // TRACE[NVM311] Allow applications to send requests for block ID 1
        idBlockLowest_uo = NVM_CFG_FIRST_USED_BLOCK;
    }

    isBlockIdValid_b = (idBlock_uo >= idBlockLowest_uo) && (idBlock_uo < NVM_CFG_NR_BLOCKS);

    if (!isBlockIdValid_b)
    {
        // TRACE[NVM188] If the NvMDevErrorDetect switch is enabled API parameter checking is enabled
        // TRACE[NVM023] Report DET errors when NvM is called with wrong parameters
        NvM_Prv_ReportDetError(idService_uo, NVM_E_PARAM_BLOCK_ID, idBlock_uo);
    }

    return isBlockIdValid_b;
}

boolean NvM_Prv_ErrorDetection_IsDefaultDataAvailable(NvM_Prv_idService_tuo idService_uo,
                                                      NvM_BlockIdType idBlock_uo)
{
    boolean isDefaultDataAvailable_b = FALSE;
    if (NvM_Prv_BlkDesc_IsDefaultDataAvailable(idBlock_uo))
    {
        isDefaultDataAvailable_b = TRUE;
    }
    else
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_BLOCK_WITHOUT_DEFAULTS, idBlock_uo);
    }
    return isDefaultDataAvailable_b;
}

boolean NvM_Prv_ErrorDetection_IsRamBlockAddressValid(NvM_Prv_idService_tuo idService_uo,
                                                      NvM_BlockIdType idBlock_uo,
                                                      void const* RamBlockAddress_pv)
{
    boolean isRamBlockAddressValid_b = FALSE;
    // check whether RAM block address is valid
    if (NULL_PTR == RamBlockAddress_pv)
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_PARAM_ADDRESS, idBlock_uo);
    }
    else
    {
        isRamBlockAddressValid_b = TRUE;
    }
    return isRamBlockAddressValid_b;
}

boolean NvM_Prv_ErrorDetection_IsBlockTypeDataset(NvM_Prv_idService_tuo idService_uo,
                                                  NvM_BlockIdType idBlock_uo)
{
    boolean isBlockTypeDataset_b = FALSE;
    if (NVM_BLOCK_DATASET == NvM_Prv_BlkDesc_GetType(idBlock_uo))
    {
        isBlockTypeDataset_b = TRUE;
    }
    else
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_PARAM_BLOCK_TYPE, idBlock_uo);
    }
    return isBlockTypeDataset_b;
}

boolean NvM_Prv_ErrorDetection_IsBlockIdxValid(NvM_Prv_idService_tuo idService_uo,
                                               NvM_BlockIdType idBlock_uo,
                                               uint8 idxData_u8)
{
    boolean isBlockIdxValid_b = FALSE;
    if (idxData_u8 < NvM_Prv_BlkDesc_GetNrDataIndexes(idBlock_uo))
    {
        isBlockIdxValid_b = TRUE;
    }
    else
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_PARAM_BLOCK_DATA_IDX, idBlock_uo);
    }
    return isBlockIdxValid_b;
}

boolean NvM_Prv_ErrorDetection_IsBlockPriorityImmediate(NvM_Prv_idService_tuo idService_uo,
                                                        NvM_BlockIdType idBlock_uo)
{
    boolean isBlockPriorityImmediate_b = FALSE;
    if (NvM_Prv_BlkDesc_HasBlockImmediateJobPriority(idBlock_uo))
    {
        isBlockPriorityImmediate_b = TRUE;
    }
    else
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_BLOCK_CONFIG, idBlock_uo);
    }
    return isBlockPriorityImmediate_b;
}

boolean NvM_Prv_ErrorDetection_IsBlockWriteProtectionChangeable(NvM_Prv_idService_tuo idService_uo,
                                                                NvM_BlockIdType idBlock_uo)
{
    boolean isBlockWriteProtectionChangeable_b = FALSE;
    if (!NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo, NVM_PRV_BLOCK_FLAG_WRITE_ONCE))
    {
        isBlockWriteProtectionChangeable_b = TRUE;
    }
    else
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_BLOCK_CONFIG, idBlock_uo);
    }
    return isBlockWriteProtectionChangeable_b;
}


/**
 * \brief
 * This NvM private function reports the Re-entrant error of MainFunction.
 * \details
 * Additionally this function stores the re-entrant error occurence in a debug variable.
 * \param[in] idService_uo
 * Id of the service which reports an error to the DET.
 */
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3266] - NvM reports Det error on RE-ENTRANT detection if Det Enabled
void NvM_Prv_ErrorDetection_ReportReentrantError(NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_dbgReentrantFunction_b = TRUE;
    NvM_Prv_ReportDetError(idService_uo, NVM_E_REENTRANT, 0u);
}

/**
 * \brief
 * This NvM private function reports the errors occured during service initiation.
 *
 * \details
 * For AR4.5 or newer if the bit NvM_Prv_Service_InitiationError_MaskBlkWriteOnceStatusUnknown_e is set,
 * this function reports the development error NVM_E_WRITE_ONCE_STATUS_UNKNOWN to the DET.
 *
 * If the bit NvM_Prv_Service_InitiationError_MaskBlkWriteProtected_e is set, this function reports internal error
 * NVM_E_RB_WRITE_PROTECTED. And for AR4.5 or newer this function additionally reports the development error
 * NVM_E_WRITE_PROTECTED to the DET.
 *
 * If the bit NvM_Prv_Service_InitiationError_MaskQueueFull_e is set, this function reports the internal error
 * NVM_E_RB_QUEUE_OVERFLOW. And for AR4.3 or newer this function additionally reports the development error
 * NVM_E_QUEUE_FULL to the DET.
 *
 * If the bit NvM_Prv_Service_InitiationError_MaskBlkPEnding_e is set, this function reports the development error
 * NVM_E_BLOCK_PENDING to the DET.
 *
 * \param[in] idService_uo
 * ID of the initiated service, this ID will be reported together with DET error
 * \param[in] idBlock_uo
 * ID of the block used in the initiated service, this ID will be reported together with DET error
 * \param[in] Errors_uo
 * Bit-Field with all errors to be reported for the initiated service
 */
void NvM_Prv_ErrorDetection_ReportServiceInitiationErrors(NvM_Prv_idService_tuo idService_uo,
                                                          NvM_BlockIdType idBlock_uo,
                                                          NvM_Prv_Service_InitiationErrors_tuo Errors_uo)
{
    if ((Errors_uo & (NvM_Prv_Service_InitiationErrors_tuo)NvM_Prv_Service_InitiationError_MaskBlkWriteOnceStatusUnknown_e) != 0u)
    {
        // TRACE[SWS_NvM_00954] Report a development error if a write is attempted for a write once block
        //                      and no previous read was made
        // TRACE[SWS_NvM_00955] Report a development error if a writePRAM is attempted for a write once block
        //                      and no previous read was made
        // TRACE[SWS_NvM_00956] Report a development error if a writeAll is attempted for a write once block
        //                      and no previous read was made
        // TRACE[SWS_NvM_00957] Report a development error if an erase is attempted for a write once block
        //                      and no previous read was made
        // TRACE[SWS_NvM_00958] Report a development error if an invalidate is attempted for a write once block
        //                      and no previous read was made
        NvM_Prv_ReportDetError(idService_uo, NVM_E_WRITE_ONCE_STATUS_UNKNOWN, idBlock_uo);
    }
    else
    {
        if ((Errors_uo & (NvM_Prv_Service_InitiationErrors_tuo)NvM_Prv_Service_InitiationError_MaskBlkWriteProtected_e) != 0u)
        {
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2818] Report an error by setting a block error flag if an
            //                                          erase or a write is attempted for a write protected block
            NvM_Prv_ErrorDetection_SetProductionError(idBlock_uo, NVM_E_RB_WRITE_PROTECTED);
#if (NVM_AR_RELEASE_MAJOR_VERSION == 4) && (NVM_AR_RELEASE_MINOR_VERSION >=5)
            // TRACE[SWS_NvM_00961] Report a development error if an erase or a write is attempted for a write protected block
            NvM_Prv_ReportDetError(idService_uo, NVM_E_WRITE_PROTECTED, idBlock_uo);
#endif
        }
    }

    if ((Errors_uo & (NvM_Prv_Service_InitiationErrors_tuo)NvM_Prv_Service_InitiationError_MaskQueueFull_e) != 0u)
    {
        //TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2815] Report internal error in case of queue overflow.
        NvM_Prv_ErrorDetection_SetProductionError(idBlock_uo, NVM_E_RB_QUEUE_OVERFLOW);
#if (NVM_AR_RELEASE_MAJOR_VERSION == 4) && (NVM_AR_RELEASE_MINOR_VERSION >=3)
        //TRACE[SWS_NvM_00948] Report a run-time error to Det if the queue is full.
        NvM_Prv_ReportDetError(idService_uo, NVM_E_QUEUE_FULL, idBlock_uo);
#endif
    }

    if ((Errors_uo & (NvM_Prv_Service_InitiationErrors_tuo)NvM_Prv_Service_InitiationError_MaskBlkPEnding_e) != 0u)
    {
        // TRACE[NVM632] Report the DET error NVM_E_BLOCK_PENDING when the NVRAM block identifier is already queued or
        //               currently in progress
        NvM_Prv_ReportDetError(idService_uo, NVM_E_BLOCK_PENDING, idBlock_uo);
    }
}

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

/**
 * This private function checks the random number generated for user data encryption and reports the DET error
 * NVM_E_CRYPTO_WRITE_NO_INIT_VECTOR if no random number has been generated before.
 *
 * \param[in] idService_uo
 * ID of the service which checks the random number. This service ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * ID of the block for which the random number is required.
 */
void NvM_Prv_ErrorDetection_ReportInitVectorNotAvailable(NvM_Prv_idService_tuo idService_uo,
                                                         NvM_BlockIdType idBlock_uo)
{
    if (!NvM_Prv_IsInitVectorAvailable())
    {
        NvM_Prv_ReportDetError(idService_uo, NVM_E_CRYPTO_WRITE_NO_INIT_VECTOR, idBlock_uo);
    }
}

#endif

#if (NVM_CRYPTO_USED == STD_ON)

/**
 * \brief
 * This private function checks the number of retries for a Csm job and reports the DET error NVM_E_CRYPTO_CSM_BUSY
 * if configured limit is exceeded.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 *
 * \param[in] idService_uo
 * ID of the service which checks the number of retries for a Csm job.
 * This ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * ID of the block for which the number of retries will be checked.
 * \param cntrCsmJobRetry_puo
 * Pointer to the Csm job retry counter.
 *
 * \return
 * Plausibility of the passed cryptographic service results:
 * - TRUE = configured limit for Csm job retries is exceeded
 * - FALSE = configured limit for Csm job retries not exceeded
 */
boolean NvM_Prv_ErrorDetection_IsCsmJobRetryLimitExceeded(NvM_Prv_idService_tuo idService_uo,
                                                          NvM_BlockIdType idBlock_uo,
                                                          uint8 const* cntrCsmJobRetry_pcuo)
{
    boolean isCsmJobRetryCounterExceeded_b;
    // check whether the configured limit value for retry counter is exceeded
    if (*cntrCsmJobRetry_pcuo > NVM_CSM_RETRY_COUNTER)
    {
        isCsmJobRetryCounterExceeded_b = TRUE;
        NvM_Prv_ReportDetError(idService_uo, NVM_E_CRYPTO_CSM_BUSY, idBlock_uo);
    }
    else
    {
        isCsmJobRetryCounterExceeded_b = FALSE;
    }
    return isCsmJobRetryCounterExceeded_b;
}

/**
 * \brief
 * This private function evaluates results of a cryptographic service and reports a DET error if required.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * The caller of this function has to ensure that a cryptographic service has completed and its results are available.
 *
 * This function executes following evaluations:
 * - if Csm job callback has been invoked by a Csm job different to the previously started
 *   then this function reports the DET error NVM_E_CRYPTO_JOB_ID
 * - if configured primary output length differs from the real primary output length then this function reports
 *   given DET error
 * - if configured secondary output length differs from the real secondadry output length then this function reports
 *   given DET error
 *
 * \param[in] idService_uo
 * ID of the service which checks whether results of a cryptographic service are plausibel.
 * This ID will be reported together with DET error.
 * \param[in] idBlock_uo
 * Block ID used in NvM service causing last DET error.
 * \param[in] idErrorPrimaryOutput_uo
 * DET error to be reported in case of primary output length mismatch
 * \param[in] idErrorSecondaryOutput_uo
 * DET error to be reported in case of secondary output length mismatch
 * \param JobResult_pcst
 * Pointer to the results of a cryptographic service.
 *
 * \return
 * Plausibility of the passed cryptographic service results:
 * - TRUE = results are plausible
 * - FALSE = results are not plausible
 */
boolean NvM_Prv_ErrorDetection_IsCsmJobResultPlausible(NvM_Prv_idService_tuo idService_uo,
                                                       NvM_BlockIdType idBlock_uo,
                                                       NvM_Prv_idDetError_tuo idErrorPrimaryOutput_uo,
                                                       NvM_Prv_idDetError_tuo idErrorSecondaryOutput_uo,
                                                       NvM_Prv_Crypto_ServiceResult_tst const* JobResult_pcst)
{
    boolean isCsmJobResultPlausible_b = FALSE;

    // check whether results are plausible
    if (JobResult_pcst->idJobStarted_uo != JobResult_pcst->idJobCallback_uo)
    {
        // ID of the started job is different to the ID of the finished job
        NvM_Prv_ReportDetError(idService_uo, NVM_E_CRYPTO_JOB_ID, idBlock_uo);
    }
    else if (JobResult_pcst->outputPrimaryLength_u32 != JobResult_pcst->outputPrimaryLengthConfigured_u32)
    {
        // Length of the primary output doesnot match the expected value
        NvM_Prv_ReportDetError(idService_uo, idErrorPrimaryOutput_uo, idBlock_uo);
    }
    else if (JobResult_pcst->outputSecondaryLength_u32 != JobResult_pcst->outputSecondaryLengthConfigured_u32)
    {
        // Length of the secondary output doesnot match the expected value
        NvM_Prv_ReportDetError(idService_uo, idErrorSecondaryOutput_uo, idBlock_uo);
    }
    else
    {
        // job result is plausible
        isCsmJobResultPlausible_b = TRUE;
    }

    return isCsmJobResultPlausible_b;
}

#endif  // (NVM_CRYPTO_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

