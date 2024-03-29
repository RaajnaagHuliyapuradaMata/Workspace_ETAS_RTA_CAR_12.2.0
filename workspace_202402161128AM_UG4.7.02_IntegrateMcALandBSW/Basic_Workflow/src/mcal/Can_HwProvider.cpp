/*******************************************************************************
 * Autogenerated by MCALgen for CAN V3.0.51.35438: Can_HwProvider.cpp
 *******************************************************************************/

/************************************************************************************************************
 *                              Include Files
 ***********************************************************************************************************/
#include "Can_HwProvider.hpp"

/************************************************************************************************************
 *                         Useful Macros
 ***********************************************************************************************************/

/************************************************************************************************************
 *                         Global Variables
 ***********************************************************************************************************/

/************************************************************************************************************
 *                         Private Variables
 ***********************************************************************************************************/
#define CAN_PROVIDER_COUNT (1)
extern Can_HwProvider * Can_Virtual_HwProviderPtr;
static Can_HwProvider* const * Can_HwProviders[] = {
  &Can_Virtual_HwProviderPtr
};


/************************************************************************************************************
 *                         API
 ***********************************************************************************************************/

Can_HwProvider* Can_GetHwProvider(uint32 ProviderID) {
  if (ProviderID < CAN_PROVIDER_COUNT) {
    return *(Can_HwProviders[ProviderID]);
  }
  return NULL_CPP_PTR;
}

Can_Hw *Can_GetHw(Can_Hwp_UpCall UpCall,
                  uint32         Context,
                  uint32         ProviderID,
                  uint32         HwId,
                  uint32         FallbackProviderID,
                  uint32         FallbackHwId) {

  Can_HwProvider* provider = Can_GetHwProvider(ProviderID);

  if (provider != NULL_CPP_PTR) {
    if (provider->IsHardwarePresent(HwId)) {
      return provider->GetHardware(UpCall, Context, HwId);
    }
    else {
      provider = Can_GetHwProvider(FallbackProviderID);
      if (provider != NULL_CPP_PTR) {
        if (provider->IsHardwarePresent(FallbackHwId)) {
          return provider->GetHardware(UpCall, Context, FallbackHwId);
        }
      }
    }
  }

  return NULL_CPP_PTR;
}

bool Can_FramePassesFiltering(uint32 frameId, const Can_Hwp_RxHwObject* dest) {
  // Return `true` if a frame with ID `frameId` would pass filtering on the receive hardware object `dest`. Otherwise,
  // return `false`.

  // From AR 4.1 onwards, hardware objects **may** own multiple `CanHwFilter`s each with `CanHwFilterCode`s and
  // `CanHwFilterMask`s.
  for(unsigned hwFilterIdx = 0; hwFilterIdx < dest->numHwFilters; hwFilterIdx++){
    const Can_Hwp_HwFilter& filter = dest->hwFilters[hwFilterIdx];

    // [$CANHWP 31] [$CANHWP 32] [$CANHWP 47]
    if(
      (dest->rxType == CAN_HWP_RXTYPE_FULL && frameId == filter.code) ||
      (dest->rxType == CAN_HWP_RXTYPE_BASIC && (frameId & filter.mask) == (filter.code & filter.mask))
    ) {
      return true;
    }
  }

  // [$CANHWP 1301]
  // We're performing a logical OR across all filters, so by default this frame should be rejected. If it passes any
  // of the filters above then we return true.
  return false;
}

void Can_LoadRxObjFromConfig(Can_Hwp_RxHwObject* dest, const Can_Hwp_RxHwObject* config) {
  // Load the data in one `Can_Hwp_RxHwObject` instance, with data from another configuration instance.

  dest->handle = config->handle;
  dest->rxType = config->rxType;
  dest->idType = config->idType;


  // [$CAN 1299]
  dest->numHwFilters = config->numHwFilters;
  dest->hwFilters = new Can_Hwp_HwFilter[dest->numHwFilters];

  for (unsigned hwFilterIdx = 0; hwFilterIdx < dest->numHwFilters; hwFilterIdx++) {
    Can_Hwp_HwFilter* filter = (Can_Hwp_HwFilter*)&dest->hwFilters[hwFilterIdx];
    const Can_Hwp_HwFilter& filterConfig = config->hwFilters[hwFilterIdx];

    filter->code = filterConfig.code;
    filter->mask = filterConfig.mask;
  }
  dest->objectPayloadLength = config->objectPayloadLength;
}

