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

#ifndef RBA_FEEFS1X_PRV_WRITEJOBTYPES_H
#define RBA_FEEFS1X_PRV_WRITEJOBTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"

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
    rba_FeeFs1x_WrJob_checkWr_stm_idle_e,
    rba_FeeFs1x_WrJob_checkWr_stm_calcCRC_e,
    rba_FeeFs1x_WrJob_checkWr_stm_compToConfig_e,
    rba_FeeFs1x_WrJob_checkWr_stm_compToDFLASH_e
}rba_FeeFs1x_WrJob_checkWr_stm_ten;


typedef struct
{
    uint8 const *   userbuffer_pcu8;
    uint32          dataCRC_u32;
    uint16          feeIndex_u16;
    uint16          persID_u16;
    uint16          blkLen_u16;
    uint16          statusByte_u16;
    boolean         isUnknownBlk_b;
}rba_FeeFs1x_WrJob_JobData_tst;


typedef struct
{
    uint32 calcedBuffCRC_u32;
    rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en;

    rba_FeeFs1x_WrJob_checkWr_stm_ten state_en;
    boolean entry_b;

}rba_FeeFs1x_WrJob_checkWr_data_tst;

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



#endif /* RBA_FEEFS1X_PRV_WRITEJOBTYPES_H */

