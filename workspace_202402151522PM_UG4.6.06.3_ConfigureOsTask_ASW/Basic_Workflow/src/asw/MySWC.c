/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Name: 
 * Description:
 * Version: 1.0
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************

 * Project : Basic_Workflow_mine
 * Component: /SwComponentTypes/MySWC
 * Runnable : All Runnables in SwComponent
 *****************************************************************************
 * Tool Version: ISOLAR-A/B 12.1.2
 * Author: WEL9YOK
 * Date : Thu Jun 08 16:12:44 2023
 ****************************************************************************/

#include "Rte_MySWC.h"

#include "BswM.h"
#include "BswM_Generic.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :MySWC) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :MySWC) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :MySWC) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define MySWC_START_SEC_CODE                   
#include "MySWC_MemMap.h"
FUNC (void, MySWC_CODE) RE_MySWC_func/* return value & FctID */
(
		CONST(Rte_SelfType_MySWC, AUTOMATIC) self
)
{

	static uint8 counter = 0;
	static uint8 once = 1;
	uint8 iRead1;
	static uint8 write2 = 0;
	Std_ReturnType retWrite2;

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :RE_MySWC_func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;

	if (once) {
		BswM_RequestMode(BSWM_CFG_USERID_BSWM_MRP_REQUESTFULLCOM, BSWM_IDX_BSWM_MRP_REQUESTFULLCOM);
		once = 0;
	}

	/*  -------------------------------------- Data Read -----------------------------------------  */
	iRead1 = Rte_IRead_RE_MySWC_RPortPrototype_0_DE_MyDataElement(self);

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */
	
	if (counter == 10) {
		counter = 0;
		write2 = write2 + 1;
		vrtaPrintf("%d", write2);
		retWrite2 = Rte_Write_PPortPrototype_0_DE_MyDataElement(self,write2);
	} else {
		counter = counter + 1;
	}
	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :RE_MySWC_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */




}
#define MySWC_STOP_SEC_CODE  
#include "MySWC_MemMap.h" 

/*PROTECTED REGION ID(FileHeaderUserDefinedFunctions :MySWC) ENABLED START */
/* Start of user defined functions  - Do not remove this comment */
/* End of user defined functions - Do not remove this comment */
/*PROTECTED REGION END */

