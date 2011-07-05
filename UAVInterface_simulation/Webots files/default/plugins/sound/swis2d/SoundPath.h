#ifndef SOUND_PATH_H
#define SOUND_PATH_H

//----------------------------------------------------------------------------------------------------------------------------
// File:     SoundPath.h
// Purpose:  Defines a potential sound path eventually bouncing of walls
// Author:   James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "Point.h"
#include "Matrix.h"

class Segment;
class SoundSource;
class SoundReceiver;

// describes the set of reflections a sound emission may follow.
class SoundPath {

public:
  // creates an empty sound path
  SoundPath();

  // creates a sound path by adding a new wall to a previous path.
  // @param wall the first reflection wall
  // @param testPnt the test point in the arena for checking that we don't do illegal reflections
  // @param prevPath the sound path up until this new wall
  // @param walls the arena walls that might occlude this new path step
  // @param numWalls the number of arena walls
  SoundPath(const Segment *wall, const Point *testPnt, const SoundPath *prevPath, Segment *walls[], int numWalls);

  // check whether this path is possible for a sound emission.
  // @param testPnt the test point in the arena for checking that we don't do illegal reflections
  // @param prevPath the sound path up until this new wall
  // @param walls the arena walls that might occlude this new path step
  // @param numWalls the number of arena walls
  // @return if the path may be possible
  bool legalPath(const Point *testPnt, Segment *walls[], int numWalls) const;

  // gets the minimum distance the sound must travel in this path.
  double getMinimumDistance() const { return distance; }

  // returns the number of reflections in this path.
  int getReflections() const { return reflections; }

  // gets the nth reflection segment (0 is the most recent)
  const Segment *getReflectionSegment(int n) const;

  // gets the point transform matrix of the path.
  Matrix getPointTransform() const { return pointTransform; }

  // gets the angle transform matrix of the path.
  Matrix getAngleTransform() const { return angleTransform; }

  // transform the given angle along this path.
  double transformAngle(double angle) const;

  // transform the given point along this path.
  Point transformPoint(const Point *pnt) const;

  // gets the distance from the given sound source to the given destination point.
  double getDistance(const SoundSource *src, const SoundReceiver *dst) const;

  // returns whether the given sound source is visible to the given point.
  // @param src the sound source
  // @param dst the destination point
  // @param walls the arena walls that might occlude the path
  // @param numWalls the number of arena walls
  // @return if the source is visible
  bool visible(const SoundSource *src, const Point *dst, Segment *walls[], int numWalls) const;

  // returns the strength of the given sound source at the given point.
  double getStrength(const SoundSource *src, const SoundReceiver *dst) const;

  // gets the maximum intensity that could be received from a source  on this path with the given intensity.
  double getMaxStrength(double intensity) const;

  // returns the delay from the given sound source to the given point
  double getDelay(const SoundSource *src, const SoundReceiver *dst) const;

  // returns the angle at which the given source observes the given point
  double getAngle(const SoundSource *src, const SoundReceiver *dst) const;

  // computes the angle at which the given point observes the given source
  double getRecAngle(const SoundSource *src, const SoundReceiver *dst) const;

  // prints sound source details
  void print() const;
  
  // draw with OpenGL
  void draw(const SoundSource *src, const Point *dst, Segment * const walls[], int numWalls) const;

private:
  // the minimum distance the sound must travel along this path.
  double distance;

  // the number of reflections from the original source.
  int reflections;

  // the reflection segments, with the most recent first.
  const Segment *reflSeg[50];

  // matrix describing how a sound source position would change from these reflections
  Matrix pointTransform;

  // matrix describing how a sound source angle would change from these reflections
  Matrix angleTransform;

  // pointer to the previous path before the current reflection segment
  const SoundPath *prevPath;
};

#endif
