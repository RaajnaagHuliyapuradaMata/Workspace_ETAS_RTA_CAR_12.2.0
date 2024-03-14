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

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/

#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"

/** ******************************************************************************************
 * \brief   Find out whether two strings are equal
 *
 * \param   str1_pcchr      The first string
 * \param   str2_pcchr      The second string
 * \param   maxLength_u32   Never compare more than maxLength_u32 bytes to avoid any out of bound read
 *
 * \retval  TRUE    Both strings are equal until string terminator sign
 * \retval  FALSE   Both strings are not equal or maxLength_u32 reached
*/
boolean rba_MemLib_StrIsEqual(char const * str1_pcchr, char const * str2_pcchr, uint32 maxLength_u32)
{
    uint32 a_u32;
    boolean isEqual_b = FALSE;

    for(a_u32 = 0u; (a_u32 < maxLength_u32) && (str1_pcchr[a_u32] == str2_pcchr[a_u32]); a_u32++)
    {
        if('\0' == str1_pcchr[a_u32])
        {
            /* String termination found, stop compare */
            isEqual_b = TRUE;
            break;
        }
    }

    return(isEqual_b);
}

/** ******************************************************************************************
 * \brief   Returns the length of a string
 *
 * \param   str_pcchr       The string
 * \param   maxLength_u32   Never inspect more than maxLength_u32 bytes to avoid any out of bound read
 *
 * \retval  The number of characters in the string, terminating \0 NOT included
 *          Will be == maxLength_u32 if the terminating \0 was not found
*/
uint32 rba_MemLib_StrLen(char const * str_pcchr, uint32 maxLength_u32)
{
    uint32 a_u32;

    for(a_u32 = 0u; a_u32 < maxLength_u32; a_u32++)
    {
        if('\0' == str_pcchr[a_u32])
        {
            /* String termination found, stop copying */
            break;
        }
    }

    return(a_u32);
}

/** ******************************************************************************************
 * \brief   Copy one string to another
 *
 * \param   srcStr_pcchr    The source string
 * \param   dstStr_pchr     The destination string
 * \param   maxLength_u32   Never copy more than maxLength_u32 bytes to avoid any out of bound write
 *
 * \retval  The number of characters copied, terminating \0 NOT included
 *          Will be == maxLength_u32 if the terminating \0 was not found
*/
uint32 rba_MemLib_StrCpy(char const * srcStr_pcchr, char * dstStr_pchr, uint32 maxLength_u32)
{
    uint32 a_u32;

    for(a_u32 = 0u; a_u32 < maxLength_u32; a_u32++)
    {
        dstStr_pchr[a_u32] = srcStr_pcchr[a_u32];
        if('\0' == srcStr_pcchr[a_u32])
        {
            /* String termination found, stop copying */
            break;
        }
    }

    return(a_u32);
}

/** ******************************************************************************************
 * \brief   Concatenate two strings
 *
 * \param   srcStr_pcchr    The source string which shall be appended to dstStr
 * \param   dstStr_pchr     The destination string where the srcStr shall be appended at the end
 * \param   maxLength_u32   Never create a string longer than maxLength_u32 to avoid any out of bound write
 *
 * \retval  The number of characters in the final string, terminating \0 NOT included
 *          Will be == maxLength_u32 if the terminating \0 was not found
*/
uint32 rba_MemLib_StrCat(char const * srcStr_pcchr, char * dstStr_pchr, uint32 maxLength_u32)
{
    uint32 lenDstStr_u32;
    uint32 lenSrcStr_u32;

    /* Find out the position of the terminating \0 in the destination string */
    lenDstStr_u32 = rba_MemLib_StrLen(dstStr_pchr, maxLength_u32);
    /* Append the string overwriting the \0 in the destination string
     * Note: lenDstStr_u32 is guaranteed <= maxLength_u32
     */
    lenSrcStr_u32 = rba_MemLib_StrCpy(srcStr_pcchr, dstStr_pchr + lenDstStr_u32, maxLength_u32 - lenDstStr_u32);

    return(lenDstStr_u32 + lenSrcStr_u32);
}

#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"
