/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2020, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_INTERNALBUFFER_TYPES_H
#define NVM_PRV_INTERNALBUFFER_TYPES_H
/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
typedef struct
{
    /// Pointer to the internal buffer used in current job if required by block configuration.
    uint8* Buffer_pu8;
    /// Pointer to current used size of the internal buffer in bytes.
    uint16* UsedSizeInBytes_pu16;
    /// Offset in bytes to the plain data (original user data) within the internal buffer.
    /// Encrypted data is always located at the beginning of the internal buffer.
    /// If encryption is disabled then offset to the plain data is 0, i.e. it is located at the beginning
    /// of the internal buffer.
    uint16 OffsetPlainData_u16;
    /// Pointer to the position within internal buffer for the block header
    uint8* BlockHeader_pu8;

} NvM_Prv_InternalBuffer_st;

/* NVM_PRV_INTERNALBUFFER_TYPES_H */
#endif

