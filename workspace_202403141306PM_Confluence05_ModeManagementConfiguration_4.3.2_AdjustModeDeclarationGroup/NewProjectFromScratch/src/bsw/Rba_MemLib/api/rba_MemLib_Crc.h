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

#ifndef RBA_MEMLIB_CRC_H
#define RBA_MEMLIB_CRC_H

/*includes*/
#include "Std_Types.h"
#include "rba_MemLib_Cfg.h"


#if RBA_MEMLIB_CRC_ENABLE == STD_ON


typedef struct
{
  uint8 const * data_pcu8;
  uint32 nrBytesToCalc_u32;
  uint32 nrBytesMaxPerCall_u32;
  uint32 dataCrc_u32;
} rba_MemLib_Crc32_tst;



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
extern void rba_MemLib_InitCrc32(rba_MemLib_Crc32_tst * Crc_pst, uint8 const * data_pcu8, uint32 nrBytes_u32, uint32 nrBytesCall_u32);

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
extern void rba_MemLib_ContinueCrc32(rba_MemLib_Crc32_tst * Crc_pst, uint8 const * data_pcu8, uint32 nrBytes_u32);


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
boolean rba_MemLib_CalcCrc32(rba_MemLib_Crc32_tst * Crc_pst);

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
extern uint32 rba_MemLib_GetCrc32(rba_MemLib_Crc32_tst const * Crc_pst);


#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"

#endif


#endif /* RBA_MEMLIB_CRC_H */
