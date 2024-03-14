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

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))
#include "Fee_Prv_Lib.h"
#include "Fee_Prv_FsIf.h"
#include "Crc.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

/* Array which weights the actions. Since this variable is constant for all instances. it is ok to keep this variable
 * locally in this unit and not move it to the Fee_Prv_Cfg unit. */
#define FEE_START_SEC_CONST_8
#include "Fee_MemMap.h"
uint8 const Fee_Prv_LibEffortWeights_au8[FEE_PRV_LIMIT_MAX_E] =
{
    0,  /* FEE_PRV_LIMIT_CRCINRAM_CPYRAM_E */
    1,  /* FEE_PRV_LIMIT_CRCINFLS_E        */
    5,  /* FEE_PRV_LIMIT_HDR_E             */
    10  /* FEE_PRV_LIMIT_CACHEREORG_E      */
};
#define FEE_STOP_SEC_CONST_8
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Static declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/**
 * \brief   Calculate the CRC from Ram buffer.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   bfr_pu8          Pointer to Ram buffer
 * \param   length_u32       Amount of bytes to consider
 * \param   crc_pu32         Pointer to an uint32 where the result is placed
 * \param   firstCall_b      Is this the first call for a chain of CRC calculations or not
 *
*/
void Fee_Prv_LibCalcCrcRam(Fee_Rb_DeviceName_ten deviceName_en, uint8 * bfr_pu8, uint32 length_u32, uint32 * crc_pu32, boolean firstCall_b)
{
    Fee_Prv_ConfigDeviceTable_tst const *   deviceConfigTable_pcst;
    Fee_Prv_LibCalcCrcRam_tst *             libCalcCrcRam_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst  = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    libCalcCrcRam_pst       = &deviceConfigTable_pcst->feeData_pst->libData_st.libCalcCrcRam_st;

    libCalcCrcRam_pst->bfr_pu8          = bfr_pu8;
    libCalcCrcRam_pst->nrBytesDone_u32  = 0u;
    libCalcCrcRam_pst->length_u32       = length_u32;
    libCalcCrcRam_pst->crc_pu32         = crc_pu32;
    libCalcCrcRam_pst->firstCall_b      = firstCall_b;

    return;
}

/**
 * \brief   Do function for the CRC calculation from Ram buffer.
 *
 * \param   deviceName_en       Device (or Instance) for which the job has to be performed
 * \return                      Job status
 * \retval  MEMIF_JOB_PENDING:  Job pending
 * \retval  MEMIF_JOB_OK:       CRC calculation done
*/
MemIf_JobResultType Fee_Prv_LibCalcCrcRamDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const *   deviceConfigTable_pcst;
    Fee_Prv_LibCalcCrcRam_tst *             libCalcCrcRam_pst;
    MemIf_JobResultType                     result_en;
    uint32                                  nrBytesToConsider_u32;
    uint32                                  nrBytesAllowedToProcess_u32;

    result_en = MEMIF_JOB_PENDING;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst  = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    libCalcCrcRam_pst       = &deviceConfigTable_pcst->feeData_pst->libData_st.libCalcCrcRam_st;

    nrBytesToConsider_u32 = (libCalcCrcRam_pst->length_u32 - libCalcCrcRam_pst->nrBytesDone_u32);

    /* Adapt data size for CRC calculation to the available Fee time budget.                                      */
    /* In case of a large data block (max. 64kB) which fits into the Medium buffer, the CRC calculation has to be */
    /* distributed over multiple Fee main calls.                                                                  */
    nrBytesAllowedToProcess_u32 = Fee_Prv_LibEffortGet(deviceName_en, FEE_PRV_LIMIT_CRCINRAM_CPYRAM_E, nrBytesToConsider_u32);

    if(nrBytesAllowedToProcess_u32 != nrBytesToConsider_u32)
    {
        /* Align to the next lower page border */
        nrBytesAllowedToProcess_u32 = Fee_Prv_LibFloor(nrBytesAllowedToProcess_u32, Fee_Prv_FsIfGetLogicalPageSize(deviceName_en));
    }

    if(0u < nrBytesAllowedToProcess_u32)
    {
        *libCalcCrcRam_pst->crc_pu32 = Crc_CalculateCRC32(  libCalcCrcRam_pst->bfr_pu8 + libCalcCrcRam_pst->nrBytesDone_u32,
                                                            nrBytesAllowedToProcess_u32,
                                                            *libCalcCrcRam_pst->crc_pu32,
                                                            libCalcCrcRam_pst->firstCall_b);

        libCalcCrcRam_pst->nrBytesDone_u32 += nrBytesAllowedToProcess_u32;
        libCalcCrcRam_pst->firstCall_b      = FALSE;
    }

    /* Length should never be smaller, but to be safe consider it */
    if(libCalcCrcRam_pst->length_u32 <= libCalcCrcRam_pst->nrBytesDone_u32)
    {
        result_en = MEMIF_JOB_OK;
    }

    return(result_en);
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"
#endif
