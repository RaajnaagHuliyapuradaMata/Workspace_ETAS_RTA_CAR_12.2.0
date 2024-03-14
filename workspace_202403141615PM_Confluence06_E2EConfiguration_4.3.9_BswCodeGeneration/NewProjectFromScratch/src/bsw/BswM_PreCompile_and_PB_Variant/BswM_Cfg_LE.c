

/**********************************************************************************************************************
 *  Include files                                                                                                    
 **********************************************************************************************************************/

#include "BswM.h"
#include "BswM_Prv.h"

/**********************************************************************************************************************
 *  Definition of Global Functions                                                                                                    
 **********************************************************************************************************************/





/********************************  LogicalExpressionEvaluateFunctions  ***************************************/
#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"


/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_GoDown (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_GoDown  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_GODOWN)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_GODOWN)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}

/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_InitBlockReadAllComplete (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_InitBlockReadAllComplete  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_INITBLOCKREADALLCOMPLETE)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_INITBLOCKREADALLCOMPLETE)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}

/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_InitBlockTwo (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_InitBlockTwo  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_INITBLOCKTWO)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_INITBLOCKTWO)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}

/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_Run (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_Run  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_RUN)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_RUN)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}

/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_Shutdown (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_Shutdown  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_SHUTDOWN)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_SHUTDOWN)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}

/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_StartCom (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_StartCom  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_STARTCOM)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_STARTCOM)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}

/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_StartupOne (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_StartupOne  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_STARTUPONE)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_STARTUPONE)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}

/*****************************************************************************************
* Function name :   void BswM_Cfg_LE_BswM_LE_SwcModeRequest (boolean *validMode_pb,boolean *evalResult_pb)
* Description   :   Evaluates the logical expression if the mode value is initialized and returns the result  
* Parameter     :   *validMode_pb: evaluates if all the modes are valid and assigns true to this address if valid, 
                    *evalResult_pb: result of the logical expression is copied to this address.
* Return value  :   void
* Remarks       :   
*****************************************************************************************/
void BswM_Cfg_LE_BswM_LE_SwcModeRequest  
(
    boolean * isValidMode_pb, 
    boolean * hasLogExpRes_pb
)
{
    /* Initialize the pointers with default values */ 
    *isValidMode_pb = FALSE;
    *hasLogExpRes_pb = FALSE;
    
    if(FALSE != BSWMMODEVALUE_BSWM_LE_SWCMODEREQUEST)
    {
        /* All the mode condition values are valid, assign TRUE to pointer */
        *isValidMode_pb = TRUE;
        if(FALSE != BSWMLOGEXP_BSWM_LE_SWCMODEREQUEST)
        {
            /* Logical Expression evaluated to TRUE */
            *hasLogExpRes_pb = TRUE;
        }
        
    }
    
    return;
}


#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"






/**********************************************************************************************************************
 *                                                                                                        
 **********************************************************************************************************************/
