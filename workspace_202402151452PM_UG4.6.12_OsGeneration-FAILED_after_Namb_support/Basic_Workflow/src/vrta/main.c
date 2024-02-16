/** \file         main.c
  *
  * \brief        Main loop for ISOLAR-EVE ( target platform: VRTA-ux/VRTA-win )
  *
  * [$crn:2007:dox
  * \copyright Copyright 2014 ETAS GmbH
  * $]
  *
  * \note         PLATFORM DEPENDENT [yes/no]: yes
  *
  * \note         TO BE CHANGED BY USER [yes/no]: no
  *
  * $Id: main.c 11894 2018-02-27 16:15:00Z SHB1YOK $
  */

#include "Std_Types.h"
#include "Main_Cfg.h"
#include "target.h"
#include "Os.h"
#include "EcuM.h"

OS_MAIN() {
    TargetInit();
    EcuM_Init();
}
