/** @file     Rte_Const.h
  *
  * @brief    RTA-RTE related configuration constants
  *
  * @note     AUTOMATICALLY GENERATED FILE! DO NOT EDIT!
  *
  * @note     Generated by ETAS RTA-RTE for Windows
  * @note     Version 12.2.0
  *
  * @date     Thu Feb 15 15:55:09 2024
  */

#ifndef RTE_CONST_H
#define RTE_CONST_H

#define ApplicationECU_FlatView_SwComponentTypes_ApplicationECU_FlatView_CPT_BswM                 SWCI1
#define ApplicationECU_FlatView_SwComponentTypes_ApplicationECU_FlatView_CPT_ComM                 SWCI2
#define ApplicationECU_FlatView_SwComponentTypes_ApplicationECU_FlatView_CPT_Det                  SWCI3
#define ApplicationECU_FlatView_SwComponentTypes_ApplicationECU_FlatView_CPT_EcuM                 SWCI4
#define ApplicationECU_FlatView_SwComponentTypes_ApplicationECU_FlatView_CPT_MySWC_ApplicationECU CPT_MySWC_ApplicationECU
#define ApplicationECU_FlatView_SwComponentTypes_ApplicationECU_FlatView_CPT_rba_DiagLib          SWCI5
#define RTE_CALPRM_NONE                                                                           (1)
#define RTE_CLUSTER_APPLICATION_MODE_ENABLED                                                      (0)
#define RTE_COMPATIBILITY_MODE                                                                    (1)
#define RTE_MAINFUNCTION_PERIOD_MS                                                                (10)
#define RTE_MAINFUNCTION_PERIOD_NS                                                                (10000000)
#define RTE_MAINFUNCTION_PERIOD_US                                                                (10000)
#define RTE_MAX_USED_CORE_ID                                                                      (-1)
#define RTE_MAX_USED_OSAPP_ID                                                                     (0)
#define RTE_MEASUREMENT_ENABLED                                                                   (0)
#define RTE_NUM_TASKS                                                                             (2)
#define RTE_NUM_TIMEOUT_ALARMS                                                                    (0)
#define RTE_OSAPI_AUTOSAR_R40                                                                     (1)
#define RTE_OSCFG_AUTOSAR_R40                                                                     (1)
#define RTE_PERIODIC_COUNTER_TICK_INTERVAL_US                                                     (1000)
#define RTE_REQUIRES_COM                                                                          (1)
#define RTE_RIPS_SUPPORT                                                                          (0)
#define RTE_TASKS_ARRAYSIZE                                                                       (2)
#define RTE_WOWP_EVENTS                                                                           (0)
#define SCHM_GETVERSIONINFO                                                                       (0)
/* Box: Rte: OS Environment begin */
/* The following OSENV_ macros are supported by this OS DLL: */
/* OSENV_UNSUPPORTED */
/* OSENV_RTAOS40 */
#if defined(OSENV_RTAOS40)
#if defined(OSENV_UNSUPPORTED)
#error "Please define exactly one of [ OSENV_RTAOS40, OSENV_UNSUPPORTED ] for OS_AUTOSAR40 OS API."
#endif /* defined(OSENV_UNSUPPORTED) */
#elif defined(OSENV_UNSUPPORTED)
#else
#error "Please define exactly one of [ OSENV_RTAOS40, OSENV_UNSUPPORTED ] for OS_AUTOSAR40 OS API."
#endif
/* Box: User declared atomic BSW types: begin */
/* Box: User declared atomic BSW types: end */
/* Box: Rte: OS Environment end */

#endif /* !RTE_CONST_H */
