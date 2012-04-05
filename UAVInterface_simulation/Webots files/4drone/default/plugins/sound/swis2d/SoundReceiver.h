#ifndef SOUND_RECEIVER_H
#define SOUND_RECEIVER_H

//----------------------------------------------------------------------------------------------------------------------------
// File:     SoundReceiver.h
// Purpose:  Defines a Microphone of similar device for sound simulation
// Author:   Yvan Bourquin
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "SoundObject.h"

// describes a sound receiver.
class SoundReceiver : public SoundObject {
public:
  // creates an empty sound receiver.
  SoundReceiver(MicrophoneRef ref);
  virtual ~SoundReceiver();

  // reference for Webots
  MicrophoneRef getRef() const { return (MicrophoneRef)ref; }

  // overloaded
  virtual void draw() const;
  
private:
};

#endif
