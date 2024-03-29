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
#include "PduR_Prv.h"
#include "Dem.h"           /* For Dem_SetEventStatus */
#if(PDUR_ENABLE_INTER_MODULE_CHECKS)
#if (!defined(DEM_AR_RELEASE_MAJOR_VERSION) || \
     (DEM_AR_RELEASE_MAJOR_VERSION != PDUR_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DEM_AR_RELEASE_MINOR_VERSION) || \
     (DEM_AR_RELEASE_MINOR_VERSION != PDUR_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif
#endif /* #if(PDUR_ENABLE_INTER_MODULE_CHECKS) */
#if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
#include "rba_BswSrv.h"
#endif
#include "PduR_BufferPool.h"
#include "PduR_Gw.h"


/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

void PduR_Init( const PduR_ConfigType * ConfigPtr )
{
#if(PDUR_ZERO_COST_OPERATION == STD_OFF)
    PduR_dInit(ConfigPtr);
#endif
}

/**
 **************************************************************************************************
 * PduR_dInit - PDU Router initialization
 *
 * \param           const PduR_PBConfigType * ConfigPtr
 *                  Pointer to post build configuration
 *
 * \retval          None
 * \seealso
 * \usedresources   PDUR108 
 **************************************************************************************************
 */

void PduR_dInit( const PduR_ConfigType * ConfigPtr )
{
    /* PDUR108:In case this function is called in any state other than PDUR_UNINIT, the
     * request shall be ignored and the error PDUR_E_INVALID_REQUEST shall be
     * reported to DET if development error detection is enabled.*/
    if (PduR_State != PDUR_UNINIT)
    {
        PDUR_REPORT_ERROR(PDUR_INSTANCE_ID, PDUR_SID_INIT,PDUR_E_INVALID_REQUEST);
    }
    else
    {
#if(PDUR_CONFIGURATION_VARIANT != PDUR_VARIANT_PRE_COMPILE)
        if( ConfigPtr == NULL_PTR )
        {
            /* PDUR222: ConfigPtr of initialization function PduR_Init() shall not be NULL.
             * Otherwise PDUR_E_CONFIG_PTR_INVALID shall be reported to DET.*/
            /* Since PduR_Init is called by both BSWM or EcuM, so while calling Det_ReportError to report
             * PDUR_E_CONFIG_PTR_INVALID, 0u is passed as the instance id */
            PDUR_REPORT_ERROR(PDUR_INSTANCE_ID, PDUR_SID_INIT,PDUR_E_CONFIG_PTR_INVALID);
        }
        else
        {
            /* PDU Router initialization */
            PduR_iInit( ConfigPtr );
        }
#else
        PduR_iInit( ConfigPtr );
#endif
    }
}


/* ------------------------------------------------------------------------ */
/* End section for code */



/* ------------------------------------------------------------------------ */
/* Begin section for code */



#if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
/**
***************************************************************************************************
* PduR_CompareVersionInfo - compare version info structures ie of type Std_VersionInfoType
*
* compare version info structures ie of type Std_VersionInfoType.
*
* \param   srcVersionInfo      pointer to first Std_VersionInfoType structure
* \param   destVersionInfo     pointer to second Std_VersionInfoType structure
* \return  stEqual         result of comparison
* \retval  0                   memory area contents are equal
* \retval  1                   memory area contents differ
  ***************************************************************************************************
*/

LOCAL_INLINE uint8 PduR_CompareVersionInfo(const Std_VersionInfoType * srcVersionInfo,
        const Std_VersionInfoType * destVersionInfo)
{
    uint8 stEqual = 1;

    if (
            ((srcVersionInfo->vendorID) == (destVersionInfo->vendorID)) &&
            ((srcVersionInfo->moduleID) == (destVersionInfo->moduleID)) &&
            ((srcVersionInfo->sw_major_version) == (destVersionInfo->sw_major_version)) &&
            ((srcVersionInfo->sw_minor_version) == (destVersionInfo->sw_minor_version)) &&
            ((srcVersionInfo->sw_patch_version) == (destVersionInfo->sw_patch_version))
    )
    {
        stEqual = 0;
    }

    return stEqual;

}
#endif  /* #if(PDUR_CONFIGURATION_VARIANT != PDUR_VARIANT_PRE_COMPILE) */

/**
 **************************************************************************************************
 * PduR_iInit - PDU Router initialization
 *
 * Note of deviation from the spec:
 * PDUR108 states that PduR_Init() should initialize the PDU Router state according to PDUR260,
 * PDUR254 and PDUR257. However, since it is impossible to call PduR_Init() more than once, the
 * behaviour in the spec is functionally equivalent to performing the relevant initialisation in
 * static initialisers in the generated code. Furthermore, the latter saves on memory. So that is
 * what we will do.
 *
 * \param           const PduR_PBConfigType * ConfigPtr
 *                  Pointer to post build configuration
 *
 * \retval          None
 * \seealso
 * \usedresources   PDUR108, PDUR106 , PDUR203 
 **************************************************************************************************
 */


void PduR_iInit( const PduR_ConfigType * ConfigPtr)
{
#if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)
    uint16 index;
#endif
#if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
    Std_VersionInfoType versionInfo;
    Std_VersionInfoType ModuleversionInfo;
    uint8 flag = 0;
#endif

    if (PduR_State == PDUR_UNINIT)
    {
        /**************  DEM report Good Condition  **************************/

        /*PDUR_E_PDU_INSTANCE_LOST: A PASSED condition shall be called during initialization */
            #if defined(PDUR_DEM_PDU_INSTANCE_LOST) && (PDUR_DEM_PDU_INSTANCE_LOST != 0)
            /*BSW modules calling DemSetEventStatus can safely ignore the return value*/
            (void)Dem_SetEventStatus( PDUR_E_PDU_INSTANCE_LOST, DEM_EVENT_STATUS_PASSED );
            #if(PDUR_DSM_REINT_SUPPORT != STD_OFF)
            PduR_DsmReInitStruct.instanceLostErrorStatus =  DEM_EVENT_STATUS_PASSED;
            PduR_DsmReInitStruct.instanceLostErrorFlag = FALSE;
            #endif
            #endif /* PDUR_DEM_PDU_INSTANCE_LOST */

        /*****************************************************************/

#if(PDUR_CONFIGURATION_VARIANT != PDUR_VARIANT_PRE_COMPILE)
        if( ConfigPtr == NULL_PTR ) /* Check for NULL pointer */
        {
            #if defined(PDUR_DEM_INIT_FAILED) && (PDUR_DEM_INIT_FAILED != 0)
            /*BSW modules calling DemSetEventStatus can safely ignore the return value*/
            (void)Dem_SetEventStatus( PDUR_E_INIT_FAILED, DEM_EVENT_STATUS_FAILED );
            #if(PDUR_DSM_REINT_SUPPORT != STD_OFF)
            PduR_DsmReInitStruct.initFailedErrorStatus =  DEM_EVENT_STATUS_FAILED;
            #endif
            #endif /* PDUR_DEM_INIT_FAILED */
        }
        else
#endif
        {
            PduR_State = PDUR_REDUCED;

            /**************  DEM report Good Condition  **************************/
            #if defined(PDUR_DEM_INIT_FAILED) && (PDUR_DEM_INIT_FAILED != 0)
            /*BSW modules calling DemSetEventStatus can safely ignore the return value*/
            (void)Dem_SetEventStatus( PDUR_E_INIT_FAILED, DEM_EVENT_STATUS_PASSED );
            #if(PDUR_DSM_REINT_SUPPORT != STD_OFF)
            PduR_DsmReInitStruct.initFailedErrorStatus =  DEM_EVENT_STATUS_PASSED;
            PduR_DsmReInitStruct.initFailedErrorFlag = FALSE;
            #endif
            #endif /* PDUR_DEM_INIT_FAILED */

            /*****************************************************************/
            /* initialize the PDU Router module */
#if(PDUR_CONFIGURATION_VARIANT != PDUR_VARIANT_PRE_COMPILE)
            /* MR12 RULE 11.5 VIOLATION : 'pointer-to-void' is being cast to a 'pointer-to-object'
             * type and this conversion will not result in any misalignment. Hence Supressed.*/
            PduR_Base = (const PduR_PBConfigType * )ConfigPtr->PduR_PBConfigDataPtr;
#if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
            /* MR12 RULE 11.5 VIOLATION: Cast from a pointer to void to a pointer to object type is required to be in
             * sync with the target (PduR_Base) type. Hence this warning can be safely suppressed */
            PduR_FunctionPtrs = (const PduR_RPTablesType * )PduR_Base->PduR_RPTablesPtr;

            PduR_GetVersionInfo(&versionInfo);
            ModuleversionInfo = (*ConfigPtr->PduR_VersionInfoPtr);
            flag = PduR_CompareVersionInfo(&versionInfo, &ModuleversionInfo);
            if (flag != 0)
            {
                /* versionInfo doesnt match .. Inform application*/
                PduRAppl_IncompatibleGenerator();
                return;
            }
#endif
#else
            (void)ConfigPtr;
            PduR_Base = &PduR_GlobalPBConfig;
#endif
            /* disable multicast initialisation if no multicast support */
            #if (PDUR_DISABLE_MULTICAST_SUPPORT == 0)
            if (PDUR_LOTP_TX_STATE_BASE != NULL_PTR)
            {
                PduIdType i;
                for(i=0; i < PDUR_NR_VALID_IDS(loTpTxToUpMc); i++)
                {
                    PduR_iInit_multicast(PDUR_LOTP_TX_STATE_BASE + i);
                }
            }
            #endif

            /*If Buffer pools are configured, then initialize all the buffers inside the pools*/
            #if defined(PDUR_BUFFERPOOL_SUPPORT)
            PduR_BufferPoolInit(&PduR_BufferPool_Config);
            #endif

            /* Initialize the array used to maintain RPG Status, with the value of PduRIsEnabledAtInit for each RPG */
            #if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)
            for(index = 0; index <= (PduR_Base->rpg_NrEntries) ; index++)
            {
                PduR_Base->pduR_Rpg_Status[index] = (PduR_Base->rpg_EnRoutingInfo[index]);
            }
            #endif


            /*Initilize TpConnection_Dynamic structure with its corresponding dynamic sessions*/
            #if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)
            {
                /* Start: Variable declaration */
                PduR_GT_TpPtr gwTpTablePtr;
                PduR_GwTp_SessionListPtr gwTpSessionPtr_L;
                PduR_TpConnDynPtr ConnectionPtr;
                uint16 sessionIndex = 0;
                uint16 TpIndex_u16;
                /* End: Variable declaration */

                for(TpIndex_u16 = 0; TpIndex_u16 < (PduR_Base->totalNoOfGwTpRps) ; TpIndex_u16++)
                {
                    gwTpTablePtr = PDUR_GW_TP_BASE + TpIndex_u16;
                    ConnectionPtr =(PduR_TpConnDynPtr)(gwTpTablePtr->GW_Tp_buffer->connectionTable);
                    gwTpSessionPtr_L = PduR_Base->gwTp_SessionListPtr;
                    /* Initialize SessionPtrs for unique buffers */
                    if(ConnectionPtr->primarySession == NULL_PTR)
                    {
                        /*Initilize primary session*/
                        ConnectionPtr->primarySession = gwTpSessionPtr_L[sessionIndex].GwTpSessionPtr;
                        /*Initilize all the members of the session struct*/

                        PduR_InitializePduR_Gw_Tp_Session(gwTpSessionPtr_L[sessionIndex].GwTpSessionPtr, gwTpTablePtr);

                        /*Increment the TpIndex_u16 in the PduR_TpBuf_SessionList array present in pdur_pbcfg.c*/
                        sessionIndex++;

                        #ifdef PDUR_QUEUEING_SUPPORT
                        if(gwTpTablePtr->TpGwQueueSize == 2u)
                        {
                            /*Initilize secondary session only if GwTp Rp Q size is 2*/
                            ConnectionPtr->secondarySession = gwTpSessionPtr_L[sessionIndex].GwTpSessionPtr;
                            /*Initilize all the members of the session struct*/
                            /* When FIFO is disabled, always FALSE and 1 is passed as second and third arguments
                             * respectively in this function.
                             * This is to avoid two declarations(for with and without FIFO enabled)
                             * of PduR_InitilizePduR_Gw_Tp_Session()
                             */
                            /* NOTE: When FIFO is enabled, QUEUEING SUPPORT must be disabled,
                             * hence no need to call this function for FIFO */
                            PduR_InitializePduR_Gw_Tp_Session(gwTpSessionPtr_L[sessionIndex].
                                 GwTpSessionPtr, gwTpTablePtr);

                            /*Increment the TpIndex_u16 in the PduR_TpBuf_SessionList array present in pdur_pbcfg.c*/
                            sessionIndex++;
                        }
                        else
                        {
                            /*If routing path is not supporting secondary session then set it to Null-Ptr*/
                            ConnectionPtr->secondarySession = NULL_PTR;
                        }
                        ConnectionPtr->activeSessionCount = 0;
                        #endif /* #ifdef PDUR_QUEUEING_SUPPORT */
                    }
                    else
                    {
                        /*Skip the sessionPtrs which has initialized
                         * else is created to remove Misra warning*/

                    }

                }
            }
            #endif/*#if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)*/

            /*Initlize PduR_FIFO_If_dynamic structure with its corresponding buffers*/
            #if defined(PDUR_IFGATEWAY_SUPPORT) && (PDUR_IFGATEWAY_SUPPORT != STD_OFF)
            {
                const PduR_GT_If * gwIfTablePtr;
                PduR_Buf_If_dynamic *dyn;
                PduInfoType *infoBuf;
                PduR_FifoType        fifo;
                PduR_FifoDynIfPtr    dyn_buf;
                uint16 IfIndex_u16;
                for(IfIndex_u16 = 0; IfIndex_u16 < (PduR_Base->totalNoOfGwIfRps) ; IfIndex_u16++)
                {
                    gwIfTablePtr = PDUR_GW_IF_BASE + IfIndex_u16;
                    /*Check if PduRTxBuffer is of type Fifo or Non-fifo*/
                    if(gwIfTablePtr->isFifoType != PduR_GwIfBufType_NonFIFO)
                    {
                        /*Initilize the IF Dynamic buffer structure*/
                        /*Fifo fixed buffer initlization is done below*/

                        /* MR12 RULE 11.5 VIOLATION : 'pointer-to-void' is being cast to a 'pointer-to-object' type
                         * and this conversion will not result in any misalignment.
                         * Hence Supressed.*/
                        fifo = (PduR_FifoType)(gwIfTablePtr->GW_If_buffer);
                        dyn_buf = (PduR_FifoDynIfPtr)(fifo->dyn);
                        /* Check if metaDataBuf has an address assigned to it */
                        if(gwIfTablePtr->metaDataIFInfo_pst != NULL_PTR)
                        {
                            dyn_buf->metaData = gwIfTablePtr->metaDataIFInfo_pst->metaDataBuf;
                        }
                        else
                        {
                            dyn_buf->metaData = NULL_PTR;
                        }
                        PduR_InitializePduR_Gw_If_DynStruct(dyn_buf, fifo);
                    }
                    else
                    {
                        /*Non-Fifo buffer initlization is done below*/
                        /* MR12 RULE 11.5,11.8 VIOLATION : 'pointer-to-void' is being cast to a 'pointer-to-object'
                         * type and this conversion will not result in any misalignment.
                         * Hence Supressed.*/
                        dyn = ( PduR_Buf_If_dynamic*) gwIfTablePtr->GW_If_buffer;
                        infoBuf =  dyn->gwBuf;
                        infoBuf->SduLength = dyn->confLen;
                        infoBuf->SduDataPtr =  dyn->begin;
                        /* Check if metaDataBuf has an address assigned to it */
                        if(gwIfTablePtr->metaDataIFInfo_pst != NULL_PTR)
                        {
                            infoBuf->MetaDataPtr = gwIfTablePtr->metaDataIFInfo_pst->metaDataBuf;
                        }
                        else
                        {
                            infoBuf->MetaDataPtr = NULL_PTR;
                        }
                        /*Check for the configured Default values.*/
                        #if defined(PDUR_DEFAULT_VALUE_SUPPORT) && (PDUR_DEFAULT_VALUE_SUPPORT != 0)
                        {
                            const PduR_DefaultValues_fixed * defaultValues;
                            uint8 IfIndexu_8;
                            uint32 BytePosition;
                            if(dyn->defaultValueElement != NULL_PTR)
                            {
                                defaultValues = dyn->defaultValueElement;
                                for(IfIndexu_8 = 0; IfIndexu_8 < dyn->defaultValueSize ; IfIndexu_8++)
                                {
                                    /*Get the byte position*/
                                    BytePosition = defaultValues[IfIndexu_8].bytePosition;
                                    dyn->begin[BytePosition] = defaultValues[IfIndexu_8].byteDefaultValue;
                                }
                            }
                        }

                        #endif/*#if defined(PDUR_DEFAULT_VALUE_SUPPORT) && (PDUR_DEFAULT_VALUE_SUPPORT != 0)*/
                    }
                }
            }
                 #endif/*#if defined(PDUR_IFGATEWAY_SUPPORT) && (PDUR_IFGATEWAY_SUPPORT != STD_OFF)*/

            /* After having finished the module initialization without errors, the PDU
             * Router state shall change to PDUR_ONLINE state, in case of errors the PDU
             * Router shall change to PDUR_REDUCED state and the error PDUR_E_INIT_FAILED shall be reported to DEM.
             */
            PduR_State = PDUR_ONLINE;
        }
    }
}

/* ------------------------------------------------------------------------ */
/* End section for code */



/* ------------------------------------------------------------------------ */
/* Begin section for code */



/**
 **************************************************************************************************
 * PduR_iInit_multicast - Initialize a multicast state variable to default value
 *
 * \param           PduR_MS_LoTpTxToUp state.
 * \retval          None
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
void PduR_iInit_multicast(PduR_MS_LoTpTxToUp * state)
{
    state->pduPtr     = NULL_PTR;
    state->allocError = BUFREQ_OK;
    state->txFailure  = E_OK;
    state->txConf     = 0;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

