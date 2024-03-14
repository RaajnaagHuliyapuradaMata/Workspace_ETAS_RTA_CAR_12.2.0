/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Ea.h"

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#include "Ea_Prv.h"
#include "Ea_Cfg_SchM.h"
#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

/**
 *********************************************************************
 * Ea_Rb_CheckInit(): Check if EA is initialised
 *
 * This function checks if EA is initialised or not
 *
 * \param    ApiId_u8:    API id of the function which calls
 * \return   Std_ReturnType
 * \retVal   E_OK   : EA is initialised
 * \retVal   E_NOT_OK   : EA is not initialised
 *********************************************************************
 */
Std_ReturnType Ea_Rb_CheckInit(uint8 ApiId_u8)
{
    Std_ReturnType result_en = E_OK;

    /* Check init status */
    if(Ea_GlobModuleState_st == MEMIF_UNINIT)
    {
        RBA_EAFS2_DET_REPORTERROR(ApiId_u8, EA_E_UNINIT);
        result_en = E_NOT_OK;
    }

    return result_en;
}

/**
 *********************************************************************
 * Ea_Rb_CheckBlkNr(): Check if Block number is in allowed range
 *
 * This function checks if Block number is in allowed range
 *
 * \param    ApiId_u8:    API id of the function which calls
 * \param    BlockNumber_u16:    Block number to be verfied
 * \return   Std_ReturnType
 * \retVal   E_OK   : Block number is in allowed range
 * \retVal   E_NOT_OK   : Block number is not in allowed range
 *********************************************************************
 */
Std_ReturnType Ea_Rb_CheckBlkNr(uint8 ApiId_u8, uint16 BlockNumber_u16)
{
    Std_ReturnType result_en = E_OK;

    if(BlockNumber_u16 > (EA_NUM_BLOCKS-1u))
    {
        RBA_EAFS2_DET_REPORTERROR(ApiId_u8, EA_E_INVALID_BLOCK_NO);
        result_en = E_NOT_OK;
    }

    return result_en;
}

/**
 *********************************************************************
 * Ea_Rb_CheckDefragErr(): Handle the Defrag Error
 *
 * This function handles the Defrag Error
 *
 * \param    ApiId_u8:    API id of the function which calls
 * \param    ErrTyp_u8:    Defrag Error type
 * \return   void
 *********************************************************************
 */
void Ea_Rb_CheckDefragErr(uint8 ApiId_u8, uint8 ErrTyp_u8)
{
    RBA_EAFS2_DET_REPORTERROR(ApiId_u8,ErrTyp_u8);

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
    SchM_Enter_Ea_Order();
    Ea_Rb_MigrationStaus_u8 = EA_RB_MIGRATION_ERROR;
    SchM_Exit_Ea_Order();
#endif

    /* Callback function from user for the error handling */
    if(Ea_Prv_Common_cst.Ea_Rb_ErrorHandling_CallBack_pfct != NULL_PTR)
    {
        (void)ApiId_u8;
        (*Ea_Prv_Common_cst.Ea_Rb_ErrorHandling_CallBack_pfct)((uint8)ErrTyp_u8);
    }
}

/**
 *********************************************************************
 * Ea_Rb_CheckDataPtr(): Check if data buffer pointer is valid
 *
 * This function checks if dats buffer pointer is valid
 *
 * \param    ApiId_u8:    API id of the function which calls
 * \param    * DataBufferPtr:    Data buffer to be verfied
 * \return   Std_ReturnType
 * \retVal   E_OK   : Data buffer is valid
 * \retVal   E_NOT_OK   : Data buffer is not valid
 *********************************************************************
 */
Std_ReturnType Ea_Rb_CheckDataPtr(uint8 ApiId_u8,uint8 const * DataBufferPtr)
{
    Std_ReturnType result_en = E_OK;

    if(NULL_PTR == DataBufferPtr)
    {
        RBA_EAFS2_DET_REPORTERROR(ApiId_u8,EA_E_PARAM_POINTER);
        result_en = E_NOT_OK;
    }

    return result_en;
}


/**
 *********************************************************************
 * rba_EaFs2_Prv_CheckBlkLength(): Check if requested length is valid
 *
 * This function checks if the requested length is valid
 *
 * \param    ApiId_u8:    API id of the function which calls
 * \param    blkNum_u16:    Block number for which the length is to be verfied
 * \param    blkoffset_u16:    block offset
 * \param    blklength_u16:    block length
 * \return   Std_ReturnType
 * \retVal   E_OK   : Block Length & Offset is valid
 * \retVal   E_NOT_OK   : Block Length & Offset is not valid
 *********************************************************************
 */
Std_ReturnType rba_EaFs2_Prv_CheckBlkLength(uint8 ApiId_u8,uint16 blkNum_u16, uint16 blkoffset_u16, uint16 blklength_u16)
{
    Std_ReturnType result_en = E_OK;

    if((blklength_u16 + blkoffset_u16) > Ea_BlockProperties_st[blkNum_u16].Ea_BlkLength_u16)
    {
        RBA_EAFS2_DET_REPORTERROR(ApiId_u8,EA_E_INVALID_BLOCK_LEN);
        result_en = E_NOT_OK;
    }

    return result_en;
}

/**
 *********************************************************************
 * Ea_Rb_CheckModuleBusy(): Check if EA is Busy processing another user request
 *
 * This function checks if EA is ready to accept new user requests
 *
 * \param    ApiId_u8:    API id of the function which calls
 * \return   Std_ReturnType
 * \retVal   E_OK   : EA is initialised
 * \retVal   E_NOT_OK   : EA is not initialised
 *********************************************************************
 */
Std_ReturnType Ea_Rb_CheckBusy(uint8 ApiId_u8)
{
    Std_ReturnType result_en = E_OK;

    /* Check busy status */
    if(EA_RB_NO_ORDER_E != Ea_Prv_Order_st.OrderType_en)
    {
        RBA_EAFS2_DET_REPORTRUNTIMEERROR(ApiId_u8, EA_E_BUSY);
        result_en = E_NOT_OK;
    }

    return result_en;
}



#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
