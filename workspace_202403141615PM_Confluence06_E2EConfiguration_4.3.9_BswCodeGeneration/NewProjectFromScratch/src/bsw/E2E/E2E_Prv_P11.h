

#ifndef E2E_PRV_P11_H
#define E2E_PRV_P11_H

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

#define E2E_P11_CRCOFFSET                 8U
#define E2E_P11_COUNTEROFFSET             4U
#define E2E_P11_MAXCOUNTER                14U                                /* Maximum counter value */
#define E2E_P11_BEYONDMAXCOUNTER          (E2E_P11_MAXCOUNTER + 1U)          /* Boundary value for counter increment */
#define E2E_P11_DELTACOUNTER_REPEATED     0U                        /* Delta counter value when message repeated */
#define E2E_P11_DELTACOUNTER_OK           1U                        /* Delta counter value when next message received */

/*
 **********************************************************************************************************************
 * TypeDefines
 **********************************************************************************************************************
 */
 
 typedef struct
{
    uint8 recCounter_u8;            /* counter value received via data stream */
    uint8 recDataIDNibble_u8;       /* dataId value received via data stream */
    uint8 recCrc_u8;                /* crc8 value received via data stream */
    uint8 crc_u8;                   /* calculated crc result from data stream */
} E2E_P11ContextType;

/*
 **********************************************************************************************************************
 private routines for E2E profile P11
 **********************************************************************************************************************
 */

/**
 ***********************************************************************************************************************
 * E2E_Prv_P11WriteDataIdNibble
 *
 * \brief Writes the lower nibble of the data ID into the designated position of the data stream.
 *        Incase of variant 11A, value of 0xF is loaded at the designated position.
 *
 * Description:
 * In case of profile 11c, writes the lower nibble of the data ID into the designated position of the data stream.
 * In case of profile 11a, writes a value of 0xF into designated position.
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
 * \param   E2E_P11ConfigType       *ConfigPtr     Pointer to the configuration structure of E2E profile P11
 * \param   uint8                   *Data_pu8       Pointer to the byte sequence within the Data to protect
 * \return  void
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void  E2E_Prv_P11WriteDataIdNibble(const E2E_P11ConfigType * const ConfigPtr, uint8 * Data_pu8)
{

    if (E2E_P11_DATAID_NIBBLE == ConfigPtr->DataIDMode)
    {
            uint16 offset_u16 = ConfigPtr->DataIDNibbleOffset / 8U;
            
            Data_pu8[offset_u16] = (Data_pu8[offset_u16] & (uint8)E2E_MASK_H_NIBBLE) |
                                   ((uint8)((ConfigPtr->DataID >> 8U) & (uint8)E2E_MASK_L_NIBBLE));
            
    }
    else if(E2E_P11_DATAID_BOTH == ConfigPtr->DataIDMode)
    {
            uint16 offset_u16 = ConfigPtr->CounterOffset / 8U;
            
            Data_pu8[offset_u16] = ((Data_pu8[offset_u16] & (uint8)E2E_MASK_L_NIBBLE) | (uint8)E2E_MASK_H_NIBBLE);
            
    }
    else
    {
        //empty block
    }
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P11GetDataIdNibble
 *
 * \brief Writes the lower nibble of the data ID hold in the data stream into a provided buffer
 *
 * Description:
 * In case of profile 11c writes the lower nibble of the data ID hold in the data stream into a provided buffer
 * In case of profile 11a, writes the dummy nibble 0xF into the provided buffer
 *
 * Restrictions:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P11ConfigType     *ConfigPtr            Pointer to the configuration structure of E2E profile P11
 * \param   uint8 *               RecDataIDNibble_pu8    Pointer to the variable provided  to get the lower nibble of
 *                                                       the dataID
 * \param   uint8                 *Data_pu8              Pointer to the byte sequence within the Data to protect
 * \return  void
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void  E2E_Prv_P11GetDataIdNibble(const E2E_P11ConfigType * const ConfigPtr, uint8 * RecDataIDNibble_pu8,
                                              const uint8 * const Data_pu8)
{   
     if (E2E_P11_DATAID_NIBBLE == ConfigPtr->DataIDMode)
    {
            *RecDataIDNibble_pu8 = (Data_pu8[ConfigPtr->DataIDNibbleOffset / 8U] & (uint8)E2E_MASK_L_NIBBLE);
    }
    else if (E2E_P11_DATAID_BOTH == ConfigPtr->DataIDMode)
    {
            *RecDataIDNibble_pu8 = (Data_pu8[ConfigPtr->CounterOffset / 8U] & (uint8)E2E_MASK_H_NIBBLE);        
    }
    else
    {
        //empty block
    }
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P11WriteCounter
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
 * \param   E2E_P11ConfigType       *ConfigPtr     Pointer to the configuration structure of E2E profile P11
 * \param   E2E_P11ProtectStateType *State_pst      Pointer to the sender status structure of of E2E profile P11
 * \param   uint8                   *Data_pu8       Pointer to the byte sequence within the Data to protect
 * \return  void
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void  E2E_Prv_P11WriteCounter(const E2E_P11ConfigType * const ConfigPtr,
                                           const E2E_P11ProtectStateType * const State_pst, uint8 * Data_pu8)
{
    uint16 offset_u16 = ConfigPtr->CounterOffset / 8U;
        Data_pu8[offset_u16] = (Data_pu8[offset_u16] & (uint8)E2E_MASK_H_NIBBLE) |
                               (State_pst->Counter & (uint8)E2E_MASK_L_NIBBLE);
       
      
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P11ReadCounter
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
 * \param   E2E_P11ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P11
 * \param   uint8                   *Data_pu8    Pointer to the data stream, a uint 8 array
 * \return  uint8                                Received counter, part of the data stream
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P11ReadCounter(const E2E_P11ConfigType * const ConfigPtr, const uint8 * const Data_pu8)
{
    uint8 recCounter_u8;    /* received counter */
       /* TRACE [SWS_E2E_00503]  */
       /* Read counter from the low nibble of the specified region of data */
        recCounter_u8 = (Data_pu8[ConfigPtr->CounterOffset / 8U] & (uint8)E2E_MASK_L_NIBBLE);
   
    
    return (recCounter_u8);
}


/**
 ***********************************************************************************************************************
 * E2E_Prv_P11CalcCRC8
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
 * \param   E2E_P11ConfigType      *ConfigPtr   Pointer to the configuration structure
 * \param   uint8                  Counter_u8    Status variable
 * \param   uint8                  *Data_pu8     Pointer to the Data to protect
 * \return  uint8                                result of CRC-8 SAE J1850 over DataId and Data
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P11CalcCRC8(const E2E_P11ConfigType * const ConfigPtr, const uint8 * const Data_pu8)
{
    uint8  dataID_au8[2U];
    uint8  crc_u8;
    uint16 crcOffset_u16;           /* crc offset in bytes */
    uint16 dataLength_u16;          /* datalength in bytes */

    /* write Data ID into temporary buffer (big endian order and bit order is not changed) */
    dataID_au8[0U] = (uint8)(ConfigPtr->DataID);
    dataID_au8[1U] = (uint8)(ConfigPtr->DataID >> 8U);
    

    /* Crc calculation over DataID */
    /* TRACE [SWS_E2E_00508] */  
    if(ConfigPtr->DataIDMode == E2E_P11_DATAID_BOTH)
    {
            crc_u8 = Crc_CalculateCRC8(dataID_au8, 2U, (uint8)E2E_MASK_BYTE, FALSE);
    }
    else
    {
            dataID_au8[1U] = 0U;

            crc_u8 = Crc_CalculateCRC8(dataID_au8, 2U, (uint8)E2E_MASK_BYTE, FALSE);
    }

    crcOffset_u16 = ConfigPtr->CRCOffset / 8U;
    dataLength_u16 = ConfigPtr->DataLength / 8U;

    /* Crc calculation over data stream (before byte with CRC), step 1 */
    if (crcOffset_u16 > 0)
    {
        crc_u8 = Crc_CalculateCRC8(Data_pu8, crcOffset_u16, crc_u8, FALSE);
        
        /* MR12 Rule 10.4 Violation: Type casting does not lead to any functional issues */
        if (dataLength_u16 > (crcOffset_u16 + 1))
        {
           crc_u8 = Crc_CalculateCRC8(&Data_pu8[crcOffset_u16 + 1], dataLength_u16 - crcOffset_u16 - sizeof(crc_u8), crc_u8, FALSE); 
        }
    }
    else
    {
        /* MR12 Rule 10.3 Violation: Type casting does not lead to any functional issues */
        crc_u8 = Crc_CalculateCRC8(&Data_pu8[1], dataLength_u16 - 1 , crc_u8, FALSE);
    }

    return (crc_u8);
}

E2E_INLINE uint8  E2E_Prv_P11MaxDeltaCounter(uint8 ReceivedCounter_u8, uint8 LastValidCounter_u8)
{
    return (ReceivedCounter_u8 - LastValidCounter_u8);
}

E2E_INLINE uint8 E2E_Prv_P11ReadCrc(const uint8 * const Data_pu8, const E2E_P11ConfigType * const ConfigPtr)
{
    uint8 recCrc_u8;
           recCrc_u8 = Data_pu8[ConfigPtr->CRCOffset / 8U];
    return (recCrc_u8);
}

E2E_INLINE void E2E_Prv_P11DoCheck(E2E_P11CheckStateType *StatePtr, const E2E_P11ConfigType * const ConfigPtr, const E2E_P11ContextType *ContextPtr)
{
    uint8 deltaCounter_u8;    /* difference between received counter and last valid received counter */

    /* Check for correctness of crc and dataID transmitted with the data*/
    if ((ContextPtr->crc_u8 == ContextPtr->recCrc_u8) && (((ConfigPtr->DataIDMode == E2E_P11_DATAID_NIBBLE) && (ContextPtr->recDataIDNibble_u8 == ((ConfigPtr->DataID >> 8U) & 0x0FU))) || ((ConfigPtr->DataIDMode == E2E_P11_DATAID_BOTH) && ((ContextPtr->recDataIDNibble_u8 >> 4U)== 15U))))
     {
        /* Calculate DeltaCounter */
        deltaCounter_u8 = E2E_Prv_P11MaxDeltaCounter(ContextPtr->recCounter_u8, StatePtr->Counter);

        /* Correct data */
        if (E2E_P11_DELTACOUNTER_OK == deltaCounter_u8)
        {
            StatePtr->Status = E2E_P11STATUS_OK;
        }
        /* Repeated data */
        else if (E2E_P11_DELTACOUNTER_REPEATED == deltaCounter_u8)
        {
            StatePtr->Status = E2E_P11STATUS_REPEATED;
        }
        /* Some data lost, but still ok */
        else if (deltaCounter_u8 <= ConfigPtr->MaxDeltaCounter)
        {
            StatePtr->Status = E2E_P11STATUS_OKSOMELOST;
        }
        /* Wrong sequence or to many data lost */
        else
        {
            StatePtr->Status = E2E_P11STATUS_WRONGSEQUENCE;
        }

        /* set counter value in state */
        StatePtr->Counter = ContextPtr->recCounter_u8;

       }
    else
    {
        StatePtr->Status = E2E_P11STATUS_ERROR;

    }
}

/* E2E_PRV_P11_H */
#endif
