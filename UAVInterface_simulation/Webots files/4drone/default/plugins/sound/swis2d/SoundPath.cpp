//----------------------------------------------------------------------------------------------------------------------------
// File:     SoundPath.cpp
// Purpose:  Defines a potential sound path eventually bouncing of walls
// Author:   James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "SoundPath.h"
#include "Segment.h"
#include "SoundSource.h"
#include "SoundReceiver.h"
#include "Config.h"
#include <iostream>
#include <cmath>
#include <stdlib.h> 

using namespace std;

SoundPath::SoundPath () {
  distance = 0;
  reflections = 0;
  pointTransform = Matrix(3);
  angleTransform = Matrix(2);
  prevPath = NULL;
  reflSeg[0] = NULL;
}

SoundPath::SoundPath(const Segment *wall, const Point *testPnt, const SoundPath *prevPath, Segment *walls[], int numWalls) {

  Segment transformedFirst(0,0,0,0);                // First wall transformed to the reflected space
  int i;

  reflections = 1 + prevPath->getReflections();
  this->prevPath = prevPath;

  /* Multiply old transforms by new wall transforms */
  Matrix tempPointTransform = prevPath->getPointTransform();
  Matrix tempAngleTransform = prevPath->getAngleTransform();
  pointTransform = (wall->getPointTransform()).mult(&tempPointTransform);
  angleTransform = (wall->getAngleTransform()).mult(&tempAngleTransform);

  /* Add new wall to old list */
  reflSeg[0] = wall;
  for (i = 1; i < reflections; i++) {
    reflSeg[i] = prevPath->getReflectionSegment(i-1);
  }

  /* Check that path is legal */
  if (legalPath(testPnt, walls, numWalls)) {

    /* Move first wall to reflected space, and find minimum distance to new wall */
    transformedFirst = Segment( transformPoint(&reflSeg[reflections-1]->a),
                transformPoint(&reflSeg[reflections-1]->b) );

    /* We use minimum distance to avoid creating unnecessarily long paths,
     * as sound attenuates with distant, so we can ignore very long paths.
     */
    distance = wall->minimum_distance(&transformedFirst);

  } else {
    distance = -1;
  }
}

/* This function applies some checks on the path to ensure legality.  This    
 * will limit the number of potential paths generated and therefore decrease  
 * the amount of processing neeed. The checks are:
 *
 * (a) make sure the distance between an arbitrary test point and it's
 *     reflection increase from the last path to this one.  This property is
 *     is guaranteed in a legal path, and will keep us from getting into
 *     infinite path generation loops from adjacent walls.
 *
 * (b) check that the newest wall is on the correct side of the previous wall.
 *     Without this check, we might "reflect" through a wall instead of
 *     against it.
 *
 * (c) make sure the newest wall isn't completely occluded from the previous
 *     wall.  This limits the set of valid reflecting walls to visible ones.
 */
bool SoundPath::legalPath(const Point *testPnt, Segment *walls[], int numWalls) const {

  int pntSide;                   // Side of reflected test point on prevSource new wall 
  int wallSide;                  // Side of new wall on prevSource new wall
  int i;

  /* Check that distance has increased of test point transformation */
  Point tempTestPnt = transformPoint(testPnt);
  Point tempTestPntPrev = prevPath->transformPoint(testPnt);
  if (testPnt->dist(&tempTestPnt) <= testPnt->dist(&tempTestPntPrev))
    return false;

  /* Perform checks for paths with 2 or more reflecions */
  if (reflections > 1) {

    /* Check that test point in prevSource reflected space and new wall are */
    /* on opposite sides of the prevSource new wall */
    pntSide = reflSeg[1]->side(&tempTestPntPrev);
    wallSide = reflSeg[1]->side(&reflSeg[0]->a) + reflSeg[1]->side(&reflSeg[0]->b);

    /* If at least one point of the new wall is on the other side from the reflected */
    /* test point, the magnitude of the sum of pntSide and wallSide should be 1 or less. */
    if (abs(pntSide + wallSide) > 1) return false;


    /* Check that no wall completely occludes the new wall and the prevSource new wall */
    for (i = 0; i < numWalls; i++) {

      /* Don't look at the two walls being considered */
      if (walls[i] == reflSeg[0] || walls[i] == reflSeg[1]) continue;
      
      if (reflSeg[0]->occluded(walls[i],reflSeg[1])) return false;

    }
  }

  /* If no tests failed, return true */
  return true;
}

const Segment *SoundPath::getReflectionSegment(int n) const {

  if (n < 0 || n >= reflections) return NULL;

  return reflSeg[n];
}

double SoundPath::transformAngle(double angle) const {

  return angleTransform.transformVal(angle);
}

Point SoundPath::transformPoint(const Point *pnt) const {

  return pointTransform.transformPoint(pnt);
}

double SoundPath::getDistance(const SoundSource *src, const SoundReceiver *dst) const {
  
  Point tempPoint = transformPoint(src);
  return dst->dist(&tempPoint);
}

/* Recursively traces the sound's propagation on the path to make sure */
/* reflections are legal and the path isn't obstructed by other walls. */
bool SoundPath::visible(const SoundSource *src, const Point *dst, Segment *walls[], int numWalls) const {

  Point tempPoint = transformPoint(src);
  Segment currentStep(dst,&tempPoint);     // Path sound travels between virtual source and dst

  /* If we're at a non-direct path, check validity */
  if (prevPath != NULL) {

    /* Segment connecting virtual source to destination must pass through reflecting wall */
    if (! reflSeg[0]->intersect(&currentStep))
      return false;

    /* Get portion of segment in arena */
    tempPoint = reflSeg[0]->get_intersect(&currentStep);
    currentStep = Segment(dst, &tempPoint);

    /* Segment should not cross any walls */
    for (int i = 0; i < numWalls; i++)
      if (currentStep.intersect(walls[i]) == 4) {
//        printf("WRONGLY INTERSECTS walls[%d]=%p\n", i, walls[i]);
        return false;
      }
 
    return prevPath->visible(src, &currentStep.b, walls, numWalls);
  }
  else {
    /* Segment should not cross any walls */
    for (int i = 0; i < numWalls; i++)
      if (currentStep.intersect(walls[i]) == 4)
        return false;

    /* If nothing blocking, we're done */
    return true;
  }
}

double SoundPath::getStrength(const SoundSource *src, const SoundReceiver *pnt) const {

    // HACK: add one to distance so 1/x^2 curve is guaranteed to be in
    // normalized portion (and hopefully gives milder dropoff for matching with
    // reality).
    return( src->getIntensity() * pow(Config::reflection_damping, reflections) 
            / pow(getDistance(src,pnt), 2.0) );
    //return( src->getIntensity() * pow(REFLECTION_DAMPING, reflections) / pow(getDistance(src,pnt), 2.0) );
}

double SoundPath::getMaxStrength(double intensity) const {
  
  /* Invalid if we're not at a reflected source */
  if ( distance == 0 ) return intensity;

  /* If illegal path, return 0 */
  if ( distance == -1) return 0;

  return( intensity * pow(Config::reflection_damping, reflections) / pow(distance, 2.0) );
}

double SoundPath::getDelay(const SoundSource *src, const SoundReceiver *pnt) const {

  return getDistance(src,pnt) / Config::sound_speed;
}

double SoundPath::getAngle(const SoundSource *src, const SoundReceiver *pnt) const {
  
  /* Reflect the source angle, then adjust by the offset angle of the receiving point */
  Point tempPoint = transformPoint(src);
  return SoundObject::mod2PI(angleTransform.getVal(0,0) * 
         (Point::getAngle(&tempPoint, pnt) - transformAngle(src->getAngle())));
}

double SoundPath::getRecAngle(const SoundSource *src, const SoundReceiver *pnt) const {
  
  /* Reflect the source angle, then adjust by the offset angle of the receiving point */
  Point tempPoint = transformPoint(src);
  return SoundObject::mod2PI(Point::getAngle(pnt, &tempPoint) - pnt->getAngle());
}

void SoundPath::print() const {

  cout << "Distance: " << distance <<"\n";
  cout << "Reflections: " << reflections <<"\n";
  cout << "Point Transform:\n";
  pointTransform.print();
  cout << "\n";
  cout << "Angle Transform:\n";
  angleTransform.print();
  cout << "\n";

  cout << "PATH: \n";
  for (int i = 0; i < reflections; i++)
    reflSeg[i]->print();
  cout << "\n\n";
}

void SoundPath::draw(const SoundSource *src, const Point *dst, Segment * const walls[], int numWalls) const {

  Point tempPoint = transformPoint(src);
  Segment currentStep(dst, &tempPoint);     // Path sound travels between current wall and dst

  /* If we're at a non-direct path, find wall intersect and print segment */
  if (prevPath != NULL) {
    /* Get portion of segment in arena */
    tempPoint = reflSeg[0]->get_intersect(&currentStep);
    currentStep = Segment(dst, &tempPoint);
    currentStep.drawAsPath();
    prevPath->draw(src, &currentStep.b, walls, numWalls);
  }
  else
    currentStep.drawAsPath();
}
