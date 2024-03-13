/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "rba_MemLib.h"

#include "rba_MemLib_Cfg_Sync.h"

/*
 **********************************************************************************************************************
 * Defines
 **********************************************************************************************************************
*/
/* Special handling for VECU - no HW sync mechanism defined/avoid unnecessary warnings */
#if defined(RBA_MEMLIB_VECU_ENABLE)
/* VARIANT_EVAL_VECU */
# define RBA_MEMLIB_PRV_MSYNC()
#else
# define RBA_MEMLIB_PRV_MSYNC()       RBA_MEMLIB_MSYNC()
#endif

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"


/**
 **********************************************************************************************************************
 * rba_MemLib_SyncWrite8
 *
 * \brief    Synchronuous write operations which ensures that data is actually stored to RAM (by performing a read-back)
 *           Note: Previous writes are completed before this write only if going to the same memory as the passed pointer
 *           (execution duration corresponds to 2+1 copy-operations, incl. call)
 *
 * \param    Var_pvu8               Pointer to target memory
 * \param    Value_u8               Value that should be written
 * \return   none
 **********************************************************************************************************************
 **/
void rba_MemLib_SyncWrite8(volatile uint8* Var_pvu8, uint8 Value_u8)
{
    // Note: Function call is adding a SEQUENCE POINT before the excution of the function body

    // This memory barrier intends to ensure that all previous writes performed before this point
    // are completed from CPU-Core point of view.
    // However this does not guarantee that the data is updated in the target-RAM, as delays in bridges
    // towards target RAM are still possible.
    // But: this helps if the same memory is used here (see statement in the function header)
    RBA_MEMLIB_PRV_MSYNC();

    *Var_pvu8 = Value_u8;

    // Following memory barrier is intended to ensure that the following data-load is not just read from a write-cache
    // but that the write is performed towards the memory before the load happens
    RBA_MEMLIB_PRV_MSYNC();

    // dummy read (works pointing to a volatile variable)
    (void) *Var_pvu8;
    return;
}


/**
 **********************************************************************************************************************
 * rba_MemLib_SyncWrite16
 *
 * \brief    Synchronuous write operations which ensures that data is actually stored to RAM (by performing a read-back)
 *           Note: Previous writes are completed before this write only if going to the same memory as the passed pointer
 *           (execution duration corresponds to 2+1 copy-operations, incl. call)
 *
 * \param    Var_pvu16              Pointer to target memory
 * \param    Value_u16              Value that should be written
 * \return   none
 **********************************************************************************************************************
 **/
void rba_MemLib_SyncWrite16(volatile uint16* Var_pvu16, uint16 Value_u16)
{
    /* note: for explanation of the sequence please refer to SyncWrite8 */
    RBA_MEMLIB_PRV_MSYNC();

    *Var_pvu16 = Value_u16;

    RBA_MEMLIB_PRV_MSYNC();
    (void) *Var_pvu16;
    return;
}


/**
 **********************************************************************************************************************
 * rba_MemLib_SyncWrite32
 *
 * \brief    Synchronuous write operations which ensures that data is actually stored to RAM (by performing a read-back)
 *           Note: Previous writes are completed before this write only if going to the same memory as the passed pointer
 *           (execution duration corresponds to 2+1 copy-operations, incl. call)
 *
 * \param    Var_pvu32              Pointer to target memory
 * \param    Value_u32              Value that should be written
 * \return   none
 **********************************************************************************************************************
 **/
void rba_MemLib_SyncWrite32(volatile uint32* Var_pvu32, uint32 Value_u32)
{
    /* note: for explanation of the sequence please refer to SyncWrite8 */
    RBA_MEMLIB_PRV_MSYNC();

    *Var_pvu32 = Value_u32;

    RBA_MEMLIB_PRV_MSYNC();
    (void) *Var_pvu32;
    return;
}


/**
 **********************************************************************************************************************
 * rba_MemLib_SyncWriteBool
 *
 * \brief    Synchronuous write operations which ensures that data is actually stored to RAM (by performing a read-back)
 *           Note: Previous writes are completed before this write only if going to the same memory as the passed pointer
 *           (execution duration corresponds to 2+1 copy-operations, incl. call)
 *
 * \param    Var_pvb               Pointer to target memory
 * \param    Value_b               Value that should be written
 * \return   none
 **********************************************************************************************************************
 **/
void rba_MemLib_SyncWriteBool(volatile boolean* Var_pvb, boolean Value_b)
{
    RBA_MEMLIB_PRV_MSYNC();

    *Var_pvb = Value_b;

    RBA_MEMLIB_PRV_MSYNC();
    (void) *Var_pvb;
    return;
}


/**
 **********************************************************************************************************************
 * rba_MemLib_MSync
 *
 * \brief    Provide memory barrier
 *           (execution duration corresponds to 0+1 copy-operations, incl. call)
 *
 * \return   none
 **********************************************************************************************************************
 **/
void rba_MemLib_MSync(void)
{
    RBA_MEMLIB_PRV_MSYNC();
    return;
}


#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"

