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

#ifndef RBA_FEEFS1X_PRV_BLOCKJOBTYPES_H
#define RBA_FEEFS1X_PRV_BLOCKJOBTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv_SearcherTypes.h"
#include "Std_Types.h"
#include "Fee_Rb_Types.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

#define RBA_FEEFS1X_BLOCKJOB_DATA_LENGTH_0 (0u)

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

typedef enum
{
    rba_FeeFs1x_BlockJob_ReturnType_pending_e,      // job is not yet finished
    rba_FeeFs1x_BlockJob_ReturnType_execute_e,      // exec action has to be perfomed
    rba_FeeFs1x_BlockJob_ReturnType_restart_e,      // restart from search is required
    rba_FeeFs1x_BlockJob_ReturnType_JobOK_e,        // data is verified and validated, job is finished
    rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e, // data is corrupted , job is finished
    rba_FeeFs1x_BlockJob_ReturnType_invalidated_e,  //consistent block copy is invalidated, job finished
    rba_FeeFs1x_BlockJob_ReturnType_error_e         // error during processing, job aborted
} rba_FeeFs1x_BlockJob_ReturnType_ten;


typedef enum
{
    rba_FeeFs1x_BlockJob_searchType_F1C_e = 0u,
    rba_FeeFs1x_BlockJob_searchType_F2C_e,
    rba_FeeFs1x_BlockJob_searchType_F1UC_e,
    rba_FeeFs1x_BlockJob_searchType_Max_e
}rba_FeeFs1x_BlockJob_searchType_ten;


// If editing, check the correct mapping of jobFunc_ten and jobFunctions_apfct
typedef enum
{
    // read functionality
    rba_FeeFs1x_BlockJob_jobFunc_checkRd_e = 0u,
    rba_FeeFs1x_BlockJob_jobFunc_execRd_e,

    // write functionality
    rba_FeeFs1x_BlockJob_jobFunc_checkWr_e,
    rba_FeeFs1x_BlockJob_jobFunc_execWr_e,

    rba_FeeFs1x_BlockJob_jobFunc_execWrRed_e,

    rba_FeeFs1x_BlockJob_jobFunc_execWrNoFB_e,
    rba_FeeFs1x_BlockJob_jobFunc_execWrNoFBRed_e,

    rba_FeeFs1x_BlockJob_jobFunc_execWrUnknownBlk_e,

    // invalidate functionality
    rba_FeeFs1x_BlockJob_jobFunc_checkInv_e,
    rba_FeeFs1x_BlockJob_jobFunc_execInv_e,

    rba_FeeFs1x_BlockJob_jobFunc_execInvRed_e,

    rba_FeeFs1x_BlockJob_jobFunc_execInvNoFB_e,
    rba_FeeFs1x_BlockJob_jobFunc_execInvNoFBRed_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_execInvUnknownBlk_e,

    // maintain functionality
    rba_FeeFs1x_BlockJob_jobFunc_checkMt_e,
    rba_FeeFs1x_BlockJob_jobFunc_execMt_e,

    rba_FeeFs1x_BlockJob_jobFunc_execMtNoFB_e,

    rba_FeeFs1x_BlockJob_jobFunc_Max_e
}rba_FeeFs1x_BlockJob_jobFunc_ten;

// If editing, check the correct mapping of jobDoFunc_ten and jobDoFunctions_apfct
typedef enum
{
    // read functionality
    rba_FeeFs1x_BlockJob_jobDoFunc_checkRdDo_e = 0u,
    rba_FeeFs1x_BlockJob_jobDoFunc_execRdDo_e,

    // write functionality
    rba_FeeFs1x_BlockJob_jobDoFunc_checkWrDo_e,
    rba_FeeFs1x_BlockJob_jobDoFunc_execWrDo_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_execWrRedDo_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_execWrNoFBDo_e,
    rba_FeeFs1x_BlockJob_jobDoFunc_execWrNoFBRedDo_e,

    rba_FeeFs1x_BlockJob_jobFunc_execWrUnknownBlkDo_e,

    // invalidate functionality
    rba_FeeFs1x_BlockJob_jobDoFunc_checkInvDo_e,
    rba_FeeFs1x_BlockJob_jobDoFunc_execInvDo_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_execInvRedDo_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_execInvNoFBDo_e,
    rba_FeeFs1x_BlockJob_jobDoFunc_execInvNoFBRedDo_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_execInvUnknownBlkDo_e,

    // maintain functionality
    rba_FeeFs1x_BlockJob_jobDoFunc_checkMtDo_e,
    rba_FeeFs1x_BlockJob_jobDoFunc_execMtDo_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_execMtNoFBDo_e,

    rba_FeeFs1x_BlockJob_jobDoFunc_Max_e
}rba_FeeFs1x_BlockJob_jobDoFunc_ten;

typedef enum
{
    rba_FeeFs1x_BlockJob_stm_search_e,
    rba_FeeFs1x_BlockJob_stm_checkReq_e,
    rba_FeeFs1x_BlockJob_stm_execJob_e,
    rba_FeeFs1x_BlockJob_stm_idle_e
}rba_FeeFs1x_BlockJob_stm_ten;

typedef void (*rba_FeeFs1x_BlockJob_stateFunctionPointer_tpfct)(rba_FeeFs1x_Searcher_RetVal_ten searchRetVal);


typedef rba_FeeFs1x_BlockJob_ReturnType_ten (*rba_FeeFs1x_BlockJob_stateDoFunctionPointer_tpfct)(void);


typedef struct
{
    uint8 const *                       userbuffer_pcu8;

    uint16                              feeIndex_u16;
    uint16                              persID_u16;
    uint16                              nrDataBytTot_u16;
    uint16                              nrDataBytCur_u16;
    uint16                              nrDataBytDone_u16;
    uint16                              statusByte_u16;

    rba_FeeFs1x_BlockJob_searchType_ten idxSearchType_en;
    Fee_Rb_JobMode_ten                  jobMode_en;
    rba_FeeFs1x_BlockJob_jobFunc_ten    idxCheckFunc_en;
    rba_FeeFs1x_BlockJob_jobDoFunc_ten  idxCheckDoFunc_en;
    rba_FeeFs1x_BlockJob_jobFunc_ten    idxExecFunc_en;
    rba_FeeFs1x_BlockJob_jobDoFunc_ten  idxExecDoFunc_en;
    rba_FeeFs1x_BlockJob_stm_ten        state_en;
    rba_FeeFs1x_Searcher_RetVal_ten     searchRetVal_en;

    boolean                             isUnknownBlk_b;
    boolean                             isChunkJob_b;
    boolean                             entry_b;
    uint8                               allowedRetries_u8;
}rba_FeeFs1x_BlockJobData_tst;

typedef enum
{
    rba_FeeFs1x_BlockJob_execWr_stm_idle_e,
    rba_FeeFs1x_BlockJob_execWr_stm_init_e,
    rba_FeeFs1x_BlockJob_execWr_stm_destroy_e,
    rba_FeeFs1x_BlockJob_execWr_stm_write_e,
    rba_FeeFs1x_BlockJob_execWr_stm_error_e
}rba_FeeFs1x_BlockJob_execWr_stm_ten;

typedef enum
{
    rba_FeeFs1x_BlockJob_execWr_type_onlyRedCpy_e,
    rba_FeeFs1x_BlockJob_execWr_type_red1Cpy_e,
    rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e
}rba_FeeFs1x_BlockJob_execWr_type_ten;


typedef enum
{
    rba_FeeFs1x_BlockJob_execWr_jobType_write,
    rba_FeeFs1x_BlockJob_execWr_jobType_invalidate,
    rba_FeeFs1x_BlockJob_execWr_jobType_maintain
}rba_FeeFs1x_BlockJob_execWr_jobType_ten;

typedef struct
{
    uint32 dataCRC_u32;  // only used for write jobs, not for invalidate, maintain
    rba_FeeFs1x_BlockJob_execWr_stm_ten state_en;
    rba_FeeFs1x_BlockJob_execWr_type_ten type_en;
    rba_FeeFs1x_BlockJob_execWr_jobType_ten jobType_en;
    boolean isCpyRed_b;
    boolean isNoFB_b; // only used for maintain
    boolean entry_b;
    boolean isCopy1Destroyed_b;
    boolean isCopy2Destroyed_b;
}rba_FeeFs1x_BlockJob_execWr_data_tst;

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

