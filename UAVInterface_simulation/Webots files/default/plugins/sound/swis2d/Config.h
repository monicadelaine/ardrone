#ifndef CONFIG_H
#define CONFIG_H

//--------------------------------------------------------------------------
// File:     Config.h
// Purpose:  Read configuration for sound simulation
// Author:   Yvan Bourquin
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007.
//---------------------------------------------------------------------------

class Config {
public:
  static int readFile(const char *filename);

  static double sound_speed;
  static double samples_rate;
  static double min_strength;
  static double additive_noise;
  static double multiplicative_noise;
  static double reflection_damping;
  static double max_speaker_strength;
  static int max_paths;
  static double test_point_x;
  static double test_point_z;
  static double box_ratio_threshold;
  static int draw;
  static int debug_plugin;
  static int debug_paths;
};

#endif
