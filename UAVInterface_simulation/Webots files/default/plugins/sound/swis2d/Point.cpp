//----------------------------------------------------------------------------------------------------------------------------
// File:     Point.cpp
// Purpose:  Describes a point, from the geometrical point of view, in 2D
// Authors:  James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "Point.h"
#include <iostream>
#include <cmath>

using namespace std;

double Point::dist(const Point *p) const {
  return sqrt(pow(p->getX() - x, 2.0) + pow(p->getY() - y, 2.0));
}

bool Point::nearlyEquals(const Point *p) const {
  return dist(p) < 0.2;
}

void Point::print() const {
  cout << "x: " << x << ", y: " << y << "\n";
}

double Point::getAngle(const Point *a, const Point *b) {
  return atan2(b->y - a->y, b->x - a->x);
}
