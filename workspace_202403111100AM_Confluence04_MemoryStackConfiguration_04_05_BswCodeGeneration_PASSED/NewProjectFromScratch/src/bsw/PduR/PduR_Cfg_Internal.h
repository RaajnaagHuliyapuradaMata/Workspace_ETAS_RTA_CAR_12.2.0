


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: PduR  / AR45.5.0.1                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/


#ifndef PDUR_CFG_INTERNAL_H
#define PDUR_CFG_INTERNAL_H

#include "ComStack_Types.h"


#if !defined(PDUR_DEV_ERROR_DETECT)
#define PDUR_DEV_ERROR_DETECT    (STD_OFF)
#endif /* PDUR_DEV_ERROR_DETECT */

#if !defined(PDUR_VERSION_INFO_API)
#define PDUR_VERSION_INFO_API    (STD_OFF)
#endif /* PDUR_VERSION_INFO_API */

#if !defined(PDUR_ZERO_COST_OPERATION)
#define PDUR_ZERO_COST_OPERATION    (STD_OFF)
#endif /* PDUR_ZERO_COST_OPERATION */

#if !defined(PDUR_IFGATEWAY_SUPPORT)
#define PDUR_IFGATEWAY_SUPPORT    (STD_OFF)
#endif /* PDUR_IFGATEWAY_SUPPORT */

#if !defined(PDUR_TPGATEWAY_SUPPORT)
#define PDUR_TPGATEWAY_SUPPORT    (STD_OFF)
#endif /* PDUR_TPGATEWAY_SUPPORT */



#define PDUR_RPGID_NULL   (0)

#if !defined(PDUR_DISABLE_MULTICAST_SUPPORT)
#define PDUR_DISABLE_MULTICAST_SUPPORT    (1) /* used internally to disable */
#endif /* PDUR_DISABLE_MULTICAST_SUPPORT */


#if !defined(PDUR_MULTICAST_TO_IF_SUPPORT)
#define PDUR_MULTICAST_TO_IF_SUPPORT    (0) /* Disabled as there are no Multicast routes to IF Modules */
#endif /* PDUR_MULTICAST_TO_IF_SUPPORT */






#if !defined(PDUR_MULTICAST_TO_TP_SUPPORT)
#define PDUR_MULTICAST_TO_TP_SUPPORT    (0) /* Disabled as there are no Multicast routes to TP Modules */
#endif /* PDUR_MULTICAST_TO_TP_SUPPORT */


#define PDUR_CONFIGURATION_ID (0)


#define PDUR_FIFO_IF_MAXSIZE (1)




#if !defined(PDUR_MULTICAST_FROM_TP_SUPPORT)
#define PDUR_MULTICAST_FROM_TP_SUPPORT    (0) /* Disabled as there are no Multicast routes from TP Modules */
#endif /* PDUR_MULTICAST_FROM_TP_SUPPORT */




#if !defined(PDUR_MODE_DEPENDENT_ROUTING)

    #define PDUR_MODE_DEPENDENT_ROUTING (0)
#endif /* PDUR_MODE_DEPENDENT_ROUTING */

/* Array used to maintain RPG Status with the value of PduRIsEnabledAtInit for each RPG */
#if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)
 /* ------------------------------------------------------------------------ */
/* Begin section for constants */

/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_CONFIG_DATA_POSTBUILD_8
#else
#define PDUR_START_SEC_CONFIG_DATA_8
#endif

#include "PduR_MemMap.h"

extern const boolean PduR_RPG_EnRouting[];


/* ------------------------------------------------------------------------ */
/* Begin section for constants */

#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_CONFIG_DATA_POSTBUILD_8
#else
#define PDUR_STOP_SEC_CONFIG_DATA_8
#endif
#include "PduR_MemMap.h"


/* ------------------------------------------------------------------------ */
/* Begin section for variables */
#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_START_SEC_VAR_CLEARED_POSTBUILD_8
#else
#define PDUR_START_SEC_VAR_CLEARED_8
#endif

#include "PduR_MemMap.h"

/* This array is used to maintain the status of each RPG */
extern boolean PduR_RPG_Status[];

/* ------------------------------------------------------------------------ */
/* End section for variables */
#if ( PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE )
#define PDUR_STOP_SEC_VAR_CLEARED_POSTBUILD_8
#else
#define PDUR_STOP_SEC_VAR_CLEARED_8
#endif

#include "PduR_MemMap.h"
/* ------------------------------------------------------------------------ */
/* End section for extern constant */

#endif

#if !defined(PDUR_DEFAULT_VALUE_SUPPORT)
#define PDUR_DEFAULT_VALUE_SUPPORT (0)
#endif /* PDUR_DEFAULT_VALUE_SUPPORT */


/*These Macros are used to indicate a unique no. assigned to each upper layer & is used in PduR_MC_SetTxConfirmationFlag() function if
    there are muticast routing paths existing from UpIf to Multiple LoIf*/

#define COM_PDURBSWMODNAME (0u)

#define PDUR_NULL_BSWMODNAME 255
#define PDUR_NULL_PDUID 0
#define PDUR_INVALID_ID      0xFFFFu
/**
 * @ingroup PDUR_H
 *
 * Define PduRTpThreshold default value if user not configured.
 */
#define PDUR_TPTHRESHOLD_DEFVALUE 0xFFFFu
/*This is generating for fulfill the structure element *
 * the code is not reach till this element */
#define PDUR_NULL_MOD  FALSE





#if !defined(PDUR_DEM_INIT_FAILED)
#define PDUR_DEM_INIT_FAILED        (0) /* STD_OFF */
#endif /* PDUR_DEM_INIT_FAILED */



#if !defined(PDUR_DEM_PDU_INSTANCE_LOST)
#define PDUR_DEM_PDU_INSTANCE_LOST  (0) /* STD_OFF */
#endif /* PDUR_DEM_PDU_INSTANCE_LOST */



#define PDUR_DSM_REINT_SUPPORT      STD_OFF










#if !defined(PDUR_CANIF_RXIND_BASE)
#define PDUR_CANIF_RXIND_BASE    (PDUR_BASE_LOIFD(0,LoIfRxToUp))
#define PDUR_CANIF_RXIND_MAP_TAB    (PDUR_BASE_LOIFD(0,LoIfRxToUpMapTable))
#endif /* PDUR_CANIF_RXIND_BASE */
#if !defined(PDUR_CANIF_TXCONF_BASE)
#define PDUR_CANIF_TXCONF_BASE    (PDUR_BASE_LOIFD(0,LoIfTxToUp))
#define PDUR_CANIF_TXCONF_MAP_TAB    (PDUR_BASE_LOIFD(0,LoIfTxToUpMapTable))
#endif /* PDUR_CANIF_TXCONF_BASE */
/* This macros are generated for postbuild as well because it is using during Test */
#if !defined(PDUR_NR_VALID_CANIFIFRXTOUP_IDS)
#define PDUR_NR_VALID_CANIFIFRXTOUP_IDS    (PDUR_BASE_LOIFD(0,LoIfRxToUp_NrEntries))
#endif /* PDUR_NR_VALID_CANIFIFRXTOUP_IDS */
#if !defined(PDUR_NR_VALID_CANIFIFTXTOUP_IDS)
#define PDUR_NR_VALID_CANIFIFTXTOUP_IDS    (PDUR_BASE_LOIFD(0,LoIfTxToUp_NrEntries))
#endif /* PDUR_NR_VALID_CANIFIFTXTOUP_IDS */




#if !defined(PDUR_COM_TX_BASE)
#define PDUR_COM_TX_BASE    (PDUR_BASE_UPIF(0,UpToLo))
#define PDUR_COM_TX_MAP_TAB    (PDUR_BASE_UPIF(0,UpToLoMapTable))
#endif /* PDUR_COM_TX_BASE */
#if !defined(PDUR_COM_MC_TX_BASE)
#define PDUR_COM_MC_TX_BASE    (PDUR_BASE_UPIF(0,UpToLoMc))
#endif /* PDUR_COM_MC_TX_BASE */
/* This macros are generated for postbuild as well because it is using during Test */
#if !defined(PDUR_NR_VALID_COM_IDS)
#define PDUR_NR_VALID_COM_IDS    (PDUR_BASE_UPIF(0,UpToLo_NrEntries))
#endif /* PDUR_NR_VALID_COM_IDS */



/* *********************************************
    Variant: __KW_COMMON
   ********************************************* */
/*Macro used for getting the size of Cdd config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_CddCfg     0

/*Macro used for getting the size of LoTp Rx config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_LoTpRxCfg  0

/*Macro used for getting the size of LoTp Tx config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_LoTpTxCfg   0

/*Macro used for getting the size of LoIf Rx with Direct data provision config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_LoIfRxDCfg   1

/*Macro used for getting the size of LoIf Tx with Direct data provision config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_LoIfTxDCfg   1

/*Macro used for getting the size of LoIf Rx with Trigger Transmit data provision config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_LoIfRxTTCfg   0

/*Macro used for getting the size of LoIf Tx with Trigger Transmit data provision config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_LoIfTxTTCfg   0

/*Macro used for getting the size of LoIf Tx with Trigger Transmit data provision config, this will be use in the reference configuration unit testing only*/
#define TableSize_PduR_UpTpCfg       0
#endif /* PDUR_CFG_INTERNAL_H */
