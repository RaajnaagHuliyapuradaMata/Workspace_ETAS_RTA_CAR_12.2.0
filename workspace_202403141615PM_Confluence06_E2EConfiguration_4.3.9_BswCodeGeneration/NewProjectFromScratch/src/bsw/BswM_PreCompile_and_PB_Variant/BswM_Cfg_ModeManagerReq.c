



#include "BswM.h" // required for BswM_RequestMode

#if (BSWM_RTE_IN_USE == TRUE)
#include "Rte_BswM.h"
#include "SchM_BswM.h"
#endif

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"



/* Set of Immediate BswMBswModeNotification Functions (Called Entities on Mode Switch Event )*/
/* These functions are used for all Immediate Mode Notifications from BSW modules via SchM */

/***********************************************************
* Function name: void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_APP_RUN( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_APP_RUN
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECU_APP_RUN ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_APP_POST_RUN( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_APP_POST_RUN
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECU_APP_POST_RUN ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE__ ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_RUN( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_RUN
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECU_RUN ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_STARTUP_ONE( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_STARTUP_ONE
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECU_STARTUP_ONE ); /* mode */
    #endif
}
/***********************************************************
* Function name: void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_STARTUP_TWO( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_ImdtBswNotification_BswM_MRP_BswM_MDG_ECU_STARTUP_TWO
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_BSWM_MDG, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECU_STARTUP_TWO ); /* mode */
    #endif
}



/* Set of Deferred BswMBswModeNotification Functions (Called Entities on Mode Switch Event )*/
/* These functions are used for all deferred mode notifications from BSW modules via SchM */

/***********************************************************
* Function name: void BswM_Cfg_DfrdBswNotification_MRP_R_BswM_AR_InitBlockTwo_0_ECU_STARTUP_ONE( void )
* Description: Called Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_DfrdBswNotification_MRP_R_BswM_AR_InitBlockTwo_0_ECU_STARTUP_ONE
(
    void
)
{
    /* Call the generic request function */

    #if (defined(BSWM_SCHM_ENABLED) && (BSWM_SCHM_ENABLED == TRUE))

    BswM_RequestMode(    BSWM_CFG_USERID_MRP_R_BSWM_AR_INITBLOCKTWO_0, /* user */
                         RTE_MODE_MDG_ECUM_STATE_ECU_STARTUP_ONE ); /* mode */
    #endif
}









/* Set of Deferred BswMSwcModeNotification Functions (Runnable Entities on Mode Switch Event )*/
/* These functions are used for all deferred mode notifications from SWC modules via RTE */


/***********************************************************
* Function name: void BswM_Cfg_DfrdSwcNotification_BswM_MRP_SwcModeRequest_SHUT_DOWN( void )
* Description: Runnable Entity on Mode Switch Event.
* Parameter: None
* Return value: None
* Remarks:
***********************************************************/
void BswM_Cfg_DfrdSwcNotification_BswM_MRP_SwcModeRequest_SHUT_DOWN
(
    void
)
{
    /* Call the generic request function */
    BswM_RequestMode(    BSWM_CFG_USERID_BSWM_MRP_SWCMODEREQUEST, /* user */
                         RTE_MODE_MDG_ShutDown_SHUT_DOWN ); /* mode */
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"


