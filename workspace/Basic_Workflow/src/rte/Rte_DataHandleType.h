/** @file     Rte_DataHandleType.h
  *
  * @brief    RTE DataElement type definitions
  *
  * @note     AUTOMATICALLY GENERATED FILE! DO NOT EDIT!
  *
  * @note     Generated by ETAS RTA-RTE for Windows
  * @note     Version 12.2.0
  *
  * @date     Thu Feb  8 15:40:06 2024
  */

#ifndef RTE_DATA_HANDLE_TYPE_H
#define RTE_DATA_HANDLE_TYPE_H

#include "Rte_Type.h"
#include "Rte_Cfg.h"

typedef struct {
   uint8 value;
} Rte_DE_uint8;

typedef struct {
   Rte_DE_uint8 rbuf0;
} Rte_TDB_OsTask_ASW;

typedef union {
   Rte_TDB_OsTask_ASW Rte_OsTask_ASW;
} Rte_ISA_1;

typedef struct {
   Rte_ISA_1 isa_1;
} Rte_ImplicitBufsType;


#endif /* !RTE_DATA_HANDLE_TYPE_H */
