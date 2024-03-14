

#ifndef E2E_PRV_P05_H
#define E2E_PRV_P05_H

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

#define E2E_P05_MAXCOUNTER                    0xFFU     /* Maximum counter value */
#define E2E_P05_DELTACOUNTER_REPEATED         0U        /* Delta counter value when message repeated */
#define E2E_P05_DELTACOUNTER_OK               1U        /* Delta counter value when next message received */
#define E2E_P05_COUNTER_OFFSET                2U        /* Counter offset within datastream */
#define E2E_P05_HEADER_SIZE                   24U       /* Size of E2E header in bits */

/*
 **********************************************************************************************************************
 Prototypes for private routines for the E2E library
 **********************************************************************************************************************
 */

E2E_INLINE boolean  E2E_Prv_P05CheckData(const E2E_P05ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                           uint16 Length_u16);
E2E_INLINE uint8    E2E_Prv_P05ReadU8(const uint8 * const Data_pu8, uint16 offset_u16);
E2E_INLINE void     E2E_Prv_P05WriteU8(uint8 * const Data_pu8, uint16 offset_u16, uint8 value_u8);
E2E_INLINE uint16   E2E_Prv_P05ReadU16(const uint8 * const Data_pu8, uint16 offset_u16);
E2E_INLINE void     E2E_Prv_P05WriteU16(uint8 * const Data_pu8, uint16 offset_u16, uint16 value_u16);
E2E_INLINE uint16   E2E_Prv_P05CalcCRC16(const E2E_P05ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                           uint16 Length_u16);

/*
 **********************************************************************************************************************
 private routines for E2E profile P05
 **********************************************************************************************************************
 */

/**
 ***********************************************************************************************************************
 * E2E_Prv_P05CheckData
 *
 * \brief Tests the input parameters DataPtr and Length for correctness
 *
 * Description: Tests the input parameter DataPtr and Length for correctness
 * In case the functions returns a FALSE, the E2E_P05Check routine is interrupted.
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
 * \param   E2E_P05ConfigType       *Config_pst   Pointer to the configuration structure of E2E profile P05
 * \param   uint8                   *Data_pu8     Pointer to the byte sequence within the Data to be checked
 * \param   uint16                  Length        Length of the data in bytes
 * \return  boolean                               returns TRUE if the check is ok, otherwise returns FALSE
 *
 ***********************************************************************************************************************
 */

E2E_INLINE boolean E2E_Prv_P05CheckData(const E2E_P05ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                          uint16 Length_u16)
{
    return ((boolean)(((NULL_PTR != Data_pu8) && (0 != Length_u16) && (Length_u16 == (Config_pst->DataLength / 8U))) ||
                      ((NULL_PTR == Data_pu8) && (0 == Length_u16))));
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P05ReadU8
 *
 * \brief Read an U8 value from data stream
 *
 * Description: Read an U8 value from datastream.
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

E2E_INLINE uint8 E2E_Prv_P05ReadU8(const uint8 * const Data_pu8, uint16 offset_u16)
{
    /* return U8 value (Bit order is not changed) */
    return (Data_pu8[offset_u16]);
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P05WriteU8
 *
 * \brief Write an U8 value into data stream
 *
 * Description: Write an U8 value into datastream.
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
 * \param   uint8                   value_u8      U8 value to write into the data stream
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void E2E_Prv_P05WriteU8(uint8 * const Data_pu8, uint16 offset_u16, uint8 value_u8)
{
    /* Write U8 value into the specified region of data (little endian order and bit order is not changed) */
    Data_pu8[offset_u16] = value_u8;
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P05ReadU16
 *
 * \brief Read an U16 value from data stream
 *
 * Description: Read an U16 value from datastream regarding the endianess.
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
 * \return  uint16                                returns U16 value from data stream
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint16 E2E_Prv_P05ReadU16(const uint8 * const Data_pu8, uint16 offset_u16)
{
    /* Read U16 value from Data (little endian order and bit order is not changed) */
    return (((uint16)Data_pu8[offset_u16]) | 
            ((uint16)((uint16)Data_pu8[offset_u16 + 1U] << 8U)));
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P05WriteU16
 *
 * \brief Write an U16 value into data stream
 *
 * Description: Write an U16 value into datastream regarding the endianess.
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
 * \param   uint16                  value_u16     U16 value to write into the data stream
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void E2E_Prv_P05WriteU16(uint8 * const Data_pu8, uint16 offset_u16, uint16 value_u16)
{
    /* Write U16 value into the specified region of data (little endian order and bit order is not changed) */
    Data_pu8[offset_u16]      = (uint8)(value_u16);
    Data_pu8[offset_u16 + 1U] = (uint8)(value_u16 >> 8U);
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_P05CalcCRC16
 *
 * \brief private routine to calculate CRC16 over the data stream and the DataID for profile P05 of E2E library
 *
 * Description:
 * private routine to calculate CRC16 in profile P05 main routines of E2E library
 * CRC calculation is split in 2 parts: crc over the data stream and crc over the data ID
 * The crc byte itself is exclude from the crc calculation. It is the last byte of the data stream
 * It is required to compensate the rest manipulation from Crc_CalculateCRC16, thus the
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
 * \param   E2E_P05ConfigType      *Config_pst  Pointer to the configuration structure
 * \param   uint8                  *Data_pu8    Pointer to the data to protect
 * \param   uint16                 Length       Length of the data in bytes
 * \return  uint8                               result of CRC16 over data and DataID
 *
 ***********************************************************************************************************************
 */

E2E_INLINE uint16 E2E_Prv_P05CalcCRC16(const E2E_P05ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                         uint16 Length_u16)
{
    uint16 crc_u16 = 0U;
    uint8 dataID_au8[2U];
    uint16 offset_u16 = Config_pst->Offset / 8U;


    E2E_PARAM_UNUSED(Length_u16);

    /* Calculate CRC over data exclusive the CRC byte */
    /* Length used for CRC calculation is the data length minus the offset minus the size of the CRC */
    if (offset_u16 >= 1U)
    {
        crc_u16 = Crc_CalculateCRC16(Data_pu8, offset_u16, (uint16)E2E_MASK_WORD, TRUE);
        crc_u16 = Crc_CalculateCRC16(&Data_pu8[offset_u16 + sizeof(crc_u16)],
                                     (Config_pst->DataLength / 8U) - offset_u16 - sizeof(crc_u16),
                                     crc_u16,
                                     FALSE);
    }
    else
    {
        /* 0 == Config_pst->Offset */
        crc_u16 = Crc_CalculateCRC16(&Data_pu8[sizeof(crc_u16)],
                                     (Config_pst->DataLength / 8U) - sizeof(crc_u16),
                                     (uint16)E2E_MASK_WORD,
                                     TRUE);
    }

    /* write Data ID into temporary buffer (little endian order) */
    /* write Data ID into temporary buffer (big endian order and bit order is not changed) */
    dataID_au8[0U] = (uint8)(Config_pst->DataID);
    dataID_au8[1U] = (uint8)(Config_pst->DataID >> 8U);
    
    /* Calculate CRC over data ID */
    crc_u16 = Crc_CalculateCRC16(dataID_au8, 2U, crc_u16, FALSE);

    return (crc_u16);
}

/* E2E_PRV_P05_H */
#endif
