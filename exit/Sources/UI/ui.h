#ifndef _UI_H_
#define _UI_H_

#include <VP_Os/vp_os_types.h>
#include <ardrone_tool/UI/ardrone_input.h>
#include <glib.h>
#include <linux/joystick.h>


#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ioctl.h>

#include <libudev.h>

C_RESULT custom_reset_user_input(input_state_t* input_state, uint32_t user_input );
C_RESULT custom_update_user_input(input_state_t* input_state, uint32_t user_input );



typedef struct {
  int num_axis;
  int num_buttons;
  gchar *name;
  gchar *filename;
  int32_t serial;
  gboolean def;
  gboolean config;
  //Control_Type commands[NUM_COMMAND];
} Controller_info;



#endif // _UI_H_
