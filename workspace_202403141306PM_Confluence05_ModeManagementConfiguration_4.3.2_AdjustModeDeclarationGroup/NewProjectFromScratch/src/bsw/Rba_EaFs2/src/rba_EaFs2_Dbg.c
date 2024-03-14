/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2020, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Ea.h"
#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#if(FALSE != EA_PRV_DEBUGGING)

#include "Ea_Prv.h"

#define EA_START_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"

rba_EaFs2_Debug_tst rba_EaFs2_Debug_st = {
        &rba_EaFs2_Prv_Write_st,
        &rba_EaFs2_Prv_Read_st,
        &rba_EaFs2_Prv_Bgo_st,
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0}
};

#define EA_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

/* Get the initial time*/
void rba_EaFs2_Prv_DebugTimeStart(rba_EaFs2_Prv_time_tst *debugTime_pst)
{
    debugTime_pst->iTime_u32 = rba_MemLib_GetTimerUs();

    if(Ea_Prv_Order_st.OrderType_en == EA_PRV_READ_ORDER_E)
    {
        rba_EaFs2_Debug_st.readTime_st.cycle_u32++;
    }
    else if(Ea_Prv_Order_st.OrderType_en == EA_PRV_WRITE_ORDER_E)
    {
        rba_EaFs2_Debug_st.writeTime_st.cycle_u32++;
    }
    else if(Ea_Prv_Order_st.OrderType_en == EA_PRV_INVALIDATE_ORDER_E)
    {

    }
    else
    {
        /*Do-nothing*/
    }
}

/* Get the final time*/
void rba_EaFs2_Prv_DebugTimeStop(rba_EaFs2_Prv_time_tst *debugTime_pst)
{
    debugTime_pst->fTime_u32 = rba_MemLib_GetTimerUs() - debugTime_pst->iTime_u32;

    /* Find the maximum time*/
    if(debugTime_pst->maxTime_u32 < debugTime_pst->fTime_u32)
    {
        debugTime_pst->maxTime_u32 = debugTime_pst->fTime_u32;
    }

    if(debugTime_pst->maxCycle_u32 < debugTime_pst->cycle_u32)
    {
        debugTime_pst->maxCycle_u32 = debugTime_pst->cycle_u32;
    }

    debugTime_pst->cycle_u32 = 0;
}

/* Initialize global variable */
void rba_EaFs2_Prv_Initialize_GlobalVarible(void)
{
#if (EA_RB_ENTER_STOP_MODE != FALSE)
    rba_EaFs2_Prv_Initialize_Adapter();
#endif
    rba_EaFs2_Prv_Initialize_Defrag();
    rba_EaFs2_Prv_Initialize_Eeprom();
    rba_EaFs2_Prv_Initialize_Fat();
    rba_EaFs2_Prv_Initialize_Init();
    rba_EaFs2_Prv_Initialize_Order();
    rba_EaFs2_Prv_Test_Initialize_EaFs2();
}

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"
#endif /* FALSE != EA_PRV_DEBUGGING */
#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
