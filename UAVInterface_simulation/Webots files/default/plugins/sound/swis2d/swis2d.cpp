//----------------------------------------------------------------------------------------------------------------------------
// File:     swis2d.cpp
// Purpose:  Interface between James' sound simulation objects and Webots
// Author:   Yvan Bourquin (based on James Pugh's original SoundRangeTestSuper.cpp code)
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "SoundSource.h"
#include "SoundReceiver.h"
#include "SoundPath.h"
#include "Segment.h"
#include "Config.h"
#include <plugins/sound.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <assert.h>
#include <stdlib.h> 

// uncomment this macros to print debug information
#define PRINTF(...) { if (Config::debug_plugin) printf(__VA_ARGS__); fflush(stdout); }

// Generate random number in [0,1]
#define R (rand()/(double)RAND_MAX)

using namespace std;

typedef struct { SoundPath *path; SoundSource *src; SoundReceiver *dst; } DrawRec;

static vector <DrawRec*> drawRecs;
static vector <SoundReceiver*> microphones;
static vector <SoundSource*> speakers;
static ObstacleRef currentRef;
static SoundPath **sndpath = NULL;
static int num_sndpath;
static Segment *walls[99];
static int numWalls;
static Point testPoint;
static SoundSource::Time elapsed;
static bool changed = true; // need to recompute paths ?
static bool needToClearDrawRecs = false;

// Generate sound paths recursively using breadth first search
static void genPaths(SoundPath *path) {

  // Only continue if not already at max
  if (num_sndpath >= Config::max_paths) return;

  // Try generating a new path for each wall
  for (int i = 0; i < numWalls; i++) {

    // Make sure we don't reflect back across the same wall
    if (walls[i] == path->getReflectionSegment(0)) continue;

    // Generate new path
    SoundPath *newpath = new SoundPath(walls[i], &testPoint, path, walls, numWalls);

    // If strong enough, add to list
    if (newpath->getMaxStrength(Config::max_speaker_strength) > Config::min_strength) {
      
      if (Config::debug_paths)
        newpath->print();

      sndpath[num_sndpath] = newpath;
      num_sndpath++;
    }
    else
      delete newpath;
  }
}

// Find all potential sound paths in the current arena setup
static void findPaths(void) {

  assert(num_sndpath == 0);

  // Start with zero path
  sndpath[0] = new SoundPath();
  num_sndpath = 1;
  
  // Go through source list until we reach end (breadth-first search)
  for (int i = 0; i < num_sndpath; i++)
    genPaths(sndpath[i]);
}

void webots_sound_init() {
  
  if (! Config::readFile("sound_config.txt"))
    webots_sound_console_printf("error: could not open file: sound_config.txt: using default config");
  
  PRINTF("webots_sound_init()\n");    

  SoundSource::setRate(Config::samples_rate / 1000.0);
  testPoint.setX(Config::test_point_x);
  testPoint.setY(Config::test_point_z);
  
  changed = true;
  numWalls = 0;
  elapsed = 0L;
  sndpath = new SoundPath*[Config::max_paths];
}

void webots_sound_add_obstacle(ObstacleRef ref, bool mobile, int level) {
  PRINTF("webots_sound_add_obstacle(%s, mobile=%s, level=%d)\n",
    webots_sound_get_name(ref), mobile ? "true" : "false", level);

  currentRef = NULL;
  if (level == 1) {
    if (! mobile)
      currentRef = ref;
    else
      webots_sound_console_printf("swis2d: ignoring boundingObject of %s object",
        webots_sound_get_name(ref));
  }
}

static void transform2d(const float r[2][2], const float t[3], float p[2]) {
  float x = p[0] * r[0][0] + p[1] * r[0][1] + t[0];
  float y = p[0] * r[1][0] + p[1] * r[1][1] + t[2];
  p[0] = x;
  p[1] = y;
}

void webots_sound_add_bounding_box(ObstacleRef ref, ShapeRef shape, const float size[3]) {
  PRINTF("webots_sound_add_bounding_box(%s, %s)\n",
    webots_sound_get_name(ref), webots_sound_get_name(shape));

  if (! currentRef)
    return;

  assert(ref == currentRef);
  const float *t = webots_sound_get_translation(shape);
  const float *m = webots_sound_get_matrix(shape);
  
  const float r[2][2] = {
    { m[0], m[8]  },
    { m[2], m[10] },
  };
  
  float sx = size[0] / 2;
  float sz = size[2] / 2;
  
  if (sx >= Config::box_ratio_threshold * sz) {
    float p0[2] = { +sx, 0 };
    float p1[2] = { -sx, 0 };
    
    transform2d(r, t, p0);
    transform2d(r, t, p1);
    
    walls[numWalls++] = new Segment(p0[0], p0[1], p1[0], p1[1]);
  }
  else if (sz >= Config::box_ratio_threshold * sx) {
    float p0[2] = { 0, +sz };
    float p1[2] = { 0, -sz };
    
    transform2d(r, t, p0);
    transform2d(r, t, p1);
    
    walls[numWalls++] = new Segment(p0[0], p0[1], p1[0], p1[1]);
  }
  else {
    float p0[2] = { +sx, -sz };
    float p1[2] = { +sx, +sz };
    float p2[2] = { -sx, +sz };
    float p3[2] = { -sx, -sz };
  
    transform2d(r, t, p0);
    transform2d(r, t, p1);
    transform2d(r, t, p2);
    transform2d(r, t, p3);
    
    walls[numWalls++] = new Segment(p0[0], p0[1], p1[0], p1[1]);
    walls[numWalls++] = new Segment(p1[0], p1[1], p2[0], p2[1]);
    walls[numWalls++] = new Segment(p2[0], p2[1], p3[0], p3[1]);
    walls[numWalls++] = new Segment(p3[0], p3[1], p0[0], p0[1]);
  }
}

void webots_sound_add_bounding_cylinder(ObstacleRef obstacle, ShapeRef shape, float radius, float height) {
  PRINTF("webots_sound_add_bounding_cylinder()\n");
  if (! currentRef)
    return;
  
  webots_sound_console_printf("swis2d: ignoring %s bounding object of %s object",
    webots_sound_get_name(shape), webots_sound_get_name(obstacle));
}

void webots_sound_add_bounding_sphere(ObstacleRef obstacle, ShapeRef shape, float radius) {
  PRINTF("webots_sound_add_bounding_sphere()\n");
  if (! currentRef)
    return;

  webots_sound_console_printf("swis2d: ignoring %s bounding object of %s object",
    webots_sound_get_name(shape), webots_sound_get_name(obstacle));
}

void webots_sound_add_speaker(SpeakerRef ref, float aperture, float power) {
  PRINTF("webots_sound_add_speaker(aperture=%f, power=%f)\n", aperture, power);
  SoundSource *src = new SoundSource(ref);
  src->setIntensity(power);
  src->updatePosition();
  speakers.push_back(src);
}

void webots_sound_add_microphone(MicrophoneRef ref, float aperture, float sensitivity) {
  PRINTF("webots_sound_add_microphone(aperture=%f, sensitivity=%f)\n", aperture, sensitivity);
  SoundReceiver *mic = new SoundReceiver(ref);
  mic->updatePosition();
  microphones.push_back(mic);
}

static double gaussian() {
  static bool flag = true;
  static double y2;

  if (flag) {
    double x1, x2, w;
    do {
      x1 = 2.0 * R - 1.0;
      x2 = 2.0 * R - 1.0;
      w = x1 * x1 + x2 * x2;
    } while (w >= 1.0);

    w = sqrt((-2.0 * log(w)) / w);
    y2 = x2 * w;
    flag = false;
    return x1 * w;
  }

  flag = true;
  return y2;
}

// Get the total sound input for specified microphone
static void *handleMic(SoundReceiver *pnt, double lastUpdate, int &sampleSize) {

  sampleSize = (int)((elapsed - lastUpdate) * SoundSource::getRate());
  
  if (sampleSize == 0)
    return NULL;

  double outp[sampleSize];         // Output received by microphone

  // background noise
  // the gaussian() function is very expensive so we avoid it if possible
  if (Config::additive_noise)
    for (int k = 0; k < sampleSize; k++)
      outp[k] = Config::additive_noise * 128.0 * gaussian();
  else
    for (int k = 0; k < sampleSize; k++)
      outp[k] = 0.0;

  // go through each sound source
  for (vector<SoundSource*>::iterator i = speakers.begin(); i < speakers.end(); i++) {
    SoundSource *source = *i;
    
    // avoid complex computation if the source has not emitted
    if (source->hasAtLeastOneSample()) {
      // Go through each sound path
      for (int j = 0; j < num_sndpath; j++) {
        SoundPath *path = sndpath[j];
  
        // Make sure signal would be strong enough
        double strength = path->getStrength(source, pnt);
        
        // Angle at which the source emits to the receiver
        double angle = path->getAngle(source, pnt);
        
        // Angle at which the receiver observes the source
        double recangle = path->getRecAngle(source, pnt);
        
        strength *= source->getAngleStr(angle) * pnt->getAngleStr(recangle);
        
        if (strength < Config::min_strength) continue;
  
        // Check visibility
        if (path->visible(source, pnt, walls, numWalls)) {
  
          // add to draw list
          DrawRec *rec = new DrawRec;
          rec->path = path;
          rec->src = source;
          rec->dst = pnt;
          drawRecs.push_back(rec);
  
          // Delay from current source to microphone
          double delay = path->getDelay(source, pnt);
          
          short snddata[sampleSize];
          source->extractSample(snddata, sampleSize, (long long int)lastUpdate - (int)(delay * 1000000 + 0.5));
          
          // Add sound signal
          for (int k = 0; k < sampleSize; k++) {
            // the gaussian() function is very expensive so we avoid it if possible
            if (Config::multiplicative_noise)
              // Multiplicative noise
              outp[k] += strength * snddata[k] * (1.0 + Config::multiplicative_noise * gaussian());
            else
              // No multiplicative noise
              outp[k] += strength * snddata[k];
          }
        }
      }
    }
  }

  // format data packet as 'short' for sending
  short *output = new short[sampleSize];
  for (int k=0; k < sampleSize; k++)
    output[k] = (short)(outp[k] / 128.0);

  // adjust sampleSize to be in bytes
  sampleSize *= sizeof(short)/sizeof(char);

  // return to Webots who is responsible for deleting this data !
  return output;
}

static void clearDrawRecs() {
  for (vector<DrawRec*>::iterator i = drawRecs.begin(); i < drawRecs.end(); i++)
    delete *i;

  drawRecs.clear();
  needToClearDrawRecs = false;
}

void *webots_sound_microphone_receive_sample(MicrophoneRef ref, double lastUpdate, int &sampleSize) {
  PRINTF("webots_sound_microphone_receive_sample(lastUpdate=%f)\n", lastUpdate);
  
  if (needToClearDrawRecs)
    clearDrawRecs();

  for (vector<SoundReceiver*>::iterator i = microphones.begin(); i < microphones.end(); i++)
    if ((*i)->getRef() == ref)
      return handleMic(*i, 1000 * lastUpdate, sampleSize);
    
  // should never be reached because this plugin creates a SoundReceiver for each Webots Microphone
  assert(0);
  return NULL;
}

void webots_sound_step(float step) {
  PRINTF("webots_sound_step(step=%f)\n", step);
  
  int mstep = (int)(step * 1000000); // from seconds to microseconds
  
  if (changed) {
    webots_sound_console_printf("swis2d: computing paths ...");
    findPaths();
    changed = false;
  }

  for (vector<SoundSource*>::iterator i = speakers.begin(); i < speakers.end(); i++) {
    (*i)->updatePosition();
    (*i)->clearSamplesUntil(elapsed - 1000000);
  }

  for (vector<SoundReceiver*>::iterator i = microphones.begin(); i < microphones.end(); i++)
    (*i)->updatePosition();

  elapsed += mstep;
  needToClearDrawRecs = true;
}

void webots_sound_speaker_emit_sample(SpeakerRef ref, const void *sample, int size) {
  // adjust size to reflect correct data type
  size /= sizeof(short)/sizeof(char);
  PRINTF("webots_sound_speaker_emit_sample(size=%d)\n", size);
  
  for (vector<SoundSource*>::iterator i = speakers.begin(); i < speakers.end(); i++) {
    SoundSource *speak = *i;
    if (speak->getRef() == ref) {
      speak->insertSample((short*)sample, size, elapsed);
      return;
    }
  }
  
  // should never be reached because this plugin creates a SoundSource for each Webots Speaker
  assert(0);
}

void webots_sound_draw() {

  glDisable(GL_LIGHTING);
  glLineWidth(4.0);

  // draw test point as a yellow vertical line
  glBegin(GL_LINES);
  glColor3f(1, 1, 0);
  glVertex3f(testPoint.getX(), -0.1f, testPoint.getY());
  glVertex3f(testPoint.getX(),  0.1f, testPoint.getY());
  glEnd();

  glDisable(GL_DEPTH_TEST);

  // draw walls
  for (int i = 0; i < numWalls; i++)
    walls[i]->drawAsWall();
    
  glEnable(GL_DEPTH_TEST);
    
  // draw microphones
  for (vector<SoundReceiver*>::iterator i = microphones.begin(); i < microphones.end(); i++)
    (*i)->draw();

  // draw speakers
  for (vector<SoundSource*>::iterator i = speakers.begin(); i < speakers.end(); i++)
    (*i)->draw();
    
  // draw paths
  for (vector<DrawRec*>::iterator i = drawRecs.begin(); i < drawRecs.end(); i++) {
    DrawRec *rec = *i;
    rec->path->draw(rec->src, rec->dst, walls, numWalls);
  }
  
  glLineWidth(1.0);
  glEnable(GL_LIGHTING);
}

void webots_sound_remove_object(ObjectRef ref) {
  PRINTF("webots_sound_remove_object(%s)\n", webots_sound_get_name(ref));
 
  for (vector<SoundSource*>::iterator i = speakers.begin(); i < speakers.end(); i++) {
    SoundSource *speaker = *i;
    if (speaker->getRef() == ref) {
      speakers.erase(i);
      delete speaker;
      clearDrawRecs();
      return;
    }
  }

  for (vector<SoundReceiver*>::iterator i = microphones.begin(); i < microphones.end(); i++) {
    SoundReceiver *micro = *i;
    if (micro->getRef() == ref) {
      microphones.erase(i);
      delete micro;
      clearDrawRecs();
      return;
    }
  }
  
  // ref could also be an ObstacleRef we ignore this for the moment
  // so we don't need to recompute the paths right now but only upon restart
}

void webots_sound_cleanup() {
  PRINTF("webots_sound_cleanup()\n");
  
  clearDrawRecs();
  
  for (int i = 0; i < num_sndpath; i++)
    delete sndpath[i];
  
  delete [] sndpath;
  sndpath = NULL;
  num_sndpath = 0;

  // in principle every microphone and speaker should already have been deleted
  // in the webots_sound_remove_object() function above
  assert(microphones.empty());
  assert(speakers.empty());

  for (int i = 0; i < numWalls; i++)
    delete walls[i];

  numWalls = 0;
}
