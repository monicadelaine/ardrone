/*
 * File:         
 * Date:         
 * Description:  
 * Author:       
 * Modifications:
 */

/*
 * You may need to add include files like <webots/distance_sensor.h> or
 * <webots/differential_wheels.h>, etc.
 */
#include <webots/robot.h>

#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/emitter.h>
#include <webots/receiver.h>
#include <webots/servo.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webots/camera.h>
#include <webots/supervisor.h>
#include <pthread.h>

#define SOCKET_SERVER1 "130.160.221.212"
#define SOCKET_SERVER "130.160.68.24"
#define SOCKET_SERVER1 "130.160.47.64"
#define REMOTE_SERVER_PORT 1502
#define REMOTE_SERVER_PORT2 1501
#define SOCKET_ERROR -1

#define pi 3.14159265


#define TIMESTEP 256
#define LEFT 3
#define FORWARD 4
#define RIGHT 5

/*
 * You may want to add defines macro here.
 */
#define TIME_STEP 64
const unsigned char *data_pack1;
int width;
int height;
int size;

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */



/////////////////////TCP/////////////////

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>         /* definition of struct sockaddr_in */
#include <netdb.h>              /* definition of gethostbyname */
#include <arpa/inet.h>          /* definition of inet_ntoa */
#include <unistd.h>    
int server_fd;
#define SOCKET_PORT 1500

/////////////////////////////////////////////


/*
 * You should put some helper functions here
 */

/*
 * This is the main program.
 * The arguments of the main function can be specified by the
 * "controllerArgs" field of the Robot node
 */
 
static int sd, clientSocket,  rc, i, n, echoLen, flags, error, timeOut;
static int sd_data, rc_data, i_data, n_data, echoLen_data, flags_data, error_data, timeOut_data;
static struct sockaddr_in cliAddr_data, remoteServAddr_data, echoServAddr_data;
static struct sockaddr_in cliAddr, remoteServAddr, echoServAddr;
static struct hostent *h;
int pack;

struct  sockaddr_in sad; /* structure to hold an IP address     */
  //int     clientSocket;    /* socket descriptor                   */
  struct  hostent  *ptrh;  /* pointer to a host table entry       */

  char    *host;           /* pointer to host name                */
 // int     port;            /* protocol port number                */

  char    Sentence[128];
  char    modifiedSentence[128];
  char    buff[128];
  //int     n;
  
          /* if host argument specified   */

  /* Extract port number  from command-line argument */
 int  port = 1502;   /* convert to binary */   






/////////////////////////////Drone Movement ////////////////////////////


static WbNodeRef drone_node;
//static WbNodeRef cam_node;
static WbFieldRef drone_trans_field;
static WbFieldRef drone_rot_field;
//static WbFieldRef cam_rot_field;
//static WbDeviceTag camera; 

static int fd;
static fd_set rfds;
double coords[3] = {-19.9352,4.21,-19.9403};
double rot[4] = {0,1,0,0};
double camCoords[4] = {1,0,0,5.2774};
double circleScale = 6.29;
static double one_degree = 0.017472;
static double rot_x, rot_y, rot_alpha, alpha_store;
double move_right, move_left, targetZ, targetX; 
int dofState = 1;
int simState = 0; // 1-> runing 0-> not runing
double currentAngle, targetAngle;
int x, y, z;
int distance = 0;
int targetDis = 15;
float Plane, Cam;
int timeout;
int turnState;
int keyVal;
int buffer[256];
int ret;






//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Movement Funtions Begin /////////////////////
//////////////////////////////////////// Movement Funtions End ////////////////////////



 void clientInit()
 {
  printf("Connecting: \n");
 // Video Socket ////////////////////////////////////////////////////////////////////////
  
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
	  sd = socket(PF_INET,SOCK_DGRAM,0);
	  if(sd<0) {
    printf("%s: cannot open socket \n","program");
	    exit(1);
	  }
	
	  /* bind any port */
	  cliAddr.sin_family = AF_INET;
	  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	  cliAddr.sin_port = htons(REMOTE_SERVER_PORT);
	
	  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
	  if(rc<0) {
	    printf("%s: cannot bind port\n", "program");
	    exit(1);
	  }
	
	/* BEGIN jcs 3/30/05 */
	
	  flags = 0;
	
	  timeOut = 100; // m
 }
 
 
 int mainUdp() {

    ////////////////////////////////////////////////// SEND SIZE //////////////////////////
    int sent = 0;

   while ( sent < 1 )
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
              //  printf("Packet sent!\n");
                }
                sent++;
                break;
                default:
                    printf("Default Reached \n");
                    break;
        }
        }


 // }

  return 1;

}

void *video()

{

  while (true){
  if (pack > 0) {
  mainUdp();
  }
}

}



int main(int argc, char **argv)
{
pack = 0;

   
  /*
  host = "juney"; 
  // create socket // 
  
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if (clientSocket < 0) {
    fprintf(stderr, "socket creation failed\n");
    exit(1);
  }

  /* determine the server's address */

//  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
//  sad.sin_port = htons((u_short)port);
//  ptrh = gethostbyname(host); /* Convert host name to equivalent IP address and copy to sad. */
 // if ( ((char *)ptrh) == NULL ) {
  //  fprintf(stderr,"invalid host: %s\n", host);
 //   exit(1);
 // }
//  memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);



// socket //


   
   clientInit();

  /* necessary to initialize webots stuff */
  //static WbFieldRef cam_field;
  
  
  
   //pthread_t key_pth; //thread for keyboard input
   //pthread_create(&key_pth,NULL,keyInput,"Data stuff.. \n"); 
    //begin reading keyboard input
  
  
  
  wb_robot_init();
  printf("top\n");
  //pthread_t vid; //thread for keyboard input
  //pthread_create(&vid,NULL,video,"Data stuff.. \n");
  
  
    move_right = -(3*one_degree);
    move_left  = (3*one_degree);
    
    
    
    drone_node = wb_supervisor_node_get_from_def("Drone2");
   // cam_node = wb_supervisor_node_get_from_def("CamNode");
    drone_trans_field = wb_supervisor_node_get_field(drone_node,"translation");
    drone_rot_field = wb_supervisor_node_get_field(drone_node,"rotation");
   // cam_rot_field = wb_supervisor_node_get_field(cam_node,"rotation");
  
  
  
  
  
  
  static WbDeviceTag cam1; 
 // static WbDeviceTag cam2; 
 cam1 = wb_robot_get_device("camera1");
  
 width = wb_camera_get_width(cam1);
 height = wb_camera_get_height(cam1);
 printf("Height: %d \n Width %d \n",height,width);
 size = width * height * 3;
 data_pack1 = malloc(size);

  size = width * height * 3;
 
  //cam2 = wb_robot_get_device("camera2");
 
 
 //width = wb_camera_get_width(camera_1);
 //height = wb_camera_get_height(camera_1);
 //data_pack1 = malloc(size * sizeof(char));
  
    
  //initialize();

  
 
 
   //pthread_t video_pth1;
   //pthread_create(&video_pth1,NULL,VideoThread,"Vid Thread..\n");

  
  
  /*
   * You should declare here DeviceTag variables for storing
   * robot devices like this:
   *  WbDeviceTag my_sensor = wb_robot_get_device("my_sensor");
   *  WbDeviceTag my_actuator = wb_robot_get_device("my_actuator");
   */
  
  /* main loop */
  
      WbDeviceTag rotor_1 = wb_robot_get_device("rotor1");
      WbDeviceTag rotor_2 = wb_robot_get_device("rotor2");
      WbDeviceTag rotor_3 = wb_robot_get_device("rotor3");
       WbDeviceTag rotor_4 = wb_robot_get_device("rotor4");
      
       WbDeviceTag rotor_5 = wb_robot_get_device("rotor5");
       WbDeviceTag rotor_6 = wb_robot_get_device("rotor6");
       WbDeviceTag rotor_7 = wb_robot_get_device("rotor7");
       WbDeviceTag rotor_8 = wb_robot_get_device("rotor8");
       
      
      wb_servo_set_position(rotor_1, WB_SERVO_INFINITY);
      wb_servo_set_position(rotor_2, WB_SERVO_INFINITY);
       wb_servo_set_position(rotor_3, WB_SERVO_INFINITY);
       wb_servo_set_position(rotor_4, WB_SERVO_INFINITY);
       
       wb_servo_set_position(rotor_5, WB_SERVO_INFINITY);
       wb_servo_set_position(rotor_6, WB_SERVO_INFINITY);
       wb_servo_set_position(rotor_7, WB_SERVO_INFINITY);
       wb_servo_set_position(rotor_8, WB_SERVO_INFINITY);
      
       
      
       wb_camera_enable(cam1, TIMESTEP);
       //wb_camera_enable(cam2, TIMESTEP);
        
       
      
       // wb_camera_get_image(camera_2);
       // int r = wb_camera_image_get_red(data_pack1,width,20,20);
        // printf("Data: %d\n",r);
       
        
        
        
      
  do {
       data_pack1 =  wb_camera_get_image(cam1);
       pack++;
       mainUdp();
       
       dofSelection(LEFT);
       //printf("Data: %c\n",data_pack1[size]);
        //wb_camera_get_image(camera_2);
       // int r = wb_camera_image_get_red(data_pack1,width,0,0);
        
       
      
    /* 
     * Read the sensors :
     * Enter here functions to read sensor data, like:
     *  double val = wb_distance_sensor_get_value(my_sensor);
     */
    
    /* Process sensor data here */
    
    /*
     * Enter here functions to send actuator commands, like:
     * wb_differential_wheels_set_speed(100.0,100.0);
     */
    
    /* 
     * Perform a simulation step of 64 milliseconds
     * and leave the loop when the simulation is over
     */
  } while (wb_robot_step(TIME_STEP) != -1);
  
  /* Enter here exit cleanup code */
  
  /* Necessary to cleanup webots stuff */
  wb_robot_cleanup();
  
   ret = close(fd);
   if (ret != 0) {
      printf("Cannot close socket\n");
    }
  
  return 0;
  
}
