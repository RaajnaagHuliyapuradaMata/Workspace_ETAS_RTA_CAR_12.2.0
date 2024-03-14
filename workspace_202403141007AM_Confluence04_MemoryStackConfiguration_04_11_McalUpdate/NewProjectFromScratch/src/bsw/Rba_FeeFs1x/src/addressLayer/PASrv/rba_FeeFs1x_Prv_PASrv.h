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

#ifndef RBA_FEEFS1X_PRV_PASRV_H
#define RBA_FEEFS1X_PRV_PASRV_H
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
    rba_FeeFs1x_PASrv_BlankCheckResult_fullyProgrammed_e,
    rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e,
    rba_FeeFs1x_PASrv_BlankCheckResult_pending_e,
    rba_FeeFs1x_PASrv_BlankCheckResult_blank_e,
    rba_FeeFs1x_PASrv_BlankCheckResult_error_e
}rba_FeeFs1x_PASrv_BlankCheckResult_ten;

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

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern uint32 rba_FeeFs1x_PASrv_getCurrWritePage(void);

extern void rba_FeeFs1x_PASrv_checkPageFullyProgrammed(uint32 page_u32);
extern rba_FeeFs1x_PASrv_BlankCheckResult_ten rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo(void);

extern void rba_FeeFs1x_PASrv_blankcheckPage(uint32 page_u32);
extern rba_FeeFs1x_PASrv_BlankCheckResult_ten rba_FeeFs1x_PASrv_blankcheckPageDo(void);

extern void rba_FeeFs1x_PASrv_blankcheckArea(uint32 startPage_u32, uint32 noOfPages_u32);
extern rba_FeeFs1x_PASrv_BlankCheckResult_ten rba_FeeFs1x_PASrv_blankcheckAreaDo(void);

extern void rba_FeeFs1x_PASrv_calcCRC32(uint32 startPage_u32, uint16 len_u16, uint32* crc_pu32, uint8 startOfs_u8);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_calcCRC32Do(void);

extern void rba_FeeFs1x_PASrv_compare(uint32 startPage_u32, uint8 startOffset_u8, uint16 nrBytes_u16, uint8 const * userbuffer_pcu8);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareDo(void);

extern void rba_FeeFs1x_PASrv_compareFls2Fls(uint32 copy1_PageNr_u32,
        uint32 copy2_PageNr_u32, uint8 startOfs_u8 , uint16 nrBytes_u16);
extern rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareFls2FlsDo(void);

extern void rba_FeeFs1x_PASrv_cancel(void);
LOCAL_INLINE void rba_FeeFs1x_PASrv_init(void);
LOCAL_INLINE void rba_FeeFs1x_PASrv_init(void)
{
    rba_FeeFs1x_PASrv_cancel();
}

# if (FALSE != RBA_FEEFS1X_PRV_IS_MULTI_SECTOR_EMULATION)
extern boolean rba_FeeFs1x_PASrv_isBlkOverlappingToNextSector(uint32 firstPageOfBlk_u32, uint32 lastPageOfBlk_u32);
# else
// for 2 sector emulation, there could never be an overlap block
LOCAL_INLINE boolean rba_FeeFs1x_PASrv_isBlkOverlappingToNextSector(uint32 firstPageOfBlk_u32, uint32 lastPageOfBlk_u32);
LOCAL_INLINE boolean rba_FeeFs1x_PASrv_isBlkOverlappingToNextSector(uint32 firstPageOfBlk_u32, uint32 lastPageOfBlk_u32)
{
    (void)firstPageOfBlk_u32;
    (void)lastPageOfBlk_u32;
    return(FALSE);
}
# endif

# if (RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR > 1u)
extern boolean rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(uint32 nrPageCurrExtract_u32);
# else
// Complete header fits into one logical page, so the check should return FALSE i.e., second header page will never lead to overlapping into next sector
LOCAL_INLINE boolean rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(uint32 nrPageCurrExtract_u32);
LOCAL_INLINE boolean rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(uint32 nrPageCurrExtract_u32)
{
    (void) nrPageCurrExtract_u32;
    return (FALSE);
}
# endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"


#endif

