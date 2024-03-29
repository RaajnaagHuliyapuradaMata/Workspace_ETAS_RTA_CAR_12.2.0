/*
 * This is vrtaIdleHandling.hpp, auto-generated for:
 *   Project: BswM_EcucValues
 *   Target:  VRTA
 *   Variant: MinGW
 *   Version: 3.0.51
 */
#ifndef VRTAIdleHandlingHPP
#define VRTAIdleHandlingHPP

// ---------------------------------------------------------------------------
// Idle subscribers must inherit from this
class vrtaIdleSubscriber {
public:
  vrtaIdleSubscriber *_next_subscriber;

  vrtaIdleSubscriber();
  virtual int Idle(unsigned core) = 0;

  static void Subscribe(vrtaIdleSubscriber *);
};


#endif /* VRTAIdleHandlingHPP */


