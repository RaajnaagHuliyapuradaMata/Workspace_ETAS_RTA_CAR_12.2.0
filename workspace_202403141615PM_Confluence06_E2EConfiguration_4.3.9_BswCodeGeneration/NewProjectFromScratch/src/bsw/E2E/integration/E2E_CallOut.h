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

#ifndef E2E_CALLOUT_H
#define E2E_CALLOUT_H


#endif /* E2E_CALLOUT_H */

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "E2E_Prv.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/

E2E_INLINE  uint8     E2E_Prv_P01CalcCRC8_CallOut(const E2E_P01ConfigType * const Config_pst, uint8 Counter_u8,
                                           const uint8 * const Data_pu8);
E2E_INLINE uint8 E2E_Prv_P02CalcCRC8H2F_CallOut(const E2E_P02ConfigType * const Config_pst, uint8 DataID_u8,
                                          const uint8 * const Data_pu8);
E2E_INLINE uint32   E2E_Prv_P04CalcCRC32P4_CallOut(const E2E_P04ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                           uint16 Length_u16);
E2E_INLINE uint16   E2E_Prv_P05CalcCRC16_CallOut(const E2E_P05ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                           uint16 Length_u16);
E2E_INLINE uint16   E2E_Prv_P06CalcCRC16_CallOut(const E2E_P06ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                           uint16 Length_u16);
E2E_INLINE uint64   E2E_Prv_P07CalcCRC64_CallOut(const E2E_P07ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                           uint32 Length_u32);
E2E_INLINE uint8 E2E_Prv_P11CalcCRC8_CallOut(const E2E_P11ConfigType * const ConfigPtr, const uint8 * const Data_pu8);

E2E_INLINE uint8 E2E_Prv_P22CalcCRC8H2F_CallOut(const E2E_P22ConfigType * const ConfigPtr, const uint8 * const Data_pu8,
                                         uint32 Length_u32, uint8 value_u8);

E2E_INLINE uint32   E2E_Prv_P44CalcCRC32P4_CallOut(const E2E_P44ConfigType * const Config_pst, const uint8 * const Data_pu8,
                                           uint16 Length_u16);

