//--------------------------------------------------------------------------
// File:     SoundSource.cpp
// Purpose:  Definition of a SoundSource (Speaker)
// Author:   Yvan Bourquin
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "SoundSource.h"
#include <string.h>

using namespace std;

double SoundSource::rate;  // global rate in bytes per microsecond

SoundSource::SoundSource(SpeakerRef ref) : SoundObject(ref) {
  intensity = 0.0;
  head = NULL;
  tail = NULL;
}

SoundSource::~SoundSource() {
  // delete sound samples list
  Sample *s = head;
  while (s) {
    Sample *t = s;
    s = s->next;
    delete [] t->data;
    delete t;
  }
}

void SoundSource::insertSample(const short *data, int size, Time from) {

  Sample *s = new Sample;
  s->data = new short[size];
  memcpy(s->data, data, size * sizeof(short));
  s->start = from;
  s->end = from + (int)(size / rate + 0.5);
  s->next = NULL;
  
  // insert at the end of the list
  if (tail) {
    if (from < tail->end) {
      // overlapping sound samples !
      // ajust size end, we dont need to realloc the data
      // this is similar to overwriting the data
      tail->end = from;
    }
  
    // link at end
    tail->next = s;
    tail = s;
  }
  else
    // fist element in this list
    head = tail = s;
}
  
void SoundSource::extractSample(short *data, int size, Time from) const {

  Time till = from + (int)(size / rate + 0.5);

  // clear whole sample
  memset(data, 0, size * sizeof(short));
  
  Sample *s = head;
  while (s) {
    if (from >= s->start && till < s->end) {
      short *src = s->data + (int)(rate * (from - s->start));
      memcpy(data, src, size * sizeof(short));
      break; // don't need to look any further
    }
    if (from < s->start && till >= s->end) {
      short *dest = data + (int)(rate * (s->start - from));
      int n = (int)(rate * (s->end - s->start));
      memcpy(dest, s->data, n * sizeof(short));
      break;
    }
    else if (from >= s->start && from < s->end) {
      short *src = s->data + (int)(rate * (from - s->start));
      int n = (int)(rate * (s->end - from));
      memcpy(data, src, n * sizeof(short));
    }
    else if (till >= s->start && till < s->end) {
      short *dest = data + (int)(rate * (s->start - from));
      int n = (int)(rate * (till - s->start));
      memcpy(dest, s->data, n * sizeof(short));
    }

    s = s->next;
  }
}

void SoundSource::clearSamplesUntil(Time then) {
  while (head && head->end <= then) {
    Sample *t = head;
    head = head->next;
    delete [] t->data;
    delete t;
  }
  
  if (head == NULL)
    tail = NULL;
}

void SoundSource::draw() const {
  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(getX(), -0.1f, getY());
  glVertex3f(getX(),  0.1f, getY());
  glEnd();
}
