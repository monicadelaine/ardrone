#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

//----------------------------------------------------------------------------------------------------------------------------
// File:     SoundSource.h
// Purpose:  Definition of a sound source (Speaker)
// Author:   Yvan Bourquin
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "SoundObject.h"

// describes a sound source
class SoundSource : public SoundObject {
public:
  // creates an empty sound source.
  SoundSource(SpeakerRef ref);
  virtual ~SoundSource();
  
  // Webots ref that uniquely identifies this object
  SpeakerRef getRef() const { return (SpeakerRef)ref; }
  
  // common time representation in the plugin: 
  // elapsed time in microseconds since the simulation started
  typedef long long int Time;
  
  // store a sound sample comming from a Webots Speaker
  void insertSample(const short *data, int size, Time from);
  
  // retrieve a sound sample intended for a Webots Microphone
  void extractSample(short *data, int size, Time from) const;
  
  // clear old samples according up to specified time
  void clearSamplesUntil(Time then);
  
  bool hasAtLeastOneSample() { return head ? true : false; }

  // gets the intensity of this source.
  double getIntensity() const { return intensity; }

  // sets the intensity of this source.
  void setIntensity(double intensity) { this->intensity = intensity; }

  // overloaded
  virtual void draw() const;
  
  // set/get global sampling rate in bytes per microsecond
  static void setRate(double r) { rate = r; }
  static double getRate() { return rate; }
  
private:
  double intensity;

  // data structure for a single sound sample
  typedef struct _Sample_ {
    short *data;
    Time start;
    Time end;
    _Sample_ *next;
  } Sample;
  
  // each SoundSource manages one sample list
  Sample *head; // head of samples list
  Sample *tail; // tail of samples list (for faster insertion only)
  
  static double rate; // global sampling rate in bytes / microsecond
};

#endif
