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
#ifndef NVM_PRV_CRC_INL_H
#define NVM_PRV_CRC_INL_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_Crc_Types.h"

/*
**********************************************************************************************************************
* Inline functions (Definition)
**********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

LOCAL_INLINE uint16 NvM_Prv_Crc_GetSizeCrc(NvM_Prv_Crc_Type_ten crcType_en);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"


/**
 * \brief
 * Function returns the number of bytes needed to store the CRC defined by the parameter crcType.
 *
 * \param crcType_en
 * CrcType
 *
 * \return
 * Number of bytes needed to store the CRC
 */
LOCAL_INLINE uint16 NvM_Prv_Crc_GetSizeCrc(NvM_Prv_Crc_Type_ten crcType_en)
{
    uint16 size_u16;

    switch (crcType_en)
    {
        case NvM_Prv_Crc_Type_8_Bit_e:
            size_u16 = 1u;
        break;

        case NvM_Prv_Crc_Type_16_Bit_e:
            size_u16 = 2u;
        break;

        case NvM_Prv_Crc_Type_32_Bit_e:
            size_u16 = 4u;
        break;

        default:
            size_u16 = 0u;
        break;
    }
    return size_u16;
}


#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif

