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
#include "rba_FeeFs1x.h"

#include "rba_FeeFs1x_Prv.h"
/* includes for unit addressLayer */
#include "rba_FeeFs1x_Prv_Sector.h"
#include "rba_FeeFs1x_Prv_PAMap.h"
#include "rba_FeeFs1x_Prv_PARead.h"
#include "rba_FeeFs1x_Prv_PAWrite.h"
#include "rba_FeeFs1x_Prv_PASrv.h"
/* includes for unit virtualPaging */
#include "rba_FeeFs1x_Prv_CacheKwn.h"
#include "rba_FeeFs1x_Prv_CacheUnkwn.h"
#include "rba_FeeFs1x_Prv_BC.h"
#include "rba_FeeFs1x_Prv_Searcher.h"
/* includes for unit BlockJobs */
#include "rba_FeeFs1x_Prv_Rb_MaintainJob.h"
#include "rba_FeeFs1x_Prv_InvalidateJob.h"
#include "rba_FeeFs1x_Prv_WriteJob.h"
#include "rba_FeeFs1x_Prv_ReadJob.h"
#include "rba_FeeFs1x_Prv_BlockJob.h"
/* includes for unit SectorJobs */
#include "rba_FeeFs1x_Prv_BackgroundJob.h"
#include "rba_FeeFs1x_Prv_TriggerReorgJob.h"
#include "rba_FeeFs1x_Prv_SectorEraseJob.h"
#include "rba_FeeFs1x_Prv_SectorReorg.h"
/* includes for unit Utils */
#include "rba_FeeFs1x_Prv_Utils.h"


#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Init_addressLayer_init(void);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Init_virtualPaging_init(void);
LOCAL_INLINE void rba_FeeFs1x_Init_BlockJob_init(void);
LOCAL_INLINE void rba_FeeFs1x_Init_SectorJobs_init(void);
LOCAL_INLINE void rba_FeeFs1x_CancelJob_addressLayer(void);
LOCAL_INLINE void rba_FeeFs1x_CancelJob_virtualPaging(void);
LOCAL_INLINE void rba_FeeFs1x_CancelJob_blockJob(void);
LOCAL_INLINE void rba_FeeFs1x_CancelJob_sectorJobs(void);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/**
 *********************************************************************
 *
 * Initialisation of FeeFs1x / run in synchronous mode
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \return  initialisation status of Fee_Fs1x
 * \retval  E_OK     : initialisation successful
 * \retval  E_NOT_OK : initialisation failed
 * \seealso
 * \usedresources
 *********************************************************************
 */

Std_ReturnType rba_FeeFs1x_Init(Fee_Rb_DeviceName_ten deviceName_en)
{
    Std_ReturnType retval_addressLayer;
    Std_ReturnType retval_virtualPaging;
    Std_ReturnType retval;

    /* unused parameters */
    (void)deviceName_en;

    /* initialisation of unit group rba_FeeFs1x_addressLayer */
    retval_addressLayer = rba_FeeFs1x_Init_addressLayer_init();

    if(E_OK == retval_addressLayer)
    {
        /* initialisation of unit rba_FeeFs1x_virtualPaging */
        retval_virtualPaging = rba_FeeFs1x_Init_virtualPaging_init();
    }
    else
    {
        retval_virtualPaging = E_NOT_OK;
    }

    /* initialisation of unit rba_FeeFs1x_BlockJob */
    rba_FeeFs1x_Init_BlockJob_init();

    /* initialisation of unit rba_FeeFs1x_SectorJobs */
    rba_FeeFs1x_Init_SectorJobs_init();

    if(E_OK != retval_virtualPaging)
    {
        retval = E_NOT_OK;
    }
    else
    {
        retval = E_OK;
    }

    return retval;
}

/**
 *********************************************************************
 *
 * Initialisation of unit group adressLayer
 *
 * \param   void
 * \return  initialisation status of unit group rba_FeeFs1x_addressLayer
 * \retval  E_OK     : initialisation successful
 * \retval  E_NOT_OK : initialisation failed
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Init_addressLayer_init(void)
{
    Std_ReturnType retval_PAMap_en;
    Std_ReturnType retval_Sector_en;
    Std_ReturnType retval_en;

    /* initialisation of unit rba_FeeFs1x_Sector */
    retval_Sector_en = rba_FeeFs1x_Sector_init();

    /* initialisation of unit rba_FeeFs1x_PASrv */
    rba_FeeFs1x_PASrv_init();

    /* initialisation of unit rba_FeeFs1x_PAMap */
    retval_PAMap_en = rba_FeeFs1x_PAMap_init();

    /* initialisation of unit rba_FeeFs1x_PARead */
    rba_FeeFs1x_PARead_init();

    /* initialisation of unit rba_FeeFs1x_PAWrite */
    rba_FeeFs1x_PAWrite_init();

    if((E_OK != retval_Sector_en) || (E_OK != retval_PAMap_en))
    {
        retval_en = E_NOT_OK;
    }
    else
    {
        retval_en = E_OK;
    }

    return retval_en;
}

/**
 *********************************************************************
 *
 * Initialisation of unit group virtualPaging
 *
 * \param   void
 * \return  initialisation status of unit group rba_FeeFs1x_virtualPaging
 * \retval  E_OK     : initialisation successful
 * \retval  E_NOT_OK : initialisation failed
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Init_virtualPaging_init(void)
{
    Std_ReturnType retval_en;

    /* initialisation of unit rba_FeeFs1x_BlockCopy */
    rba_FeeFs1x_BC_init();

    /* initialisation of Cache units*/
    rba_FeeFs1x_CacheKwn_init();
    rba_FeeFs1x_CacheUnkwn_init();

    /* initialisation of unit rba_FeeFs1x_Searcher */
    retval_en =rba_FeeFs1x_Searcher_init();

    return retval_en;
}

/**
 *********************************************************************
 *
 * Initialisation of unit group BlockJob
 *
 * \param   void
 * \return  initialisation status of unit group rba_FeeFs1x_BlockJob
 * \retval  void
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_Init_BlockJob_init(void)
{
    /* initialisation of unit rba_FeeFs1x_Maintain */
    rba_FeeFs1x_MtJob_init();

    /* initialisation of unit rba_FeeFs1x_Invalidate */
    rba_FeeFs1x_InvJob_init();

    /* initialisation of unit rba_FeeFs1x_Read */
    rba_FeeFs1x_RdJob_init();

    /* initialisation of unit rba_FeeFs1x_Write */
    rba_FeeFs1x_WrJob_init();

    /* initialisation of unit rba_FeeFs1x_BlockCopy */
    rba_FeeFs1x_BlockJob_init();
}


/**
 *********************************************************************
 *
 * \param   void
 * \return  initialisation status of unit group SectorJobs
 * \retval  void
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_Init_SectorJobs_init(void)
{
    /* initialisation of unit rba_FeeFs1x_SectorEraseJob */
    rba_FeeFs1x_SectorEraseJob_init();

    /* initialisation of unit rba_FeeFs1x_SectorReorg */
    rba_FeeFs1x_Reorg_init();

    /* initialisation of unit rba_FeeFs1x_BackgroundJob */
    rba_FeeFs1x_BackgroundJob_init();

    /* initialisation of unit rba_FeeFs1x_TriggerReorgJob */
    rba_FeeFs1x_TriggerReorgJob_init();
}

# if(defined(RBA_FEEFS1X_PRV_CFG_CANCEL) && (TRUE == RBA_FEEFS1X_PRV_CFG_CANCEL))
/**
 *********************************************************************
 *
 * Cancel any ongoing job and reset the state machines to start with the new job.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_CancelJob(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* unused parameters */
    (void)deviceName_en;

    /* Cancel unit group rba_FeeFs1x_addressLayer */
    rba_FeeFs1x_CancelJob_addressLayer();

    /* Cancel unit rba_FeeFs1x_virtualPaging */
    rba_FeeFs1x_CancelJob_virtualPaging();

    /* Cancel unit rba_FeeFs1x_BlockJob */
    rba_FeeFs1x_CancelJob_blockJob();

    /* Cancel unit rba_FeeFs1x_SectorJobs */
    rba_FeeFs1x_CancelJob_sectorJobs();

}

/**
 *********************************************************************
 *
 * cancel unit group adressLayer
 *
 * \param   void
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_CancelJob_addressLayer(void)
{
    /* cancel unit rba_FeeFs1x_Sector */
    rba_FeeFs1x_Sector_cancel();

    /* cancel unit rba_FeeFs1x_PASrv */
    rba_FeeFs1x_PASrv_cancel();

    /* cancel unit rba_FeeFs1x_PAMap */
    rba_FeeFs1x_PAMap_cancel();

    /* cancel unit rba_FeeFs1x_PARead */
    rba_FeeFs1x_PARead_cancel();

    /* cancel unit rba_FeeFs1x_PAWrite */
    rba_FeeFs1x_PAWrite_cancel();
}

/**
 *********************************************************************
 *
 * Cancel unit group virtualPaging
 *
 * \param   void
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_CancelJob_virtualPaging(void)
{
    /* initialisation of unit rba_FeeFs1x_BlockCopy */
    rba_FeeFs1x_BC_cancel();

    /* initialisation of Cache units*/
    rba_FeeFs1x_CacheKwn_cancel();
    rba_FeeFs1x_CacheUnkwn_cancel();

    /* initialisation of unit rba_FeeFs1x_Searcher */
    rba_FeeFs1x_Searcher_cancel();
}

/**
 *********************************************************************
 *
 * cancel unit group BlockJob
 *
 * \param   void
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_CancelJob_blockJob(void)
{
    /* initialisation of unit rba_FeeFs1x_Maintain */
    rba_FeeFs1x_MtJob_cancel();

    /* initialisation of unit rba_FeeFs1x_Invalidate */
    rba_FeeFs1x_InvJob_cancel();

    /* initialisation of unit rba_FeeFs1x_Read */
    rba_FeeFs1x_RdJob_cancel();

    /* initialisation of unit rba_FeeFs1x_Write */
    rba_FeeFs1x_WrJob_cancel();

    /* initialisation of unit rba_FeeFs1x_BlockCopy */
    rba_FeeFs1x_BlockJob_cancel();
}


/**
 *********************************************************************
 *
 * \param   void
 * \retval  void
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_CancelJob_sectorJobs(void)
{
    /* initialisation of unit rba_FeeFs1x_SectorEraseJob */
    rba_FeeFs1x_SectorEraseJob_cancel();

    /* initialisation of unit rba_FeeFs1x_SectorReorg */
    rba_FeeFs1x_Reorg_cancel();

    /* initialisation of unit rba_FeeFs1x_BackgroundJob */
    rba_FeeFs1x_BackgroundJob_cancel();

    /* initialisation of unit rba_FeeFs1x_TriggerReorgJob */
    rba_FeeFs1x_TriggerReorgJob_cancel();
}
# endif
#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

