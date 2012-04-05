#ifndef POINT_H
#define POINT_H

//----------------------------------------------------------------------------------------------------------------------------
// File:     Point.h
// Purpose:  Describes a point, from the geometrical point of view, in 2D
// Authors:  James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

class Point {
public:
  // constructors
  Point() { x = 0.0; y = 0.0; }
  Point(double x, double y) { this->x = x; this->y = y; }
  virtual ~Point() {}

  double getX() const { return x; }
  double getY() const { return y; }

  void setX(double x) { this->x = x; }
  void setY(double y) { this->y = y; }

  // returns the distance between passed point and this one.
  double dist(const Point *p) const;
  
  // tests the nearly equality.
  bool nearlyEquals(const Point *p) const;

  // prints the point.
  virtual void print() const;
  
  // returns the angle formed by two points
  // a: the first point in line
  // b: the second point in line
  // returns: the angle of the connecting line, from -PI to PI
  static double getAngle(const Point *a, const Point *b);

public:
  double x, y;
};

#endif
