/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/**************************************************************************************************/
/* Preprocessor includes                                                                          */
/**************************************************************************************************/

#include "EcuM.h"

#if (ECUM_CFG_MODE_HANDLING == STD_ON)

#include "EcuM_Prv.h"

/*************************************************************************************************
                            API's
*************************************************************************************************/
#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"

/***************************************************************************************************
 Function name    : EcuM_KillAllRUNRequests
 Syntax           : void EcuM_KillAllRUNRequests( void )
 Description      : This function unconditionally releases all pending requests to RUN.
 Parameter        : None
 Return value     : None
 ***************************************************************************************************/
void EcuM_KillAllRUNRequests( void )
{
    EcuM_UserType cntrLoop_u16;

    /* EcuM_KillAllRUNRequests has been called hence no further RUN request will be serviced */
    EcuM_Prv_KillAllRUNRequests_b = TRUE;

    /*Release all the RUN requests*/
    for(cntrLoop_u16 = 0;cntrLoop_u16 < ECUM_CFG_NUM_FLEX_USERS;cntrLoop_u16++)
    {
        EcuM_Prv_userRUNReqStatus_ab[cntrLoop_u16] = FALSE;
    }


}

/***************************************************************************************************
 Function name    : EcuM_KillAllPostRUNRequests
 Syntax           : void EcuM_KillAllPostRUNRequests( void )
 Description      : This function unconditionally releases all pending requests to PostRUN.
 Parameter        : None
 Return value     : None
 ***************************************************************************************************/
void EcuM_KillAllPostRUNRequests( void )
{
    EcuM_UserType cntrLoop_u16;

    /* EcuM_KillAllRUNRequests has been called hence no further POST RUN request will be serviced */
    EcuM_Prv_KillAllPostRUNRequests_b = TRUE;

    /*Release all the Post_RUN requests*/
    for(cntrLoop_u16 = 0;cntrLoop_u16 < ECUM_CFG_NUM_FLEX_USERS;cntrLoop_u16++)
    {
       EcuM_Prv_userPostRUNReqStatus_ab[cntrLoop_u16] = FALSE;
    }


}

/***************************************************************************************************
 Function name    : EcuM_Rb_KillAllRequests
 Syntax           : void EcuM_Rb_KillAllRequests( void )
 Description      : This function unconditionally releases all pending requests to RUN and PostRUN.
 Parameter        : None
 Return value     : None
 ***************************************************************************************************/
void EcuM_Rb_KillAllRequests( void )
{
    /*Kill All RUN Requests*/
    EcuM_KillAllRUNRequests();

    /*Kill All Post RUN Requests*/
    EcuM_KillAllPostRUNRequests();
}


#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"

#else /*ECUM_CFG_MODE_HANDLING*/
   /*This fuction is not compiled because mode handling parameter is not configured or feature is disabled */
#endif /*ECUM_CFG_MODE_HANDLING*/



 

