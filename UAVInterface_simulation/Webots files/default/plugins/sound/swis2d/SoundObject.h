#ifndef SOUND_OBJECT_H
#define SOUND_OBJECT_H

//----------------------------------------------------------------------------------------------------------------------------
// File:     SoundObject.h
// Purpose:  Baseclass for SoundSource and SoundReceiver
// Author:   James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "Point.h"
#include <plugins/sound.h>

class SoundObject : public Point {
public:
  // creates a sound source with the given coordinates and angle
  SoundObject(ObjectRef ref);
  virtual ~SoundObject() {}

  // sets the position and angle of this source.
  void setPosition(double x, double y, double angle);
  
  // query Webots and update the object position and angle
  void updatePosition();

  // gets the angle of this source.
  double getAngle() const { return angle; }

  // sets the angle of this source.
  void setAngle(double angle) { this->angle = angle; }

  // gets the portion of the full intensity received at the given angle
  // angle: the offset angle on the receiver
  // returns: the proportional intensity at the angle 
  double getAngleStr(double angle) const { return 1.0; }
  
  // returns the given value mod 2 PI
  // val: the value to be adjusted
  // returns: the value mod 2 PI
  static double mod2PI(double val);

  // prints details for debugging
  virtual void print() const;
  
  // draw using OpenGL
  virtual void draw() const = 0;

protected:
  double angle;
  ObjectRef ref;  // unique reference provided by Webots
};

#endif
