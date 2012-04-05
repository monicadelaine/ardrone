//----------------------------------------------------------------------------------------------------------------------------
// File:     SoundReceiver.cpp
// Purpose:  Defines a Microphone of similar device for sound simulation
// Author:   Yvan Bourquin
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "SoundReceiver.h"
#include <iostream>

using namespace std;

SoundReceiver::SoundReceiver(MicrophoneRef ref) : SoundObject(ref) {
}

SoundReceiver::~SoundReceiver() {
}

void SoundReceiver::draw() const {
  glBegin(GL_LINES);
  glColor3f(0, 0, 1);
  glVertex3f(getX(), -0.1f, getY());
  glVertex3f(getX(),  0.1f, getY());
  glEnd();
}
