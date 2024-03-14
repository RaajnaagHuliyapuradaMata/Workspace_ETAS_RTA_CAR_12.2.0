

#ifndef CRC_H
#define CRC_H


/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/
#include "Std_Types.h"             /* AUTOSAR standard type definitions */

#include "Crc_Cfg.h"               /* Configuration file */



/*
**********************************************************************************************************************
* Defines/Macros
**********************************************************************************************************************
*/


/*
**********************************************************************************************************************
* Type definitions
**********************************************************************************************************************
*/
/*  data type passed to hw calculation function, based on whether the data is 8bit or 16bit the following are used */
typedef union
{
    const uint8* b8Ptr;
    const uint16* b16Ptr;
}Crc_DataType; 
 

/*

**********************************************************************************************************************
* Variables
**********************************************************************************************************************
*/


/*
**********************************************************************************************************************
* Extern declarations
**********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/

#define CRC_START_SEC_CODE
#include "Crc_MemMap.h"

CRC_INLINE uint32 Crc_BitSwapHw(uint32 StartValue);
extern void Crc_GetVersionInfo(Std_VersionInfoType * const VersionInfo);
extern uint8 Crc_CalculateCRC8(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8, boolean Crc_IsFirstCall);
extern uint8 Crc_CalculateCRC8H2F(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8, boolean Crc_IsFirstCall);
extern uint16 Crc_CalculateCRC16(const uint8* Crc_DataPtr, uint32 Crc_Length, uint16 Crc_StartValue16, boolean Crc_IsFirstCall);
extern uint32 Crc_CalculateCRC32(const uint8* Crc_DataPtr, uint32 Crc_Length, uint32 Crc_StartValue32, boolean Crc_IsFirstCall);
extern uint32 Crc_CalculateCRC32P4(const uint8* Crc_DataPtr, uint32 Crc_Length, uint32 Crc_StartValue32, boolean Crc_IsFirstCall);
extern uint64 Crc_CalculateCRC64(const uint8* Crc_DataPtr, uint32 Crc_Length, uint64 Crc_StartValue64, boolean Crc_IsFirstCall);


CRC_INLINE uint32 Crc_BitSwapHw(uint32 StartValue)
{
uint32 mask = 0xFFFF0000UL;
StartValue = ((StartValue & mask) >> 16) | ((StartValue & ~mask) << 16);
mask = 0xFF00FF00UL;
StartValue = ((StartValue & mask) >> 8) | ((StartValue & ~mask) << 8);
mask = 0xF0F0F0F0UL;
StartValue = ((StartValue & mask) >> 4) | ((StartValue & ~mask) << 4);
mask = 0xCCCCCCCCUL;
StartValue =((StartValue & mask) >> 2) | ((StartValue & ~mask) << 2);
mask = 0xAAAAAAAAUL;
StartValue = ((StartValue & mask) >> 1) | ((StartValue & ~mask) << 1);
return (StartValue);
}


#define CRC_STOP_SEC_CODE
#include "Crc_MemMap.h"

#endif /* CRC_H */


