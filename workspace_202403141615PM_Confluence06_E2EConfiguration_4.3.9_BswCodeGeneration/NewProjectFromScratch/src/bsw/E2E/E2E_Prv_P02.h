

#ifndef E2E_PRV_P02_H
#define E2E_PRV_P02_H

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
//[SWS_E2E_00128]
#define E2E_P02_MAXCOUNTER                    15U       /* Maximum counter value */
#define E2E_P02_DELTACOUNTER_REPEATED         0U        /* Delta counter value when message repeated */
#define E2E_P02_DELTACOUNTER_OK               1U        /* Delta counter value when next message received */
#define E2E_P02_HEADER_SIZE                   16U       /* Size of E2E header in bits */


/*
 **********************************************************************************************************************
 private routines for E2E profile P02
 **********************************************************************************************************************
 */

/**
 ***********************************************************************************************************************
 * E2E_Prv_P02CheckProtectStateType
 *
 * \brief Tests the input parameter E2E_P02ProtectStateType for correctness
 *
 * Description: Tests the input parameter E2E_P01ProtectStateType for correctness
 * The counter values are in principle restricted to the range from 0 to E2E_P02_MAXCOUNTER
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
 * \param   E2E_P02ProtectStateType  *State_pst   Pointer to the sender status structure of of E2E profile P02
 * \return  boolean                               returns TRUE if the check is ok, otherwise returns FALSE
 *
 ***********************************************************************************************************************
 */

E2E_INLINE boolean E2E_Prv_P02CheckProtectStateType(const E2E_P02ProtectStateType * const State_pst)
{
    return ((boolean)(State_pst->Counter <= E2E_P02_MAXCOUNTER));
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P02CheckCheckStateType
 *
 * \brief Tests the input parameter E2E_P02CheckStateType for correctness
 *
 * Description: Tests the input parameter E2E_P01CheckStateType for correctness
 * The counter value are in principle restricted to the range from 0 to E2E_P02_MAXCOUNTER.
 * In case the functions returns a FALSE, the E2E_P02Check routine is interrupted.
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
 * \param   E2E_P02CheckStateType  *State_pst     Pointer to the receiver status structure of of E2E profile P02
 * \return  boolean                               returns TRUE if the check is ok, otherwise returns FALSE
 *
 ***********************************************************************************************************************
 */

E2E_INLINE boolean E2E_Prv_P02CheckCheckStateType(const E2E_P02CheckStateType * const State_pst)
{
    return ((boolean)((State_pst->MaxDeltaCounter <= E2E_P02_MAXCOUNTER) &&
                      (State_pst->LastValidCounter <= E2E_P02_MAXCOUNTER)));
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P02CalcCRC8H2F
 *
 * \brief private routine to calculate CRC-8H2F over the data stream and the DataID for profile 2 of E2E library
 *
 * Description:
 * private routine to calculate CRC-8H2F in profile 2 main routines of E2E library
 * CRC calculation is split in 2 parts: crc over the data stream and crc over the data ID
 * The crc byte itself is exclude from the crc calculation. It is the last byte of the data stream
 * It is required to compensate the rest manipulation from Crc_CalculateCRC8, thus the
 * temporary results have to be manipulated before their use as initial value for next part of crc calculation
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
 * \param   E2E_P02ConfigType      *Config_pst  Pointer to the configuration structure
 * \param   uint8                  DataID_u8    Data identifier
 * \param   uint8                  *Data_pu8    Pointer to the Data to protect
 * \return  uint8                               result of CRC-8H2F over Data and DataID
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P02CalcCRC8H2F(const E2E_P02ConfigType * const Config_pst, uint8 DataID_u8,
                                          const uint8 * const Data_pu8)
{
    uint8 crc_u8;


    /* Calculate CRC over data exclusive the CRC byte, localised at the first byte */
    /* Length is DataLength, given in bits divided by the byte size minus the first byte for the CRC */
    /* Start is the second byte: byte[1] */
    //[SWS_E2E_00117]
    crc_u8 = Crc_CalculateCRC8H2F(&Data_pu8[sizeof(crc_u8)], ((Config_pst->DataLength / 8U) - sizeof(crc_u8)), 
                                  (uint8)E2E_MASK_BYTE, TRUE);

    /* Note: result of CRC division is inverted by the method Crc_CalcualteCRC8H2F */
    /* Next call of the CRC division starts with an additionally inverted CRC result */
    /* initial value of CRC division is the result of CRC division over data => re-inversion required: (CRC ^ 0xFF) */
    /* This is comparable to a data stream, where the data ID is succeeding the data stream without XOR operation */
    /* pointer to input: DataID_u8, length in bytes: 1, initial value: (CRC ^ 0xFF) */
    //[SWS_E2E_00117]
    crc_u8 = Crc_CalculateCRC8H2F(&DataID_u8, sizeof(DataID_u8), crc_u8 , FALSE);

    return (crc_u8);
}

E2E_INLINE void  E2E_Prv_P02DoCheck(const E2E_P02ConfigType *ConfigPtr, E2E_P02CheckStateType *StatePtr, uint8 recCounter_u8)
{
    uint8          deltaCounter_u8;   /* difference between received counter and last valid received counter */
    
    /* Calculate DeltaCounter, respect the limitation to value (E2E_P02_MAXCOUNTER + 1U) */
    deltaCounter_u8 = E2E_Prv_MaxDeltaCounter(recCounter_u8, StatePtr->LastValidCounter,
                                              (E2E_P02_MAXCOUNTER + 1U));

    /* Correct data */
    if (E2E_P02_DELTACOUNTER_OK == deltaCounter_u8)
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
            StatePtr->Status = E2E_P02STATUS_SYNC;
        }
        else
        {
            /* TRACE[SWS_E2E_00147] */
            StatePtr->Status = E2E_P02STATUS_OK;
        }
    }
    /* Repeated data */
    else if (E2E_P02_DELTACOUNTER_REPEATED == deltaCounter_u8)
    {
        E2E_Prv_IncreaseCounter(&StatePtr->NoNewOrRepeatedDataCounter, E2E_P02_MAXCOUNTER);
        /* TRACE[SWS_E2E_00146] */
        StatePtr->Status = E2E_P02STATUS_REPEATED;
    }
    /* Some data lost, but still ok */
    else if (deltaCounter_u8 <= StatePtr->MaxDeltaCounter)
    {
        StatePtr->MaxDeltaCounter  = ConfigPtr->MaxDeltaCounterInit;
        StatePtr->LastValidCounter = recCounter_u8;
        /* TRACE[SWS_E2E_00149] */
        StatePtr->LostData         = deltaCounter_u8 - 1U;

        /* check if synchronization is running */
        if (FALSE != E2E_Prv_CheckSyncCounter(&StatePtr->SyncCounter,
                                              &StatePtr->NoNewOrRepeatedDataCounter,
                                              ConfigPtr->SyncCounterInit,
                                              ConfigPtr->MaxNoNewOrRepeatedData))
        {
            StatePtr->Status = E2E_P02STATUS_SYNC;
        }
        else
        {
            /* TRACE[SWS_E2E_00148] */
            StatePtr->Status = E2E_P02STATUS_OKSOMELOST;
        }
    }
    /* Wrong sequence or to many data lost */
    else
    {
        StatePtr->NoNewOrRepeatedDataCounter = 0U;
        StatePtr->SyncCounter = ConfigPtr->SyncCounterInit;

        if (StatePtr->SyncCounter > 0U)
        {
            StatePtr->MaxDeltaCounter  = ConfigPtr->MaxDeltaCounterInit;
            StatePtr->LastValidCounter = recCounter_u8;
        }
        /* TRACE[SWS_E2E_00145] */
        StatePtr->Status = E2E_P02STATUS_WRONGSEQUENCE;
    }
}

E2E_INLINE Std_ReturnType E2E_Prv_P02VerifyCheckInput(const E2E_P02ConfigType *ConfigPtr, const E2E_P02CheckStateType *const StatePtr,
                            const uint8 *DataPtr)
{
    boolean IsStateOk;
    Std_ReturnType error = E2E_E_OK;
    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr) || (NULL_PTR == DataPtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input data */
    else 
    {
        IsStateOk = E2E_Prv_P02CheckCheckStateType(StatePtr);
        
        if (!IsStateOk)
        {
            error = E2E_E_INPUTERR_WRONG;
        }
    }
    
    return error;
}

/* E2E_PRV_P02_H */
#endif
