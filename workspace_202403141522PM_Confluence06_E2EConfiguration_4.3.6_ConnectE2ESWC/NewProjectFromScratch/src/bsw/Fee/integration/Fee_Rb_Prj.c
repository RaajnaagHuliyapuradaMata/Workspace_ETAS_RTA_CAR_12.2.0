/*
* *********************************************************************************************************************
* This is a template file.
* It defines integration functions which are necessary to be implemented.
* The integrator must complete the templates before deploying software containing functions defined in this file.
* Once templates have been completed, the integrator should delete the "removal part"
* Note: The integrator is responsible for updates made to this file.
*
* To suppress following error message, define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT
* during compiler invocation (e.g. -DNOT_READY_FOR_TESTING_OR_DEPLOYMENT)
* The suppression of the error only allows the user to proceed with the building phase.
* *********************************************************************************************************************
*/

/*---- Removal start ----*/
/* Hint for the integrator: remove everything between Removal start and Removal end after proper implementation */

#ifndef NOT_READY_FOR_TESTING_OR_DEPLOYMENT
# error The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#else
# warning The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#endif /* NOT_READY_FOR_TESTING_OR_DEPLOYMENT */
/*---- Removal end ----*/

/*
 **********************************************************************************************************************
 *
 **********************************************************************************************************************
*/

/**
 **********************************************************************************************************************
 * \file   Fee_Rb_Prj.c
 * \brief  Extension definition provided by the project and used by the FeeHL/FeeFsX
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Fls.h"            // Required for the define FLS_AR_RELEASE_MINOR_VERSION and extern Fls_Compare

#if (defined(FLS_AR_RELEASE_MINOR_VERSION) && (0 == FLS_AR_RELEASE_MINOR_VERSION))

#include "Fee_Rb_Prj.h"     // Required for extern Fls_BlankCheck
#include "Fee_Prv_Cfg.h"    // Required for the define FEE_PRV_CFG_RB_NR_PAGE_BUFFER and typedef Fee_Prv_Cfg_MediumBuffer(2)_tun

/* Find out the erased pattern of the data flash */
#if (defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

# include "rba_FeeFs1x_Prv_Cfg.h"   // Required for the define RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT
# define FEE_RB_PRJ_ERASEPATTERN_8BIT    (RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT)

#elif (defined(RBA_FEEFS2_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS2_PRV_CFG_ENABLED))

# include "rba_FeeFs2_Prv_Cfg.h"    // Required for the define RBA_FEEFS2_PRV_CFG_ERASE_PATTERN_8BIT
# define FEE_RB_PRJ_ERASEPATTERN_8BIT    (RBA_FEEFS2_PRV_CFG_ERASE_PATTERN_8BIT)

#elif (defined(RBA_FEEFS3_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS3_PRV_CFG_ENABLED))

# include "rba_FeeFs3_Prv_Cfg.h"    // Required for the define RBA_FEEFS3_PRV_CFG_ERASE_PATTERN_8BIT
# define FEE_RB_PRJ_ERASEPATTERN_8BIT    (RBA_FEEFS3_PRV_CFG_ERASE_PATTERN_8BIT)

#else

# error No erase pattern found

#endif


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* At least 1024 bytes blank check shall be possible for all basic algorithm operations */
#define FEE_RB_PRJ_BLANK_CHECK_SIZE_BASE    (1024u)

/* Assure FEE_RB_PRJ_BLANK_CHECK_SIZE_BASE is bigger or equal to the size of the medium buffers -
 * if this is not the case then FEE_RB_PRJ_BLANK_CHECK_SIZE_BASE must be set to a higher value
 */
#if (1 <= FEE_PRV_CFG_RB_NR_PAGE_BUFFER)
extern const uint8 Fee_Rb_PrjAssert_Fee_Prv_Cfg_MediumBuffer_tun_BiggerThanBlankChecksizeBase [(FEE_RB_PRJ_BLANK_CHECK_SIZE_BASE <= sizeof(Fee_Prv_Cfg_MediumBuffer_tun ))?1:-1];
#endif
#if (2 <= FEE_PRV_CFG_RB_NR_PAGE_BUFFER)
extern const uint8 Fee_Rb_PrjAssert_Fee_Prv_Cfg_MediumBuffer_tun2_BiggerThanBlankChecksizeBase[(FEE_RB_PRJ_BLANK_CHECK_SIZE_BASE <= sizeof(Fee_Prv_Cfg_MediumBuffer2_tun))?1:-1];
#endif

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CONST_8
#include "Fee_MemMap.h"
static const uint32 Fee_Rb_Prj_ErasePattern_acu8[FEE_RB_PRJ_BLANK_CHECK_SIZE_BASE] = {FEE_RB_PRJ_ERASEPATTERN_8BIT};
#define FEE_STOP_SEC_CONST_8
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/**
 * \brief       Fls_BlankCheck checks whether a flash area can be programmed
 *              On standard single ended flashes this can typically be implemented
 *              by comparing the flash data with its erase pattern. Fls_Compare can be reused here.
 *              On complementary flash technologies normally dedicated hardware operations are available which
 *              require a hardware specific implementation in an own flash driver function
 *              This cannot be achived by reusing Fls_Compare.
 *              Most devices have a single ended flash so typically Fls_Compare can be used.
 *
 * \param       TargetAddress   The flash address that shall be blank checked
 * \param       Length          The amount of bytes to blank check, note: this should be page aligned
 *
 * \attention   Fls_BlankCheck was standardized by AUTOSAR only in version >= AR4.2
 *              In case a <AR4.2 Fls version is used Fls_BlankCheck is not available.
 *              The Fee however needs a Fls_BlankCheck implementation to work properly.
 *
 * \attention   Fls_Compare will compare the data in Fee_Rb_Prj_ErasePattern_acu8 against the data in the DFLASH.
 *              Therefore Fee_Rb_Prj_ErasePattern_acu8 must absolutely be bigger or equal the biggest blank check job.
 *              Else Fls_Compare will read data bytes behind Fee_Rb_Prj_ErasePattern_acu8 which can result
 *              in undefined behavior.
 *
 * \retval      E_OK        Job accepted and will be processed asynchronously
 * \retval      E_NOT_OK    Job not accepted by below Fls_Compare or array containing erase pattern too small
*/
Std_ReturnType Fls_BlankCheck(Fls_AddressType TargetAddress, Fls_LengthType Length)
{
    Std_ReturnType retVal_en;

    if(sizeof(Fee_Rb_Prj_ErasePattern_acu8) >= Length)
    {
        retVal_en = Fls_Compare(TargetAddress, (uint8 const *)Fee_Rb_Prj_ErasePattern_acu8, Length);
    }
    else
    {
        /* This should never be reached. The array Fee_Rb_Prj_ErasePattern_acu8 has to be big enough.
         * To absolutely guarantee any out of bounds read access this branch is added though.
         */
        retVal_en = E_NOT_OK;
    }

    return(retVal_en);
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif /* (defined(FLS_AR_RELEASE_MINOR_VERSION) && (0 == FLS_AR_RELEASE_MINOR_VERSION)) */
