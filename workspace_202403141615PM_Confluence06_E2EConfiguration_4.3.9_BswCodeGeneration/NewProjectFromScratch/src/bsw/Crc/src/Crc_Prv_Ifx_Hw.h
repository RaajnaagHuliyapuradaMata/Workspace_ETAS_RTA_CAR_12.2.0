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

#ifndef CRC_PRV_IFX_HW_H
#define CRC_PRV_IFX_HW_H

#include "Mcu.h"
#include "Crc.h"
#include "Os.h"
/*
**********************************************************************************************************************
* Typedefines
**********************************************************************************************************************
*/

typedef struct
{
    volatile uint32 IR;
    volatile uint32 RES;
    volatile uint32 CFG;
    volatile uint32 STS;
    volatile uint32 LENGTH;
    volatile uint32 CHECK;
    volatile uint32 CRC;
    volatile uint32 CTR;
}Crc_IfxRegType;

/*
**********************************************************************************************************************
* Defines/Macros
**********************************************************************************************************************
*/
/* CFG register values for each HW supported CRC, for IFX 65nm only CRC32 is supported and for IFX 40nm CRC8, CRC16,  *
 * CRRC32 and CRC32P4 are supported                                                                                   */
#if(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM))
#define CRC_HW_CFG32                (0x00000700U)
#else
#define CRC_HW_CFG8                 (0x00030C00U)
#define CRC_HW_CFG16                (0x00020800U)
#define CRC_HW_CFG32P4              (0x00010F00U)
#define CRC_HW_CFG32                (0x00000F00U)
#endif
#define CRC_HW_POL_MASK             (0x000F0000U)
#define CLC                         (*(volatile uint32*)(0xF0000000UL))

#if(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM))

/* IFX TC2xxxx devices fits to below configuration       */
/* FCE engine of IFX2xxx are not flexible to handle all CRC methods, CRC0 & CRC1 handles CRC32, CRC2 handles CRC16 and
   CRC3 handles CRC8. CRC32 request from core0, core1 & core2 are handled in CRC0, CRC1 and Table method respectively */
#define CRC0  (*(volatile Crc_IfxRegType*)(0xF0003F20UL))
#define CRC1  (*(volatile Crc_IfxRegType*)(0xF0003F40UL))
#define CRC2  (*(volatile Crc_IfxRegType*)(0xF0003F60UL))
#define CRC3  (*(volatile Crc_IfxRegType*)(0xF0003F80UL))
#else
/* IFX TC33xxx and above fits to  below configuration    */
/* At the moment for each core one register is allocated *
*  to handle all HW CRC request in that core             */
#define CRC0  (*(volatile Crc_IfxRegType*)(0xF0000100UL)) /* [m=0-3] [0xF0000000 + 100 + m*20] */
#define CRC1  (*(volatile Crc_IfxRegType*)(0xF0000120UL))
#define CRC2  (*(volatile Crc_IfxRegType*)(0xF0000140UL))
#define CRC3  (*(volatile Crc_IfxRegType*)(0xF0000160UL))
#define CRC4  (*(volatile Crc_IfxRegType*)(0xF0000180UL))
#define CRC5  (*(volatile Crc_IfxRegType*)(0xF00001A0UL))
#define CRC6  (*(volatile Crc_IfxRegType*)(0xF00001C0UL))
#define CRC7  (*(volatile Crc_IfxRegType*)(0xF00001E0UL))
#endif

#if (OS_NUM_CORES == 1U)
#define Crc_GetCoreId()        (0U)
#else
#define Crc_GetCoreId()       GetCoreID()
#endif
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/

CRC_INLINE uint32 Crc_Prv_CalcCRC32_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue);
#if(!(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM)))
CRC_INLINE uint32 Crc_Prv_CalcCRC32P4_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue);
CRC_INLINE uint32 Crc_Prv_CalcCRC16_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue);
CRC_INLINE uint32 Crc_Prv_CalcCRC8_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue);
#endif

/**
 **********************************************************************************************************************
 * \brief Calculates a CRC using the Data CRC Function of the IFX microcontroller
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
    uint32 result = 0U;

    /*Call appropriate CRC function to calclate HW based CRC based on configuration */
    switch(Config)
    {
        case CRC_HW_CFG32:
        {
            result = Crc_Prv_CalcCRC32_Hw(Config, Data, Length, StartValue);
            break;
        }
#if(!(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM)))
        case CRC_HW_CFG32P4:
        {
            result = Crc_Prv_CalcCRC32P4_Hw(Config, Data, Length, StartValue);
            break;
        }
        case CRC_HW_CFG16:
        {
            result = Crc_Prv_CalcCRC16_Hw(Config, Data, Length, StartValue);
            break;
        }
        case CRC_HW_CFG8:
        {
            result = Crc_Prv_CalcCRC8_Hw(Config, Data, Length, StartValue);
            break;
        }
#endif
        default:
        {
            /* do nothing*/
            break;
        }
    }

    return result;
}

/**
 **********************************************************************************************************************
 * \brief Calculates a CRC using the FCE engine of the IFX microcontroller for CRC32 Polynomial 0x04C11DB7
 *
 * \param    Config       configuration of the crc channel
 * \param    Data         holds a pointer to the start address of the data block to be calculated
 * \param    Length       length of data block to be calculated in bytes
 * \param    StartValue   start value when the algorithm starts
 *
 * \return   result of CRC calculation
 **********************************************************************************************************************
*/
CRC_INLINE uint32 Crc_Prv_CalcCRC32_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue)
{
    /* Declaration and Initialisation of Local variables */
    static const uint32  LookUp[16]={0x00000000U,0x1DB71064U,0x3B6E20C8U,0x26D930ACU,0x76DC4190U,0x6B6B51F4U,0x4DB26158U,0x5005713CU,
                              0xEDB88320U,0xF00F9344U,0xD6D6A3E8U,0xCB61B38CU,0x9B64C2B0U,0x86D3D2D4U,0xA00AE278U,0xBDBDF21CU};
    const uint8 *Dptr = Data->b8Ptr;
    uint32 SoftBytes;
    uint32 HardBytes;
    uint32 i=0;
    uint32 Result = StartValue;

    /* HW CRC32 of IFX can handle only multiple of 32 bitS      */
    /* rest of the bytes are handled as software CRC            */
    /* in IFX65nm device CRC32 is realiseable only in FCE0, FCE1*/
    /* crc32 call from Core0 & 1 will use HW otherise SW method */
    #if(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM)) /* IFX 65nm code*/
    SoftBytes = (Crc_GetCoreId() < 0x02U)?(Length%4U):Length;
    #else /* IFX 40nm code*/
    SoftBytes = Length%4U;
    #endif
    HardBytes = Length - SoftBytes;

    /* CRC32 calculation using Hardware */
    if(HardBytes > 0U)
    {
        /* Required declarion for Reentrancy*/
        Crc_IfxRegType Backup;
        /* All supported FCE consolidated*/
        #if(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM)) /* IFX 65nm code*/
        volatile Crc_IfxRegType* const IfxReg[] = {&CRC0, &CRC1, &CRC2, &CRC3};
        #else /* IFX 40nm code*/
        volatile Crc_IfxRegType* const IfxReg[] = {&CRC0, &CRC1, &CRC2, &CRC3, &CRC4, &CRC5, &CRC6, &CRC7};
        #endif
        volatile Crc_IfxRegType* Context;
        uint32 data_u32 = 0U;
        uint32 ResultSwap_u32 = 0U;

        /*bit swap the start value is required in HW block*/
        for(i=0U; i<32U; i++)
        {
            if((Result & (0x01U << i) ) > 0U)
            {
                ResultSwap_u32 |= (0x01U << (31U-i));
            }
        }

        /*For each core one FCE is allocated */
        Context = IfxReg[Crc_GetCoreId()];

        /* save register context for Reentrancy */
        Backup.CHECK = Context->CHECK;
        Backup.CRC = Context->CRC;
        Backup.CFG = Context->CFG;

        /* configure Registers of FCE, protected Registers are called with protection */
        Mcu_Rb_ResetEndInit();
        if(*(&CLC) != 0U)
        {
            *(&CLC) = 0U;
        }
        Context->CFG = Config;
        Mcu_Rb_SetEndInit();

        /*As each time 4 byte data is Inputed */
        HardBytes/= 4U;
        /*Start values is updated to CRC register*/
        Context->CRC = ResultSwap_u32;
        /*loop for Hardbyte times and input the data*/
        for(i=0U; i<HardBytes; i++)
        {
            data_u32 = 0U;
            /* construct 32 bit data from uint8* data buffer                                    */

        #if(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM)) /* IFX 65nm code*/

        /* data swapping is not configured in CFG register hence data is construted accordingly */
            data_u32 = data_u32 | ((uint32)*Dptr << 24U);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 16U);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 8U);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr) );

        #else /* IFX 40nm code*/

        /* data swapping is configured in CFG register hence data is construted accordingly */
            data_u32 = data_u32 | ((uint32)*Dptr);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 8U);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 16U);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 24U );

        #endif
            Dptr++;
            Context->IR = data_u32;
        }

         Result = Context->RES;

         /* backedup register values Restored to FCE registers to ensure Reentrancy */
         Context->CRC = Backup.CRC;
         Context->CHECK = Backup.CHECK;
         Mcu_Rb_ResetEndInit();
         Context->CFG = Backup.CFG;
         Mcu_Rb_SetEndInit();
    }
    else
    {
        /*if datalength is less than 4, start value is XORed with 0xFFFFFFFF to ensure
        * input to software block is from both if and else part is XORed value.*/
         Result ^= CRC_XOR_VALUE32;
    }

    /*Software calculation block - Begin*/

    /*Remove the XOR on the RESULT from Hardware calculation region */
    Result ^= CRC_XOR_VALUE32;

    /* CRC32 calculation using software */

    while(SoftBytes>0U)
    { /* Software method used is: HalfByte method to reduce the overhead of bigger Lookup table. */
        Result = LookUp[(Result ^ (*Dptr     )) & 0x0FU] ^ (Result >> 4U);
        Result = LookUp[(Result ^ (*Dptr >> 4U)) & 0x0FU] ^ (Result >> 4U);
        Dptr++;
        SoftBytes--;
    }

    /* Removed XOR is added back*/
    Result ^= CRC_XOR_VALUE32;

    return Result;
}

#if(!(defined(CRC_HW_IFX_DEV4_65NM) && (CRC_HW == CRC_HW_IFX_DEV4_65NM)))
/**
 **********************************************************************************************************************
 * \brief Calculates a CRC using the FCE engine of the IFX microcontroller for CRC32P4 Polynomial 0xF4ACFB13
 *
 * \param    Config       configuration of the crc channel
 * \param    Data         holds a pointer to the start address of the data block to be calculated
 * \param    Length       length of data block to be calculated in bytes
 * \param    StartValue   start value when the algorithm starts
 *
 * \return   result of CRC calculation
 **********************************************************************************************************************/

CRC_INLINE uint32 Crc_Prv_CalcCRC32P4_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue)
{
    /* Declaration and Initialisation of Local variables */
    const uint8 *Dptr = Data->b8Ptr ;
    uint32 SoftBytes;
    uint32 HardBytes;
    uint32 i=0;
    uint32 Result = StartValue;
    uint32 index;
    uint32 crcTemp_u32;
    uint32 crcSave_u32;
    uint8 bitPos;

    /* HW CRC32 of IFX can handle only multiple of 32 bitS */
    /* rest of the bytes are handled as software CRC*/
    SoftBytes = Length%4U;
    HardBytes = Length - SoftBytes;



    /* CRC32P4 calculation using Hardware */
    if(HardBytes > 0U)
    {
        /* Required declarion for Reentrancy*/
        Crc_IfxRegType Backup;
        /* All supported FCE consolidated*/
        volatile Crc_IfxRegType* const IfxReg[] = {&CRC0, &CRC1, &CRC2, &CRC3, &CRC4, &CRC5, &CRC6, &CRC7};
        volatile Crc_IfxRegType* Context;
        uint32 data_u32 = 0U;
        uint32 ResultSwap_u32 = 0U;

        /*bit swap the start value is required in HW block*/
        for(i=0U; i<32U; i++)
        {
            if((Result & (0x01U << i) ) > 0U)
            {
                ResultSwap_u32 |= (0x01U << (31U-i));
            }
        }

        Context = IfxReg[Crc_GetCoreId()];

        /* save register context for Reentrancy */
        Backup.CHECK = Context->CHECK;
        Backup.CRC = Context->CRC;
        Backup.CFG = Context->CFG;

        /* configure Registers of FCE, protected Registers are called with protection */
        Mcu_Rb_ResetEndInit();
        if(*(&CLC) != 0U)
        {
            *(&CLC) = 0U;
        }
        Context->CFG = CRC_HW_CFG32P4;
        Mcu_Rb_SetEndInit();

        /*As each time 4 byte data is Inputed */
        HardBytes/= 4U;
        /*Start value is updated to CRC register*/
        Context->CRC = ResultSwap_u32;
        /*loop for Hardbyte times and input the data*/
        for(i=0U; i<HardBytes; i++)
        {
            data_u32 = 0U;
            /* construct 32 bit data from uint8* data buffer                                    */
            /* data swapping is configured in CFG register hence data is construted accordingly */
            data_u32 = data_u32 | ((uint32)*Dptr);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 8U);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 16U);
            Dptr++;
            data_u32 = data_u32 | (((uint32)*Dptr)<< 24U );
            Dptr++;
            Context->IR = data_u32;
        }

         Result = Context->RES;

         /* backedup register values Restored to FCE registers to ensure Reentrancy */
         Context->CRC = Backup.CRC;
         Context->CHECK = Backup.CHECK;
         Mcu_Rb_ResetEndInit();
         Context->CFG = Backup.CFG;
         Mcu_Rb_SetEndInit();
    }
    else
    {
        /*if datalength is less than 4, start value is XORed with 0xFFFFFFFF to ensure
        * input to software block is from both if and else part is XORed value.*/
        Result ^= CRC_XOR_VALUE32;
    }

    /*Software calculation block - Begin*/

    /*Remove the XOR on the RESULT from Hardware calculation region */

    crcTemp_u32 = Result ^ CRC_XOR_VALUE32;

    /* CRC32P4 calculation using software */
    if(SoftBytes > 0)
    {

        for (index = 0U; index < SoftBytes; ++index)
        {

            crcTemp_u32 ^= ((uint32)Dptr[index]) ; /* Impact of temporary rest on next crc rest */

            for (bitPos = CRC_JUNK; bitPos > 0U; bitPos--)
            {
                crcSave_u32 = crcTemp_u32;
                crcTemp_u32 >>= 1U;
                /* Length of rest = order of crc polynomial */
                crcTemp_u32 ^= (0U != (crcSave_u32 & CRC_32P4_BITMASK)) ? ((uint32)CRC_32P4_REV_POLYNOMIAL) : 0U;
            }
        }
    }
    /* Removed XOR is added back*/
    Result = crcTemp_u32 ^ CRC_XOR_VALUE32;

    return Result;
}

/**
 **********************************************************************************************************************
 * \brief Calculates a CRC using the FCE engine of the IFX microcontroller for CRC16 Polynomial 0x1021
 *
 * \param    Config       configuration of the crc channel
 * \param    Data         holds a pointer to the start address of the data block to be calculated
 * \param    Length       length of data block to be calculated in bytes
 * \param    StartValue   start value when the algorithm starts
 *
 * \return   result of CRC calculation
 **********************************************************************************************************************/
CRC_INLINE uint32 Crc_Prv_CalcCRC16_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue)
{
    /* Declaration and Initialisation of Local variables */
    const uint8 *Dptr = Data->b8Ptr;;
    uint32 SoftBytes;
    uint32 HardBytes;
    uint32 i=0;
    uint32 Result = StartValue;
    uint32 index;
    uint16 crcTemp_u16;
    uint16 crcSave_u16;
    uint8 bitPos;

    /* HW CRC16 of IFX can handle only multiple of 16 bitS */
    /* rest of the bytes are handled as software CRC*/
    SoftBytes = Length%2U;
    HardBytes = Length - SoftBytes;

    /* CRC16 calculation using Hardware */
    if(HardBytes > 0U)
    {
        /* Required declarion for Reentrancy*/
        Crc_IfxRegType Backup;
        /* All supported FCE consolidated*/
        volatile Crc_IfxRegType* const IfxReg[] = {&CRC0, &CRC1, &CRC2, &CRC3, &CRC4, &CRC5, &CRC6, &CRC7};
        volatile Crc_IfxRegType* Context;
        uint16 data_u16 = 0U;;

        /*For each core one FCE is allocated */
        Context = IfxReg[Crc_GetCoreId()];


        /* save register context for Reentrancy */
        Backup.CHECK = Context->CHECK;
        Backup.CRC = Context->CRC;
        Backup.CFG = Context->CFG;

        /* configure Registers of FCE, protected Registers are called with protection */
        Mcu_Rb_ResetEndInit();
        if(*(&CLC) != 0U)
        {
            *(&CLC) = 0U;
        }
        Context->CFG = CRC_HW_CFG16;
        Mcu_Rb_SetEndInit();

        /*As each time 2 byte data is Inputed */
        HardBytes/= 2U;
        /*Start value is updated to CRC register*/
        Context->CRC = Result;
        /*loop for Hardbyte times and input the data*/
        for(i=0U; i<HardBytes; i++)
        {
            data_u16 = 0U;
            /*construct 16 bit data from uint8* data buffer*/
            data_u16 = data_u16 | ((uint16)*Dptr);
            Dptr++;
            data_u16 = data_u16 | (((uint16)*Dptr)<< 8U);
            Dptr++;
            Context->IR = data_u16;
        }

         Result = Context->RES;

         /* backedup register values Restored to FCE registers to ensure Reentrancy */
         Context->CRC = Backup.CRC;
         Context->CHECK = Backup.CHECK;
         Mcu_Rb_ResetEndInit();
         Context->CFG = Backup.CFG;
         Mcu_Rb_SetEndInit();
    }
    else
    {
        /*if datalength is less than 2, start value is XORed with 0xFFFF to ensure
        * input to software block is from both if and else part is XORed value.*/
        Result ^= CRC_XOR_VALUE16;
    }

    /*Software calculation block - Begin*/

    /*Remove the XOR on the RESULT from Hardware calculation region */
    crcTemp_u16 = (Result & 0x0000FFFF) ^ CRC_XOR_VALUE16;

    if(SoftBytes > 0)
    {

        for (index = 0U; index < SoftBytes; ++index)
        {

            crcTemp_u16 ^= ((uint16)Dptr[index]) << CRC_16_JUNKREST; /* Impact of temporary rest on next crc rest */

            for (bitPos = CRC_JUNK; bitPos > 0U; bitPos--)
            {
                crcSave_u16 = crcTemp_u16 ;
                crcTemp_u16 <<= 1U;
                /* Length of rest = order of crc polynomial */
                crcTemp_u16 ^= (0U != (crcSave_u16 & CRC_16_BITMASK)) ? ((uint16)CRC_16_POLYNOMIAL) : 0U;
            }
        }
    }

    /* Removed XOR is added back*/
    Result = crcTemp_u16 ^ CRC_XOR_VALUE16;

    return Result;
}

/**
 **********************************************************************************************************************
 * \brief Calculates a CRC using the FCE engine of the IFX microcontroller for CRC8 Polynomial 0x1D
 *
 * \param    Config       configuration of the crc channel
 * \param    Data         holds a pointer to the start address of the data block to be calculated
 * \param    Length       length of data block to be calculated in bytes
 * \param    StartValue   start value when the algorithm starts
 *
 * \return   result of CRC calculation
 **********************************************************************************************************************/
CRC_INLINE uint32 Crc_Prv_CalcCRC8_Hw(uint32 Config, const Crc_DataType *Data, uint32 Length, uint32 StartValue)
{
    /* Declaration and Initialisation of Local variables */
    const uint8 *Dptr = Data->b8Ptr;
    uint32 Result = StartValue;
    uint32 i = 0;

    /* CRC8 calculation using Hardware */
    if(Length > 0U)
    {
        /* Required declarion for Reentrancy*/
        Crc_IfxRegType Backup;
        /* All supported FCE consolidated*/
        volatile Crc_IfxRegType* const IfxReg[] = {&CRC0, &CRC1, &CRC2, &CRC3, &CRC4, &CRC5, &CRC6, &CRC7};
        volatile Crc_IfxRegType* Context;

        /*For each core one FCE is allocated */
        Context = IfxReg[Crc_GetCoreId()];

        /* save register context for Reentrancy */
        Backup.CHECK = Context->CHECK;
        Backup.CRC = Context->CRC;
        Backup.CFG = Context->CFG;

        /* configure Registers of FCE, protected Registers are called with protection */
        Mcu_Rb_ResetEndInit();
        if(*(&CLC) != 0U)
        {
            *(&CLC) = 0U;
        }
        Context->CFG = CRC_HW_CFG8;
        Mcu_Rb_SetEndInit();

        /*Start value is updated to CRC register*/
        Context->CRC = Result;
        /*loop for Length times and input the data*/
        for(i=0U; i<Length; i++)
        {
            Context->IR = (uint8)Dptr[i];
        }

         Result = Context->RES;

         /* backedup register values Restored to FCE registers to ensure Reentrancy */
         Context->CRC = Backup.CRC;
         Context->CHECK = Backup.CHECK;
         Mcu_Rb_ResetEndInit();
         Context->CFG = Backup.CFG;
         Mcu_Rb_SetEndInit();
    }

    return Result;
}
#endif /* IFX 40nm code*/

#endif /* CRC_PRV_IFX_HW_H */
