/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
/* PDU Router Configuratio ID  */

#include "PduR_Prv.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

PduR_PBConfigIdType PduR_GetConfigurationId( void )
{
    PduR_PBConfigIdType return_val;
#if defined(PDUR_ZERO_COST_OPERATION) && (PDUR_ZERO_COST_OPERATION!= STD_OFF)
    return_val = (PduR_PBConfigIdType)0;
#else
    return_val=  PduR_dGetConfigurationId();
#endif /* PDUR_ZERO_COST_OPERATION */
    return return_val;
}


PduR_PBConfigIdType PduR_dGetConfigurationId( void )
{
    /* Instance Id of 0u is passed while reporting PDUR_E_NULL_POINTER during calling of DetReportError*/
    PDUR_CHECK_STATE_RET(PDUR_INSTANCE_ID, PDUR_SID_GETCFGID, 0 )
    return PduR_iGetConfigurationId();
}

/* ------------------------------------------------------------------------ */
/* End section for code */



/* ------------------------------------------------------------------------ */
/* Begin section for code */



/**
 **************************************************************************************************
 * PduR_iGetConfigurationId
 *        This function returns the unique identifier of the post-build time configuration of the PDU Router
 *
 * \param           None
 * \retval          unique identifier of the post-build time configuration
 *
 * \seealso         PDUR242, PDUR280
 * \usedresources
 **************************************************************************************************
 */
PduR_PBConfigIdType PduR_iGetConfigurationId( void )
{
    PduR_PBConfigIdType return_val;
#if(PDUR_CONFIGURATION_VARIANT != PDUR_VARIANT_PRE_COMPILE)
    return_val = PduR_Base->configId; /* configuration id */
#else
    return_val = PDUR_CONFIGURATION_ID;
#endif

    return return_val;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

