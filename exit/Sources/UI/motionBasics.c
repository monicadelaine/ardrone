/**
 *  \brief    gamepad handling implementation
 *  \author   Sylvain Gaeremynck <sylvain.gaeremynck@parrot.fr>
 *  \version  1.0
 *  \date     04/06/2007
 *  \warning  Subject to completion
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <linux/joystick.h>

#include <ardrone_api.h>
#include <VP_Os/vp_os_print.h>
#include "gamepad.h"

#define theta 0
#define phi 1
#define psi 2
#define vx  3
#define vy 4
#define vz 5
#define bat 6
#define time 7
#define dis 8
#define oT 9
#define flightT 10










#define MAX_LINE 20


double line[256];


double rDistance = 0;
double t = .005;
double m = .001;
double offset = 1.506;
double meters = 2;
double target_d;
double status[4];
double turnS;
int factor = 1000;
//static int oldTime = 0;
double newTime = 0;
double stateS = 1;
double dynamicT = 0;


typedef struct {
  int32_t bus;
  int32_t vendor;
  int32_t product;
  int32_t version;
  char    name[MAX_NAME_LENGTH];
  char    handlers[MAX_NAME_LENGTH];
} device_t;

extern int32_t MiscVar[];

static C_RESULT add_device(device_t* device, const int32_t id);

static C_RESULT parse_proc_input_devices(FILE* f, const int32_t id);

input_device_t gamepad = {
  "Gamepad",
  open_gamepad,
  update_gamepad,
  close_gamepad
};

static int32_t joy_dev = 0;

input_device_t radioGP = {
  "GreatPlanes",
  open_radioGP,
  update_radioGP,
  close_radioGP
};


input_device_t ps3pad = {
  "PS3Gamepad",
  open_ps3pad,
  update_ps3pad,
  close_ps3pad
};

///////////////////////////////
//  RadioGP input functions  //
///////////////////////////////
C_RESULT open_radioGP(void)
{
  C_RESULT res = C_FAIL;

  FILE* f = fopen("/proc/bus/input/devices", "r");

  if( f != NULL )
  {
    res = parse_proc_input_devices( f,  RADIO_GP_ID);

    fclose( f );

    if( SUCCEED( res ) && strcmp(radioGP.name, "GreatPlanes")!=0)
    {
			char dev_path[20]="/dev/input/";
			strcat(dev_path, radioGP.name);
      joy_dev = open(dev_path, O_NONBLOCK | O_RDONLY);

			return res;
    }
		else
		{
			return C_FAIL;
		}
  }

  return res;
}

C_RESULT update_radioGP(void)
{
  static float32_t roll = 0, pitch = 0, gaz=0, yaw=0;
  static bool_t refresh_values = FALSE;
  ssize_t res;
  static struct js_event js_e_buffer[64];

  res = read(joy_dev, js_e_buffer, sizeof(struct js_event) * 64);

  if( !res || (res < 0 && errno == EAGAIN) )
    return C_OK;

  if( res < 0 )
    return C_FAIL;

  if (res < (int) sizeof(struct js_event))// If non-complete bloc: ignored
    return C_OK;

  // Buffer decomposition in blocs (if the last is incomplete, it's ignored)
  int32_t idx = 0;
  refresh_values = FALSE;
  for (idx = 0; idx < res / sizeof(struct js_event); idx++)
  {
    if(js_e_buffer[idx].type & JS_EVENT_INIT )// If Init, the first values are ignored
    {
      break;
    }
    else if(js_e_buffer[idx].type & JS_EVENT_BUTTON )// Event Button detected
    {
      switch( js_e_buffer[idx].number )
      {
        case GP_BOARD_LEFT :
					ardrone_tool_set_ui_pad_start(js_e_buffer[idx].value);
					break;
        case GP_SIDE_RIGHT :
					ardrone_tool_set_ui_pad_r2(js_e_buffer[idx].value);
					break;
        case GP_IMPULSE :
					ardrone_tool_set_ui_pad_select(js_e_buffer[idx].value);
					break;
        case GP_SIDE_LEFT_DOWN :
					ardrone_tool_set_ui_pad_ad(js_e_buffer[idx].value);
					break;
        case GP_SIDE_LEFT_UP :
					ardrone_tool_set_ui_pad_ad(js_e_buffer[idx].value);
					break;
        default: break;
      }
    }
    else if(js_e_buffer[idx].type & JS_EVENT_AXIS )// Event Axis detected
    {
      refresh_values = TRUE;
      switch( js_e_buffer[idx].number )
      {
        case GP_PITCH:
          pitch = js_e_buffer[idx].value;
          break;
        case GP_GAZ:
          gaz = js_e_buffer[idx].value;
          break;
        case GP_ROLL:
          roll = js_e_buffer[idx].value;
          break;
        case GP_PID:
          break;
        case GP_YAW:
          yaw = js_e_buffer[idx].value;
          break;
        default:
          break;
      }
    }
    else
    {// TODO: default: ERROR (non-supported)
    }
  }

  if(refresh_values)// Axis values to refresh
    {
       ardrone_at_set_progress_cmd( 1,
                                               roll/25000.0,
                                               pitch/25000.0,
                                               -gaz/25000.0,
                                               yaw/25000.0);
    }

  return C_OK;
}

C_RESULT close_radioGP(void)
{
  close( joy_dev );

  return C_OK;
}


///////////////////////////////
//  GamePad input functions  //
///////////////////////////////

C_RESULT open_gamepad(void)
{
  C_RESULT res = C_OK;
   ardrone_tool_set_ui_pad_select(1);

  //FILE* f = fopen("/proc/bus/input/devices", "r");

 // if( f != NULL )
 // {
   // res = parse_proc_input_devices( f, GAMEPAD_LOGICTECH_ID);

   // fclose( f );

    //if( SUCCEED( res ) && strcmp(gamepad.name, "Gamepad")!=0)
    //{
			//char dev_path[20]="/dev/input/";
			//strcat(dev_path, gamepad.name);
            //joy_dev = open(dev_path, O_NONBLOCK | O_RDONLY);
    //}

		/*
		else
		{
			return C_FAIL;
		}
		*/
  //}

  return res;
}

C_RESULT update_gamepad(void)
{

 static double distance = 0;
 static double oldT = 0;
 static int takeOff_status = 0;
 static int loop = 1;
 static double dyFtime = 0;
 static int lastOpt = 0;
 static int opt = 5;
 int side = 0;
 if (oldT == 0)
    { oldT = line[time];}
  readPipe();

 // printf("Distance: %f OldT: %f \n",distance, oldT);

    target_d = meters - offset;
    newTime = ((line[time] - oldT)/factor)*m;

    if (takeOff_status == 0)
    {

    takeOff_status++;
    static int32_t start = 0;
    start ^= 1;
    ardrone_tool_set_ui_pad_start(start);
    printf("Takeoff: \n ");

    ardrone_at_set_flat_trim();


    //ardrone_at_set_progress_cmd(1,0,-0.05,0,0);
    //ardrone_tool_set_ui_pad_xy(0,1);
     //stateS = turn45(1);

     }


    rDistance = (line[vx] * newTime)* m;
    printf("rDis: %f \n", rDistance);
    distance = distance + rDistance;
   // printf("Distance: %f rDistance: %f \n", dTraveled, rDistance);
   //if ( fabs(distance) > 2.3)
  // printf("ft: %f \n", line[flightT]);


   // opt = 1;

if (line[flightT] > 10 )

{

    if (opt == 0 && line[flightT] < 30)
    { //hover

      ardrone_at_set_progress_cmd(1,0,0,0,0);
    }
    else if(line[flightT] > 30)
    {
     //printf("Land: %f \n", line[time]);
     ardrone_tool_set_ui_pad_start(0);
    }





    if (( opt == 1) || (opt == 2) )
    {

        // turn

        if (loop == 1)
        {
            dyFtime = line[flightT];
            loop = 0;
        }

    // remove later
     if ((lastOpt != 1) || (lastOpt != 2) )


       if (opt == 1) { side = 1;}
       if (opt == 2) { side = -1;}

       //printf("turning: \n");
      {stateS = turn45(side, dyFtime, line[flightT]);}

    }


    if (opt == 3)
    {

        //

        if (loop == 1)
        {
            dyFtime = line[flightT];
            loop = 0;
        }

    // remove later
     if (lastOpt != 3)



     //printf("turning: \n");
      {stateS = forward(-1, dyFtime, line[flightT], distance);}
      printf("finshed: \n");

    }


    if ((opt == 4) || (opt == 5))
    {

        //

        if (loop == 1)
        {
            dyFtime = line[flightT];
            loop = 0;
        }

    // remove later
     if ((lastOpt != 4) && (lastOpt != 5) )

     if (opt == 4) { side = 1;}
     if (opt == 5) { side = -1;}

     //printf("turning: \n");
      {stateS = turnforward(side, dyFtime, line[flightT], distance);}

    }

    if ( stateS == 0)
    {
        //ardrone_tool_set_ui_pad_start(0);
        // accept
        loop = 1;
        // hover mode
        lastOpt = opt;
        opt = 0;
        stateS = 1;

        }

}

    oldT = line[time];


  return C_OK;
}

int turn45(int LoR, double stamp, double cTime)
{
 double activeT = cTime - stamp;
 double fac = 0.45;
 double t45 = fac * LoR;
 ardrone_at_set_progress_cmd(1,0,0,0,t45);
 if (activeT > (2))
 {
    //printf("Inactive: \n");
    return 0;}
 return 1;

}


int forward(int LoR, double stamp, double cTime, double disF)
{
 //double activeT = cTime - stamp;
 double fac = 0.075 ;
 double tD = fac * LoR;
 ardrone_at_set_progress_cmd(1,0,tD,0,0);
 printf("Dis: %f\n", disF);
 if (disF > (2.0))
 {
   // printf("Inactive: \n");
    return 0;}
 return 1;

}

int turnforward(int LoR, double stamp, double cTime, double disF)
{
 double activeT = cTime - stamp;
 double fac = 0.06;
 double tD = fac * (-1);
 double fac2 = 0.45;
 double t45 = fac2 * LoR;

 if (activeT < (2))
 {
 ardrone_at_set_progress_cmd(1,0,tD,0,t45);
 }
 else{ ardrone_at_set_progress_cmd(1,0,tD,0,0);}

 if (disF > (2.3))
 {
    printf("Inactive: \n");
    return 0;}
 return 1;

}




void readPipe(void)

{
   line[0] = '\0';
   line[255] = '\0';
   int pipe;
   int l;
// open a named pipe
   pipe = open("/home/juney/navD", O_RDONLY);

   if (pipe == -1){
       printf("Error reading. \n");
   }

    l = read(pipe, line, 255);
/*
    if(l)
    { printf("Gamepad: %f \n",line[psi]); }
*/
   close(pipe);

}

C_RESULT close_gamepad(void)
{
  close( joy_dev );

  return C_OK;
}






///////////////////////////////
//  Playstation 3 Gamepad input functions  //
///////////////////////////////

C_RESULT open_ps3pad(void)
{
    PRINT("Searching PS3 Pad device ...\n");

  C_RESULT res = C_FAIL;

  FILE* f = fopen("/proc/bus/input/devices", "r");

  if( f != NULL )
  {
    res = parse_proc_input_devices( f, GAMEPAD_PLAYSTATION3_ID );

    fclose( f );

    if( SUCCEED( res ) && strcmp(ps3pad.name, "PS3Gamepad")!=0)
    {
			char dev_path[20]="/dev/input/";
			strcat(dev_path, ps3pad.name);
            joy_dev = open(dev_path, O_NONBLOCK | O_RDONLY);

            if (joy_dev)
            {
                printf("Joydev %s ouvert\n",dev_path);
            }
            else{
                printf("Joydev %s pas ouvert\n",dev_path);
            }
    }
		else
		{
		    PRINT("PS3 Pad device not found.\n");
			return C_FAIL;

		}
  }

  return res;
}

C_RESULT update_ps3pad(void)
{

  static int32_t stick1LR = 0, stick1UD = 0 , stick2LR = 0 , stick2UD = 0;
  static bool_t refresh_values = FALSE;
  ssize_t res;
  static struct js_event js_e_buffer[64];
  static int32_t start = 0;
  input_state_t* input_state;

  static int center_x1=0;
  static int center_y1=0;
  static int center_x2=0;
  static int center_y2=0;

  res = read(joy_dev, js_e_buffer, sizeof(struct js_event) * 64);


  if( !res || (res < 0 && errno == EAGAIN) )
    return C_OK;


  if( res < 0 )
    return C_FAIL;


  if (res < (int) sizeof(struct js_event))// If non-complete bloc: ignored
    return C_OK;


  int32_t idx = 0;
  refresh_values = FALSE;
  input_state = ardrone_tool_get_input_state();
  for (idx = 0; idx < res / sizeof(struct js_event); idx++)
  {
    if(js_e_buffer[idx].type & JS_EVENT_INIT )// If Init, the first values are ignored
    {
      break;
    }
    else if(js_e_buffer[idx].type & JS_EVENT_BUTTON )// Event Button detected
    {
      switch( js_e_buffer[idx].number )
      {
        case PS3BTN_LEFTARROW :
		ardrone_tool_set_ui_pad_ag(js_e_buffer[idx].value);
		break;
        case PS3BTN_DOWNARROW :
		ardrone_tool_set_ui_pad_ab(js_e_buffer[idx].value);
		break;
        case PS3BTN_RIGHTARROW :
		ardrone_tool_set_ui_pad_ad(js_e_buffer[idx].value);
		break;
        case PS3BTN_UPARROW :
		ardrone_tool_set_ui_pad_ah(js_e_buffer[idx].value);
		break;
        case PS3BTN_L1 :
		ardrone_tool_set_ui_pad_l1(js_e_buffer[idx].value);
		break;
        case PS3BTN_R1 :
		ardrone_tool_set_ui_pad_r1(js_e_buffer[idx].value);
		break;
        case PS3BTN_L2 :
		ardrone_tool_set_ui_pad_l2(js_e_buffer[idx].value);
		break;
        case PS3BTN_R2 :
		ardrone_tool_set_ui_pad_r2(js_e_buffer[idx].value);
		break;
        case PS3BTN_SELECT :
		ardrone_tool_set_ui_pad_select(js_e_buffer[idx].value);
		break;
        case PS3BTN_START :
            if( js_e_buffer[idx].value )   { start ^= 1;  ardrone_tool_set_ui_pad_start( start );   }
		break;
        case PS3BTN_PS3:
            /* Calibrate joystick */
           /* center_x1 = stick1LR;
            center_y1 = stick1UD;
            center_x2 = stick2UD;
            center_y2 = stick2LR;*/
        break;
        default:
		break;
      }
    }
    else if(js_e_buffer[idx].type & JS_EVENT_AXIS )// Event Axis detected
    {
      refresh_values = TRUE;
      switch( js_e_buffer[idx].number )
      {
        case PS3AXIS_STICK1_LR:
          stick1LR = ( js_e_buffer[idx].value ) ;
          break;
        case PS3AXIS_STICK1_UD:
          stick1UD = ( js_e_buffer[idx].value ) ;
          break;

        case PS3AXIS_STICK2_LR:
          stick2LR = ( js_e_buffer[idx].value ) ;
          break;
        case PS3AXIS_STICK2_UD:
          stick2UD = ( js_e_buffer[idx].value ) ;
          break;

        default:
          break;
      }
    }
    else
    {// TODO: default: ERROR (non-supported)
    }
  }

  if(refresh_values)// Axis values to refresh
    {
      ardrone_at_set_progress_cmd( 1,
                                    /*roll*/(float)(stick1LR-center_x1)/32767.0f,
                                    /*pitch*/(float)(stick1UD-center_y1)/32767.0f,
                                    /*gaz*/-(float)(stick2UD-center_x2)/32767.0f,
                                    /*yaw*/(float)(stick2LR-center_y2)/32767.0f );
    }
  return C_OK;
}





C_RESULT close_ps3pad(void)
{
  close( joy_dev );

  return C_OK;
}





static int32_t make_id(device_t* device)
{
  return ( (device->vendor << 16) & 0xffff0000) | (device->product & 0xffff);
}

static C_RESULT add_device(device_t* device, const int32_t id_wanted)
{
  int32_t id = make_id(device);

  if( id_wanted == GAMEPAD_LOGICTECH_ID && id == id_wanted)
  {
		PRINT("logitech\n");
		PRINT("Input device %s found\n", device->name);
    strncpy(gamepad.name, device->handlers, MAX_NAME_LENGTH);
    return C_OK;
	}

	if(id_wanted == RADIO_GP_ID && id==id_wanted)
	{
	    PRINT("radio");
		PRINT("Input device %s found\n", device->name);
    strncpy(radioGP.name, device->handlers, MAX_NAME_LENGTH);
    return C_OK;
  }

    if(id_wanted == GAMEPAD_PLAYSTATION3_ID && id==id_wanted)
	{
	    PRINT("ps3");
		PRINT("PS3 : Input device %s found\n", device->name);
        strncpy(ps3pad.name, device->handlers, MAX_NAME_LENGTH);
        return C_OK;
  }

  return C_FAIL;
}





/** simple /proc/bus/input/devices generic LL(1) parser **/

#define KW_MAX_LEN 64

typedef enum {
  KW_BUS,
  KW_VENDOR,
  KW_PRODUCT,
  KW_VERSION,
  KW_NAME,
  KW_HANDLERS,
  KW_MAX
} keyword_t;

typedef enum {
  INT,
  STRING,
} value_type_t;

typedef struct {
  const char*   name;
  value_type_t  value_type;
  int32_t       value_offset;
} kw_tab_entry_t;

static int current_c;
static int next_c; // look ahead buffer

static device_t current_device;

static const int separators[] = { ' ',  ':', '=', '\"', '\n' };
static const int quote = '\"';
static const int eol = '\n';

static kw_tab_entry_t kw_tab[] = {
  [KW_BUS]      = {  "Bus",      INT,    offsetof(device_t, bus)       },
  [KW_VENDOR]   = {  "Vendor",   INT,    offsetof(device_t, vendor)    },
  [KW_PRODUCT]  = {  "Product",  INT,    offsetof(device_t, product)   },
  [KW_VERSION]  = {  "Version",  INT,    offsetof(device_t, version)   },
  [KW_NAME]     = {  "Name",     STRING, offsetof(device_t, name)      },
  [KW_HANDLERS] = {  "Handlers", STRING, offsetof(device_t, handlers)  }
};

static const char* handler_names[] = {
  "js0",
  "js1",
  "js2",
  "js3",
  0
};

static bool_t is_separator(int c)
{
  int32_t i;
  bool_t found = FALSE;

  for( i = 0; i < sizeof separators && !found; i++ )
  {
    found = ( c == separators[i] );
  }

  return found;
}

static bool_t is_quote(int c)
{
  return c == quote;
}

static bool_t is_eol(int c)
{
  return c == eol;
}

static C_RESULT fetch_char(FILE* f)
{
  C_RESULT res = C_FAIL;

  current_c = next_c;

  if( !feof(f) )
  {
    next_c = fgetc(f);
    res = C_OK;
  }

  // PRINT("current_c = %c, next_c = %c\n", current_c, next_c );

  return res;
}

static C_RESULT parse_string(FILE* f, char* str, int32_t maxlen)
{
  int32_t i = 0;
  bool_t is_quoted = is_quote(current_c);

  if( is_quoted )
  {
    while( SUCCEED(fetch_char(f)) && ! ( is_separator(current_c) && is_quote(current_c) ) )  {
      str[i] = current_c;
      i++;
    }
  }
  else
  {
    while( SUCCEED(fetch_char(f)) && !is_separator(current_c) )  {
      str[i] = current_c;
      i++;
    }
  }

  str[i] = '\0';
  // PRINT("parse_string: %s\n", str);

  return is_eol( current_c ) ? C_FAIL : C_OK;
}

static C_RESULT parse_int(FILE* f, int32_t* i)
{
  C_RESULT res = C_OK;
  int value;

  *i = 0;

  while( !is_separator(next_c) && SUCCEED(fetch_char(f)) && res == C_OK )  {
    value = current_c - '0';

    if (value > 9 || value < 0)
    {
      value = current_c - 'a' + 10;
      res = (value > 0xF || value < 0xa) ? C_FAIL : C_OK;
    }

    *i *= 16;
    *i += value;
  }

  return res;
}

static C_RESULT skip_line(FILE* f)
{
  while( !is_eol(next_c) && SUCCEED(fetch_char(f)) );

  return C_OK;
}

static C_RESULT match_keyword( const char* keyword, keyword_t* kw )
{
  int32_t i;
  C_RESULT res = C_FAIL;

  for( i = 0; i < KW_MAX && res != C_OK; i++ )
  {
    res = ( strcmp( keyword, kw_tab[i].name ) == 0 ) ? C_OK : C_FAIL;
  }

  *kw = i-1;

  return res;
}

static C_RESULT match_handler( void )
{
  int32_t i = 0;
  bool_t found = FALSE;

  while( !found && handler_names[i] != 0 )
  {
    found = strcmp( (char*)((char*)&current_device + kw_tab[KW_HANDLERS].value_offset), handler_names[i] ) == 0;

    i ++;
  }

	if(found)
	{
		strcpy(current_device.handlers, handler_names[i-1]);
	}

  return found ? C_OK : C_FAIL;
}

static C_RESULT parse_keyword( FILE* f, keyword_t kw )
{
  C_RESULT res = C_OK;

  while( is_separator(next_c) && SUCCEED(fetch_char(f)) );

  switch( kw_tab[kw].value_type ) {
    case INT:
      parse_int( f, (int32_t*)((char*)&current_device + kw_tab[kw].value_offset) );
      //PRINT("%s = %x\n", kw_tab[kw].name, *(int32_t*)((char*)&current_device + kw_tab[kw].value_offset) );
      break;

    case STRING:
      parse_string( f, (char*)((char*)&current_device + kw_tab[kw].value_offset), KW_MAX_LEN );
      //PRINT("%s = %s\n", kw_tab[kw].name, (char*)((char*)&current_device + kw_tab[kw].value_offset) );
      break;

    default:
      res = C_FAIL;
      break;
  }

  return res;
}

static C_RESULT parse_I(FILE* f)
{
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    keyword_t kw;

    parse_string( f, keyword, KW_MAX_LEN );
    if( SUCCEED( match_keyword( keyword, &kw ) ) )
    {
      parse_keyword( f, kw );
    }
  }

  return C_OK;
}

static C_RESULT parse_N(FILE* f)
{
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    keyword_t kw;

    parse_string( f, keyword, KW_MAX_LEN );
    if( SUCCEED( match_keyword( keyword, &kw ) ) )
    {
      parse_keyword( f, kw );
    }
  }


  return C_OK;
}

static C_RESULT parse_H(FILE* f)
{
  C_RESULT res = C_FAIL;
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    parse_string( f, keyword, KW_MAX_LEN );
    if( strcmp( keyword, kw_tab[KW_HANDLERS].name ) == 0 )
    {
      while( FAILED(res) && SUCCEED( parse_string(f,
                                                  (char*)((char*)&current_device + kw_tab[KW_HANDLERS].value_offset),
                                                  KW_MAX_LEN ) ) )
      {
        res = match_handler();
      }
    }
  }

  return res;
}

static C_RESULT end_device(const int32_t id)
{
  C_RESULT res = C_FAIL;
  res=add_device(&current_device, id);
  vp_os_memset( &current_device, 0, sizeof(device_t) );

  return res;
}

static C_RESULT parse_proc_input_devices(FILE* f, const int32_t id)
{
  C_RESULT res = C_FAIL;

  next_c = '\0';
  vp_os_memset( &current_device, 0, sizeof(device_t) );

  while( res != C_OK && SUCCEED( fetch_char(f) ))
  {
    switch( next_c )
    {
      case 'I': parse_I(f); break;
      case 'N': parse_N(f); break;
      case 'H': if( SUCCEED( parse_H(f) ) ) res = end_device(id); break;
      case 'P':
      case 'S':
      case 'B':
      default: skip_line(f); break;
    }
  }

  return res;
}




