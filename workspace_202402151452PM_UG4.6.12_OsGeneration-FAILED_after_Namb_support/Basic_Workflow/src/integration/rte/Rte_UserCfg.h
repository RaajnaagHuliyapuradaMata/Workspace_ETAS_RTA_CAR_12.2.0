/** \file         Rte_UserCfg.h
  *
  * \brief        RTE user configuration header file
  *
  * [$crn:2007:dox 
  * \copyright Copyright 2007 - 2008 ETAS Engineering Tools Application and Services Ltd.
  *            Copyright 2008 ETAS GmbH.
  * $]
  *
  * \note         Implemented SWS: N/A
  *
  * \note         PLATFORM DEPENDANT [yes/no]: no
  *
  * \note         TO BE CHANGED BY USER [yes/no]: yes
  *
  */

#ifndef RTE_USERCFG_H
#define RTE_USERCFG_H

#ifdef _GNU_C_TRICORE_
#include "Can.h"
#endif

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR Specification Version Information */
#define RTE_AR_RELEASE_MAJOR_VERSION         4
#define RTE_AR_RELEASE_MINOR_VERSION         2
#define RTE_AR_RELEASE_REVISION_VERSION      2

/* Software Version Information */
#define RTE_SW_MAJOR_VERSION         4
#define RTE_SW_MINOR_VERSION         2


#endif /* RTE_USERCFG_H */
