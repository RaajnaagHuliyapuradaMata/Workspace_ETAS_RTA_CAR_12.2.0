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
#include "Std_Types.h"

#include "Fee_Prv_Cfg.h"

#if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv_Cfg.h"

#include "rba_FeeFs1x_Prv_SectorEraseJob.h"
#include "rba_FeeFs1x_Prv_SectorEraseJobTypes.h"

#include "rba_FeeFs1x_Prv_Sector.h"
#include "Fee_Prv_Lib.h"
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

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_SectorEraseJob_erase_data_tst rba_FeeFs1x_SectorEraseJob_erase_data_st)

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"


static Std_ReturnType rba_FeeFs1x_SectorEraseJob_findSector2BeErased(uint8 * sector2BeErased_pu8);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_SectorEraseJob_eraseDo_findSector2BeErased(rba_FeeFs1x_SectorEraseJob_erase_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_SectorEraseJob_eraseDo_erase(rba_FeeFs1x_SectorEraseJob_erase_data_tst * fsm_pst);
/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_SectorEraseJob_cancel
 * initializes the filestatic variables
 * \seealso
 * \usedresources
 *********************************************************************
 */

void rba_FeeFs1x_SectorEraseJob_cancel(void)
{
    rba_FeeFs1x_SectorEraseJob_erase_data_st.state_en = rba_FeeFs1x_SectorEraseJob_erase_stm_idle_e;
    rba_FeeFs1x_SectorEraseJob_erase_data_st.idxSector2BeErased_u8 = RBA_FEEFS1X_MAXUINT8;
}
/**
 *********************************************************************
 * rba_FeeFs1x_SectorEraseJob_isEraseRequired
 * detect whether a sector needs to be erased
 * \return  erase required?
 * \retval  FALSE : not required
 * \retval  TRUE  : erase is required
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_SectorEraseJob_isEraseRequired(void)
{
    uint8 sector2BeErased_u8;
    boolean eraseRequired_b;

    eraseRequired_b = FALSE;

    if(E_OK == rba_FeeFs1x_SectorEraseJob_findSector2BeErased(&sector2BeErased_u8))
    {
        eraseRequired_b = TRUE;
    }

    return eraseRequired_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_SectorEraseJob_erase
 * inits the erase job
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_SectorEraseJob_erase(void)
{
    // first state is finding the sector to be erased
    rba_FeeFs1x_SectorEraseJob_erase_data_st.state_en = rba_FeeFs1x_SectorEraseJob_erase_stm_findSector2BeErased_e;
    rba_FeeFs1x_SectorEraseJob_erase_data_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_SectorEraseJob_eraseDo
 * jobDo function of erasejob
 * \return  rba_FeeFs1x_RetVal_ten:      is the erase successful?
 * \retval  rba_FeeFs1x_Pending_e:       erase ongoing
 * \retval  rba_FeeFs1x_JobOK_e:         erase successful
 * \retval  rba_FeeFs1x_ErrorInternal_e: erase failed, internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: external erase failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten  rba_FeeFs1x_SectorEraseJob_eraseDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // call sector_eraseDo if the request was accepted by sector
    switch (rba_FeeFs1x_SectorEraseJob_erase_data_st.state_en)
    {
        case rba_FeeFs1x_SectorEraseJob_erase_stm_findSector2BeErased_e:
        {
            retVal_en = rba_FeeFs1x_SectorEraseJob_eraseDo_findSector2BeErased(&rba_FeeFs1x_SectorEraseJob_erase_data_st);
        }break;
        case rba_FeeFs1x_SectorEraseJob_erase_stm_erase_e:
        {
            retVal_en = rba_FeeFs1x_SectorEraseJob_eraseDo_erase(&rba_FeeFs1x_SectorEraseJob_erase_data_st);

        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(rba_FeeFs1x_Pending_e != retVal_en)
    {
        rba_FeeFs1x_SectorEraseJob_erase_data_st.state_en = rba_FeeFs1x_SectorEraseJob_erase_stm_idle_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * state function findSector2BeErased
 *
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e:   erase ongoing
 * \retval  rba_FeeFs1x_JobOK_e:     erase successful
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_SectorEraseJob_eraseDo_findSector2BeErased(rba_FeeFs1x_SectorEraseJob_erase_data_tst * fsm_pst)
{
    uint8 sector2BeErased_u8;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(E_OK == rba_FeeFs1x_SectorEraseJob_findSector2BeErased(&sector2BeErased_u8))
    {
        // a sector to be erased was found
        fsm_pst->idxSector2BeErased_u8 = sector2BeErased_u8;
        // erase the found sector
        FEE_PRV_LIBSC(rba_FeeFs1x_SectorEraseJob_erase_stm_erase_e);
        // stay pending
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // no sector to be erased was found -> leave eraseDo with Job OK
        retVal_en = rba_FeeFs1x_JobOK_e;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * state function erase
 *
 * \return  rba_FeeFs1x_RetVal_ten:  is the erase successful?
 * \retval  rba_FeeFs1x_Pending_e:   erase ongoing
 * \retval  rba_FeeFs1x_JobOK_e:     erase successful
 * \retval  rba_FeeFs1x_ErrorInternal_e: erase failed, internal error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_SectorEraseJob_eraseDo_erase(rba_FeeFs1x_SectorEraseJob_erase_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retValInner_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_Sector_erase(fsm_pst->idxSector2BeErased_u8, FALSE);
    }

    /* Cyclic Do function call */
    retValInner_en = rba_FeeFs1x_Sector_eraseDo();

    switch(retValInner_en)
    {
        case rba_FeeFs1x_JobOK_e:
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValInner_en;
        }break;
        // if sector erase fails, it is retried infinite times. No external error is reported
        // so error external must not occur and is mapped to default
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_SectorEraseJob_findSector2BeErased
 * detect whether a sector needs to be erased and return it if required
 * \param   sector2BeErased_pu8 : if required, this parameter returns the sector index of the sector to be erased
 * \return  erase required?
 * \retval  E_NOT_OK : not required
 * \retval  E_OK  : erase is required
 * \seealso
 * \usedresources
 *********************************************************************
 */
static Std_ReturnType rba_FeeFs1x_SectorEraseJob_findSector2BeErased(uint8 * sector2BeErased_pu8)
{
    uint8 idxSector_u8;
    Std_ReturnType  retVal_en;
    rba_FeeFs1x_Sector_SectorState_ten sectorState_en;

    // search as long as no sector to be erased was found
    retVal_en = E_NOT_OK;
    for(
            idxSector_u8 = 0;
            (idxSector_u8 < RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE) && (E_OK != retVal_en);
            idxSector_u8++)
    {
        sectorState_en = rba_FeeFs1x_Sector_getState(idxSector_u8);

        // only, in case of sector state is UNDEF or R2E
        if(     (rba_FeeFs1x_Sector_SectorState_UNDEF_e == sectorState_en) ||
                (rba_FeeFs1x_Sector_SectorState_R2E_e == sectorState_en)   )
        {
            // the first sector to be erased in logical sector order is found
            // stop the loop and return the found sector
            retVal_en = E_OK;
            *sector2BeErased_pu8 = idxSector_u8;
        }
    }

    return retVal_en;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

