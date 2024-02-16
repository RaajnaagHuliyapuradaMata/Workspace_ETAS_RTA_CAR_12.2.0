


 
/*<VersionHead>
 * This Configuration File is generated using versions (automatically filled in) as listed below.
 *
 * $Generator__: Com / AR45.5.0.0                Module Package Version
 * $Editor_____: RTA-CAR 12.2.0_12.2.0                Tool Version
 *
 
 </VersionHead>*/

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Com_Prv.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/
/* START: TMS NONE Details  */

#define COM_START_SEC_CONST_UNSPECIFIED
#include "Com_MemMap.h"
const Com_TransModeInfo_tst Com_NONE_TransModeInfo_cst =
{

    0, /* timePeriod_u16 */
    0, /* timeOffset_u16 */

    0, /* repetitionPeriod_u16 */
    0, /* numOfRepetitions_u8 */

#ifdef COM_MIXEDPHASESHIFT
    COM_TXMODE_NONE, /* mode_u8 */
    COM_FALSE   /* mixedPhaseShift_b status */
#else

    COM_TXMODE_NONE /* mode_u8 */

#endif /* #ifdef COM_MIXEDPHASESHIFT */


};
#define COM_STOP_SEC_CONST_UNSPECIFIED
#include "Com_MemMap.h"

/* END: TMS NONE Details  */

/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"


/*
 **********************************************************************************************************************
 Rx-MainFunction Timebase - 0.01 s
 **********************************************************************************************************************
*/
void Com_MainFunctionRx_ComMainFunctionRx(void)
{
#ifdef COM_ENABLE_MAINFUNCTION_RX
    Com_Prv_InternalMainFunctionRx( (Com_MainFunc_tuo)ComMainFunctionRx_Internal_ComMainFunctionRx );
#endif
}



/*
 **********************************************************************************************************************
 Tx-MainFunction Timebase - 0.01 s
 **********************************************************************************************************************
*/
void Com_MainFunctionTx_ComMainFunctionTx(void)
{
#ifdef COM_ENABLE_MAINFUNCTION_TX
    Com_Prv_InternalMainFunctionTx( (Com_MainFunc_tuo)ComMainFunctionTx_Internal_ComMainFunctionTx );
#endif
}





#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"




