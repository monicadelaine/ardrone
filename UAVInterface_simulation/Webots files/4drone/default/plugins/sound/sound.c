/***************************************************************/
/* This file is common to all sound plugin projects.           */
/* It is included automatically by the Makefile.include and    */
/* should be included in other non make-based projects         */
/***************************************************************/

#include <plugins/sound.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
  
#ifndef LINUX

/* This is necessary to make the shared library work on Windows and MacOSX */

const char * (*webots_sound_get_name_func)(ObjectRef) = NULL;
void (*webots_sound_console_printf_func)(const char *, ...) = NULL;
const float * (*webots_sound_get_matrix_func)(ObjectRef) = NULL;

DLLEXPORT void webots_sound_special_init(
  const char *(*webots_sound_get_name_arg)(ObjectRef),
  void (*webots_sound_console_printf_arg)(const char *, ...),
  const float *(*webots_sound_get_matrix_arg)(ObjectRef))
{    
  webots_sound_get_name_func = webots_sound_get_name_arg;
  webots_sound_console_printf_func = webots_sound_console_printf_arg;
  webots_sound_get_matrix_func = webots_sound_get_matrix_arg;
}

#endif

#ifdef __cplusplus
}
#endif
