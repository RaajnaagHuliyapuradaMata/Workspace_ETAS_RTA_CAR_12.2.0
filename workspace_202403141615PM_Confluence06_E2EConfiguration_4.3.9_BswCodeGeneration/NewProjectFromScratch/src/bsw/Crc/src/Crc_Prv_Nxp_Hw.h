/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef CRC_PRV_NXP_HW_H
#define CRC_PRV_NXP_HW_H

/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/
#include "Crc_Cfg.h"

/*
**********************************************************************************************************************
* Typedefines
**********************************************************************************************************************
*/

typedef struct
{
    volatile uint32 CFG;                                             //[0xFBF64000] : Configuration Register
    volatile uint32 INP;                                             //[0xFBF64004] : Input Register
    volatile uint32 CSTAT;                                           //[0xFBF64008] : Current Status Register
    volatile uint32 OUTP;                                            //[0xFBF6400C] : Output Register
}Crc_NxpRegType;

/*
**********************************************************************************************************************
* Defines/Macros
**********************************************************************************************************************
*/

#include "Os.h"
#if (OS_NUM_CORES == 1U)
#define Crc_GetCoreId()        (0U)
#else
#define Crc_GetCoreId()       GetCoreID()
#endif


#define CRC_HW_CFG8                 (0x00000019U)
#define CRC_HW_CFG8H2F              (0x0000001DU)
#define CRC_HW_CFG16                (0x00000010U)
#define CRC_HW_CFG32                (0x00000027U)
#define CRC_HW_POL_MASK             (0x0000000CU)


#define CRC0  (*(volatile Crc_NxpRegType*)(0x40190000UL)) // CFG-0 INP-4 STAT-8 OUTPUT-C
#define CRC1  (*(volatile Crc_NxpRegType*)(0x40190010UL))
#define CRC2  (*(volatile Crc_NxpRegType*)(0x40190020UL))


/**
 **********************************************************************************************************************
 * \brief Calculates a CRC using the Data CRC Function of the NXP
 *
 * \param    Config       configuration of the crc channel
 * \param    Data         holds a pointer to the start address of the data block to be calculated
 * \param    Length       length of data block to be calculated in bytes
 * \param    StartValue   start value when the algorithm starts
 *
 * \return   result of CRC calculation
 **********************************************************************************************************************
*/
CRC_INLINE uint32 Crc_Prv_CalcCRC_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue)
{
    Crc_NxpRegType Backup;
    volatile Crc_NxpRegType* const Nxp[] = {&CRC0, &CRC1, &CRC2};
    const uint8 *Dptr = Data->b8Ptr;
    volatile Crc_NxpRegType* Context;

    volatile uint8 *INP;
    uint32 i;
    uint32 result;
    uint32 StartValueTemp = StartValue;

    #if (defined(CRC_HW_NXP_S32G2) && (CRC_HW == CRC_HW_NXP_S32G2))
    Context = Nxp[Crc_GetCoreId()];
    #endif

    INP = ((volatile uint8 *)Context) + 0x004U;   // 0xF7F64000UL + 0x004 + n*0x10 + 0x00

    /* save register context for Reentrancy */
    Backup.CFG = Context->CFG;
    Backup.INP = Context->INP;
    Backup.CSTAT = Context->CSTAT;

    /* if swapbits are set start value should be bitwise swapped*/
    if((Config & 0x00000002U) > 0U)
    {
        StartValueTemp = (uint32)Crc_BitSwapHw(StartValue);
    }

    /* configure CRC generation process and execute it */
    Context->CFG = Config;
    Context->CSTAT = StartValueTemp ;


    for(i=0U; i<Length; i++)
    {
        *INP = Dptr[i];
    }

    result = Context->OUTP;

    Context->CFG = Backup.CFG;
    Context->INP = Backup.INP;
    Context->CSTAT = Backup.CSTAT;

    return result;
}

#endif /* CRC_PRV_NXP_HW_H */
