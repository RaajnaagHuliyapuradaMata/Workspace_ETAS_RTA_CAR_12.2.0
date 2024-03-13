


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/

#if !defined(COM_CFG_SYMBOLICNAMES_H)
# define COM_CFG_SYMBOLICNAMES_H


/* if COM_DontUseExternalSymbolicNames is defined while including this file, then the below symbolic names will not
   be visible in the including file */
# if !defined(COM_DontUseExternalSymbolicNames)

/* ------------------------------------------------------------------------ */
/* Begin section for IPdu symbolic names */

/* Tx IPdus */
        #  define ComConf_ComIPdu_IP_ApplicationECU_TestResult_Tx_Can_Network_Channel_CAN_Tx 0
    /* Rx IPdus */
        #  define ComConf_ComIPdu_IP_TestECU_TestCode_Tx_Can_Network_Channel_CAN_Rx 0

/* ------------------------------------------------------------------------ */
/* End section */

/* ------------------------------------------------------------------------ */
/* Begin section for Signal symbolic names */



/* Signal IDs*/
/* Tx Signal ID*/
    #  define ComConf_ComSignal_S_ApplicationECU_SG_TestResult_Tx_Can_Network_Channel_CAN_Tx 0

/* Rx Signal ID*/
    #  define ComConf_ComSignal_S_TestECU_SG_TestCode_Tx_Can_Network_Channel_CAN_Rx 0



/* ------------------------------------------------------------------------ */
/* End section */

/* ------------------------------------------------------------------------ */
/* Begin section for Signal group symbolic names */



/* Tx SignalGroup ID*/

/* Rx SignalGroup ID*/





/* ------------------------------------------------------------------------ */
/* End section */

/* ------------------------------------------------------------------------ */
/* Begin section for Group signal symbolic names */



/* Tx GroupSignal ID*/

/* Rx GroupSignal ID*/




/* ------------------------------------------------------------------------ */
/* End section */

/* ------------------------------------------------------------------------ */
/* Begin section for IPdu group symbolic names */


/* IPduGroup ID*/
    #  define ComConf_ComIPduGroup_ComIPduGroup_Rx 0
    #  define ComConf_ComIPduGroup_ComIPduGroup_Tx 1
/* ------------------------------------------------------------------------ */
/* End section */

# endif /* end of COM_DontUseExternalSymbolicNames */


#endif /* COM_CFG_SYMBOLICNAMES_H */


