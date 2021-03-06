/**
 * @file main.c
 * @author sylvain.gaeremynck@parrot.com
 * @date 2009/07/01
 */
#include <ardrone_testing_tool.h>

//ARDroneLib
#include <ardrone_tool/ardrone_time.h>
#include <ardrone_tool/Navdata/ardrone_navdata_client.h>
#include <ardrone_tool/Control/ardrone_control.h>
#include <ardrone_tool/UI/ardrone_input.h>

//Common
#include <config.h>
#include <ardrone_api.h>

//VP_SDK
#include <ATcodec/ATcodec_api.h>
#include <VP_Os/vp_os_print.h>
#include <VP_Api/vp_api_thread_helper.h>
#include <VP_Os/vp_os_signal.h>

//Local project
#include <UI/gamepad.h>
#include <Video/video_stage.h>
#include <ihm/ihm.h>
#include <ihm/ihm_stages_o_gtk.h>
#include <Navdata/navdata_client.h>
#include <common/mobile_config.h>

PROTO_THREAD_ROUTINE(remote_console, params);
PROTO_THREAD_ROUTINE(stages_vision, data);

static int32_t exit_ihm_program = 1;

static mobile_config_t cfg;

/* Implementing Custom methods for the main function of an ARDrone application */

/* The delegate object calls this method during initialization of an ARDrone application */
C_RESULT ardrone_tool_init_custom(int argc, char **argv)
{
  /* Registering for a new device of game controller */
  ardrone_tool_input_add( &gamepad );

  /* Start all threads of your application */
 // START_THREAD( video_stage, NULL );
 ardrone_navdata_handler_table[NAVDATA_IHM_PROCESS_INDEX].data   = &cfg;
 // load_ini();
  START_THREAD(ihm, &cfg);
  START_THREAD(remote_console, &cfg);
  START_THREAD(stages_vision, &cfg);







  return C_OK;
}

/* The delegate object calls this method when the event loop exit */
C_RESULT ardrone_tool_shutdown_custom()
{
  /* Relinquish all threads of your application */
  //JOIN_THREAD( video_stage );

  /* Unregistering for the current device */

  JOIN_THREAD(ihm);
   JOIN_THREAD(stages_vision);
   JOIN_THREAD(remote_console);
  ardrone_tool_input_remove( &gamepad );

  return C_OK;
}

/* The event loop calls this method for the exit condition */
bool_t ardrone_tool_exit()
{
  return exit_ihm_program == 0;
}

C_RESULT signal_exit()
{
  exit_ihm_program = 0;

  return C_OK;
}

/* Implementing thread table in which you add routines of your application and those provided by the SDK */
BEGIN_THREAD_TABLE
  THREAD_TABLE_ENTRY( ihm, 20 )
  THREAD_TABLE_ENTRY( stages_vision, 20 )
  //THREAD_TABLE_ENTRY( remote_console, 20 )

  THREAD_TABLE_ENTRY( ardrone_control, 20 )
  THREAD_TABLE_ENTRY( navdata_update, 20 )
  //THREAD_TABLE_ENTRY( video_stage, 20 )

END_THREAD_TABLE

