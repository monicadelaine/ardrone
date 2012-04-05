#ifndef SEGMENT_H
#define SEGMENT_H

//----------------------------------------------------------------------------------------------------------------------------
// File:     Segment.h
// Purpose:  Defines a 2d SoundPath segment or a wall for sound simulation
// Authors:  James Pugh (adapted as Webots plugin by Yvan Bourquin)
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "Point.h"
#include "Matrix.h"

class Segment {
public:
  // constructs a segment with the given coordinates
  Segment(double x1, double y1, double x2, double y2);
  Segment(Point start, Point end);
  Segment(const Point *start, const Point *end);

  // return the p of the internal representation.
  double getP() const { return p; }

  // return the q of the internal representation.
  double getQ() const { return q; }
  
  // return the r of the internal representation.
  double getR() const { return r; }

  // returns the angle of the internal representation.
  double getAngle() const { return angle; }

  // return the point transformation of the internal representation.
  const Matrix &getPointTransform() const { return pntmat; }

  // returns the angle transformation of the internal representation.
  const Matrix &getAngleTransform() const { return angmat; }

  // return the start point
  const Point *getA() const { return &a; }

  // return the end point.
  const Point *getB() const { return &b; }

  // returns the length of the segment.
  double getLength() const;

  // returns whether the passed segment intersects with this one.
  // returns: 4 full intersection, 2 one segment ends on the other, 1 ends meet, 0 no intersection
  int intersect(const Segment *seg) const;

  // returns the point of intersection between this and the given segment
  Point get_intersect(const Segment *seg) const;

  // reflect an angle across this segment.
  double reflect_angle(double angle) const;

  // reflect a point across this segment.
  Point reflect_point(const Point *pnt) const;

  // reflect a segment across this segment.
  // @param x the segment to be reflected
  // @return the segment location after it has been reflected
  Segment reflect_segment(const Segment *x) const;

  // check if the given point is occluded by the given segment
  // @param p the point we're checking
  // @param blk the segment that might be occluding
  // @return 2 if completely occluded, 1 if partially occluded, 0 if not occluded
  int occluded(const Segment *blk, const Point *p) const;

  // check if the given segment is completely occluded by the other given segment
  // @param seg the segment we're trying to see
  // @param blk the segment that might be occluding
  // @return if seg is completely occluded by blk
  bool occluded(const Segment *blk, const Segment *seg) const;

  // find which side of the segment is the point on
  // @return -1 if it's on one side, 0 if on neither, 1 if on the other
  int side(const Point *pnt) const;

  // compute the minimum distance from this to the given segment
  double minimum_distance(const Segment *seg) const;

  // compute the minimum distance from this to the given point
  double dist2point(const Point *pnt) const;

  // prints the segment.
  void print() const;
  
  // render the segment.
  void drawAsWall() const;
  void drawAsPath() const;
 
public:
  Point a, b;      // starting and ending point of the segment.
  double p, q, r;  // the segment in the p * x + q * y = r format
  double angle;    // the segment angle
  Matrix pntmat;   // the segment point reflection transformation 
  Matrix angmat;   // the segment angle reflection transformation 
  
private:
  // initializes the internal representation of the segment.
  void initialize();
};

#endif
