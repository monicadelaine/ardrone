//----------------------------------------------------------------------------------------------------------------------------
// File:     Segment.cpp
// Purpose:  Defines a 2d SoundPath segment or a wall for sound simulation
// Authors:  James Pugh (adapted as Webots plugin by Yvan Bourquin)
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "Segment.h"
#include <iostream>
#include <cmath>
#include <plugins/sound.h>
#include <stdlib.h> 

#define ROUNDING_ERROR 0.0001

using namespace std;

Segment::Segment(double x1, double y1, double x2, double y2) {
  
  a = Point(x1, y1);
  b = Point(x2, y2);

  initialize ();
}

Segment::Segment (Point start, Point end) {

  this->a = start;
  this->b = end;

  initialize ();
}

Segment::Segment(const Point *start, const Point *end) {

  this->a = *start;
  this->b = *end;

  initialize ();
}

void Segment::initialize() {

  double dx, dy;

  if ((b.getX() - a.getX()) == 0) {
    dx = 0;
    dy = b.getY() - a.getY();

    p = 1;
    q = 0;
    r = a.getX ();
    angle = -M_PI / 2.0;

  } else {
    double x1 = a.getX();
    double y1 = a.getY();
    double x2 = b.getX();
    double y2 = b.getY();

    dx = x2-x1;
    dy = y2-y1;

    p = - (y1 - y2) / (x1 - x2);
    q = 1;
    r = p * x1 + y1;
    angle = atan((y1 - y2) / (x1 - x2));

  }

  /* Initialize transformation matrices */
  double pvals[3][3];
  double avals[2][2];

  double dst = a.dist(&b) * a.dist(&b);
  pvals[0][0] = (dx*dx - dy*dy)/dst;
  pvals[0][1] = 2.0*dx*dy/dst;
  pvals[0][2] = a.getX() - a.getX()*pvals[0][0] - a.getY()*pvals[0][1];
  pvals[1][0] = pvals[0][1];
  pvals[1][1] = -pvals[0][0];
  pvals[1][2] = a.getY() - a.getX()*pvals[1][0] - a.getY()*pvals[1][1];
  pvals[2][0] = 0;
  pvals[2][1] = 0;
  pvals[2][2] = 1;
  pntmat = Matrix(pvals[0][0], pvals[0][1], pvals[0][2],
                  pvals[1][0], pvals[1][1], pvals[1][2],
                  pvals[2][0], pvals[2][1], pvals[2][2]);
  avals[0][0] = -1.0;
  avals[0][1] = 2*angle;
  avals[1][0] = 0;
  avals[1][1] = 1;
  angmat = Matrix(avals[0][0], avals[0][1], avals[1][0], avals[1][1]);
}

double Segment::getLength() const {
  
  double aSquare = pow((b.getX() - a.getX()), 2);
  double bSquare = pow((b.getY() - a.getY()), 2);

  return sqrt(aSquare + bSquare);
}

int Segment::intersect(const Segment *seg) const {

  return abs(side(&seg->a) - side(&seg->b)) * abs(seg->side(&a) - seg->side(&b));
}

Point Segment::get_intersect(const Segment *seg) const {

  double p2, q2, r2;
  Point pnt;                 // Intersection point
  
  /* Intermediate variables */
  p2 = seg->p;
  q2 = seg->q;
  r2 = seg->r;

  /* Check if lines are parallel */
  if (p == p2) return pnt;

  /* Find the intersection point */
  pnt.x = (q*r2 - q2*r) / (q*p2 - q2*p);
  pnt.y = (p*r2 - p2*r) / (p*q2 - p2*q);

  return pnt;
}

double Segment::reflect_angle(double angle) const {

  /* Find wall angle and calculate reflected angle */
  angle = angmat.transformVal(angle);

  /* Make sure angle within bounds */
  if (angle > M_PI) angle -= 2*M_PI;
  if (angle < -M_PI) angle += 2*M_PI;

  return angle;
}

Point Segment::reflect_point(const Point *pnt) const {
  return pntmat.transformPoint(pnt);
}

Segment Segment::reflect_segment(const Segment *x) const {

  Segment newx(reflect_point(&x->a), reflect_point(&x->b));

  return newx;
}

int Segment::occluded(const Segment *blk, const Point *p) const {

  Point p_;               // Source reflected across this
  bool chk1;              // Is either end of blk in the triangle formed by p and this?
  bool chk2;              // Does blk completely occlude this?

  /* Set segments */
  Segment pa(p, &a);      // Triangle edges from p to this
  Segment pb(p, &b);
  p_ = reflect_point(p);  // Reflected point
  Segment p_blka(p_, blk->a);  // Connections from p_ to blk
  Segment p_blkb(p_, blk->b);

  /* If p_ and a point of blk intersect this, but not pa or pb, that point of seg is occluding this */
  chk1 = ( intersect(&p_blka) && !pa.intersect(&p_blka) && !pb.intersect(&p_blka) )
    || ( intersect(&p_blkb) && !pa.intersect(&p_blkb) && !pb.intersect(&p_blkb) );

  /* If blk intersects the line between p and a, blk occludes this */
  chk2 = pa.intersect(blk);

  /* If chk2 but not chk1, this completely occluded by blk (we assume this and blk don't intersect) */
  if (!chk1 && chk2) return 2;

  /* Otherwise, if chk1, this partially occluded Otherwise not occluded */
  return chk1 ? 1 : 0;
}

bool Segment::occluded(const Segment *blk, const Segment *seg) const {

  Segment asega(a,seg->a);
  Segment asegb(a,seg->b);
  Segment bsega(b,seg->a);
  Segment bsegb(b,seg->b);

  /* seg occluded if all lines between endpoints of seg and this intersect blk */
  return( asega.intersect(blk) && asegb.intersect(blk) && 
      bsega.intersect(blk) && bsegb.intersect(blk) );
}

int Segment::side(const Point *pnt) const {

  if (p*pnt->x + q*pnt->y > r + ROUNDING_ERROR) return 1;
  if (p*pnt->x + q*pnt->y < r - ROUNDING_ERROR) return -1;
  return 0;

}

double Segment::minimum_distance(const Segment *seg) const {

  double dist, temp;     // Minimum distance so far, and temporary distance

  /* Minimum distance between segments is the minimum of the distance from */
  /* an end point of one segment to the other segment */
  dist = dist2point(&seg->a);
  temp = dist2point(&seg->b);
  if (temp < dist) dist = temp;
  temp = seg->dist2point(&a);
  if (temp < dist) dist = temp;
  temp = seg->dist2point(&b);
  if (temp < dist) dist = temp;

  return(dist);
}

double Segment::dist2point(const Point *pnt) const {

  double len;               // Length of segment
  Point newp;               // Rotated point

  /* Rotate so segment lies on x access */
  len = getLength();
  newp.x = (pnt->x - a.x)*(b.x - a.x) + (pnt->y - a.y)*(b.y - a.y);
  newp.x /= len;
  newp.y = (pnt->y - a.y)*(b.x - a.x) - (pnt->x - a.x)*(b.y - a.y);
  newp.y /= len;

  /* If point not above segment, take distance to nearest segment end */
  if (newp.x < 0)
    return(sqrt(newp.x*newp.x + newp.y*newp.y));
  if (newp.x > len) {
    newp.x -= len;
    return(sqrt(newp.x*newp.x + newp.y*newp.y));
  }

  /* Otherwise, distance is y value of new point */
  return(fabs(newp.y));
}

void Segment::print() const {
  cout << "x1: " << a.getX() << ", y1: " << a.getY();
  cout << ", x2: " << b.getX() << ", y2: " << b.getY();
  cout << ", p: " << p << ", q: " << q << ", r: " << r << "\n";
}

void Segment::drawAsWall() const {
  glBegin(GL_LINES);
  glColor3f(0, 1, 0);
  glVertex3f(a.getX(), 0.01, a.getY());
  glVertex3f(b.getX(), 0.01, b.getY());
  glEnd();
}

void Segment::drawAsPath() const {
  glBegin(GL_LINES);
  glColor3f(1, 1, 1);
  glVertex3f(a.getX(), 0.01, a.getY());
  glVertex3f(b.getX(), 0.01, b.getY());
  glEnd();
}
