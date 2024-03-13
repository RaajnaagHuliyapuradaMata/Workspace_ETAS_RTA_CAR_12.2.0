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

/*
 **********************************************************************************************************************
 * Based on AUTOSAR_SWS_StandardTypes.pdf; AR4.2; Rev 2 and BSW_CodingGuidelines_Cur_v.1.10.pdf
 **********************************************************************************************************************
 */




 /*TRACE[SWS_Std_00004]: It is not allowed to add any project or supplier specific extension to this file.
                         Any extension invalidates the AUTOSAR conformity.*/

/* TRACE[SWS_Std_00014]: protected against multiple inclusion. */
#ifndef STD_TYPES_H
#define STD_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

/*TRACE[SWS_Std_00019]: include file structure. */
#include "Platform_Types.h"
#include "Compiler.h"

/*
 **********************************************************************************************************************
 * Types and Defines
 **********************************************************************************************************************
 */

/* TRACE[Rule BSW_VersionInfo_002]: Module Vendor Identifier. */
/* TRACE[SWS_BSW_00059, SWS_BSW_00161]  */
#define STD_TYPES_VENDOR_ID 0x0006u

/* TRACE[Rule BSW_VersionInfo_003]: Module Identifier. */
/* TRACE[SWS_BSW_00059, SWS_BSW_00161]  */
#define STD_TYPES_MODULE_ID 0x00C5u

/* TRACE[Rule BSW_VersionInfo_004]: Software Version Number and AUTOSAR Specification Version Number. */
/* TRACE[SWS_BSW_00059, SWS_BSW_00161]  */
#define STD_TYPES_SW_MAJOR_VERSION 0x04u
#define STD_TYPES_SW_MINOR_VERSION 0x01u
#define STD_TYPES_SW_PATCH_VERSION 0x00u
#define STD_TYPES_AR_RELEASE_MAJOR_VERSION 0x04u
#define STD_TYPES_AR_RELEASE_MINOR_VERSION 0x05u
#define STD_TYPES_AR_RELEASE_REVISION_VERSION 0x00u

/*As per Autosar spec STD_TYPES  is renamed into STD , in order to prevent backward compatibility cucel is providing*/
/*both the macro names in future it will be eliminated completely */
/* TRACE[Rule BSW_VersionInfo_002]: Module Vendor Identifier. */
/* TRACE[SWS_BSW_00059, SWS_BSW_00161]  */
#define STD_VENDOR_ID 0x0006u

/* TRACE[Rule BSW_VersionInfo_003]: Module Identifier. */
/* TRACE[SWS_BSW_00059, SWS_BSW_00161]  */
#define STD_MODULE_ID 0x00C5u

/* TRACE[Rule BSW_VersionInfo_004]: Software Version Number and AUTOSAR Specification Version Number. */
/* TRACE[SWS_BSW_00059, SWS_BSW_00161]  */
#define STD_SW_MAJOR_VERSION 0x04u
#define STD_SW_MINOR_VERSION 0x01u
#define STD_SW_PATCH_VERSION 0x00u
#define STD_AR_RELEASE_MAJOR_VERSION 0x04u
#define STD_AR_RELEASE_MINOR_VERSION 0x05u
#define STD_AR_RELEASE_REVISION_VERSION 0x00u

/* TRACE[SWS_Std_00006]: Because E_OK is already defined within OSEK, the symbol E_OK has to be shared. */
/* To avoid name clashes and redefinition problems, the symbols have to be defined in the following way */
/* (approved within implementation):                                                                    */
#ifndef STATUSTYPEDEFINED
    #define STATUSTYPEDEFINED
    #define E_OK 0x00u     /* No error occurs            */
    typedef unsigned char StatusType; /* OSEK compliance */
#endif

#define E_NOT_OK   0x01u   /* An error occurs            */

/* TRACE[SWS_Std_00007]: STD_HIGH and STD_Low */
#define STD_HIGH   0x01u   /* Physical state 5V or 3.3V  */
#define STD_LOW    0x00u   /* Physical state 0V          */

/*TRACE[SWS_Std_00013]: STD_ACTIVE and STD_IDLE */
#define STD_ACTIVE 0x01u   /* Logical state active       */
#define STD_IDLE   0x00u   /* Logical state idle         */

/*TRACE[SWS_Std_00010]: STD_On and STD_OFF */
#define STD_ON     0x01u   /* On state                   */
#define STD_OFF    0x00u   /* Off state                  */

/*TRACE[SWS_Std_00005]: Standard Return Type */
/* This type can be used as standard API return type which is shared between the RTE and the BSW modules.      */
/*TRACE[SWS_Std_00011]*/
/* The Standard Return Type shall normally be used with value E_OK or E_NOT_OK. If those return values are not */
/* sufficient user specific values can be defined by using the 6 least specific bits. (SWS_Std_00011)          */
typedef uint8 Std_ReturnType;

/*TRACE[SWS_Std_00015]: Standard Version Info Type */
/* This type shall be used to request the version of a BSW module using the <Module name>_GetVersionInfo()function. */
typedef struct
{
    uint16 vendorID;
    uint16 moduleID;
    uint8 sw_major_version;
    uint8 sw_minor_version;
    uint8 sw_patch_version;
} Std_VersionInfoType;

/*TRACE[SWS_Std_00022] */
/*The values are specified for each transformer class in ASWS_TransformerGeneral.*/
typedef uint8 Std_TransformerErrorCode;

/*TRACE[SWS_Std_00024] */
/* The Std_TransformerClass represents the transformer class in which the error occurred.*/
typedef enum
{
    STD_TRANSFORMER_UNSPECIFIED =0x00, /* Transformer of a unspecified transformer class.*/
    STD_TRANSFORMER_SERIALIZER  =0x01, /*Transformer of a serializer class.*/
    STD_TRANSFORMER_SAFETY      =0x02, /*Transformer of a safety class.*/
    STD_TRANSFORMER_SECURITY    =0x03, /*Transformer of a security class.*/
    STD_TRANSFORMER_CUSTOM      =0xFF  /*Transformer of a custom class not standardized by AUTOSAR.*/
} Std_TransformerClass;

/*TRACE[SWS_Std_00021] */
/* Std_TransformerError represents a transformer error in the context of a certain transformer chain. 
The structure contains the error code and the transformer class to which the error belongs.*/
typedef struct
{
    Std_TransformerErrorCode errorCode;
    Std_TransformerClass transformerClass;
} Std_TransformerError;

/*TRACE[SWS_Std_91001] */
/*This type is used to encode the different type of messages.Currently this encoding is limited to the distinction between requests and responses in C/S communication. */
typedef enum
{
    STD_MESSAGETYPE_REQUEST  =  0x00, /* Message type for a request message.*/
    STD_MESSAGETYPE_RESPONSE =  0x01 /* Message type for a response message. */
} Std_MessageTypeType;

/*TRACE[SWS_Std_91002] */
/*This type is used to encode different types of results for response messages.Currently this encoding is limited to the distinction between OK and ERROR responses.*/
typedef enum
{
    STD_MESSAGERESULT_OK    =  0x00, /* Message result for an OK response .*/
    STD_MESSAGERESULT_ERROR =  0x01 /* Message result for an ERROR response */
} Std_MessageResultType;

/*TRACE[SWS_Std_91003] */
/* Type for the function pointer to extract the relevant protocol header fields of the message and 
   the type of the message result of a transformer. */
typedef Std_ReturnType (*Std_ExtractProtocolHeaderFieldsType) ( const uint8* buffer, uint32 bufferLength, 
                                                        Std_MessageTypeType* messageType, 
                                                        Std_MessageResultType* messageResult );
                                                        
#endif /* STD_TYPES_H */

