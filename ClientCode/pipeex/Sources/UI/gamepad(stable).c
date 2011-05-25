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

#include "ihm/ihm_stages_o_gtk.h"










#define REMOTE_SERVER_PORT 1500
#define MAX_MSG 100

/* BEGIN jcs 3/30/05 */

#define SOCKET_ERROR -1


#include <linux/joystick.h>

#include <gtk/gtk.h>
#include <ardrone_api.h>
#include <VP_Os/vp_os_print.h>
#include "gamepad.h"


#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#define SOCKET_PORT 10020
#define SOCKET_SERVER "130.160.47.134"   /* local host */

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












static int fd;
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
//extern int32_t pixbuf_width      = 0;

extern int32_t copy_width;
extern int32_t copy_height;
extern int32_t copy_rowstide;
extern uint8_t* copy_data;
extern GtkImage* image_copy;

extern gchar* buf;
extern gsize buf_size;
extern int set;


extern uint8_t  data_pack1[57600];
extern uint8_t  data_pack2[57600];
extern uint8_t  data_pack3[57600];
extern uint8_t  data_pack4[57600];
extern uint8_t  data_pack5[57600];

static int sd, rc, i, n, echoLen, flags, error, timeOut;
static struct sockaddr_in cliAddr, remoteServAddr, echoServAddr;
static struct hostent *h;
static char msg[MAX_MSG];
int size_of_buffer[16];
int sent = 0;
int tOut = 0;




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
   // ardrone_tool_set_ui_pad_select(1);
 //   printf("Connecting: \n");
    clientConnect();
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

 //serverMsg();
 mainUdp();
 // printf("Packet1[0]: %d\n",data_pack1[0]);
// printf("height: %d \n", copy_width);

 static double distance = 0;
 static double oldT = 0;
 static int takeOff_status = 0;
 static int loop = 1;
 static double dyFtime = 0;
 static int lastOpt = 0;
 static int opt = 0;
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
   // printf("rDis: %f \n", rDistance);
    if (abs(rDistance) < 1)
    {distance = distance + rDistance;}
   // printf("Distance: %f rDistance: %f \n", dTraveled, rDistance);
   //if ( fabs(distance) > 2.3)
  // printf("ft: %f \n", line[flightT]);


   // opt = 1;

if (line[flightT] > 10 )

{
//30
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
        {

       if (opt == 1) { side = 1;}
       if (opt == 2) { side = -1;}

       //printf("turning: \n");
      stateS = turn45(side, dyFtime, line[flightT]);
        }

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
      //printf("finshed: \n");

    }


    if ((opt == 4) || (opt == 5))
    {


        //

        if (loop == 1)
        {
            dyFtime = line[flightT];
            //printf("Stamp: %f \n",dyFtime);
            loop = 0;
        }

    // remove later
     if ((lastOpt != 4) && (lastOpt != 5) ){

     if (opt == 4) { side = 1;}
     if (opt == 5) { side = -1;}


     //printf("turning: \n");
      double temp = line[flightT] - dyFtime;

      //printf("Active: %f \n", temp);
      stateS = turnforward(side, dyFtime, line[flightT], distance);

    }

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



int isReadable(int sd,int * error,int timeOut) { // milliseconds
  fd_set socketReadSet;
  FD_ZERO(&socketReadSet);
  FD_SET(sd,&socketReadSet);
  struct timeval tv;
  if (timeOut) {
    tv.tv_sec  = timeOut / 1000;
    tv.tv_usec = (timeOut % 1000) * 1000;
  } else {
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
  } // if
  if (select(sd+1,&socketReadSet,0,0,&tv) == SOCKET_ERROR) {
    *error = 1;
    return 0;
  } // if
  *error = 0;
  return FD_ISSET(sd,&socketReadSet) != 0;
} /* isReadable */

/* END jcs 3/30/05 */

int mainUdp() {





/* END jcs 3/30/05 */

  /* send data */
  //for(i=2;i<2;i++) {
      //define size & give buffer
    if (set == 1){
    //int sSize = (int)buf[0];

    // char* p [256];
    //p[0] = (char*)&buf[0];

   // printf("buf size: %d\n",buf_size);

     //printf((gint64)&buf[0]);
    // printf("\n");

     size_of_buffer[0] = (int)buf_size;
  //   printf("Client got this: ");
  //   printf((int)&buf[0]);
   //  printf("\n");





    ////////////////////////////////////////////////// SEND SIZE //////////////////////////
    sent = 0;

   while ( sent < 5 )
    {



        switch(sent)
        {



            //////////////////////////////////////////////////////////////////// packet 1 ////////////////////////////
            case 0:
                rc = sendto(sd,data_pack1,57600, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));


                 if(rc<0) {
                printf("%s: cannot send size data %d \n","program",i-1);
                close(sd);
                //exit(1);
                }
                else{
                   // printf("Sent Packet %d\n",sent);
                }


                vp_os_memset(msg,0x0,MAX_MSG);


                // Make sure we break back to send if packet wasn't sent
                 tOut = 0;
                 while (!isReadable(sd,&error,timeOut))
                 {
                 printf(".");
                 tOut++;
                 //printf("tOut: %d\n",tOut);

                 // break out while loop
                 if (tOut > 50)
                   // printf("Breaking out of timeout loop\n\n\n\n\n\n\n");
                    break;
                 }

                 // Switch statement break
                 if (tOut > 50)
                    break;
                // printf("After timeout im not here\n"); //temp
               // printf("\n");



                        //Did server get message?
                    /* receive echoed message */
                    echoLen = sizeof(echoServAddr);
                    printf("I AM RECEIVING!!!!!!!!!!!!!!!!!!!!!!!! \n");
                    n = recvfrom(sd, msg, MAX_MSG, flags,
                    (struct sockaddr *) &echoServAddr, &echoLen);

                    if(n<0) {
                    printf("%s: cannot receive buffer data \n","program");
                    // continue;
                    }

                    /* print received message */
                    /*
                    printf("%s: echo from %s:UDP%u : %s \n",
                    "program",inet_ntoa(echoServAddr.sin_addr),
                    ntohs(echoServAddr.sin_port),msg);
                    */

                    // Only send next package when server says it got the previous one
                    if( strncmp(msg,"yes",4) == 0)
                    {
                   // printf("Server got packet %d\n",sent);
                    sent++;

                    }

                break;




                ////////////////////////////////////////////////////////////////// packet 2 ////////////////////////
            case 1:
                rc = sendto(sd,data_pack2,57600, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));




                 if(rc<0) {
                printf("%s: cannot send size data %d \n","program",i-1);
                close(sd);
                //exit(1);
                }
                 else{
                    printf("Sent Packet %d\n",sent);
                }



                  vp_os_memset(msg,0x0,MAX_MSG);


                // Make sure we break back to send if packet wasn't sent
                 tOut = 0;
                 while (!isReadable(sd,&error,timeOut))
                 {
                 printf(".");
                 tOut++;
                 if (tOut > 50)
                    break;
                 }

                   if (tOut > 200)
                    break;



                        //Did server get message?
                    /* receive echoed message */
                    echoLen = sizeof(echoServAddr);
                    n = recvfrom(sd, msg, MAX_MSG, flags,
                    (struct sockaddr *) &echoServAddr, &echoLen);

                    if(n<0) {
                    printf("%s: cannot receive buffer data \n","program");
                    // continue;
                    }

                    /* print received message */
                    printf("%s: echo from %s:UDP%u : %s \n",
                    "program",inet_ntoa(echoServAddr.sin_addr),
                    ntohs(echoServAddr.sin_port),msg);


                    // Only send next package when server says it got the previous one
                    if( strncmp(msg,"yes",4) == 0)
                    {
                    printf("Server got packet %d\n",sent);
                    sent++;

                    }









                break;


                ////////////////////////////////////////////     packet 3 ///////////////////////////////////
            case 2:
                rc = sendto(sd,data_pack3,57600, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));




                 if(rc<0) {
                printf("%s: cannot send size data %d \n","program",i-1);
                close(sd);
                //exit(1);
                }
                 else{
                    printf("Sent Packet %d\n",sent);
                }


                  vp_os_memset(msg,0x0,MAX_MSG);


                // Make sure we break back to send if packet wasn't sent
                 tOut = 0;
                 while (!isReadable(sd,&error,timeOut))
                 {
                 printf(".");
                 tOut++;
                 if (tOut > 50)
                    break;
                 }


                 if (tOut > 50)
                    break;



                        //Did server get message?
                    /* receive echoed message */
                    echoLen = sizeof(echoServAddr);
                    n = recvfrom(sd, msg, MAX_MSG, flags,
                    (struct sockaddr *) &echoServAddr, &echoLen);

                    if(n<0) {
                    printf("%s: cannot receive buffer data \n","program");
                    // continue;
                    }

                    /* print received message */
                    printf("%s: echo from %s:UDP%u : %s \n",
                    "program",inet_ntoa(echoServAddr.sin_addr),
                    ntohs(echoServAddr.sin_port),msg);


                    // Only send next package when server says it got the previous one
                    if( strncmp(msg,"yes",4) == 0)
                    {
                    printf("Server got packet %d\n",sent);
                    sent++;

                    }

                break;
            case 3:
                rc = sendto(sd,data_pack4,57600, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));




                 if(rc<0) {
                printf("%s: cannot send size data %d \n","program",i-1);
                close(sd);
                //exit(1);
                }
                 else{
                    printf("Sent Packet %d\n",sent);

                }


                  vp_os_memset(msg,0x0,MAX_MSG);


                // Make sure we break back to send if packet wasn't sent
                 tOut = 0;
                 while (!isReadable(sd,&error,timeOut))
                 {
                 printf(".");
                 tOut++;
                 if (tOut > 50)
                    break;
                 }

                 if (tOut > 50)
                    break;

                        //Did server get message?
                    /* receive echoed message */
                    echoLen = sizeof(echoServAddr);
                    n = recvfrom(sd, msg, MAX_MSG, flags,
                    (struct sockaddr *) &echoServAddr, &echoLen);

                    if(n<0) {
                    printf("%s: cannot receive buffer data \n","program");
                    // continue;
                    }

                    /* print received message */
                    printf("%s: echo from %s:UDP%u : %s \n",
                    "program",inet_ntoa(echoServAddr.sin_addr),
                    ntohs(echoServAddr.sin_port),msg);


                    // Only send next package when server says it got the previous one
                    if( strncmp(msg,"yes",4) == 0)
                    {
                    printf("Server got packet %d\n",sent);
                    sent++;

                    }


                break;
          case 4:
                rc = sendto(sd,data_pack5,57600, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));




                 if(rc<0) {
                printf("%s: cannot send size data %d \n","program",i-1);
                close(sd);
                //exit(1);
                }
                 else{
                    printf("Sent Packet %d\n",sent);

                }


                  vp_os_memset(msg,0x0,MAX_MSG);


                // Make sure we break back to send if packet wasn't sent
                 tOut = 0;
                 while (!isReadable(sd,&error,timeOut))
                 {
                 printf(".");
                 tOut++;
                 if (tOut > 50)
                    break;
                 }

                 if (tOut > 50)
                    break;

                        //Did server get message?
                    /* receive echoed message */
                    echoLen = sizeof(echoServAddr);
                    n = recvfrom(sd, msg, MAX_MSG, flags,
                    (struct sockaddr *) &echoServAddr, &echoLen);

                    if(n<0) {
                    printf("%s: cannot receive buffer data \n","program");
                    // continue;
                    }

                    /* print received message */
                    printf("%s: echo from %s:UDP%u : %s \n",
                    "program",inet_ntoa(echoServAddr.sin_addr),
                    ntohs(echoServAddr.sin_port),msg);


                    // Only send next package when server says it got the previous one
                    if( strncmp(msg,"yes",4) == 0)
                    {
                    printf("Server got packet %d\n",sent);
                    sent++;

                    }




                default:
                    printf("Default Reached \n");
                    break;
        }
        }





/*


    if(rc<0) {
      printf("%s: cannot send size data %d \n","program",i-1);
      close(sd);
      exit(1);
    }

/* BEGIN jcs 3/30/05 */

    /* init buffer */
   // vp_os_memset(msg,0x0,MAX_MSG);




    /* receive echoed message */
    /*
    echoLen = sizeof(echoServAddr);
    n = recvfrom(sd, msg, MAX_MSG, flags,
      (struct sockaddr *) &echoServAddr, &echoLen);

    if(n<0) {
      printf("%s: cannot receive buffer data \n","program");
     // continue;
    }

    /* print received message */
    /*
    printf("%s: echo from %s:UDP%u : %s \n",
      "program",inet_ntoa(echoServAddr.sin_addr),
      ntohs(echoServAddr.sin_port),msg);



/*
    while( strncmp(msg,"yes",4) != 0)
    {
        printf("waiting for server response: %s \n",msg);
    }
*/


    //////////////////////////////////////////////////////


    ////////////////////////////////////////////// SEND PIXBUF ///////////////////////////////////
/*
     rc = sendto(sd,buf,size_of_buffer[0], flags,
		(struct sockaddr *) &remoteServAddr,
		sizeof(remoteServAddr));

    if(rc<0) {
      printf("%s: cannot send data %d \n","program",i-1);
      close(sd);
      exit(1);
    }

    while (!isReadable(sd,&error,timeOut)) printf(".");
    printf("\n");


     echoLen = sizeof(echoServAddr);
     n = recvfrom(sd, msg, MAX_MSG, flags,
      (struct sockaddr *) &echoServAddr, &echoLen);

       if(n<0) {
      printf("%s: cannot receive data \n","program");
     // continue;
    }

/*
     while( strncmp(msg,"yes",4) != 0)
    {
        printf("waiting for server response: %s \n",msg);
    }
    */


/* END jcs 3/30/05 */

 // }
  }

  return 1;

}

















int clientConnect(void)
{

//////////////////////////////////////////////////////////////////UDP////////////////////////



  /* check command line args */
 /*
  if(argc<3) {
    printf("usage : %s <server> <data1> ... <dataN> \n", argv[0]);
    exit(1);
  }

  */

  /* get server IP address (no check if input is IP address or DNS name */
  printf("Connecting: \n");
  h = gethostbyname(SOCKET_SERVER);
  if(h==NULL) {
    printf("%s: unknown host '%s' \n", "program", "program");
    exit(1);
  }

  printf("%s: sending data to '%s' (IP : %s) \n", "program", h->h_name,
	 inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr,
	 h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  /* socket creation */
  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    printf("%s: cannot open socket \n","program");
    exit(1);
  }

  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(1500);

  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("%s: cannot bind port\n", "program");
    exit(1);
  }

/* BEGIN jcs 3/30/05 */

  flags = 0;

  timeOut = 100; // ms

///////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////TCP //////////////////////////////////////




/*




    static struct sockaddr_in address;
    static struct hostent *server;

    static int rc;


#ifdef WIN32
    /* initialize the socket api */
   // WSADATA info;
    /*

    rc = WSAStartup(MAKEWORD(1, 1), &info); /* Winsock 1.1 */
  /*  if (rc != 0) {
        printf("cannot initialize Winsock\n");

        return -1;
    }
#endif
    /* create the socket */
    /*
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("cannot create socket\n");

        return -1;
    }

    /* fill in the socket address */
    /*
    vp_os_memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(SOCKET_PORT);
    server = gethostbyname(SOCKET_SERVER);

    if (server) {
        memcpy((char *) &address.sin_addr.s_addr, (char *) server->h_addr,
               server->h_length);
    } else {
        printf("cannot resolve server name: %s\n", SOCKET_SERVER);

        return -1;
    }

    /* connect to the server */
    /*
    rc = connect(fd, (struct sockaddr *) &address, sizeof(struct sockaddr));
    if (rc == -1) {
        printf("cannot connect to the server\n");

        return -1;
    }
    printf("connected: \n");

return 0;

*/

}


int serverMsg(void)
{


        int h = ((int)copy_height/10);
        int w = ((int)copy_width/10);
        int s = h*w;
         //printf("size %d: \n",s);
        //GtkImage* buffer[8];
        //buffer[0] = image_copy;
        uint8_t buffer2[255];
        int n;
        int32_t stride[16];
        uint8_t* d[255];


      //  buffer[n++] = '\n';     /* append carriage return */
      //  buffer[n] = '\0';
        int32_t hold[9];
      //  printf("size: %d\n",copy_data[1]);

       // memcpy(buffer,copy_data,6);

        if (copy_data != NULL)
        {

            //printf("cData: %d \n",copy_data[1]);
/*    */  //buffer[0] = copy_data[0];


/*
        int i = 0;
        for (i; i<sizeof(d); i++)
        {
            buffer[i] = d[i];
            //printf("Buf[%d]: %d\n",i,buffer[i]);
        }

*/

        //int s = sizeof(buffer)*;
        //printf("before");
      //  memcpy(buffer,copy_data,s);
        //printf("here");

        //uint8_t **pp = &p;

        //printf(p);
        //printf("--- \n");
        //printf(pp);



       // printf((int8_t)copy_data[0]);
       // printf("\n");



       ////// buffer[0] = hold;
    //buffer[1] = copy_height;
       // buffer[2] = copy_width;
      //  buffer[3] = copy_rowstide;
       //int size1 = (76800 *sizeof(uint8_t));
     // printf("sending: %d \n",copy_rowstide);
        /*
       d[0] = 1;
       uint8_t* s[76800];
       int i = 0;
       for(i; i<76800; i++)
       {
        s[i] = copy_data[i];

       }
       */
      // printf("data: %d\n",copy_data[76800]);

        //int size1 = sizeof(uint8_t)*76800;
       // printf("check: %d\n",s[1447]);
        n = send(fd,copy_data,1000000,0);





   /*     if (strncmp(buffer, "exit", 4) == 0) {
            return -1;
        }

        */

      //  n = recv(fd, buffer2, sizeof(buffer2), 0);
        buffer2[254] = '\0';

        stride[0] = copy_rowstide;
        stride[1] = copy_height;
        stride[2] = copy_width;
     //   printf("row: %d\n",copy_width);
        int size2 = sizeof(int32_t)*16;
   //       n = send(fd,stride,sizeof(stride),0 );
      //  printf("Msg Back: %d\n",buffer2[0]);
        }

        //printf("> %c", buffer);

        return 0;
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
 //double fac = 0.075 ;
 static double tD = 0.10 * -1;

  if (disF < 0)
    tD = tD - 0.01;

 printf("dis: %f td: %f \n", disF, tD);
 ardrone_at_set_progress_cmd(1,0,tD,0,0);
 //printf("Dis: %f\n", disF);

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
   // printf("Inactive: \n");
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
    #ifdef WIN32
        closesocket(fd);
        #else
        close(fd);
        #endif

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




