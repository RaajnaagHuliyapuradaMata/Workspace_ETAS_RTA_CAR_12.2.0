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
#include "Eep.h"

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
rba_EaFs2_Prv_Eeprom_tst rba_EaFs2_Prv_Eeprom_st;
#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

/**
 *********************************************************************
 * rba_EaFs2_Prv_SetIterator: Updates the iterator
 *
 * Function to set the iterator
 *
 * \param    addr_u32: address to be set as iterator value
 * \return   (void)
 *********************************************************************
 */
void rba_EaFs2_Prv_SetIterator(uint32 addr_u32)
{
    rba_EaFs2_Prv_Eeprom_st.iterator_u32 = addr_u32;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EepromRead
 *
 * Wrapper function to call EEP Read API
 *
 * \param    addr_u32: Eeprom address
 * \param    *buffer_pu8: Data buffer
 * \param    length_u32: Length of the data
 * \return   (void)
 *********************************************************************
 */
void rba_EaFs2_Prv_EepromRead(uint32 addr_u32, uint8 *buffer_pu8,uint32 length_u32)
{
    rba_EaFs2_Prv_Eeprom_st.lastJobResult_en = EA_EEP_READ(addr_u32,buffer_pu8,length_u32);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EepromWrite
 *
 * Wrapper function to call EEP Write API
 *
 * \param    addr_u32: Eeprom address
 * \param    *buffer_pcu8: Data buffer
 * \param    length_u32: Length of the data
 * \return   (void)
 *********************************************************************
 */
void rba_EaFs2_Prv_EepromWrite(uint32 addr_u32, uint8 const *buffer_pcu8,uint32 length_u32)
{
    rba_EaFs2_Prv_Eeprom_st.lastJobResult_en = EA_EEP_WRITE(addr_u32,buffer_pcu8,length_u32);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EepromCompare
 *
 * Wrapper function to call EEP Compare API
 *
 * \param    addr_u32: Eeprom address
 * \param    *buffer_pcu8: Data buffer
 * \param    length_u32: Length of the data
 * \return   (void)
 *********************************************************************
 */
void rba_EaFs2_Prv_EepromCompare(uint32 addr_u32, uint8 const *buffer_pcu8,uint32 length_u32)
{
    rba_EaFs2_Prv_Eeprom_st.lastJobResult_en = Eep_Compare(addr_u32,buffer_pcu8,length_u32);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EepromErase
 *
 * Wrapper function to call EEP Erase API
 *
 * \param    addr_u32: Eeprom address
 * \param    length_u32: Length of the data
 * \return   (void)
 *********************************************************************
 */
void rba_EaFs2_Prv_EepromErase(uint32 addr_u32, uint32 length_u32)
{
    rba_EaFs2_Prv_Eeprom_st.lastJobResult_en = Eep_Erase(addr_u32, length_u32);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EepromStatus
 *
 * Function to poll the EEP status and result of the last placed request
 *
 * \param    (void)
 * \return   result_en: MemIf_JobResultType
 * \retval   MEMIF_JOB_FAILED: Last Eep Job Failed/Read request result is inconsistent
 * \retval   MEMIF_JOB_OK: Last Eep Job result is completed
 *********************************************************************
 */
MemIf_JobResultType rba_EaFs2_Prv_EepromStatus(void)
{
    MemIf_JobResultType result_en = MEMIF_JOB_FAILED;

    if(E_OK == rba_EaFs2_Prv_Eeprom_st.lastJobResult_en)
    {
        if((boolean)FALSE != rba_EaFs2_Prv_Eeprom_st.sync_b)
        {
            do
            {
                Eep_MainFunction();
                Ea_Rb_EepScheduleDependency();
            } while(MEMIF_JOB_PENDING == Eep_GetJobResult());
        }

        result_en = Eep_GetJobResult();

        if(MEMIF_BLOCK_INCONSISTENT == result_en)
        {
            result_en = MEMIF_JOB_FAILED;
        }
    }
    return result_en;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_PrepareEeprom
 *
 * Function to reset the RAM content validity
 *
 * \param    (void)
 * \return   (void)
 *********************************************************************
 */
void rba_EaFs2_Prv_PrepareEeprom(void)
{
    rba_EaFs2_Prv_Eeprom_st.upLimit_u32  = 0uL;
    rba_EaFs2_Prv_Eeprom_st.lowLimit_u32 = 0uL;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EepromLoad
 *
 * Function to load the Eeprom contents to internal buffer.
 *
 * \param    addr_u32: Eeprom address
 * \param    len_u16: Length of the data
 * \param    Mode_en: To read Forward or Backward
 * \return   result_en: MemIf_JobResultType
 *********************************************************************
 */
MemIf_JobResultType rba_EaFs2_Prv_EepromLoad(uint32 addr_u32,uint16 len_u16,rba_EaFs2_Prv_EepromLoadMode_ten Mode_en)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(
            (rba_EaFs2_Prv_Eeprom_st.lowLimit_u32 <= addr_u32) &&
            (rba_EaFs2_Prv_Eeprom_st.upLimit_u32 >= (addr_u32 + len_u16))
    )
    {
        /*content already in buffer*/
        result_en = MEMIF_JOB_OK;
    }
    else
    {

        if(rba_EaFs2_Prv_Eeprom_st.state_en != RBA_EAFS2_PRV_LOAD_BUSY_E)
        {
            /* sanitize address and length ranges*/
            if(
                    (EA_EEP_START_OFFSET <= (addr_u32 + 0uL) ) && /* address to read is greater than or equal to zero*/
                    ((EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE) >= (addr_u32 + len_u16)) &&/*read is not allowed after end of data area*/
                    (len_u16 <= RBA_EAFS2_PRV_FATS_PER_READ)
            )
            {
                if(RBA_EAFS2_PRV_NXTLOAD_E == Mode_en)
                {
                    /* read forwards*/
                    rba_EaFs2_Prv_Eeprom_st.loadAddr_u32 = rba_EaFs2_Prv_LibMin(
                            addr_u32,
                            ((EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE) - RBA_EAFS2_PRV_FATS_PER_READ));
                }
                else
                {
                    /* read backwards*/
                    rba_EaFs2_Prv_Eeprom_st.loadAddr_u32 = rba_EaFs2_Prv_LibMax(
                            ( addr_u32  + len_u16 ),
                            ( EA_EEP_START_OFFSET + RBA_EAFS2_PRV_FATS_PER_READ)
                    );

                    rba_EaFs2_Prv_Eeprom_st.loadAddr_u32 -= RBA_EAFS2_PRV_FATS_PER_READ;
                }

                /* Reset buffer access*/
                rba_EaFs2_Prv_PrepareEeprom();

                rba_EaFs2_Prv_EepromRead(rba_EaFs2_Prv_Eeprom_st.loadAddr_u32,
                        &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                        RBA_EAFS2_PRV_FATS_PER_READ
                );

                rba_EaFs2_Prv_Eeprom_st.state_en = RBA_EAFS2_PRV_LOAD_BUSY_E;
            }
            else
            {
                result_en = MEMIF_JOB_FAILED;
            }
        }

        if(MEMIF_JOB_PENDING == result_en)
        {
            result_en = rba_EaFs2_Prv_EepromStatus();
        }

        if(result_en != MEMIF_JOB_PENDING)
        {
            rba_EaFs2_Prv_Eeprom_st.state_en = RBA_EAFS2_PRV_LOAD_IDLE_E;

            if(result_en == MEMIF_JOB_OK)
            {
                /* update low and Upper limts*/
                rba_EaFs2_Prv_Eeprom_st.lowLimit_u32 = rba_EaFs2_Prv_Eeprom_st.loadAddr_u32;
                rba_EaFs2_Prv_Eeprom_st.upLimit_u32  = (rba_EaFs2_Prv_Eeprom_st.loadAddr_u32 + RBA_EAFS2_PRV_FATS_PER_READ);
            }
        }
    }

    return result_en;
}

#if(FALSE != EA_PRV_DEBUGGING)
/**
 *********************************************************************
 * rba_EaFs2_Prv_Initialize_Eeprom():
 *
 * This function used as initialzation for global variable
 *
 * \param    None
 *
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_EaFs2_Prv_Initialize_Eeprom(void)
{
    uint16 index=0;

    rba_EaFs2_Prv_Eeprom_st.state_en=RBA_EAFS2_PRV_LOAD_IDLE_E;

    for(index =0;index < (EA_DATA_BUFFER_SIZE/4u);index++)
    {
        rba_EaFs2_Prv_Eeprom_st.buffer_u.u32[index] = 0;
    }
    for(index =0;index < EA_DATA_BUFFER_SIZE;index++)
    {
        rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[index] = 0;
    }

    rba_EaFs2_Prv_Eeprom_st.iterator_u32=0;
    rba_EaFs2_Prv_Eeprom_st.upLimit_u32=0;
    rba_EaFs2_Prv_Eeprom_st.lowLimit_u32=0;
    rba_EaFs2_Prv_Eeprom_st.indexer_u32=0;
    rba_EaFs2_Prv_Eeprom_st.loadAddr_u32=0;
    rba_EaFs2_Prv_Eeprom_st.sync_b=FALSE;
    rba_EaFs2_Prv_Eeprom_st.lastJobResult_en=E_OK;
}
#endif
#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
