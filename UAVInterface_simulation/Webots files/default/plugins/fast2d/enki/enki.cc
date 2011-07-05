// Enki plugin for Webots
// Author: yvan.bourquin@epfl.ch
// Date:   16.08.2005

#include <plugins/fast2d.h>
#include <enki/PhysicalEngine.h>
#include <enki/interactions/IRSensor.h>
#include <an/Geometry.h>
#include <vector>

// uncomment these macros to obtain debug information
#define PRINTF(...) //printf(__VA_ARGS__)
#define PLOT_WORLD() //plot_world()

using namespace Enki;
using namespace An;
using namespace std;

static double SensorModel(double dist) {
  return dist;
}

class F2dSensor : public IRSensor {
public:
  F2dSensor(double range, double aperture, int rayCount, const double weights[])
  : IRSensor(NULL, Point(0, 0), 0, 0, range, aperture, rayCount, const_cast<double*>(weights), SensorModel) {}
      
  void setOrientation(double angle) { orientation = angle; }
  void setPosition(double x, double y) { pos = Point(x, y); }
  void setOwner(Robot *robot) { owner = robot; }
  double getActivation() const { return finalValue; }
  double getRange() const { return range; }
  int getNumRays() const { return rayValues.size(); }
  double getAperture() const { return aperture; }
  const Point &getAbsPosition() const { return absPos; }
  double getSmartRadius() const { return smartRadius; }
  const Vector &getAbsSmartPos() const { return absSmartPos; }
  double getAbsOrientation() const { return absOrientation; }
};

class F2dRobot : public Robot {
public:
  F2dRobot() : Robot() {
    mass = 1;
    collisionAngularFrictionFactor = 0.7;
    viscousFrictionTau = 0.5;
    viscousMomentFrictionTau = 0.0;
  }

  const vector<LocalInteraction*> &getLocalInteractions() const { return localInteractions; }
  void setSpeed(const Vector &s) { speed = desiredSpeed = s; }
  const Vector &getSpeed() const { return desiredSpeed; }

private:
  Vector desiredSpeed;
};

class F2dSolid : public PhysicalObject {
public:
  F2dSolid() : PhysicalObject() {
    mass = -1; // default: infinite mass
  }
};

static Enki::World *world = NULL;
static vector<F2dSensor*> sensors;
static vector<Polygone*> polygons;

void plot_circle(FILE *fd, double x, double y, double r) {
  // draw object's bounding circle
  double x1 = x;
  double y1 = y + r; 
  for (double a = 0; a < 6.2832; a += 0.15708) {
    double x2 = x + r * sin(a);
    double y2 = y + r * cos(a);
    fprintf(fd, "%f %f\n%f %f\n\n", x1, y1, x2, y2);
    x1 = x2;
    y1 = y2;
  }
}

// this function generate a file that can be viewed with gnuplot with
// the following command:
// gnuplot> plot [1:-1] [-1:1] "plot.txt" with lines
void plot_world() {
  FILE *fd = fopen("fast2d_plot.txt","w");
  if (fd==NULL) {
    fprintf(stderr, "cannot write fast2d_plot.txt\n");
    return;
  }
  
  if (world) {
    // for each physical object in the world
    for (unsigned i = 0; i < world->objects.size(); i++) {
      PhysicalObject *obj = world->objects[i];
      const Polygone *bounds = &obj->absBoundingSurface;
      if (obj->boundingSurface && bounds->size() > 1) {
        // draw object's bounding polygon
        Point p0 = (*bounds)[bounds->size() - 1];
        for (unsigned j = 0; j < bounds->size(); j++) {
          Point p1 = (*bounds)[j];
          fprintf(fd, "%f %f\n%f %f\n\n", p0.x, p0.y, p1.x, p1.y);
          p0 = p1;
        } 
      }
      else
        // draw object's bounding circle
        plot_circle(fd, obj->pos.x, obj->pos.y, obj->r);
      
      // if the object is a robot ...
      F2dRobot *robot = dynamic_cast<F2dRobot*>(obj);
      if (robot) {
        Point pos = robot->pos;
        Point dir = pos + robot->getSpeed();
        fprintf(fd, "%f %f\n%f %f\n\n", pos.x, pos.y, dir.x, dir.y);
        
        // plot sensor rays
        const vector<LocalInteraction*> interactions = robot->getLocalInteractions();
        for (unsigned j = 0; j < interactions.size(); j++) {
          F2dSensor *sensor = dynamic_cast<F2dSensor*>(interactions[j]);
          if (sensor) {
            int n = sensor->getNumRays();
            for (int k = 0; k < n; k++) {
              // plot sensor ray
              double alpha = sensor->getAbsOrientation();
              if (n > 1)
                alpha += 2 * k * sensor->getAperture() / (n - 1) - sensor->getAperture();
                
              Point pos = sensor->getAbsPosition();
              Point end = pos + Vector(cos(alpha), sin(alpha)) * sensor->getRange();
              fprintf(fd, "%f %f\n%f %f\n\n", pos.x, pos.y, end.x, end.y);
            }
            // uncomment this line to debug the smart circles
            //plot_circle(fd, sensor->getAbsSmartPos().x, sensor->getAbsSmartPos().y, sensor->getSmartRadius());
          }
        }
      }
    }
  }

  fclose(fd);
}

void webots_fast2d_init() {
  PRINTF("webots_fast2d_init\n");
  if (world)
    webots_fast2d_cleanup();

  world = new Enki::World(numeric_limits<double>::max(), numeric_limits<double>::max());
}

void webots_fast2d_cleanup() {
  PRINTF("webots_fast2d_cleanup\n");
  delete world;
  world = NULL;
  
  for (vector<F2dSensor*>::iterator i = sensors.begin(); i < sensors.end(); i++)
    delete *i;

  sensors.clear();

  for (vector<Polygone*>::iterator i = polygons.begin(); i < polygons.end(); i++)
    delete *i;

  polygons.clear();
}

void webots_fast2d_step(double dt) {
  world->step(dt);
  PLOT_WORLD();
}

RobotRef webots_fast2d_create_robot() {
  PRINTF("webots_fast2d_create_robot\n");
  return (RobotRef) new F2dRobot();
}

SolidRef webots_fast2d_create_solid() {
  PRINTF("webots_fast2d_create_solid\n");
  return (SolidRef) new F2dSolid();
}

void webots_fast2d_add_object(ObjectRef objectRef) {
  PRINTF("webots_fast2d_add_object: %p\n", objectRef);
  PhysicalObject *physicalObject = (PhysicalObject*) objectRef;
  world->addObject(physicalObject);
}

SensorRef webots_fast2d_create_irsensor(double range, double aperture, int numRays, const double rayWeights[]) {
  PRINTF("webots_create_irsensor: %f, %f, %d\n", range, aperture, numRays);
  F2dSensor *sensor = new F2dSensor(range, aperture, numRays, rayWeights);
  sensors.push_back(sensor);
  return (SensorRef) sensor;
}

void webots_fast2d_robot_add_sensor(RobotRef robotRef, SensorRef sensorRef, double x, double y, double angle) {
  PRINTF("webots_fast2d_robot_add_sensor: %p, %p, %f, %f, %f\n", robotRef, sensorRef, x, y, angle);  
  F2dRobot *robot = (F2dRobot*) robotRef;
  F2dSensor *sensor = (F2dSensor*) sensorRef;
  sensor->setOwner(robot);
  sensor->setPosition(x, y);
  sensor->setOrientation(angle);
  robot->addLocalInteraction(sensor);
}

double webots_fast2d_sensor_get_activation(SensorRef sensorRef) {
  F2dSensor *sensor = (F2dSensor*) sensorRef;
  return sensor->getActivation();
}

void webots_fast2d_object_set_bounding_rectangle(ObjectRef objectRef, const double x[4], const double y[4]) {
  PRINTF("webots_fast2d_object_set_bounding_rectangle: %p\n", objectRef);  
  PhysicalObject *physicalObject = (PhysicalObject*) objectRef;
  Polygone *polygon = new Polygone();
  
  for (int i = 0; i < 4; i++)
    polygon->push_back(Point(x[i], y[i]));

  physicalObject->setBoundingSurface(polygon);
  polygons.push_back(polygon);
}

void webots_fast2d_object_set_position(ObjectRef objectRef, double xpos, double ypos) {
  PRINTF("webots_fast2d_object_set_position: %p, %f, %f\n", objectRef, xpos, ypos);  
  PhysicalObject *physicalObject = (PhysicalObject*) objectRef;
  physicalObject->pos = Point(xpos, ypos);
}

void webots_fast2d_robot_set_speed(RobotRef robotRef, double dx, double dy) {
  PRINTF("webots_fast2d_robot_set_speed: %p, %f, %f\n", robotRef, dx, dy);
  F2dRobot *robot = (F2dRobot*) robotRef;
  robot->setSpeed(Vector(dx, dy));
}

void webots_fast2d_object_set_angle(ObjectRef objectRef, double angle) {
  PRINTF("webots_fast2d_object_set_angle: %p, %f\n", objectRef, angle);  
  PhysicalObject *physicalObject = (PhysicalObject*) objectRef;
  physicalObject->angle = angle;
}

void webots_fast2d_robot_set_angular_speed(RobotRef robotRef, double angSpeed) {
  PRINTF("webots_fast2d_robot_set_angular_speed: %p, %f\n", robotRef, angSpeed); 
  F2dRobot *robot = (F2dRobot*) robotRef; 
  robot->angSpeed = angSpeed;
}

void webots_fast2d_object_set_bounding_radius(ObjectRef objectRef, double radius) {
  PRINTF("webots_fast2d_object_set_bounding_radius: %p, %f\n", objectRef, radius);  
  PhysicalObject *object = (PhysicalObject*) objectRef;
  object->r = radius;
  delete object->boundingSurface;
  object->boundingSurface = NULL;
}

void webots_fast2d_object_set_mass(ObjectRef objectRef, double mass) {
  PRINTF("webots_fast2d_object_set_mass: %p, %f\n", objectRef, mass);  
  PhysicalObject *object = (PhysicalObject*) objectRef;
  object->mass = mass;
}

void webots_fast2d_object_get_transform(ObjectRef objectRef, double *xpos, double *ypos, double *angle) {
  PRINTF("webots_fast2d_object_get_transform: %p\n", objectRef);
  PhysicalObject *object = (PhysicalObject*) objectRef;
  *xpos = object->pos.x;
  *ypos = object->pos.y;
  *angle = object->angle;
}
