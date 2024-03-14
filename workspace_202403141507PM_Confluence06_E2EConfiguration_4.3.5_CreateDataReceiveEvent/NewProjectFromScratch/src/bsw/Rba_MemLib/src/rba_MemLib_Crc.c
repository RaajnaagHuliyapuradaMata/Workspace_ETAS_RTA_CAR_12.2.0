/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "rba_MemLib_Crc.h"

#if RBA_MEMLIB_CRC_ENABLE == STD_ON
#include "Crc.h"

#include "rba_MemLib_Inl.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"


/**
 **********************************************************************************************************************
 * rba_MemLib_InitCrc32
 *
 * \brief initializes the structure rba_MemLib_Crc32_tst to start a new Crc calculation
 *
 * \param    Crc_pst               Pointer to the structure rba_MemLib_Crc32_tst
 * \param    data_pcu8
 * \param    nrBytes_u32           Total number of bytes for algorithm to be calculated
 * \param    nrBytesCall_u32       maximum number of bytes for which algorith can be calculated per each call of
 *                                 Crc_CalculateCRC32()
 * \return   none
 **********************************************************************************************************************
 **/
void rba_MemLib_InitCrc32(rba_MemLib_Crc32_tst * Crc_pst, uint8 const * data_pcu8, uint32 nrBytes_u32, uint32 nrBytesCall_u32)
{
    if(Crc_pst != NULL_PTR)
    {
        Crc_pst->data_pcu8 = data_pcu8;
        Crc_pst->nrBytesToCalc_u32 = nrBytes_u32;
        Crc_pst->nrBytesMaxPerCall_u32 = nrBytesCall_u32;
        /* initialize in a way that the result corresponds to the result if checksum is calulated with 0 size!
         * by this no initial call of the Crc32 routine is needed! */
        Crc_pst->dataCrc_u32 = 0u;
    }
    return;
}

/**
 **********************************************************************************************************************
 * rba_MemLib_CalcCrc32
 *
 * \brief
 * wrapper fuction for Crc_CalculateCRC32 which Calculates the remainder of a polynomial division
 * Crc_CalculateCRC32 calculates the remainder of a polynomial division by the CRC polynomial.
 * The data stream is transmitted via pointer *Crc_pst
 *
 * \param    *Crc_pst              Pointer to data stream for Crc calculation
 *
 * \return   boolean   Function returns
 *                                  1.TRUE : Crc calculation done
 *                                  2.FALSE: Crc calculation is on-going
 **********************************************************************************************************************
 **/
boolean rba_MemLib_CalcCrc32(rba_MemLib_Crc32_tst * Crc_pst)
{
    boolean retVal = (boolean)TRUE;
    if(Crc_pst != NULL_PTR)
    {
        uint32 Length;
        /* the number of bytes for per run needs to be >=1*/
        Crc_pst->nrBytesMaxPerCall_u32 = rba_MemLib_Max(Crc_pst->nrBytesMaxPerCall_u32, 1u);

        /*calculate number of byte to be processed if the number of bytes is greater than the bytes per call*/
        if ( Crc_pst->nrBytesToCalc_u32 > Crc_pst->nrBytesMaxPerCall_u32)
        {
            Length = Crc_pst->nrBytesMaxPerCall_u32;
            /*calculate the number of remaining bytes */
            Crc_pst->nrBytesToCalc_u32 = Crc_pst->nrBytesToCalc_u32 - Crc_pst->nrBytesMaxPerCall_u32;
            retVal = (boolean)FALSE;
        }
        else
        {
            /*if the length of number of bytes for Crc calculation is less than max number of bypes per call*/
            Length = Crc_pst->nrBytesToCalc_u32;
            Crc_pst->nrBytesToCalc_u32 = 0u;
        }

        /* calculate next chunk of CRC*/
        /* Note: length=0 is a valid parameter length, effectively leaving the dataCrc unchanged */
        Crc_pst->dataCrc_u32 = Crc_CalculateCRC32(Crc_pst->data_pcu8, Length, Crc_pst->dataCrc_u32, (boolean)FALSE);
        Crc_pst->data_pcu8 += Length;
    }

    return retVal;
}

/**
 **********************************************************************************************************************
 * rba_MemLib_ContinueCrc32
 *
 * \brief fuction to get the remaining data to continue a on-the-fly Crc calculation of junks of data
 * Note: it is possible to call this even if the previous calculation is not finished
 *
 * \param    * Crc_pst              Pointer to data stream for Crc calculation
 * \param    * data_pcu8            Pointer to start address of data block
 * \param    nrBytes_u32            Remaining bytes for Crc calculation
 *
 * \return   none
 **********************************************************************************************************************
 **/
void rba_MemLib_ContinueCrc32(rba_MemLib_Crc32_tst * Crc_pst, uint8 const * data_pcu8, uint32 nrBytes_u32)
{
    /* update crc calculation parameter no matter of current setting */
    Crc_pst->data_pcu8         = data_pcu8;
    Crc_pst->nrBytesToCalc_u32 = nrBytes_u32;

    return;
}


/**
 **********************************************************************************************************************
 * rba_MemLib_GetCrc32
 *
 * \brief fuction to get the calculated Crc
 *
 * \param    * Crc_pst              Pointer to data stream for Crc calculation
 * \return   dataCrc_u32            calculated crc
 **********************************************************************************************************************
 **/
uint32 rba_MemLib_GetCrc32(rba_MemLib_Crc32_tst const * Crc_pst)
{
    return (Crc_pst->dataCrc_u32);
}


#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"

#endif
