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

#ifndef PLATFORM_TYPES_CFG_H
#define PLATFORM_TYPES_CFG_H

/* Integration hint: User definitions overriding the default ones in Platform_Types.h can be added here */

/* HINT : RBA_VECU_ENABLED is a compiler macro which is used for VECU activation, By default this macro
 * will bd enabled in VECU Projects.Currently VECU is not part of RTA-CAR SW, So RTA-CAR will define
 * RBA_VECU_ENABLED in Plaform_Types_Cfg.h to make the boolean typedef portable in PC environments.*/

// #define RBA_VECU_ENABLED

/* HINT : By default "unsigned char" native declaration will be defined for boolean.
 * RBA_VECU_ENABLED_BOOL_CONFIGURABLE macro definition is provided to support the user configuration
 * boolean native declaration via BCT Parameter PlatformRbNativeBoolean. */

// #define RBA_VECU_ENABLED_BOOL_CONFIGURABLE



#endif /* PLATFORM_TYPES_CFG_H */
