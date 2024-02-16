/*
 **********************************************************************************************************************
 *
 * Copyright (c) ETAS GmbH 2023, all rights reserved.
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 
 **********************************************************************************************************************
 Machine Type: MCU_RB_GENERIC
 Generated by MEMMAPGEN 12.2.0  
 **********************************************************************************************************************
*/ 
/* MR12 DIR 4.10 VIOLATION: Memmap file must be included multiple times */
/* MR12 RULE 20.5 VIOLATION: AUTOSAR MemMap concept requires #undef, AUTOSAR MemMap requirements are incompatible to MISRA */ 
#if defined RBA_DIAGLIB_START_SEC_CODE
  /* CODE pragma configuration not available, please update the respective template file */ 
			 
  #define BSW_START_SEC_CODE
  #include "Bsw_MemMap.h"
			 
  #undef RBA_DIAGLIB_START_SEC_CODE           
#elif defined RBA_DIAGLIB_STOP_SEC_CODE      
  /* CODE pragma configuration not available, please update the respective template file */ 
			 
  #define BSW_STOP_SEC_CODE
  #include "Bsw_MemMap.h"
			 
  #undef RBA_DIAGLIB_STOP_SEC_CODE             
#elif defined rba_DiagLib_START_SEC_CODE
  /* CODE pragma configuration not available, please update the respective template file */ 
			 
  #define BSW_START_SEC_CODE
  #include "Bsw_MemMap.h"
			 
  #undef rba_DiagLib_START_SEC_CODE           
#elif defined rba_DiagLib_STOP_SEC_CODE      
  /* CODE pragma configuration not available, please update the respective template file */ 
			 
  #define BSW_STOP_SEC_CODE
  #include "Bsw_MemMap.h"
			 
  #undef rba_DiagLib_STOP_SEC_CODE             
#else 
#error "rba_DiagLib_MemMap.h, wrong pragma command" 
#endif
