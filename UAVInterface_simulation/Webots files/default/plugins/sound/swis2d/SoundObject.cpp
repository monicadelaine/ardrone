//----------------------------------------------------------------------------------------------------------------------------
// File:     SoundObject.cpp
// Purpose:  Baseclass for SoundSource and SoundReceiver
// Author:   James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "SoundObject.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>

using namespace std;

SoundObject::SoundObject(ObjectRef ref) : Point() {
  this->ref = ref;
  this->angle = 0.0;
}

void SoundObject::setPosition(double x, double y, double angle) { 
  this->x = x;
  this->y = y;
  this->angle = angle;
}

void SoundObject::updatePosition() {
  const float *pos = webots_sound_get_translation(ref);
  setX(pos[0]);
  setY(pos[2]);
  const float *norm = webots_sound_get_normal_axis(ref);
  setAngle(mod2PI(M_PI - atan2(norm[0], norm[2])));
}

void SoundObject::print() const {
  cout << "Position: (" << x << ", " << y << ") Angle: " << angle << "\n";
}

double SoundObject::mod2PI(double val) {
  while (val > M_PI) { val -= 2.0 * M_PI; }
  while (val < -M_PI) { val += 2.0 * M_PI; }
  return val;
}
