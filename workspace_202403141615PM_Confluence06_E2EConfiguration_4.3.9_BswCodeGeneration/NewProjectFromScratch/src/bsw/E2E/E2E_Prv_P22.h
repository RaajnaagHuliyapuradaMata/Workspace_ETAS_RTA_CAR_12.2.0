

#ifndef E2E_PRV_P22_H
#define E2E_PRV_P22_H

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
//[SWS_E2E_00523]
#define E2E_P22_MAXCOUNTER                    15U                           /* Maximum counter value */
#define E2E_P22_BEYONDMAXCOUNTER              (E2E_P22_MAXCOUNTER + 1U)     /* Boundary value for counter increment */
#define E2E_P22_DELTACOUNTER_REPEATED         0U        /* Delta counter value when message repeated */
#define E2E_P22_DELTACOUNTER_OK               1U        /* Delta counter value when next message received */
#define E2E_P22_HEADER_SIZE                   16U       /* Size of E2E header in bits */

/*
 **********************************************************************************************************************
 * TypeDefines
 **********************************************************************************************************************
 */
 
 typedef struct
{
    uint8 recCounter_u8;      /* counter value received via data stream */
    uint8 recCrc_u8;          /* crc8H2F value received via data stream */
    uint8 crc_u8;             /* calculated crc result from data stream */
} E2E_P22ContextType;


/*
 **********************************************************************************************************************
 Prototypes for private routines for the E2E library
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 private routines for E2E profile P22
 **********************************************************************************************************************
 */


/**
 ***********************************************************************************************************************
 * E2E_Prv_P22WriteU8
 *
 * \brief Write an U8 Counter value into data stream
 *
 * Description: Write an U8 value into datastream regarding the endianess and bit order.
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
 * \param   uint8                   *Data_pu8     Pointer to the byte sequence within the Data
 * \param   uint8                  offset_u8    Offset of value within data stream
 * \param   uint8                  value_u8     U8 value to write into the data stream
 *
 ***********************************************************************************************************************
 */
 
E2E_INLINE void E2E_Prv_P22WriteU8(const E2E_P22ConfigType *Config_pst, uint8 * const Data_pu8, uint8 value_u8)
{
    uint8 CRCCounter;
    
    /* TRACE [SWS_E2E_00530] */
    CRCCounter = value_u8;
    Data_pu8[(Config_pst->Offset >> 3U) + 1] = CRCCounter & 0x0FU;
}


/**
 ***********************************************************************************************************************
 * E2E_Prv_P22CalcCRC8H2F
 *
 * \brief private routine to calculate CRC-8H2F over the data stream and the DataID for profile 22 of E2E library
 *
 * Description:
 * private routine to calculate CRC-8H2F in profile 22 main routines of E2E library
 * CRC calculation is split in 2 parts: crc over the data stream and crc over the data ID
 * The crc byte itself is exclude from the crc calculation. It is the last byte of the data stream
 * It is required to compensate the rest manipulation from Crc_CalculateCRC8H2F, thus the
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
 * \param   E2E_P22ConfigType      *Config_pst  Pointer to the configuration structure
 * \param   uint8                  *Data_pu8    Pointer to the Data to protect
 * \param   uint32                 Length       Length of the data in bytes
 * \param   uint8                  value_u8     Counter value from E2E_P22ProtectStateType
 * \return  uint8                               result of CRC-8H2F over Data and DataID
 *
 ***********************************************************************************************************************
 */
 
E2E_INLINE uint8 E2E_Prv_P22CalcCRC8H2F(const E2E_P22ConfigType * const ConfigPtr, const uint8 * const Data_pu8,
                                         uint32 Length_u32, uint8 value_u8)
{
    uint8 crc_u8 = 0U;
    const uint8* Data_apu8 = (const uint8 *)&(ConfigPtr->DataIDList[value_u8]);
    uint16 offset_u16 = ConfigPtr->Offset/8U;
    
    /* TRACE[SWS_E2E_00526], TRACE[SWS_E2E_00527] */
    /* Calculate CRC over data exclusive the CRC byte */
    /* Length used for CRC calculation is the data length minus the offset minus the size of the CRC */
    if(offset_u16 > 0U)
    {
        crc_u8 = Crc_CalculateCRC8H2F(Data_pu8, offset_u16, E2E_MASK_BYTE, TRUE);
        if(Length_u32 > (offset_u16 + 1U))
        {
            crc_u8 = Crc_CalculateCRC8H2F(&Data_pu8[offset_u16 + 1U],
                                     Length_u32 - offset_u16 - 1U,
                                     crc_u8,
                                     FALSE);
         }
         else
         {
            /* Nothing will be done. */
         }
    }
    else
    {
        crc_u8 = Crc_CalculateCRC8H2F(Data_pu8, Length_u32 - 1U, E2E_MASK_BYTE, TRUE);
    }
        crc_u8 = Crc_CalculateCRC8H2F(Data_apu8, 1U, crc_u8, FALSE);
        
    return(crc_u8);
    
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P22WriteCrcU8
 *
 * \brief Write an U8 value into data stream
 *
 * Description: Write an U16 value into datastream regarding the endianess and bit order.
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
 * \param   uint8                   *Data_pu8     Pointer to the byte sequence within the Data
 * \param   uint16                  offset_u16    Offset of value within data stream
 * \param   uint8                   value_u8     U8 value to write into the data stream
 *
 ***********************************************************************************************************************
 */
 
E2E_INLINE void E2E_Prv_P22WriteCrcU8(uint8 * const Data_pu8, uint16 offset_u16, uint8 value_u8)
{
    /* TRACE[SWS_E2E_00532] */
    Data_pu8[offset_u16] = (uint8)(value_u8);
    
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P22ReadU8
 *
 * \brief Read an U8 Counter value from data stream
 *
 * Description: Read an U8 value from datastream regarding the endianess.
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
 * \param   uint8                   *Data_pu8     Pointer to the byte sequence within the Data
 * \param   uint16                  offset_u16    Offset of value within data stream
 * \return  uint8                                 returns U8 value from data stream
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P22ReadU8(const uint8 * const Data_pu8, uint16 offset_u16)
{
    uint8 recCounter_u8;
    /* TRACE[SWS_E2E_00536] */
    /* Read U8 value from Data (Little endian order and bit order is not changed) */
    recCounter_u8 = ((Data_pu8[(offset_u16 >> 3U) + 1U]) & E2E_MASK_L_NIBBLE);
    return(recCounter_u8);
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P22ReadCrcU8
 *
 * \brief Read an U8 CRC value from data stream
 *
 * Description: Read an U8 value from datastream regarding the endianess.
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
 * \param   uint8                   *Data_pu8     Pointer to the byte sequence within the Data
 * \param   uint16                  offset_u16    Offset of value within data stream
 * \return  uint8                                returns U8 value from data stream
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P22ReadCrcU8(const uint8 * const Data_pu8, uint16 offset_u16)
{
    uint8 recCrc_u8;
    /* TRACE[SWS_E2E_00537] */
    /* Read U8 value from Data */
    recCrc_u8 = (Data_pu8[offset_u16]);
    return(recCrc_u8);
}

/**
 *******************************************************************************************************************
 * E2E_Prv_P22MaxDeltaCounter
 *
 * \brief       Calculates the deltaCounter as difference between ReceivedCounter and LastValidCounter.
  *
 * Description: Service calculates the Delta of two counter
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
 * \param   uint8  ReceivedCounter_u8   New Counter
 * \param   uint8  LastValidCounter_u8  Old valid Counter
 * \return  uint8  deltaCounter_u8      Difference of two Counters
 *
 *******************************************************************************************************************
 */

E2E_INLINE uint8 E2E_Prv_P22MaxDeltaCounter(uint8 ReceivedCounter_u8, uint8 LastValidCounter_u8)
{
    return (ReceivedCounter_u8 - LastValidCounter_u8);
}

E2E_INLINE void E2E_Prv_P22DoCheck(E2E_P22CheckStateType *StatePtr, const E2E_P22ConfigType * const ConfigPtr, const E2E_P22ContextType *ContextPtr)
{
    uint8 deltaCounter_u8;    /* difference between received counter and last valid received counter */
    
                /* Check for correctness of crc transmitted with the data */
                if ((ContextPtr->crc_u8 != ContextPtr->recCrc_u8))
                {
                    StatePtr->Status = E2E_P22STATUS_ERROR;
                }
                /* Crc OK: Received and calculated Crc is the same */
                else
                {
                    /* Calculate DeltaCounter */
                    deltaCounter_u8 = E2E_Prv_P22MaxDeltaCounter(ContextPtr->recCounter_u8, StatePtr->Counter);
                
                    /* Correct data */
                    if (E2E_P22_DELTACOUNTER_OK == deltaCounter_u8)
                    {
                        StatePtr->Status = E2E_P22STATUS_OK;
                    }
                    /* Repeated data */
                    else if (E2E_P22_DELTACOUNTER_REPEATED == deltaCounter_u8)
                    {
                        StatePtr->Status = E2E_P22STATUS_REPEATED;
                    }
                    /* Some data lost, but still ok */
                    else if (deltaCounter_u8 <= ConfigPtr->MaxDeltaCounter)
                    {
                        StatePtr->Status = E2E_P22STATUS_OKSOMELOST;
                    }
                    /* Wrong sequence or to many data lost */
                    else
                    {
                        StatePtr->Status = E2E_P22STATUS_WRONGSEQUENCE;
                    }

                        /* set counter value in state */
                        StatePtr->Counter = ContextPtr->recCounter_u8;
                }
}

/* E2E_PRV_P22_H */
#endif
