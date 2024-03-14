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

#ifndef RBA_FEEFS1X_PRV_PAMAP_H
#define RBA_FEEFS1X_PRV_PAMAP_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

# if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv.h"
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "rba_FeeFs1x_Prv_Sector.h"
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

typedef struct
{
    uint32 logSectStartAddress_u32;
    uint32 actualAddress_u32;
    uint8  logSectIdx_u8;
}rba_FeeFs1x_PAMap_passAddrRange_tst;

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

LOCAL_INLINE uint8 rba_FeeFs1x_PAMap_getLogIdxByPagePrechecked(uint32 page_u32);

LOCAL_INLINE void    rba_FeeFs1x_PAMap_passAddrRange_init(rba_FeeFs1x_PAMap_passAddrRange_tst * passAddrRange_pst, uint32 page_u32);
LOCAL_INLINE boolean rba_FeeFs1x_PAMap_passAddrRange_decrementAddress(rba_FeeFs1x_PAMap_passAddrRange_tst * passAddrRange_pst);
LOCAL_INLINE uint32  rba_FeeFs1x_PAMap_passAddrRange_getPage(rba_FeeFs1x_PAMap_passAddrRange_tst const * passAddrRange_pst);
LOCAL_INLINE uint32  rba_FeeFs1x_PAMap_passAddrRange_getAddress(rba_FeeFs1x_PAMap_passAddrRange_tst const * passAddrRange_pst);

extern void                     rba_FeeFs1x_PAMap_fillSectorEnd(void);
extern rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_PAMap_fillSectorEndDo(void);

extern Std_ReturnType           rba_FeeFs1x_PAMap_getLogIdxByPage(uint32 page_u32 , uint8 * logSectIdx_pu8);
extern uint32                   rba_FeeFs1x_PAMap_getAddressByPage(uint32 page_u32);

extern uint32                   rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(uint32 startPage_u32, uint32 ofs_u32 ,
                uint32 requestedNBytes_u32);

extern Std_ReturnType           rba_FeeFs1x_PAMap_init(void);
extern void                     rba_FeeFs1x_PAMap_cancel(void);

extern boolean                  rba_FeeFs1x_PAMap_isPageSmallerWrAdr(uint32 page_u32);

extern Std_ReturnType           rba_FeeFs1x_PAMap_reduceOwnPages(uint32 amountOfPages_u32);

extern void                     rba_FeeFs1x_PAMap_prepareSectorStates(uint32 nextBlockStartPage_u32);
extern Std_ReturnType           rba_FeeFs1x_PAMap_prepareSectorStatesOnCurrWrPage(void);

extern rba_FeeFs1x_RetVal_ten   rba_FeeFs1x_PAMap_prepareSectorStatesDo(void);

extern void                     rba_FeeFs1x_PAMap_setCurrWrPage(uint32 page_u32);
extern uint32                   rba_FeeFs1x_PAMap_getCurrWrPage(void);

extern uint32                   rba_FeeFs1x_PAMap_getLastPageNrOfChunk(uint32 chunkStartPage_u32 , uint8 offsetInStartPage_u8 , uint16 nrBytesInChunk_u16);



/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_getLogIdxByPagePrechecked
 * returns in which log. sector a page is located in
 * This function doesn't perform safety checks, the inboundness has to be prechecked by the caller
 *
 * \param   page_u32 : page to be checked for it's logical sector index
 * \return  index of the sector the requested page is located in
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE uint8 rba_FeeFs1x_PAMap_getLogIdxByPagePrechecked(uint32 page_u32)
{
    const uint32 pagesPerSect_u32 = RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR;

    return (uint8)(page_u32 / pagesPerSect_u32);

}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_passAddrRange_init
 * initializes the structure for performing address based cache build up
 * calculates the start address of the range to be gone through out of given page number
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   passAddrRange_pst: pointer to working data
 * \param   page_u32         : page where address based backward scan shall be started
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_PAMap_passAddrRange_init(rba_FeeFs1x_PAMap_passAddrRange_tst * passAddrRange_pst, uint32 page_u32)
{
    // calculate the scan start address from the given page number
    passAddrRange_pst->logSectIdx_u8 = rba_FeeFs1x_PAMap_getLogIdxByPagePrechecked(page_u32);
    passAddrRange_pst->logSectStartAddress_u32 = rba_FeeFs1x_Sector_getLogStartAddressPrechecked(passAddrRange_pst->logSectIdx_u8);
    passAddrRange_pst->actualAddress_u32 = ((page_u32 % RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR) * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE) + passAddrRange_pst->logSectStartAddress_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_passAddrRange_decrementAddress
 * decrements the address until complete address range is gone through
 * sector boundary passing is considered
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   passAddrRange_pst: pointer to working data
 * \return  gone through the complete address range?
 * \retval  FALSE: not gone through the complete address range
 * \retval  TRUE: gone through the complete address range
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_PAMap_passAddrRange_decrementAddress(rba_FeeFs1x_PAMap_passAddrRange_tst * passAddrRange_pst)
{
    uint32 addrOffset_u32;

    boolean retVal_b = FALSE;
    // sector limit reached
    if(passAddrRange_pst->actualAddress_u32 <= passAddrRange_pst->logSectStartAddress_u32)
    {
        if(passAddrRange_pst->logSectIdx_u8 > 0)
        {
            passAddrRange_pst->logSectIdx_u8 --;
            passAddrRange_pst->logSectStartAddress_u32 = rba_FeeFs1x_Sector_getLogStartAddressPrechecked(passAddrRange_pst->logSectIdx_u8);

            // block header is never surpassing the sector boundary.
            // Scan starts at the next possible page where a block header may be located

            // address offset: address of the first page of the previously scanned sector -
            // address range required by a block header
            addrOffset_u32 = (RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR - RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR)
                    * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;

            passAddrRange_pst->actualAddress_u32 = passAddrRange_pst->logSectStartAddress_u32 + addrOffset_u32;
        }
        else
        {
            // all pages are scanned
            retVal_b = TRUE;
        }
    }
    else
    {
        passAddrRange_pst->actualAddress_u32 -= RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
    }

    return retVal_b;
}

/**
*********************************************************************
* rba_FeeFs1x_PAMap_passAddrRange_getAddress
* get the address to be scanned
*
* \param   passAddrRange_pst : pointer to working data
* \return  address to be scanned
* \seealso
* \usedresources
*********************************************************************
*/
LOCAL_INLINE uint32  rba_FeeFs1x_PAMap_passAddrRange_getAddress(rba_FeeFs1x_PAMap_passAddrRange_tst const * passAddrRange_pst)
{
    return passAddrRange_pst->actualAddress_u32;
}

/**
*********************************************************************
* rba_FeeFs1x_PAMap_passAddrRange_getPage
* get the page nuber of the actual scanned address
*
* \param   passAddrRange_pst : pointer to working data
* \return  page number of the actual scanne address
* \seealso
* \usedresources
*********************************************************************
*/
LOCAL_INLINE uint32 rba_FeeFs1x_PAMap_passAddrRange_getPage(rba_FeeFs1x_PAMap_passAddrRange_tst const * passAddrRange_pst)
{
    uint32 addrOffs_u32;
    uint32 pageOffs_u32;

    addrOffs_u32 = passAddrRange_pst->actualAddress_u32 - passAddrRange_pst->logSectStartAddress_u32;
    pageOffs_u32 = addrOffs_u32 / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
    return((passAddrRange_pst->logSectIdx_u8 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR) + pageOffs_u32);
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

# endif /* (defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED)) */

#endif

