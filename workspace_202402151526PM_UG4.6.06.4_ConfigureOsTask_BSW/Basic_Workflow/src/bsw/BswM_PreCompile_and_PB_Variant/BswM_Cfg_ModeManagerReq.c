



#include "BswM.h" // required for BswM_RequestMode

#if (BSWM_RTE_IN_USE == TRUE)
#include "Rte_BswM.h"
#include "SchM_BswM.h"
#endif

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"






/* Set of Deferred BswMBswModeNotification Functions (Called Entities on Mode Switch Event )*/
/* These functions are used for all deferred mode notifications from BSW modules via SchM */

/***********************************************************
* Function name: void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_APP_RUN( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_APP_RUN
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_APP_RUN ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_APP_POST_RUN( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_APP_POST_RUN
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_APP_POST_RUN ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_PREP_SHUTDOWN( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_PREP_SHUTDOWN
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_PREP_SHUTDOWN ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_RUN( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_DfrdBswNotification_BswM_MRP_BswM_MDG_ECUM_STATE_RUN
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_RUN ); /* mode */
    #endif
}










#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"


