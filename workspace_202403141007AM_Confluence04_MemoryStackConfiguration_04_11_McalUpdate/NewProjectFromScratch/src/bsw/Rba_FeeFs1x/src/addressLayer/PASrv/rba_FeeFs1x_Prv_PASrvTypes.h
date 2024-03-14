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

#ifndef RBA_FEEFS1X_PRV_PASRVTYPES_H
#define RBA_FEEFS1X_PRV_PASRVTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv.h"

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

typedef enum
{
    rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo_blankcheck_e,
    rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo_idle_e

}rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoStm_ten;




typedef struct
{
    rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoStm_ten state_en;
}rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoData_tst;

typedef enum
{
    rba_FeeFs1x_PASrv_blankcheckPageDo_blankcheck_e,
    rba_FeeFs1x_PASrv_blankcheckPageDo_idle_e

}rba_FeeFs1x_PASrv_blankcheckPageDoStm_ten;

typedef struct
{

    rba_FeeFs1x_PASrv_blankcheckPageDoStm_ten state_en;
}rba_FeeFs1x_PASrv_blankcheckPageDoData_tst;


typedef enum
{
    rba_FeeFs1x_PASrv_calcCRC32_stm_idle_e,
    rba_FeeFs1x_PASrv_calcCRC32_stm_limit_e,
    rba_FeeFs1x_PASrv_calcCRC32_stm_calc_e
}rba_FeeFs1x_PASrv_calcCRC32_stm_ten;
typedef struct
{
    uint32 startPage_u32;

    uint32 nextChunkPage_u32;

    uint32 remainingNBytes_u32;

    uint32 * crc_pu32;

    uint16 length_u16;
    uint16 currentChunkLen_u16;

    rba_FeeFs1x_PASrv_calcCRC32_stm_ten state_en;
    uint8 startOfs_u8;
    boolean entry_b;

}rba_FeeFs1x_PASrv_calcCRC32_data_tst;

typedef enum
{
    rba_FeeFs1x_PASrv_compare_stm_idle_e,
    rba_FeeFs1x_PASrv_compare_stm_limit_e,
    rba_FeeFs1x_PASrv_compare_stm_compare_e
}rba_FeeFs1x_PASrv_compare_stm_ten;

typedef struct
{
    uint32 startPage_u32;
    uint8 startOffset_u8;
    uint16 length_u16;
    uint8 const * userbuffer_pcu8;

    uint32 nrBytesRemaining_u32;
    uint32 nrBytesDone_u32;

    uint32 nextChunkPage_u32;
    uint8  nextChunkOffset_u8;

    uint32 currChunkAdr_u32;
    uint32 currChunkNrBytes_u32;
    uint8  currChunkOffset_u8;

    rba_FeeFs1x_PASrv_compare_stm_ten state_en;
    boolean entry_b;
}rba_FeeFs1x_PASrv_compare_data_tst;

typedef enum
{
    rba_FeeFs1x_PASrv_compareFls2Fls_stm_idle_e,
    rba_FeeFs1x_PASrv_compareFls2Fls_stm_compare_e
}rba_FeeFs1x_PASrv_compareFls2Fls_stm_ten;

typedef struct
{
    rba_FeeFs1x_PASrv_compareFls2Fls_stm_ten state_en;
    boolean entry_b;

    uint16 nrBytesRemaining_u16;
    uint16 nrBytesCurrChunk_u16;

    uint32 cpy1_nextChunkPage_u32;
    uint32 cpy2_nextChunkPage_u32;

    uint8 nextByteOfsInPage_u8;

}rba_FeeFs1x_PASrv_compareFls2Fls_data_tst;

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


#endif

