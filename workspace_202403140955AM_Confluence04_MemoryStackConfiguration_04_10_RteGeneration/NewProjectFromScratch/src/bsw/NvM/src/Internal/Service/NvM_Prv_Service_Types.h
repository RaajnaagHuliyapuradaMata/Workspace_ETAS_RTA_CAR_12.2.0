/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_SERVICE_TYPES_H
#define NVM_PRV_SERVICE_TYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Types.h"

/*
 **********************************************************************************************************************
 * Defines/Constants
 **********************************************************************************************************************
 */
/**
 * \brief
 * Definition of different bit-masks for all possible errors which can occur during service (user request) initiation.
 * \details
 * These bit-masks are used to access corresponding bits in error bit-field of type NvM_Prv_Service_InitiationErrors_tuo.
 * A set bit in the error bit-field signals that the corresponding error has occured during service initiation.
 *
 * Definition of bit masks is done as anonymous enumeration instead of several prepocessor defines
 * to get better structuring to show that all these bit masks belongs together.
 */
enum
{
    /// Bit mask definition to access the bit set in case of a block write protected error in bit-field of type
    /// NvM_Prv_Service_InitiationErrors_tuo.
    NvM_Prv_Service_InitiationError_MaskBlkWriteProtected_e         = 0x01u,
    /// Bit mask definition to access the bit set in case of a block write once status unknown error in bit-field
    /// of type NvM_Prv_Service_InitiationErrors_tuo.
    NvM_Prv_Service_InitiationError_MaskBlkWriteOnceStatusUnknown_e = 0x02u,
    /// Bit mask definition to access the bit setin case of a queue full error in bit-field of type
    /// NvM_Prv_Service_InitiationErrors_tuo.
    NvM_Prv_Service_InitiationError_MaskQueueFull_e                 = 0x04u,
    /// Bit mask definition to access the bit set in case of a block pending error in bit-field of type
    /// NvM_Prv_Service_InitiationErrors_tuo.
    NvM_Prv_Service_InitiationError_MaskBlkPEnding_e                = 0x08u
};

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * Type definition for container of errors which can occur during service (user request) initiation.
 * The container is a bit-field where each bit represents an error. Bit-masks for all possible errors are defined
 * via enumeration values NvM_Prv_Service_InitiationError_Mask<description>_e.
 */
typedef uint8 NvM_Prv_Service_InitiationErrors_tuo;

/**
 * \typedef
 * Type definition for functions to check if NvM is currently enqueuing a block for a multi-block service.
 *
 * This type makes it possible to check whether a user request is pending for a block in a common way
 * with minimum scheduler lock time, see also NvM_Prv_Block_IsNvmEnqueuingForMulti.
 */
typedef boolean (*NvM_Prv_Block_IsNvmEnqueuingForMulti_tpfct)(NvM_BlockIdType idBlock_uo,
                                                              NvM_Prv_Service_InitiationErrors_tuo* Errors_puo);

/**
 * Structure definition of block data required for initiation of a single-block service
 */
typedef struct
{
    /**
     * Data provided while enqueuing a service (user request).
     */
    NvM_Prv_QueueEntry_tst QueueEntry_st;

    /**
     * Result of the requested service.
     */
    NvM_RequestResultType Result_uo;
    /**
     * Detailed error information.
     */
    NvM_Rb_ErrorInfoDetail_ten errorInfoDetail_en;
    /**
     * Dataset index.
     */
    uint8 idxDataset_u8;

    /**
     * Bit mask which defines state bits to be changed during service initiation, see also NvM_Prv_stBlock_auo.
     * \attention
     * This bit mask shall be used only together with bit mask maskBitsNewValue_uo.
     */
    uint16 maskBitsToChange_uo;
    /**
     * Bit mask which defines new values for state bits to be set during service initiation, see also NvM_Prv_stBlock_auo.
     * \attention
     * This bit mask shall be used only together with bit mask maskBitsToChange_uo.
     */
    uint16 maskBitsNewValue_uo;

} NvM_Prv_BlockData_tst;

/**
 * \typedef
 * Type definition for functions to check parameters used in a single-block service.
 *
 * This type makes it possible to check parameters used in a single-block service in a common way.
 */
typedef boolean (*NvM_Prv_Service_CheckParameter_tpfct)(NvM_Prv_BlockData_tst const* BlockData_pcst);
/**
 * \typedef
 * Type definition for functions to update data used in a single-block service.
 *
 * This type makes it possible to update internal user request queue and block data used in a single-block service
 * in a common way.
 * For some services the NvM shall update user request queue or block data before initiating the actual service.
 * Typically in this function the RAM block to be used for the service can be set to permanent RAM block
 * if RAM block is required and user has passed NULL_PTR to the service.
 * Also changing of the service type can be done in this function (e.g. read service shall be changed to restore service
 * if block to be read is in ROM).
 */
typedef NvM_Prv_idQueue_tuo (*NvM_Prv_Service_UpdateData_tpfct)(NvM_Prv_BlockData_tst* BlockData_pst);
/**
 * \typedef
 * Type definition for functions to check block data used in a single-block service.
 *
 * This type makes it possible to check block data used in a single-block service in a common way.
 */
typedef boolean (*NvM_Prv_Service_CheckBlockData_tpfct)(NvM_Prv_BlockData_tst const* BlockData_pcst,
                                                        NvM_Prv_Service_InitiationErrors_tuo *Errors_puo);
/**
 * \typedef
 * Type definition for functions to set block data for a single-block service.
 *
 * This type makes it possible to set block data for a single-block service in a common way.
 */
typedef void (*NvM_Prv_Service_SetBlockData_tpfct)(NvM_Prv_BlockData_tst const* BlockData_pcst);

/**
 * Structure definition for configuration data required to initiate a single-block service.
 */
typedef struct
{
    /**
     * This configuration element enables/disables the check of pending asynchronous user services
     * while initiating a single-block service.
     * If this check is enabled then a single-block service will be initiated only if no other asynchronous user
     * services are pending for requested block.
     */
    boolean CheckPendingBlock_b;
    /**
     * This configuration element defines the block used for the check of pending asynchronous user services
     * while initiating a single-block service.
     * Normally the block used in user request is also used for this check.
     * In case of a request for auxiliary user the check shall be done for the special block NVM_PRV_AUX_ADMIN_BLOCK.
     * If no check of pending user services shall be done (CheckPendingBlock_b == (boolean)FALSE) NvM will not evaluate
     * this element, i.e. it can contain arbitrary block ID, but it is reasonable to use the same block ID
     * as in user request.
     */
    NvM_BlockIdType idBlockPendingCheck_uo;
    /**
     * This configuration element defines function which updates data used in a single-block service
     * before checking all service parameters.
     * \attention
     * The NvM calls this function with disabled scheduler lock.
     */
    NvM_Prv_Service_UpdateData_tpfct UpdateData_pfct;
    /**
     * This configuration element defines function which checks whether all parameters used in a single-block service
     * are valid.
     * \attention
     * The NvM calls this function with disabled scheduler lock.
     */
    NvM_Prv_Service_CheckParameter_tpfct CheckParameter_pfct;
    /**
     * This configuration element defines function which checks whether all block data used in a single-block service
     * is valid.
     * \attention
     * The NvM calls this function with enabled scheduler lock.
     */
    NvM_Prv_Service_CheckBlockData_tpfct CheckBlockData_pfct;
    /**
     * This configuration element defines function which sets all required block data for a single-block service.
     * \attention
     * The NvM calls this function with enabled scheduler lock.
     */
    NvM_Prv_Service_SetBlockData_tpfct SetBlockData_pfct;

} NvM_Prv_Service_Configuration_tst;

/**
 * \typedef
 * Type definition for functions to update data for all blocks participating in a multi-block service.
 *
 * This type makes it possible to set block data for a multi-block service in a common way.
 */
typedef void (*NvM_Prv_Service_UpdateBlockDataMulti_tpfct)(void);
/**
 * \typedef
 * Type definition for functions to finalize a multi-block service.
 *
 * This type makes it possible to finalize a multi-block service in a common way.
 */
typedef void (*NvM_Prv_Service_FinalizeMulti_tpfct)(void);

/**
 * Structure definition for configuration data required to initiate a multi-block service.
 */
typedef struct
{
    /**
     * This configuration element defines function which updates data as required for all blocks participating
     * in a multi-block service.
     *
     * Typically this function updates data for special block NVM_PRV_MULTI_BLOCK used for multi-block services and
     * calls the function NvM_Prv_Block_SetIsNvmEnqueuingForMulti with appropriate parameter,
     * see also description of NvM_Prv_Block_IsNvmEnqueuingForMulti_pfct.
     * In general this function contains all actions which shall be done with enabled scheduler lock.
     *
     * \attention
     * The NvM calls this function with enabled scheduler lock.
     */
    NvM_Prv_Service_UpdateBlockDataMulti_tpfct UpdateBlockDataForMulti_pfct;
    /**
     * This configuration element defines function which executes all actions required to finalize initiation
     * of a multi-block service.
     *
     * Typically this function finds all blocks which participate in required multi-block service and
     * updates block data correspondingly. Also this function calls the function NvM_Prv_Block_SetIsNvmEnqueuingForMulti,
     * see also description of NvM_Prv_Block_IsNvmEnqueuingForMulti_pfct.
     * In general this function contains all actions which can be done with disabled scheduler lock.
     *
     * \attention
     * The NvM calls this function with disabled scheduler lock.
     */
    NvM_Prv_Service_FinalizeMulti_tpfct FinalizeMulti_pfct;

} NvM_Prv_Service_ConfigurationMulti_tst;

/* NVM_PRV_SERVICE_TYPES_H */
#endif
