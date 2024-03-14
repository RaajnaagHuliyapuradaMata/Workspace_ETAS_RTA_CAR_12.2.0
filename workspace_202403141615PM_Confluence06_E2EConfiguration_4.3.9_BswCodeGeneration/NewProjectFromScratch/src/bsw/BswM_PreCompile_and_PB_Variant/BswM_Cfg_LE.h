
#ifndef BSWM_CFG_LE_H
#define BSWM_CFG_LE_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"


            #include "Mcu.h"
        
            #include "CanIf.h"
        
            #include "Can.h"
        
            #include "Rte_Main.h"
        
            #include "CanSM.h"
        
            #include "NvM.h"
        
            #include "CanTp.h"
        
            #include "Fim.h"
        
            #include "WdgM.h"
        
            #include "Fls.h"
        
            #include "Fee.h"
        
            #include "Nvm_MultiBlockCallback.h"
        
            #include "Xcp.h"
        
            #include "Dcm.h"
        
            #include "Det.h"
        
    



/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/



/******************************     BswM Logical Expression    *****************************************/

#define BSWMLOGEXP_BSWM_LE_GODOWN  \
                                         (  ( RTE_MODE_null_null  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )  \
                                         &&  ( NVM_REQ_PENDING  !=  BswM_Cfg_NvMJobModeInfo_ast[BSWM_IDX_BSWM_MRP_NVMWRITEALLCOMPLETE].dataMode_en ) )
        
#define BSWMLOGEXP_BSWM_LE_INITBLOCKREADALLCOMPLETE  \
                                         (  ( RTE_MODE_MDG_ECUM_STATE_ECU_STARTUP_TWO  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )  \
                                         &&  ( NVM_REQ_PENDING  !=  BswM_Cfg_NvMJobModeInfo_ast[BSWM_IDX_BSWM_MRP_NVMREADALLCOMPLETE].dataMode_en ) )
        
#define BSWMLOGEXP_BSWM_LE_INITBLOCKTWO  \
                                         ( RTE_MODE_MDG_ECUM_STATE_ECU_STARTUP_ONE  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )
        
#define BSWMLOGEXP_BSWM_LE_RUN  \
                                         (  ( RTE_MODE_MDG_ECUM_STATE_ECU_APP_RUN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )  \
                                         &&  ( 1  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_APPLICATIONREQUESTPORT].dataMode_u16 ) )
        
#define BSWMLOGEXP_BSWM_LE_SHUTDOWN  \
                                         ( RTE_MODE_MDG_ECUM_STATE_ECU_APP_POST_RUN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )
        
#define BSWMLOGEXP_BSWM_LE_STARTCOM  \
                                         ( RTE_MODE_MDG_ECUM_STATE_ECU_RUN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )
        
#define BSWMLOGEXP_BSWM_LE_STARTUPONE  \
                                         ( RTE_MODE_MDG_ECUM_STATE_ECU_STARTUP_ONE  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_MRP_R_BSWM_AR_INITBLOCKTWO_0].dataMode_u16 )
        
#define BSWMLOGEXP_BSWM_LE_SWCMODEREQUEST  \
                                         ( RTE_MODE_MDG_ShutDown_SHUT_DOWN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_SWCMODEREQUEST].dataMode_u16 )
        



/******************   Macros for checking whether the ModeValues are defined   ******************************/

#define BSWMMODEVALUE_BSWM_LE_GODOWN  \
                                      ( ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )  \
                                       && ( FALSE  !=  BswM_Cfg_NvMJobModeInfo_ast[BSWM_IDX_BSWM_MRP_NVMWRITEALLCOMPLETE].isValidModePresent_b ) )
        
#define BSWMMODEVALUE_BSWM_LE_INITBLOCKREADALLCOMPLETE  \
                                      ( ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )  \
                                       && ( FALSE  !=  BswM_Cfg_NvMJobModeInfo_ast[BSWM_IDX_BSWM_MRP_NVMREADALLCOMPLETE].isValidModePresent_b ) )
        
#define BSWMMODEVALUE_BSWM_LE_INITBLOCKTWO  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )
        
#define BSWMMODEVALUE_BSWM_LE_RUN  \
                                      ( ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )  \
                                       && ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_APPLICATIONREQUESTPORT].isValidModePresent_b ) )
        
#define BSWMMODEVALUE_BSWM_LE_SHUTDOWN  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )
        
#define BSWMMODEVALUE_BSWM_LE_STARTCOM  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )
        
#define BSWMMODEVALUE_BSWM_LE_STARTUPONE  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_MRP_R_BSWM_AR_INITBLOCKTWO_0].isValidModePresent_b )
        
#define BSWMMODEVALUE_BSWM_LE_SWCMODEREQUEST  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_SWCMODEREQUEST].isValidModePresent_b )
        


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

/********************************  LogicalExpressionEvaluateFunctions  ***************************************/
#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

extern void BswM_Cfg_LE_BswM_LE_GoDown  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_InitBlockReadAllComplete  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_InitBlockTwo  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_Run  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_Shutdown  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_StartCom  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_StartupOne  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_SwcModeRequest  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);


#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"






#endif  /* BSWM_CFG_LE_H */
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
