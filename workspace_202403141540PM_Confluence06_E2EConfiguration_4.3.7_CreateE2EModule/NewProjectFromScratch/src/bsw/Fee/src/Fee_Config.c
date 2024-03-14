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

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))
#include "Fee_Prv_Config.h"
#include "Fee_PrvTypes.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Static declarations
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/*
 * In the end this shall be a constant "variable" or a define.
 * To better find out the maximal limit that is possible it is currently located in RAM.
 * Here we can change it with the debugger which is important for testing.
 * Since this variable is only for internal measurement support, it is not needed to be part of the Fee_Prv_ConfigData
 * structure that is generated for each instance sepeartely.
 */
#define FEE_START_SEC_CONST_32
#include "Fee_MemMap.h"
uint32 const Fee_Prv_ConfigMaxEffort_cu32 = 1024u;
#define FEE_STOP_SEC_CONST_32
#include "Fee_MemMap.h"

/**
 * \brief   Find the block number (i.e. index) in the block properties table by persistent ID.
 *
 * \param   persistentId_u16            The persistent ID that shall be checked
 * \param   nrOfBlks_u16                Total number of blocks configured in the instance
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \return  The block number (i.e. index) of the corresponding block in the block properties table - 0xFFFF if unknown.
*/
#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"
uint16 Fee_Prv_ConfigGetBlockNrByPersistentId(uint16 persistentId_u16, uint16 nrOfBlks_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    uint16_least   a_qu16;
    uint16_least   b_qu16;
    uint16_least   check_qu16;
    uint16         result_u16 = FEE_PRV_MAX_UINT16;

    /*
     * The Block Properties table struct is stored ascending by persistent ID.
     * This function uses that fact and does a binary search.
     */
    /* Evaluation of while loop: The while loop will be exited after a maximum of nrOfBlks_u16/2 calls, which is a constant value.
     * Hence, the while loop will not be stuck infinetly. */

    /* In case of evenly distributed persistent Id's this is the most likely block number to find the persistent Id
     * immediately without any search. Therefore this value is taken as the initial value for the check */
    check_qu16 = (uint16_least)(((uint32_least)persistentId_u16 * (uint32_least)nrOfBlks_u16) / 0x10000u);
    if(blockPropertiesTable_pcst[check_qu16].BlockPersistentId_u16 == persistentId_u16)
    {
        result_u16 = (uint16)check_qu16;
    }
    else
    {
        a_qu16 = 0u;
        b_qu16 = nrOfBlks_u16;
        while(a_qu16 != b_qu16)
        {
            check_qu16 = a_qu16 + ((b_qu16 - a_qu16) / 2u);
            if(blockPropertiesTable_pcst[check_qu16].BlockPersistentId_u16 < persistentId_u16)
            {
                if(check_qu16 == a_qu16)
                {
                    /*
                     * This can only happen if a_u16 + 1 = b_u16.
                     * Either b_u16 was once checked and was not index of the searched persistent ID
                     * or b_u16 is still = nrOfBlks_u16 (which is 1 bigger than the last valid index of block properties table)
                     * and the searched persistent ID is bigger than the biggest configured persistent ID.
                     * So if this branch is reached the persistent ID is not known.
                     */
                    b_qu16 = a_qu16;
                }
                else
                {
                    a_qu16 = check_qu16;
                }
            }
            else if(blockPropertiesTable_pcst[check_qu16].BlockPersistentId_u16 > persistentId_u16)
            {
                b_qu16 = check_qu16;
            }
            else
            {
                b_qu16       = a_qu16;
                result_u16  = (uint16)check_qu16;
            }
        }
    }

    return(result_u16);
}

/**
 * \brief   Get the length of a block specified by its block number.
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h - This is not the persistent ID!
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \return  The length of the specified block
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
uint16 Fee_Prv_ConfigGetBlockLengthByBlockNr(uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    return(blockPropertiesTable_pcst[blockNr_u16].Length_u16);
}

/**
 * \brief   Get the length of a block specified by its persistent ID.
 *
 * \param   persistentId_u16            Persistent ID of this block
 * \param   nrOfBlks_u16                Total number of blocks configured in the instance
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \return  The length of the specified block - 0 if the passed persistent ID is unknown.
*/
uint16 Fee_Prv_ConfigGetBlockLengthByPersistentId(uint16 persistentId_u16, uint16 nrOfBlks_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    uint16 result_u16   = 0u;
    uint16 index_u16    = Fee_Prv_ConfigGetBlockNrByPersistentId(persistentId_u16, nrOfBlks_u16, blockPropertiesTable_pcst);

    if(FEE_PRV_MAX_UINT16 != index_u16)
    {
        result_u16 = blockPropertiesTable_pcst[index_u16].Length_u16;
    }

    return(result_u16);
}

/**
 * \brief   Get the persistent ID of a specified block.
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \return  Persistent ID of the specified block
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
uint16 Fee_Prv_ConfigGetBlockPersistentIdByBlockNr(uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    return(blockPropertiesTable_pcst[blockNr_u16].BlockPersistentId_u16);
}

/**
 * \brief   returns the uint16 statusbits of the selected block
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \return  status bytes of the selected block
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
uint16 Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    return blockPropertiesTable_pcst[blockNr_u16].Flags_u16;
}

/**
 * \brief   Get the user of that block. Can be either NvM or adapter.
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h - This is not the persistent ID!
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  FEE_RB_USER_NVM_E     Block is handled by the NvM
 * \retval  FEE_RB_USER_ADAPTER_E Block is handled by the adapter
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
Fee_Rb_User_ten Fee_Prv_ConfigGetBlockUserByBlockNr(uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    Fee_Rb_User_ten result_en;
    uint32 userId_u32 = (blockPropertiesTable_pcst[blockNr_u16].Flags_u16 & FEE_PRV_USER_BITS) >> FEE_PRV_USER_POS;
    if(1u == userId_u32)
    {
        result_en = FEE_RB_USER_NVM_E;
    }
    else if(2u == userId_u32)
    {
        result_en = FEE_RB_USER_ADAPTER_E;
    }
    else
    {
        result_en = FEE_RB_USER_DEBUG_E;
    }
    return (result_en);
}

/**
 * \brief   This function returns the start address of the physical sector.
 *
 * \param   phySectNo_u8         Physical sector number for which the start address is required.
 * \param   flashPropTable_pcst  Pointer to flash properties table
 *
 * \return  Start address of the given physical sector.
 *
 * \attention  This is a simple get-function without check if phySectNo_u8 is valid!
*/
uint32 Fee_Prv_ConfigGetPhysStartAddress (uint8 physSectNr_u8, const Fee_Rb_FlashProp_tst* flashPropTable_pcst)
{
    return(flashPropTable_pcst[physSectNr_u8].Fee_PhysStartAddress_u32);
}

/**
 * \brief   This function returns the end address of the physical sector.
 *
 * \param   phySectNo_u8         Physical sector number for which the end address is required.
 * \param   flashPropTable_pcst  Pointer to flash properties table
 *
 * \return  End address of the given physical sector.
 *
 * \attention  This is a simple get-function without check if phySectNo_u8 is valid!
*/
uint32 Fee_Prv_ConfigGetPhysEndAddress (uint8 physSectNr_u8, const Fee_Rb_FlashProp_tst* flashPropTable_pcst)
{
    return(flashPropTable_pcst[physSectNr_u8].Fee_PhysEndAddress_u32);
}


/**
 * \brief   Check whether a block with the specified persistent ID is known by configuration.
 *
 * \param   persistentId_u16            Peristent ID which shall be checked
 * \param   nrOfBlks_u16                Number of blocks in the given instance
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    There exists a block in the configuration with the specified persistent ID
 * \retval  FALSE   There does not exist a block in the configuration with the specified persistent ID
*/
boolean Fee_Prv_ConfigIsBlockKnownByPersistentId(uint16 persistentId_u16, uint16 nrOfBlks_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    boolean result_b = FALSE;

    if(FEE_PRV_MAX_UINT16 != Fee_Prv_ConfigGetBlockNrByPersistentId(persistentId_u16, nrOfBlks_u16, blockPropertiesTable_pcst))
    {
        result_b = TRUE;
    }

    return(result_b);
}

/**
 * \brief   Check whether migration feature is active for a block.
 *
 * \param   blkNr_u16                   Block number of the specified block
 * \param   migrationBitmask_u16        Mask for the migration bit in the status byte
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    Migration is active
 * \retval  FALSE   Migration is not active, or the passed persistent ID unknown
*/
boolean Fee_Prv_ConfigIsBlockMigrationActiveByBlockNr(uint16 blkNr_u16, uint16 migrationBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
   boolean result_b = FALSE;

   if (0u != (blockPropertiesTable_pcst[blkNr_u16].Flags_u16 & migrationBitmask_u16))
   {
       result_b = TRUE;
   }

   return(result_b);
}

/**
 * \brief   Check whether migration feature is active for a block.
 *
 * \param   persistentId_u16            Persistent ID of the specified block
 * \param   nrOfBlks_u16                Total number of blocks configured in the instance
 * \param   migrationBitmask_u16        Mask for the migration bit in the status byte
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    Migration is active
 * \retval  FALSE   Migration is not active, or the passed persistent ID unknown
*/
boolean Fee_Prv_ConfigIsBlockMigrationActiveByPersistentId(uint16 persistentId_u16, uint16 nrOfBlks_u16, uint16 migrationBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
   boolean result_b = FALSE;

   uint16 blockNr_u16 = Fee_Prv_ConfigGetBlockNrByPersistentId(persistentId_u16, nrOfBlks_u16, blockPropertiesTable_pcst);

   if (0u != (blockPropertiesTable_pcst[blockNr_u16].Flags_u16 & migrationBitmask_u16))
   {
       result_b = TRUE;
   }

   return(result_b);
}

/**
 * \brief   Check whether the block length is variable and calculated on runtime.
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h
 * \param   varLenBitmask_u16           Mask for the variable lenght bit in the status byte
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    Block length is variable
 * \retval  FALSE   Block length is not variable
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
boolean Fee_Prv_ConfigIsBlockLengthVariableByBlockNr(uint16 blockNr_u16, uint16 varLenBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
   boolean result_b = FALSE;

   if (0u != (blockPropertiesTable_pcst[blockNr_u16].Flags_u16 & varLenBitmask_u16))
   {
       result_b = TRUE;
   }

   return(result_b);
}

/**
 * \brief   Does a specified block support writing from an unstable buffer? (For example EnvRam)
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h - This is not the persistent ID!
 * \param   robBitmask_u16              Mask for the robustness bit in the status byte
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    The block does support writing from an unstable buffer
 *                  This means if the flash content is not equal to the RAM content after writing this is not an error
 * \retval  FALSE   The block does not support writing from an unstable buffer
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
boolean Fee_Prv_ConfigIsBlockRobustnessActiveByBlockNr(uint16 blockNr_u16, uint16 robBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    boolean result_b = FALSE;

    if(0u != (blockPropertiesTable_pcst[blockNr_u16].Flags_u16 & robBitmask_u16))
    {
        result_b = TRUE;
    }

    return(result_b);
}

/**
 * \brief   Is a specified block stored twice in write or invalidate order?
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h - This is not the persistent ID!
 * \param   redBitmask_u16              Mask for the redundant bit in the status byte
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    The block is stored twice when writing or invalidating
 * \retval  FALSE   The block is single stored when writing or invalidating
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
boolean Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr(uint16 blockNr_u16, uint16 redBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    boolean result_b = FALSE;

    if(0u != (blockPropertiesTable_pcst[blockNr_u16].Flags_u16 & redBitmask_u16))
    {
        result_b = TRUE;
    }

    return(result_b);
}

/**
 * \brief   Shall the block be ignored during flash reorganization even when the software does not know it by configuration?
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h - This is not the persistent ID!
 * \param   survivalBitmask_u16         Mask for the redundant bit in the status byte
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    The block shall not be ignored during flash reorganization even when the software does not know it by configuration
 * \retval  FALSE   The block shall be ignored during flash reorganization if it is not known by configuration
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
boolean Fee_Prv_ConfigIsBlockSurvivalActiveByBlockNr(uint16 blockNr_u16, uint16 survivalBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    boolean result_b = FALSE;

    if(0u != (blockPropertiesTable_pcst[blockNr_u16].Flags_u16 & survivalBitmask_u16))
    {
        result_b = TRUE;
    }

    return(result_b);
}

/**
 * \brief   Is the block configured to be NoFallback?
 *
 * \param   blockNr_u16                 Block number as defined in Fee.h - This is not the persistent ID!
 * \param   noFBBitmask_u16             Mask for the redundant bit in the status byte
 * \param   blockPropertiesTable_pcst   Pointer to the block properties table
 *
 * \retval  TRUE    The block has the NoFallback config active
 * \retval  FALSE   The block doesn't have the NoFallback config active
 *
 * \attention  This is a simple get-function without check if blockNr_u16 is valid !
*/
boolean Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr(uint16 blockNr_u16, uint16 noFBBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst)
{
    boolean result_b = FALSE;

    if(0u != (blockPropertiesTable_pcst[blockNr_u16].Flags_u16 & noFBBitmask_u16))
    {
        // NoFallback block
        result_b = TRUE;
    }

    return (result_b);
}

/**
 * \brief   Get the Device name from the configured FeeRbGeneralDeviceIndex. This DeviceID name is then used while
 * accessing the corresponding instance via the API Fee_Rb_Idx_.
 * When device index is not found, Fee_Rb_FeeDevice_Max is returned -> indicating that the device does not exist.
 *
 * \param   feeDeviceIdx_u8         Device index as configured in paramdef FeeRbGeneralDeviceIndex
 *
 * \retval  Fee_Rb_DeviceName_ten   DeviceID name corresponding to the passed DeviceIndex
 */
Fee_Rb_DeviceName_ten Fee_Prv_ConfigGetDeviceNameFromDeviceIndex(uint8 feeDeviceIdx_u8)
{
    uint8 idx;
    Fee_Rb_DeviceName_ten deviceName_en = Fee_Rb_Device_Max;

    for(idx=0u ; idx < (uint8)Fee_Rb_Device_Max; idx++)
    {
        if(Fee_Prv_ConfigDeviceTable_cast[idx].deviceIdx_u8 == feeDeviceIdx_u8)
        {
            deviceName_en = Fee_Prv_ConfigDeviceTable_cast[idx].deviceName_en;
            break;
        }
    }

    return (deviceName_en);
}


# if(defined(FEE_PRV_CFG_MULTIINSTANCE_ENABLED) && (FEE_PRV_CFG_MULTIINSTANCE_ENABLED != FALSE))
/**
 * \brief   Get the Fee file system configured for passed Device name. This API is needed within the Fee component and
 * is hence private function. The calling function has to ensure that the passed device name is a valid name. When
 * device name is not found, FEE_FS10 is returned -> so that the given job will fail and easily detected during testing.
 *
 * \param   deviceName_en        Device name for which configured file system has to be returned
 *
 * \retval  Fee_Prv_Fs_ten       Configured file system corresponding to the passed deviceName_en
 */
Fee_Prv_Fs_ten Fee_Prv_ConfigGetFSFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint8           idx;
    Fee_Prv_Fs_ten  configFS_en = FEE_FS10;

    for(idx=0u ; idx < (uint8)Fee_Rb_Device_Max; idx++)
    {
        if(Fee_Prv_ConfigDeviceTable_cast[idx].deviceName_en == deviceName_en)
        {
            configFS_en = Fee_Prv_ConfigDeviceTable_cast[idx].fs_en;
            break;
        }
    }

    return (configFS_en);
}

/**
 * \brief   Get the pointer to the config table for the passed Device name. This API is needed within the Fee component and
 * is hence private function. The calling function has to ensure that the passed device name is a valid name. When
 * device name is not found, NULL_PTR is returned -> so that the given job will fail and easily detected during testing.
 *
 * \param   deviceName_en                Device name for which configured file system has to be returned
 *
 * \retval  Fee_Prv_ConfigDeviceTable_tst Pointer to the config table corresponding to the passed deviceName_en
 */
Fee_Prv_ConfigDeviceTable_tst const* Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint8                               idx;
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = NULL_PTR;

    for(idx=0u ; idx < (uint8)Fee_Rb_Device_Max; idx++)
    {
        if(Fee_Prv_ConfigDeviceTable_cast[idx].deviceName_en == deviceName_en)
        {
            deviceConfigTable_pcst = &Fee_Prv_ConfigDeviceTable_cast[idx];
            break;
        }
    }

    return (deviceConfigTable_pcst);
}

/*
 * \brief   Returns the block number for the persistent data block
 *
 * \param   FeeDeviceName_en    Device instance for which job has to be performed
 */
uint16 Fee_Prv_ConfigGetBlockNumberOfPersistentData(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint8 idx;
    uint16 deviceIdx_u16 = 0xFFFF;

    for(idx=0u ; idx < (uint8)Fee_Rb_Device_Max; idx++)
    {
        if(Fee_Prv_ConfigDeviceTable_cast[idx].deviceName_en == deviceName_en)
        {
            deviceIdx_u16 = Fee_Prv_ConfigDeviceTable_cast[idx].persDataBlkNr_u16;
            break;
        }
    }

    return (deviceIdx_u16);
}
# endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"
#endif
