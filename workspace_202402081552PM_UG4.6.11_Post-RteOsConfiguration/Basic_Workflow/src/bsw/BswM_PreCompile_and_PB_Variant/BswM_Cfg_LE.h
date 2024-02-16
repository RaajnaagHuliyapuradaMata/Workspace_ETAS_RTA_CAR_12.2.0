
#ifndef BSWM_CFG_LE_H
#define BSWM_CFG_LE_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"


            #include "CanSM_ComM.h"
        
            #include "Mcu.h"
        
            #include "CanIf.h"
        
            #include "Can.h"
        
            #include "Rte_Main.h"
        
    



/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/



/******************************     BswM Logical Expression    *****************************************/

#define BSWMLOGEXP_BSWM_LE_GODOWN  \
                                         ( RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_PREP_SHUTDOWN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )
        
#define BSWMLOGEXP_BSWM_LE_INITBLOCKREADALLCOMPLETE  \
                                         ( 1  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_REQUESTFULLCOM].dataMode_u16 )
        
#define BSWMLOGEXP_BSWM_LE_RUN  \
                                         (  ( 1  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_APPLICATIONREQUESTPORT].dataMode_u16 )  \
                                         &&  ( RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_APP_RUN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 ) )
        
#define BSWMLOGEXP_BSWM_LE_SHUTDOWN  \
                                         (  ( RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_APP_POST_RUN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )  \
                                         &&  ( COMM_NO_COMMUNICATION  ==  BswM_Cfg_ComMIndicationModeInfo_ast[BSWM_IDX_BSWM_MRP_COMM_CAN0].dataMode_u8 ) )
        
#define BSWMLOGEXP_BSWM_LE_STARTCOM  \
                                         ( RTE_MODE_MDG_ECUM_STATE_ECUM_STATE_RUN  ==  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].dataMode_u16 )
        



/******************   Macros for checking whether the ModeValues are defined   ******************************/

#define BSWMMODEVALUE_BSWM_LE_GODOWN  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )
        
#define BSWMMODEVALUE_BSWM_LE_INITBLOCKREADALLCOMPLETE  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_REQUESTFULLCOM].isValidModePresent_b )
        
#define BSWMMODEVALUE_BSWM_LE_RUN  \
                                      ( ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_APPLICATIONREQUESTPORT].isValidModePresent_b )  \
                                       && ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b ) )
        
#define BSWMMODEVALUE_BSWM_LE_SHUTDOWN  \
                                      ( ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )  \
                                       && ( FALSE  !=  BswM_Cfg_ComMIndicationModeInfo_ast[BSWM_IDX_BSWM_MRP_COMM_CAN0].isValidModePresent_b ) )
        
#define BSWMMODEVALUE_BSWM_LE_STARTCOM  \
                                      ( FALSE  !=  BswM_Cfg_GenericReqModeInfo_ast[BSWM_IDX_BSWM_MRP_BSWM_MDG].isValidModePresent_b )
        


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
extern void BswM_Cfg_LE_BswM_LE_Run  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_Shutdown  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);
extern void BswM_Cfg_LE_BswM_LE_StartCom  (boolean * isValidMode_pb, boolean * hasLogExpRes_pb);


#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"






#endif  /* BSWM_CFG_LE_H */
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
