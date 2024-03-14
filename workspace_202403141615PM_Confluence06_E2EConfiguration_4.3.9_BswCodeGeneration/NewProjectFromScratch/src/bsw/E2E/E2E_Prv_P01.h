

#ifndef E2E_PRV_P01_H
#define E2E_PRV_P01_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Defines
 **********************************************************************************************************************
 */

#define E2E_P01_CRCOFFSET                     8U
#define E2E_P01_COUNTEROFFSET                 4U
#define E2E_P01_MAXCOUNTER                    14U             /* Maximum counter value */
#define E2E_P01_DELTACOUNTER_REPEATED         0U              /* Delta counter value when message repeated */
#define E2E_P01_DELTACOUNTER_OK               1U              /* Delta counter value when next message received */

/*
 **********************************************************************************************************************
 Prototypes for private routines for the E2E library
 **********************************************************************************************************************
 */

E2E_INLINE boolean   E2E_Prv_P01CheckProtectStateType(const E2E_P01ProtectStateType * const State_pst);
E2E_INLINE boolean   E2E_Prv_P01CheckCheckStateType(const E2E_P01CheckStateType * const State_pst);
E2E_INLINE void      E2E_Prv_P01SetDataIdNibble(const E2E_P01ConfigType * const Config_pst, uint8 * Data_pu8);
E2E_INLINE void      E2E_Prv_P01GetDataIdNibble(const E2E_P01ConfigType * const Config_pst,
                                                  uint8 * RecDataIDNibble_pu8, const uint8 * const Data_pu8);
E2E_INLINE void      E2E_Prv_P01WriteCounter(const E2E_P01ConfigType * const Config_pst,
                                               const E2E_P01ProtectStateType * const State_pst, uint8 * Data_pu8);
E2E_INLINE uint8     E2E_Prv_P01ReadCounter(const E2E_P01ConfigType * const Config_pst, const uint8 * const Data_pu8);
E2E_INLINE uint8     E2E_Prv_P01CalcCRC8(const E2E_P01ConfigType * const Config_pst, uint8 Counter_u8,
                                           const uint8 * const Data_pu8);

/*
 **********************************************************************************************************************
 private routines for E2E profile P01
 **********************************************************************************************************************
 */

/**
 ***********************************************************************************************************************
 * E2E_Prv_P01CheckProtectStateType
 *
 * \brief Tests the input parameter E2E_P01ProtectStateType for correctness
 *
 * Description: Tests the input parameter E2E_P01ProtectStateType for correctness
 * The counter values are in principle restricted to the range from 0 to E2E_P01_MAXCOUNTER
 *
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P01ProtectStateType  *State_pst  Pointer to the sender status structure of of E2E profile P01
 * \return  boolean                              returns TRUE if the check is ok, otherwise returns FALSE
 *
 ***********************************************************************************************************************
 */

E2E_INLINE boolean E2E_Prv_P01CheckProtectStateType(const E2E_P01ProtectStateType * const State_pst)
{
    return ((boolean)(State_pst->Counter <= E2E_P01_MAXCOUNTER));
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P01CheckCheckStateType
 *
 * \brief Tests the input parameter E2E_P01CheckStateType for correctness
 *
 * Description: Tests the input parameter E2E_P01CheckStateType for correctness
 * The counter value are in principle restricted to the range from 0 to E2E_P01_MAXCOUNTER.
 * In case the functions returns a FALSE, the E2E_P01Check routine is interrupted.
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P01CheckStateType  *State_pst        Pointer to the receiver status structure of of E2E profile P01
 * \return  boolean                                  returns TRUE if the check is ok, otherwise returns FALSE
 *
 ***********************************************************************************************************************
 */

E2E_INLINE boolean E2E_Prv_P01CheckCheckStateType(const E2E_P01CheckStateType * const State_pst)
{
    return ((boolean)(State_pst->MaxDeltaCounter <= E2E_P01_MAXCOUNTER));
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P01SetDataIdNibble
 *
 * \brief Writes the lower nibble of the data ID into the designated position of the data stream.
 *
 * Description:
 * In case of profile 1c writes the lower nibble of the data ID into the designated position of the data stream.
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P01ConfigType       *Config_pst     Pointer to the configuration structure of E2E profile P01
 * \param   uint8                   *Data_pu8       Pointer to the byte sequence within the Data to protect
 * \return  void
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void  E2E_Prv_P01SetDataIdNibble(const E2E_P01ConfigType * const Config_pst, uint8 * Data_pu8)
{
    uint16 offset_u16 = Config_pst->DataIDNibbleOffset / 8U;


    /*  Look for the configured DataIdMode */
    if (E2E_P01_DATAID_NIBBLE == Config_pst->DataIDMode)
    {
        /* in case of Profile 1C add the lower nibble of the dataId into the data stream at the correct position */
        if (0U == (Config_pst->DataIDNibbleOffset % 8U))
        {
            Data_pu8[offset_u16] = (Data_pu8[offset_u16] & E2E_MASK_H_NIBBLE) |
                                   ((uint8)((Config_pst->DataID >> 8U) & E2E_MASK_L_NIBBLE));
        }
        else
        {
            Data_pu8[offset_u16] = (Data_pu8[offset_u16] & E2E_MASK_L_NIBBLE) |
                                   ((uint8)((Config_pst->DataID >> 4U) & E2E_MASK_H_NIBBLE));
        }
    }
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P01GetDataIdNibble
 *
 * \brief Writes the lower nibble of the data ID hold in the data stream into a provided buffer
 *
 * Description:
 * In case of profile 1c writes the lower nibble of the data ID hold in the data stream into a provided buffer
 *
 * Restrictions:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P01ConfigType     *Config_pst            Pointer to the configuration structure of E2E profile P01
 * \param   uint8 *               RecDataIDNibble_pu8    Pointer to the variable provided  to get the lower nibble of
 *                                                       the dataID
 * \param   uint8                 *Data_pu8              Pointer to the byte sequence within the Data to protect
 * \return  void
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void  E2E_Prv_P01GetDataIdNibble(const E2E_P01ConfigType * const Config_pst, uint8 * RecDataIDNibble_pu8,
                                              const uint8 * const Data_pu8)
{
    /*  Look for the configured DataIdMode */
    if (E2E_P01_DATAID_NIBBLE == Config_pst->DataIDMode)
    {
        /* in case of Profile 1C add the lower nibble of the dataId into the data stream at the correct position */
        if (0 == (Config_pst->DataIDNibbleOffset % 8))
        {
            *RecDataIDNibble_pu8 = (Data_pu8[Config_pst->DataIDNibbleOffset / 8U] & E2E_MASK_L_NIBBLE);
        }
        else
        {
            /* MR12 RULE 2.2 VIOLATION: This operation is not redundant */
            *RecDataIDNibble_pu8 = ((Data_pu8[Config_pst->DataIDNibbleOffset / 8U] >> 4U) & E2E_MASK_L_NIBBLE);
        }
    }
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P01WriteCounter
 *
 * \brief Writes the counter value into the protected data stream
 *
 * Description:
 * Writes the counter value into the data stream. The position is given by the value of the CounterOsset.
 * The CounterOffset is given in bits. The Counter is a nibble in the data stream and is in the value from 0 to 14.
 * Value 15 is an error value.
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P01ConfigType       *Config_pst     Pointer to the configuration structure of E2E profile P01
 * \param   E2E_P01ProtectStateType *State_pst      Pointer to the sender status structure of of E2E profile P01
 * \param   uint8                   *Data_pu8       Pointer to the byte sequence within the Data to protect
 * \return  void
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void  E2E_Prv_P01WriteCounter(const E2E_P01ConfigType * const Config_pst,
                                           const E2E_P01ProtectStateType * const State_pst, uint8 * Data_pu8)
{
    uint16 offset_u16 = Config_pst->CounterOffset / 8U;


    /*  Look for the configured Counter Offset */
    if (0U == (Config_pst->CounterOffset % 8U))
    {
        /* Write counter into the low nibble of the specified region of data */
        /* new counter = saved high nibble and actualised low nibble */
        Data_pu8[offset_u16] = (Data_pu8[offset_u16] & E2E_MASK_H_NIBBLE) |
                               (State_pst->Counter & E2E_MASK_L_NIBBLE);
    }
    else
    {
        /* Write counter into the high nibble of the specified region of data */
        /* new counter  = saved low nibble and actualised high nibble */
        /* MR12 RULE 2.2 VIOLATION: This operation is not redundant */
        Data_pu8[offset_u16] = (Data_pu8[offset_u16] & E2E_MASK_L_NIBBLE) |
                               ((uint8)(State_pst->Counter << 4U) & E2E_MASK_H_NIBBLE);
    }
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P01ReadCounter
 *
 * \brief Reads the in counter value, transmitted with the data stream
 *
 * Description:
 * Reads the in counter value, transmitted with the data data stream
 * position of the counter is hold in the configuration structure.
 * The position is given in bits. The function returns the counter as an uint8 value.
 * The received counter is a nibble in the counter. The functions cancels the unused nibble of the counter byte
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P01ConfigType       *Config_pst  Pointer to the configuration structure of E2E profile P01
 * \param   uint8                   *Data_pu8    Pointer to the data stream, a uint 8 array
 * \return  uint8                                Received counter, part of the data stream
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P01ReadCounter(const E2E_P01ConfigType * const Config_pst, const uint8 * const Data_pu8)
{
    uint8 recCounter_u8;    /* received counter */


    /*  Look for the configured Counter Offset */
    if (0U == (Config_pst->CounterOffset % 8U))
    {
        /* Read counter from the low nibble of the specified region of data */
        recCounter_u8 = (Data_pu8[Config_pst->CounterOffset / 8U] & E2E_MASK_L_NIBBLE);
    }
    else
    {
        /* Read counter from the high nibble of the specified region of data */
        /* MR12 RULE 2.2 VIOLATION: This operation is not redundant */
        recCounter_u8 = ((Data_pu8[Config_pst->CounterOffset / 8U] >> 4U) & E2E_MASK_L_NIBBLE);
    }

    return (recCounter_u8);
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P01CalcCRC8
 *
 * \brief private routine to calculate CRC-8 SAE J1850 rest profile 1 of E2E library
 *
 * Description:
 * private routine to calculate CRC-8 SAE J1850 in profile 1 main routines of E2E library
 * CRC calculation is split in 3 parts: DataID from Configuration structure, part of Data before and past CRC-byte.
 * The crc byte itself is exclude from the crc calculation.
 * It is required to compensate the rest manipulation from Crc_CalculateCRC8, thus the
 * temporary results have to be manipulated before their use as initial value for next part of crc calculation
 * Past crc calculation over DataId, the crc over the data stream is performed. It is done in two steps:
 * Step 1 before the crc byte:  starts at byte[0] and ends at byte[(CRCOffset / 8) - 1].
 * The length of the first part of the stream is (CRCOffset / 8)
 * Step 2 past the crc byte: starts at byte[(CRCOffset / 8) + 1] and end at byte[datalength / 8]
 * The length is of the second part is (datalength - CRCOffset - 8) / 8
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P01ConfigType      *Config_pst   Pointer to the configuration structure
 * \param   uint8                  Counter_u8    Status variable
 * \param   uint8                  *Data_pu8     Pointer to the Data to protect
 * \return  uint8                                result of CRC-8 SAE J1850 over DataId and Data
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P01CalcCRC8(const E2E_P01ConfigType * const Config_pst, uint8 Counter_u8,
                                       const uint8 * const Data_pu8)
{
    uint8  dataID_au8[2U];
    uint8  crc_u8;
    uint16 crcOffset_u16;           /* crc offset in bytes */
    uint16 dataLength_u16;          /* datalength in bytes */


    /* Data ID: low byte [0], high byte[1] */
    dataID_au8[0U] = (uint8)(Config_pst->DataID);
    dataID_au8[1U] = (uint8)(Config_pst->DataID >> 8U);

    /* Crc calculation over DataID */
    switch (Config_pst->DataIDMode)
    {
        case E2E_P01_DATAID_BOTH:
        {
            /* if START-/XOR-values of CRC and E2E not equal an additional XOR of the start value must be done */
            crc_u8 = Crc_CalculateCRC8(dataID_au8, 2U, E2E_MASK_BYTE, FALSE);
        }
        break;

        case E2E_P01_DATAID_LOW:
        {
            /* if START-/XOR-values of CRC and E2E not equal an additional XOR of the start value must be done */
            crc_u8 = Crc_CalculateCRC8(dataID_au8, 1U, E2E_MASK_BYTE, FALSE);
        }
        break;

        case E2E_P01_DATAID_NIBBLE:
        {
            dataID_au8[1U] = 0U;

            /* if START-/XOR-values of CRC and E2E not equal an additional XOR of the start value must be done */
            crc_u8 = Crc_CalculateCRC8(dataID_au8, 2U, E2E_MASK_BYTE, FALSE);
        }
        break;

        case E2E_P01_DATAID_ALT:
        default:
        {
            if (0U == (Counter_u8 % 2U))
            {
                /* counter is odd: low byte */
                /* if START-/XOR-values of CRC and E2E not equal an additional XOR of the start value must be done */
                crc_u8 = Crc_CalculateCRC8(dataID_au8, 1U, E2E_MASK_BYTE, FALSE);
            }
            else
            {
                /* counter is even: high byte */
                /* if START-/XOR-values of CRC and E2E not equal an additional XOR of the start value must be done */
                crc_u8 = Crc_CalculateCRC8(&dataID_au8[1U], 1U, E2E_MASK_BYTE, FALSE);
            }
        }
        break;
    }

    crcOffset_u16 = Config_pst->CRCOffset / 8U;
    dataLength_u16 = Config_pst->DataLength / 8U;

    /* Crc calculation over data stream (before byte with CRC), step 1 */
    if (crcOffset_u16 >= 1U)
    {
        crc_u8 = Crc_CalculateCRC8(Data_pu8, crcOffset_u16, crc_u8, FALSE);
    }

    /* Crc calculation over data stream (after byte with CRC), step 2 */
    if (crcOffset_u16 < (dataLength_u16 - sizeof(crc_u8)))
    {
        crc_u8 = Crc_CalculateCRC8(&Data_pu8[crcOffset_u16 + sizeof(crc_u8)],
                                   (dataLength_u16 - crcOffset_u16 - sizeof(crc_u8)),
                                   crc_u8,
                                   FALSE);
    }

    crc_u8 ^= E2E_MASK_BYTE; /* To negate last XOR 0xFF operation on computed CRC additional XORing performed */

    return (crc_u8);
}


E2E_INLINE void E2E_Prv_P01DoCheck(const E2E_P01ConfigType *ConfigPtr, E2E_P01CheckStateType *StatePtr, uint8 recCounter_u8)
{
    uint8          deltaCounter_u8;         /* difference between received counter and last valid received counter */
    
    /* Check for disposition of the receiver */
    if (FALSE != StatePtr->WaitForFirstData)
    {
        StatePtr->WaitForFirstData = FALSE;
        StatePtr->MaxDeltaCounter  = ConfigPtr->MaxDeltaCounterInit;
        StatePtr->LastValidCounter = recCounter_u8;
        StatePtr->Status           = E2E_P01STATUS_INITIAL;
    }
    /* Receiver is ready to get Data */
    else
    {
        /* Calculate DeltaCounter, respect the limitation to value (E2E_P01_MAXCOUNTER + 1U) */
        deltaCounter_u8 = E2E_Prv_MaxDeltaCounter(recCounter_u8, StatePtr->LastValidCounter, (E2E_P01_MAXCOUNTER + 1U));
    
        /* Wrong sequence or to many data lost */
        if(deltaCounter_u8 > StatePtr->MaxDeltaCounter)
        {
            StatePtr->NoNewOrRepeatedDataCounter = 0U;
            StatePtr->SyncCounter = ConfigPtr->SyncCounterInit;
    
            if (StatePtr->SyncCounter > 0U)
            {
                StatePtr->MaxDeltaCounter  = ConfigPtr->MaxDeltaCounterInit;
                StatePtr->LastValidCounter = recCounter_u8;
            }
    
            StatePtr->Status = E2E_P01STATUS_WRONGSEQUENCE;
        }
        /* Some data lost, but still ok */
        else if ((deltaCounter_u8 <= StatePtr->MaxDeltaCounter) && (deltaCounter_u8 > E2E_P01_DELTACOUNTER_OK))
        {
            StatePtr->MaxDeltaCounter  = ConfigPtr->MaxDeltaCounterInit;
            StatePtr->LastValidCounter = recCounter_u8;
            StatePtr->LostData         = deltaCounter_u8 - 1U;
    
            /* check if synchronization is running */
            if (FALSE != E2E_Prv_CheckSyncCounter(&StatePtr->SyncCounter,
                                                  &StatePtr->NoNewOrRepeatedDataCounter,
                                                  ConfigPtr->SyncCounterInit,
                                                  ConfigPtr->MaxNoNewOrRepeatedData))
            {
                StatePtr->Status = E2E_P01STATUS_SYNC;
            }
            else
            {
                StatePtr->Status = E2E_P01STATUS_OKSOMELOST;
            }
        }
        /* Correct data */
        else if (E2E_P01_DELTACOUNTER_OK == deltaCounter_u8)
        {
            StatePtr->MaxDeltaCounter  = ConfigPtr->MaxDeltaCounterInit;
            StatePtr->LastValidCounter = recCounter_u8;
            StatePtr->LostData         = 0U;
    
            /* check if synchronization is running */
            if (FALSE != E2E_Prv_CheckSyncCounter(&StatePtr->SyncCounter,
                                                  &StatePtr->NoNewOrRepeatedDataCounter,
                                                  ConfigPtr->SyncCounterInit,
                                                  ConfigPtr->MaxNoNewOrRepeatedData))
            {
                StatePtr->Status = E2E_P01STATUS_SYNC;
            }
            else
            {
                StatePtr->Status = E2E_P01STATUS_OK;
            }
        }
        /* Repeated data */
        else
        {
            E2E_Prv_IncreaseCounter(&StatePtr->NoNewOrRepeatedDataCounter, E2E_P01_MAXCOUNTER);
    
            StatePtr->Status = E2E_P01STATUS_REPEATED;
        }
    }
}

E2E_INLINE Std_ReturnType E2E_Prv_P01VerifyCheckInput(const E2E_P01ConfigType *ConfigPtr, const E2E_P01CheckStateType *const StatePtr,
                            const uint8 *DataPtr)
{
    Std_ReturnType error = E2E_E_OK;
    boolean IsStateOk;
    
    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr) || (NULL_PTR == DataPtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input data */
    else 
    {
        IsStateOk = E2E_Prv_P01CheckCheckStateType(StatePtr);
        
        if (!IsStateOk)
        {
            error = E2E_E_INPUTERR_WRONG;
        }
    }
    
    return error;
}

/* E2E_PRV_P01_H */
#endif
