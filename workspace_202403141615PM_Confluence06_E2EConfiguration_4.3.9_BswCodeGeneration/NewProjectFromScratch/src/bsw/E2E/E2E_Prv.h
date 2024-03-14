

#ifndef E2E_PRV_H
#define E2E_PRV_H

/*
 **********************************************************************************************************************
 E2E LOCAL DEFINES
 **********************************************************************************************************************
*/

/* Masks required in uint8 context */
#define E2E_INLINE  LOCAL_INLINE
#define E2E_MASK_BYTE       0xFFU                   // Used in Profiles E2E_P01, E2E_P02 and E2E_Helpers
#define E2E_MASK_QWORD4     0xFFFFFFFFU             // Used in Profile E2E_P04 and E2E_Helpers
#define E2E_MASK_WORD       0xFFFFU                 // Used in Profiles E2E_P05, E2E_P06 and E2E_Helpers
#define E2E_MASK_QWORD      0xFFFFFFFFFFFFFFFFUL     // Used in Profile E2E_P07 and E2E_Helpers
#define E2E_MASK_L_NIBBLE   0x0FU                   // Used in Profiles E2E_P01 and E2E_P02
#define E2E_MASK_H_NIBBLE   0xF0U                   // Used in Profiles E2E_P01 and E2E_P02

/*
 **********************************************************************************************************************
 Prototypes for private routines for the E2E library
 **********************************************************************************************************************
 */

E2E_INLINE boolean E2E_Prv_CheckSyncCounter(uint8 *const SyncCounter_pu8, uint8 *const RepeatedDataCounter_pu8,
                                              uint8 SyncCounterInit_u8, uint8 MaxRepeatedDataCounter_u8);
E2E_INLINE uint8 E2E_Prv_MaxDeltaCounter(uint8 ReceivedCounter_u8, uint8 LastValidCounter_u8, uint16 MaxCounter_u16);
E2E_INLINE void  E2E_Prv_IncreaseCounter(uint8 * const Counter_pu8, uint8 Limit_u8);

 /*
 **********************************************************************************************************************
 private routines for E2E component
 **********************************************************************************************************************
 */

/**
 ***********************************************************************************************************************
 * E2E_Prv_CheckSyncCounter
 *
 * \brief Checks if synchronization of sender and receiver is running
 *
 * Description: private function for E2E library routines.
 *              If a maximum of repeated messages is reached the following messages synchronizes the connection.
 *              The synchronization counter is initialized with the maximum and decreased until enough messages
 *              are received.
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 * \param   uint8   const* SyncCounter_pu8      constant pointer to the synchronization counter
 * \param   uint8   const* RPCounter_pu8        constant pointer to the counter defining the maximal repeated messages
 * \param   uint8   SyncCounterInit_u8          initialization value for synchronization counter
 * \param   uint8   MaxRepeatedDataCounter_u8   Limit of the repeated messages
 * \return  boolean                             TRUE: synchronization is running
 *                                              FALSE: no synchronization necessary or enough sync messages are received
 *
 ***********************************************************************************************************************
 */

E2E_INLINE boolean E2E_Prv_CheckSyncCounter(uint8* const SyncCounter_pu8, uint8* const RepeatedDataCounter_pu8,
                                              uint8 SyncCounterInit_u8, uint8 MaxRepeatedDataCounter_u8)
{
    boolean sync_b = FALSE;


    if (*RepeatedDataCounter_pu8 <= MaxRepeatedDataCounter_u8)
    {
        if (*SyncCounter_pu8 > 0)
        {
            /* synchronization is running */
            (*SyncCounter_pu8)--;

            sync_b = TRUE;
        }
    }
    else
    {
        /* start synchronization */
        *SyncCounter_pu8 = SyncCounterInit_u8;

        sync_b = TRUE;
    }

    /* reset repetition counter */
    *RepeatedDataCounter_pu8 = 0;

    return (sync_b);
}

/**
 ***********************************************************************************************************************
 * E2E_Prv_IncreaseCounter
 *
 * \brief Increases Calculates the maximum of counter with respect to a limit
 *
 * Description: private function for E2E library routines.
 *              Service increases the counter until a maximum is reached
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   uint8   const* Counter_pu8  constant pointer to the counter
 * \param   uint8   Limit_u8            Limit of the counter
 * \return  void
 *
 ***********************************************************************************************************************
 */

E2E_INLINE void E2E_Prv_IncreaseCounter(uint8 *const Counter_pu8, uint8 Limit_u8)
{
    if (*Counter_pu8 < Limit_u8)
    {
        (*Counter_pu8)++;
    }
    else
    {
        *Counter_pu8 = Limit_u8;
    }
}

/**
 *******************************************************************************************************************
 * E2E_Prv_MaxDeltaCounter
 *
 * \brief       Calculates the deltaCounter as difference between ReceivedCounter and LastValidCounter.
 *              deltaCounter is restricted to the values [0, MaxCounter]
 *
 * Description: private function for E2E library routines.
 *              Service calculates the Delta of two counter and limits the result
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   uint8   ReceivedCounter_u8    New Counter
 * \param   uint8   LastValidCounter_u8   Old valid Counter
 * \param   uint16  MaxCounter_u16        Border of the counter. Counter maximally can reach MaxCounter.
 *                                        MaxCounter is the maximum of deltaCounter + 1.
 * \return  uint8   deltaCounter_u8       Difference of two Counters with respect to a limit
 *
 *******************************************************************************************************************
 */
//[SWS_E2E_00135]
E2E_INLINE uint8 E2E_Prv_MaxDeltaCounter(uint8 ReceivedCounter_u8, uint8 LastValidCounter_u8, uint16 MaxCounter_u16)
{
    uint8 deltaCounter_u8;


    /* Calculate the Delta of the two Counter, respect the limitation */
    if (ReceivedCounter_u8 >= LastValidCounter_u8)
    {
        deltaCounter_u8 = ReceivedCounter_u8 - LastValidCounter_u8;
    }
    else
    {
        deltaCounter_u8 = (uint8)((MaxCounter_u16 - LastValidCounter_u8) + ReceivedCounter_u8);
    }

    return (deltaCounter_u8);
}

/* E2E_PRV_H */
#endif
