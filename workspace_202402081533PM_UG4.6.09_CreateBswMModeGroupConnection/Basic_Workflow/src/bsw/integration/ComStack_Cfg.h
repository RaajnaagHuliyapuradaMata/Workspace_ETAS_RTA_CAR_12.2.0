/*
* *********************************************************************************************************************
* This is a template file.
* It defines integration functions which are necessary to be implemented.
* The integrator must complete the templates before deploying software containing functions defined in this file.
* Once templates have been completed, the integrator should delete the "removal part"
* Note: The integrator is responsible for updates made to this file.
*
* To suppress following error message, define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT
* during compiler invocation (e.g. -DNOT_READY_FOR_TESTING_OR_DEPLOYMENT)
* The suppression of the error only allows the user to proceed with the building phase.
* *********************************************************************************************************************
*/

/*---- Removal start ----*/
/* Hint for the integrator: remove everything between Removal start and Removal end after proper implementation */

#ifndef NOT_READY_FOR_TESTING_OR_DEPLOYMENT
# error The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#else
# warning The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#endif /* NOT_READY_FOR_TESTING_OR_DEPLOYMENT */
/*---- Removal end ----*/

/*
*******************************************************************************************************************
* Based on AUTOSAR_SWS_CommunicationStackTypes.pdf; AR4.2; Rev 2 and BSW_CodingGuidelines_Cur_v.1.10.pdf
*******************************************************************************************************************
*/





/* TRACE[SWS_COMTYPE_00016]: protected against multiple inclusion. */
#ifndef COMSTACK_CFG_H
#define COMSTACK_CFG_H


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* TRACE[Rule BSW_VersionInfo_004]: Software Version Number and AUTOSAR Specification Version Number. */
/* TRACE[SRS_BSW_00374]  */
#define COMTYPE_SW_MAJOR_VERSION 0x04u
#define COMTYPE_SW_MINOR_VERSION 0x00u
#define COMTYPE_SW_PATCH_VERSION 0x00u
#define COMTYPE_AR_RELEASE_MAJOR_VERSION 0x04u
#define COMTYPE_AR_RELEASE_MINOR_VERSION 0x05u
#define COMTYPE_AR_RELEASE_REVISION_VERSION 0x00u

 

/*
*******************************************************************************************************************
* Types and Defines
*******************************************************************************************************************
*/


/* TRACE[SWS_COMTYPE_00005], TRACE[SWS_COMTYPE_00029] and TRACE[SWS_COMTYPE_00030]:
  Type of PDU ID. Allowed ranges: uint8/uint16 */
typedef uint16    PduIdType;

/* TRACE[SWS_COMTYPE_00008], TRACE[SWS_COMTYPE_00029] and TRACE[SWS_COMTYPE_00030]:
  Type of PDU Length. Allowed ranges: uint8/uint16/uint32 */
typedef uint16    PduLengthType;



  
/*TRACE[SWS_COMTYPE_00011]:Variables of this type shall be used to store the basic information about a PDU of any type, 
namely a pointer variable pointing to its SDU (payload), a pointer to Meta Data of the PDU, 
and the corresponding length of the SDU in bytes. */

typedef struct
{
    uint8 * SduDataPtr;
    uint8 * MetaDataPtr;
    PduLengthType SduLength;
} PduInfoType;  
 


 



/*TRACE[SWS_COMTYPE_00039]: IcomConfigIdType-IcomConfigIdType defines the configuration ID.*/
typedef uint8   IcomConfigIdType;

/*TRACE[SWS_COMTYPE_00040]: IcomSwitch_ErrorType-IcomSwitch_ErrorType*/
typedef enum
{
    ICOM_SWITCH_E_OK,
    ICOM_SWITCH_E_FAILED
} IcomSwitch_ErrorType;
 


#endif /* COMSTACK_CFG_H */

