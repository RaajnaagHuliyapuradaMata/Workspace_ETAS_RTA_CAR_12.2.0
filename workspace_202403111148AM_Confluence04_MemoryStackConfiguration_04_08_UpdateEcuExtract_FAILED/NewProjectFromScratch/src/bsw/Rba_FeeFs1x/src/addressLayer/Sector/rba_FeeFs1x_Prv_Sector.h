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

#ifndef RBA_FEEFS1X_PRV_SECTOR_H
#define RBA_FEEFS1X_PRV_SECTOR_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

# if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "MemIf_Types.h"
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "rba_FeeFs1x_Prv.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#define RBA_FEEFS1X_SECTOR_LOG_SIZE  ( (RBA_FEEFS1X_PRV_CFG_PHYS_SEC_END0 - (RBA_FEEFS1X_PRV_CFG_PHYS_SEC_START0 + RBA_FEEFS1X_PRV_SECTOR_HEADER_SIZE)) + 1uL )


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

typedef enum
{
    rba_FeeFs1x_Sector_SectorState_UNDEF_e,
    rba_FeeFs1x_Sector_SectorState_ERASED_e,
    rba_FeeFs1x_Sector_SectorState_USED_e,
    rba_FeeFs1x_Sector_SectorState_FULL_e,
    rba_FeeFs1x_Sector_SectorState_R2E_e,
    rba_FeeFs1x_Sector_SectorState_CLONE_e
} rba_FeeFs1x_Sector_SectorState_ten;

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

extern Std_ReturnType           rba_FeeFs1x_Sector_init(void);
extern void                     rba_FeeFs1x_Sector_cancel(void);

extern uint8                    rba_FeeFs1x_Sector_getActiveSector(void);
extern Std_ReturnType           rba_FeeFs1x_Sector_getLogStartAddress(uint8 logSectIdx_u8, uint32* logStartAddress_pu32);
extern uint32                   rba_FeeFs1x_Sector_getLogStartPage(uint8 logSectIdx_u8);
extern uint32                   rba_FeeFs1x_Sector_getLogEndPage(uint8 logSectIdx_u8);
extern boolean                  rba_FeeFs1x_Sector_isMemFull(uint32 lastPageOfNewBlock_u32);

extern uint8                    rba_FeeFs1x_Sector_getPhysIdxByLogIdx(uint8 logSectIdx_u8);
extern uint8                    rba_FeeFs1x_Sector_getLogIdxByPhysIdx(uint8 idxPhysSect_u8);
extern uint32                   rba_FeeFs1x_Sector_getSectCngCnt(uint8 idxLogSector_u8);
extern rba_FeeFs1x_Sector_SectorState_ten rba_FeeFs1x_Sector_getState(uint8 logSectIdx_u8);
extern Std_ReturnType           rba_FeeFs1x_Sector_getStartMarkerContentAsPageNo(uint8 logSectIdx_u8 , uint32* page_pu32);

extern uint32                   rba_FeeFs1x_Sector_getLogStartAddressPrechecked(uint8 logSectIdx_u8);

extern void                     rba_FeeFs1x_Sector_erase(uint8 logSectIdx_u8, boolean isReerasing_b);
extern rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Sector_eraseDo(void);

extern void                     rba_FeeFs1x_Sector_markUsed(uint8 logSectIdx_u8);
extern void                     rba_FeeFs1x_Sector_markFull(uint8 logSectIdx_u8);
extern void                     rba_FeeFs1x_Sector_markStart(uint8 logSectIdx_u8, uint32 addrOfFirstBlk_u32);
extern void                     rba_FeeFs1x_Sector_markReady4Erase(uint8 logSectIdx_u8);
extern void                     rba_FeeFs1x_Sector_markClone(uint8 logSectIdx_u8);

extern rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_Sector_markSectorDo(void);

extern void                     rba_FeeFs1x_Sector_reorderLogSect0(void);
extern void                     rba_FeeFs1x_Sector_switchLogSectors(uint8 idxLogSect1_u8, uint8 idxLogSect2_u8);

extern uint32                   rba_FeeFs1x_Sector_getSectChngCnt(void);
extern uint32                   rba_FeeFs1x_Sector_getNrOfLogPagesPerSector(void);
extern uint8                    rba_FeeFs1x_Sector_getTotalNrOfSectors(void);

extern boolean                  rba_FeeFs1x_Sector_isSectorEraseOngoing(void);

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif /* (defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED)) */

#endif

