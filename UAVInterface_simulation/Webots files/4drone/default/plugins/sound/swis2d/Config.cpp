//----------------------------------------------------------------------------------------------------------------------------
// File:     Config.cpp
// Purpose:  Read configuration for sound simulation
// Author:   Yvan Bourquin
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007.
//----------------------------------------------------------------------------------------------------------------------------

#include "Config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

double Config::sound_speed = 340.29;
double Config::samples_rate = 20.0;
double Config::min_strength = 1.0;
double Config::additive_noise = 5.0;
double Config::multiplicative_noise = 1.5;
double Config::reflection_damping = 0.2;
double Config::max_speaker_strength = 20.0;
int Config::max_paths = 100;
double Config::test_point_x = 0.0;
double Config::test_point_z = 0.0;
double Config::box_ratio_threshold = 10.0;
int Config::draw = 1;
int Config::debug_plugin = 0;
int Config::debug_paths = 0;

// retrieve constants from configuration file
int Config::readFile(const char *filename) {

  FILE *file = fopen(filename, "r");
  if (! file)
    return 0;

  struct { const char *name; void *var; char type; } P[] = {
    { "sound_speed", &sound_speed, 'f' },
    { "samples_rate", &samples_rate, 'f' },
    { "min_strength", &min_strength, 'f' },
    { "additive_noise", &additive_noise, 'f' },
    { "multiplicative_noise", &multiplicative_noise, 'f' },
    { "reflection_damping", &reflection_damping, 'f' },
    { "max_speaker_strength", &max_speaker_strength, 'f' },
    { "max_paths", &max_paths, 'd' },
    { "test_point_x", &test_point_x, 'f' },
    { "test_point_z", &test_point_z, 'f' },
    { "box_ratio_threshold", &box_ratio_threshold, 'f' },
    { "draw", &draw, 'd' },
    { "debug_plugin", &debug_plugin, 'd' },
    { "debug_paths", &debug_paths, 'd' },
    { NULL }
  };

  // start reading
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    char *comment = strchr(line, '#');
    if (comment) *comment = '\0';
    char value[128], variable[128];
    if (line[0] != '#' && sscanf(line, "%s %*s %s", variable, value) == 2) {
      for (int i = 0; P[i].name; i++) {
        if (! strcmp(variable, P[i].name)) {
          switch (P[i].type) {
          case 'd':
            *((int *) (P[i].var)) = atoi(value);
            break;
          case 'f':
            *((double *) (P[i].var)) = atof(value);
            break;
          }
          break;
        }
      }
    }
  }

  fclose(file);
  return 1;
}
