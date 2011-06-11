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

#define SOCKET_SERVER "130.160.221.212"
#define REMOTE_SERVER_PORT 1500
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
 
static int sd, rc, i, n, echoLen, flags, error, timeOut;
static int sd_data, rc_data, i_data, n_data, echoLen_data, flags_data, error_data, timeOut_data;
static struct sockaddr_in cliAddr_data, remoteServAddr_data, echoServAddr_data;
static struct sockaddr_in cliAddr, remoteServAddr, echoServAddr;
static struct hostent *h;






/////////////////////////////Drone Movement ////////////////////////////


static WbNodeRef drone_node;
//static WbNodeRef cam_node;
static WbFieldRef drone_trans_field;
static WbFieldRef drone_rot_field;
//static WbFieldRef cam_rot_field;
//static WbDeviceTag camera; 

static int fd;
static fd_set rfds;
double coords[3] = {0,5,0};
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









//////////////////////////////////////// Movement Funtions Begin ////////////////////////
double degreeToRadian(double degree) {
    double radian = 0;
    radian = degree * (pi/180);
    return radian;
}


double getAngle() {
  
  //angle function does some finicky calculations to 
  //account for the plane being positioned at a 270 
  //degree angle
  
   double angle; 
  if ( turnState != 4){
  
  angle = (rot[3]/one_degree);
  //printf("Turn %d\n",turnState);
  }
  else{
    angle = (rot[3]/one_degree)-90;
    }
  
  //System.out.println("rot[3]: " + rot[3] + " Angle: " + angle);
  //angle = abs(angle);
  
  
  return degreeToRadian(angle);
 
}

void rotatePlane(double x, double y, double z, double alpha, char mode) {
   
   double temp_x, temp_y, temp_z, temp_alpha;
   
   if (mode != 's'){
   
    temp_x = (rot[0] + x);
    temp_y = (rot[1] + y);
    temp_z = (rot[2] + z);
    temp_alpha = (rot[3] + alpha);
    
    
    
    rot[0] = temp_x;
    rot[1] = temp_y;
    rot[2] = temp_z;
    rot[3] = temp_alpha;
    
    //rot[0] = fmod(circleScale, (rot[0] + x));
    //rot[1] = fmod(circleScale, (rot[1] + y));
    //rot[2] = fmod(circleScale, (rot[2] + z));
    //rot[3] = fmod(circleScale, (rot[3] + alpha));
    
    }
    else{
    rot[0] = x;
    rot[1] = y;
    rot[2] = z;
    //alpha_store = rot[3];
    //rot[3] = alpha;
    }
    
     wb_supervisor_field_set_sf_rotation(drone_rot_field, rot);
}












void movePlane(double y, double angle)
{

    double r = 20*sqrt(2);
    double angle_x, angle_z;
    double factor =  0.001;
    angle_x = factor * (r * sin(angle));
    angle_z = factor * (r * cos(angle));
   // printf("Angle: %f \n", angle);
   // printf("X: %f \n", angle_x);
   // printf("Z: %f \n", angle_z);
    
    coords[0] -= angle_x;
    coords[1] += y;
    coords[2] -= angle_z;
    
    wb_supervisor_field_set_sf_vec3f(drone_trans_field,coords);
}
    

void printLoc()
{
  printf("X: %f \n",coords[0]);
  printf("Y: %f \n",coords[1]);
  printf("Z: %f \n",coords[2]);
}


void leftTurn(double altitude)
{
  if (dofState != 0){
        targetAngle = rot[3] + 0.78624;
        dofState = 0;
        }
      if (rot[3] < targetAngle)
      {
        rotatePlane(0.0,0.2,0,move_left,'i');
      }
      if (distance < targetDis){
        movePlane(altitude,getAngle());
        distance++;
        }
      if (!(rot[3] < targetAngle) && !(distance < targetDis)){
        dofState = 1;
        simState = 0;
        distance = 0;
        }
}

void forward(double altitude)
{
if (dofState != 0){
        dofState = 0;
        }
      if (distance < targetDis){
      movePlane(altitude,getAngle());
      //printf("Distance: %i \n", distance);
      distance++;
      }
      else if (!(distance < targetDis)){
        dofState = 1;
        simState = 0;
        distance = 0;
      }
      
}

void rightTurn(double altitude)
{

  if (dofState != 0){
        targetAngle = rot[3] - 0.78624;
        dofState = 0;
        }
      if (rot[3] > targetAngle)
      {
        rotatePlane(0.0,0.2,0,move_right,'i');
      }
      if (distance < targetDis){
        movePlane(altitude,getAngle());
        distance++;
        }
      if (!(rot[3] > targetAngle) && !(distance < targetDis)){
        dofState = 1;
        simState = 0;
        distance = 0;
        }
}


void dofSelection(int DOF)
{
  
  double targetY; 
  // dofState = 0->locked , 1->open
  
  turnState = DOF;
  switch(DOF){
   
    case 0:
      leftTurn(0.01);
      break;
    case 1:
      forward(0.01);
      break;
    case 2:
      rightTurn(0.01);
      break;
    case 3:
      //printf("TargetZ: %f TargetX: %f CurrentZ: %f CurrentX: %f \n", targetZ, targetX, coords[2], coords[0]);
      leftTurn(0.0);
      break;
    case 4:
      //printf("TargetZ: %f CurrentZ: %f \n", targetZ, coords[2]);
      forward(0.0);
      
      break;
    case 5:
      rightTurn(0.0);
      break;
    case 6:
      leftTurn(-0.01);
      break;
    case 7:
      forward(-0.01);
      break;
    case 8:
      rightTurn(-0.01);
      break;
    default:
      ;
  }
}



//////////////////////////////////////// Movement Funtions End ////////////////////////



 void clientInit(void)
 {
 
 // Video Socket ////////////////////////////////////////////////////////////////////////
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

 
 
 
 
void *keyInput()
{

// create server socket /// 

    int sfd, rc;
    struct sockaddr_in address;

#ifdef WIN32
    /* initialize the socket api */
    WSADATA info;

    rc = WSAStartup(MAKEWORD(1, 1), &info); /* Winsock 1.1 */
    if (rc != 0) {
        printf("cannot initialize Winsock\n");
        return -1;
    }
#endif
    /* create the socket */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        printf("cannot create socket\n");
        return -1;
    }

    /* fill in socket address */
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons((unsigned short) SOCKET_PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    /* bind to port */
    rc = bind(sfd, (struct sockaddr *) &address, sizeof(struct sockaddr));
    if (rc == -1) {
        printf("cannot bind port %d\n",SOCKET_PORT);
        return -1;
    }

    /* listen for connections */
    if (listen(sfd, 1) == -1) {
        printf("cannot listen for connections\n");
        return -1;
    }
    printf("Waiting for a connection on port %d...\n",SOCKET_PORT);
  
  //////////////////////////////


  // translation 
  
  server_fd = sfd;
  


   // Accept client //////
  
    int cfd;
    struct sockaddr_in client;
#ifndef WIN32
    socklen_t asize;
#else
    int asize;
#endif 
    struct hostent *client_info;

    asize = sizeof(struct sockaddr_in);

    cfd = accept(server_fd, (struct sockaddr *) &client, &asize);
    if (cfd == -1) {
        printf("cannot accept client\n");
        return -1;
    }
    client_info = gethostbyname((char *) inet_ntoa(client.sin_addr));
    printf("Accepted connection from: %s \n",
                         client_info->h_name);

    //return cfd;


////////////////////////////////////




  
    while (true){
     n = recv(fd, buffer, 256, 0);
    if (n < 0) {
        printf("error reading from socket\n");
    }
    }
  
  }

void *trialFunc(){


  
  
    move_right = -(3*one_degree);
    move_left  = (3*one_degree);
    
    
    
    drone_node = wb_supervisor_node_get_from_def("Drone1");
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
        
        //clientInit();
        
      
  do {
       data_pack1 =  wb_camera_get_image(cam1);
       //mainUdp();
       
       dofSelection(RIGHT);
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
  


}


void *data()

{




}



int main(int argc, char **argv)
{


  /* necessary to initialize webots stuff */
  //static WbFieldRef cam_field;
  
  
  
   //pthread_t key_pth; //thread for keyboard input
   //pthread_create(&key_pth,NULL,keyInput,"Data stuff.. \n"); 
   //pthread_t trial_pth; //thread for keyboard input
   //pthread_create(&trial_pth,NULL,trialFunc,"Data stuff.. \n"); //begin reading keyboard input
  
  
  wb_robot_init();
  
  
    move_right = -(3*one_degree);
    move_left  = (3*one_degree);
    
    
    
    drone_node = wb_supervisor_node_get_from_def("Drone1");
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
        
        clientInit();
        
      
  do {
       data_pack1 =  wb_camera_get_image(cam1);
       mainUdp();
       
       dofSelection(FORWARD);
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
