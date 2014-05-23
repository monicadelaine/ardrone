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
#include <math.h>

#define SOCKET_SERVER1 "130.160.221.212"	//mac server
#define SOCKET_SERVER "130.160.68.24"		//Matlab server??
#define SOCKET_SERVER1 "130.160.47.64"		//mac server
#define REMOTE_SERVER_PORT 1502 //video port
#define REMOTE_SERVER_PORT_WP 1507 //wp data port
#define REMOTE_SERVER_PORT2 1501
#define SOCKET_ERROR -1

#define pi 3.14159265


#define TIMESTEP 256
#define LEFT 3
#define FORWARD 4
#define RIGHT 5
#define X_OFFSET 19.9352
#define Y_OFFSET 19.9403
#define UNIT_X 11.37745
#define UNIT_Y 11.48603

/*
 * You may want to add defines macro here.
 */
#define TIME_STEP 64
unsigned char *data_pack1;
char pack_send[12290];
int width;
int height;
int size;
int x_send, y_send;
float turn;
int moving_block = 0;

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
float ipadCoords[2]; 
int storage[4];
int init_wp=0;
int got_info;
float temp_wp[100];
static int x_i,y_i;
int get_wp;

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

static int sd_wp, rc_wp, i_wp, n_wp, echoLen_wp, flags_wp, error_wp, timeOut_wp;
static struct sockaddr_in cliAddr_wp, remoteServAddr_wp, echoServAddr_wp;
static struct hostent *h2;

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
 
 
float waypoints[100]; //array that holds the user waypoints
int head = 0; //head of waypoints array






/////////////////////////////Drone Movement ////////////////////////////


static WbNodeRef drone_node;
//static WbNodeRef cam_node;
static WbFieldRef drone_trans_field;
static WbFieldRef drone_rot_field;
//static WbFieldRef cam_rot_field;
//static WbDeviceTag camera; 

static int fd;
static fd_set rfds;
double coords[3] = {-11,7,-16};
double rot[4] = {0,1,0,0};
double camCoords[4] = {1,0,0,5.2774};
double circleScale = 6.29;
float inti_coords[4];
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
int wp_status[64];
float wp_info[12290];
int turning_state = 0;
int waypoint_active = 0;
float start_coords[2];
int got_it = 0;
float x_coord, y_coord;
float curr_wp;
float last_heading = 0;



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
    angle = (rot[3]/one_degree)-90; //watch it
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
    temp_alpha = fmod((rot[3] + alpha),(2*pi));
    
    
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
     wb_supervisor_field_set_sf_vec3f(drone_trans_field,coords);
     wb_supervisor_field_set_sf_rotation(drone_rot_field, rot);
}












void movePlane(double y, double angle)
{

    double r = 20*sqrt(2);
    double angle_x, angle_z;
    double factor =  0.00010;
    angle = angle - (pi/2);;
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




void leftTurn(double altitude, float turn)
{
  if (dofState != 0){
       // targetAngle = rot[3] + turn;
        dofState = 0;
        }
        
        printf("LEFT: My Angle: %f\nTurn: %f\nRot: %f\n",getAngle(),turn,rot[3]);
      if (rot[3] < last_heading + turn)
      {
        turning_state = 0;
     //   rot[3] = rot[3] + turn;
       // rotatePlane(0.0,0.2,0,move_left,'i');
      }
      else{
          turning_state = 1;
      }
      /*if (distance < targetDis){
        movePlane(altitude,getAngle());
        distance++;
        }*/
      if (!(rot[3] < targetAngle) && !(distance < targetDis)){
        dofState = 1;
        simState = 0;
        distance = 0;
        }
}

float getDistance(float cX, float cY, float iX, float iY){
  cX = cX + 19.9352;
  cY = cY + 19.9403;
  iX = iX + 19.9352;
  iY = iY + 19.9403;
  return sqrt(pow(cX - iX, 2) + pow(cY - iY, 2));
}

void forward(double altitude, float x)
{
if (dofState != 0){
        dofState = 0;
        }
      if (getDistance(coords[0],coords[2],start_coords[0],start_coords[1]) < x){
      moving_block = 1;
     
      movePlane(altitude,getAngle());
     // printf("Target Distance: %f\n Distance Traveled: %f\n",x,getDistance(coords[0],coords[2],start_coords[0],start_coords[1]));
      //printf("Distance: %i \n", distance);
      //distance++;
      }
      else if (!(getDistance(coords[0],coords[2],start_coords[0],start_coords[1]) < x)){
        printf("Done!\n");
        get_wp = 0;
        dofState = 1;
        simState = 0;
        distance = 0;
        moving_block = 0;
        x_i = x_i+4;
        //head = head + 4;
        x_coord = wp_info[x_i+1];
        y_coord = wp_info[x_i+2];
        curr_wp = wp_info[3];
        waypoints[0] = x_coord + 19.9352 ;
        waypoints[1] = y_coord + 19.9403 ;
        waypoint_active = 0;
        goToPos();
        printf("Going to waypoint: %d at (%f,%f)\n",(head/4),x_coord,y_coord);
        
      }
      
}

void Turn(double altitude, float turn)
{
  if ( turn < 0){
    turn = -1 * turn;
    leftTurn(altitude,turn);
    }
    else{
    rightTurn(altitude,turn);
    }
}
void rightTurn(double altitude, double turn)
{

  if (dofState != 0){
        targetAngle = getAngle() + turn;
        dofState = 0;
        }
        printf("RIGHT: My Angle: %f\nTurn: %f\nRot: %f\nTarget: %f\n",getAngle(),turn,rot[3],targetAngle);
      
      if (rot[3] > last_heading - turn)
      {
        turning_state = 0; 
        //rot[3] = rot[3] + turn;
       // rotatePlane(0.0,0.2,0,move_right,'i');
      } else{
          turning_state = 1;
      }
      /*if (distance < targetDis){
        movePlane(altitude,getAngle());
        distance++;
        }*/
      if (!(rot[3] > targetAngle) && !(distance < targetDis)){
        dofState = 1;
        simState = 0;
        distance = 0;
        }
}


void dofSelection(int DOF, float x)
{
  
  double targetY; 
  // dofState = 0->locked , 1->open
  
  turnState = DOF;
  switch(DOF){
   /*
    case 0:
      leftTurn(0.01);
      break;
    case 1:
      forward(0.01);
      break;
    case 2:
      rightTurn(0.01,1.5);
      break;*/
    case 3:
      //printf("TargetZ: %f TargetX: %f CurrentZ: %f CurrentX: %f \n", targetZ, targetX, coords[2], coords[0]);
      //leftTurn(0.0);
      break;
    case 4:
      //printf("TargetZ: %f CurrentZ: %f \n", targetZ, coords[2]);
      forward(0.0,x);
      
      break;
    case 5:
      //printf("Angle: %f\n",getAngle());
      Turn(0.0,x);
      break;/*
    case 6:
      leftTurn(-0.01);
      break;
    case 7:
      forward(-0.01);
      break;
    case 8:
      rightTurn(-0.01);
      break;*/
    default:
      ;
  }
}



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
 float input[2];
 float *x_array;
 float *y_array;
 
 int mainUdp() {

    ////////////////////////////////////////////////// SEND SIZE //////////////////////////
    int sent = 0;

   while ( sent < 1 )
    {

       
        //x_array = (char)x_send;
        //y_array = (char)y_send;
        //input[0] = x_send;
        //input[1] = y_send;
       // memset(data_pack1,y_send,2);
        //memset(data_pack1,x_send,1);
        
        //printf("X_send: %f\n",x_send);
        //printf("Y_send: %f\n",y_send);
       
        
        //memcpy(pack_send, data_pack1,12288);
       
      //  memcpy(pack_send, x_array,1);
       // memcpy(pack_send, y_array,2);
       //pack_send[0] = x_send;
        //pack_send[1] = y_send;
        
        //printf("X: %f\n",data_pack1[0]);
        //printf("Y: %f\n",data_pack1[1]);
        
        
       //x_send = (int)coords[0]+20;
       //y_send = (int)coords[2]+20;
       
       toIpad(coords[0],coords[2]);
       //printf("Ipad: %d\n",ipadCoords[0]);
      // data_pack1[0] = ipadCoords[0];
       //data_pack1[1] = ipadCoords[1];
       
      
        switch(sent)
        {

            

            //////////////////////////////////////////////////////////////////// packet 1 ////////////////////////////
            case 0:
                rc = sendto(sd,data_pack1,12290, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));
                /*
                rc = sendto(sd,storage,4, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));
                */


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


int receiveInfo() {

    ////////////////////////////////////////////////// SEND SIZE //////////////////////////
    int sent = 0;
    echoLen_wp = sizeof(echoServAddr_wp);
    int rec_head;
    int temp_head = 0;
   
    
   
    
    if (init_wp == 0){
    
               wp_status[0] = 99;
               wp_status[1] = 98;
               rc_wp = sendto(sd_wp,wp_status,64, flags,
                (struct sockaddr *) &remoteServAddr_wp,
                sizeof(remoteServAddr_wp));
                /*
                rc = sendto(sd,storage,4, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));
                */


                 if(rc_wp<0) {
                printf("%s: cannot send size data %d \n","program",i-1);
                close(sd_wp);
                //exit(1);
                }
                else{
                printf("Packet sent!\n");
                }
                init_wp++;
    
    }

   while ( sent < 1 )
    {

       
        //x_array = (char)x_send;
        //y_array = (char)y_send;
        //input[0] = x_send;
        //input[1] = y_send;
       // memset(data_pack1,y_send,2);
        //memset(data_pack1,x_send,1);
        
        //printf("X_send: %f\n",x_send);
        //printf("Y_send: %f\n",y_send);
       
        
        //memcpy(pack_send, data_pack1,12288);
       
      //  memcpy(pack_send, x_array,1);
       // memcpy(pack_send, y_array,2);
       //pack_send[0] = x_send;
        //pack_send[1] = y_send;
        
        //printf("X: %f\n",data_pack1[0]);
        //printf("Y: %f\n",data_pack1[1]);
        
        
       //x_send = (int)coords[0]+20;
       //y_send = (int)coords[2]+20;
       
      // toIpad(coords[0],coords[2]);
       //printf("Ipad: %d\n",ipadCoords[0]);
      // data_pack1[0] = ipadCoords[0];
       //data_pack1[1] = ipadCoords[1];
       
      
        switch(sent)
        {

            

            //////////////////////////////////////////////////////////////////// packet 1 ////////////////////////////
            case 0:
                //printf("tryin to receive\n");
                if (get_wp == 0 ){
               // printf("Waiting\n");
                rc_wp = recvfrom(sd_wp,wp_info,12290, flags,
                (struct sockaddr *) &echoServAddr_wp,
                &echoLen_wp);
                }
                /*
                rc = sendto(sd,storage,4, flags,
                (struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));
                */


                 if(rc_wp<0) {
               // printf("%s: cannot recv size data %d \n","program",i-1);
                close(sd_wp);
                //exit(1);
                }
                else{
               // printf("Got packet\n");
               
                //printf("Receiving: %f\n",wp_info[temp_head+3]);
               
                if (wp_info[0] == 1.000 && (wp_info[3] != curr_wp)){
                printf("Got new waypoint! (%f,%f)\n",wp_info[x_i+1],wp_info[x_i+2]);
                get_wp = 1;
                 
                x_coord = wp_info[x_i+1];
                y_coord = wp_info[x_i+2];
                 
                curr_wp = wp_info[3];
                //
                //temp_head = temp_head + 4;
                
               
                
                waypoints[0] = x_coord + 19.9352 ;
                waypoints[1] = y_coord + 19.9403 ;
                waypoint_active =0;
                rec_head = rec_head + 2;
            
                got_info = 1;
                //head = head + 2;
               // waypoints[0] = x_coord + 19.9352;
               // waypoints[1] = y_coord + 19.9403;
               
                //head = head + 2;
                printf("Received X: %f\nReceived Y: %f\n",x_coord,y_coord);
              //  int result2 = (int)(wp_info);
               // printf("Result: %d\n",result2);
               got_it++;
               
               
               
               //waypoint_active = 0;
               
               
               
               }
               
               
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


void toIpad(double x, double y){

  //  int disx = x - TOP_RIGHT_X;
   // int disy = y - TOP_RIGHT_Y;
    float newX = ((x + 19.9352)  * UNIT_X);
   // printf("x: %f\n",newX);
    float newY = ((y + 19.9403) * UNIT_Y);
    //printf("X: %d\n",newX);
    //storage[0] = -999;
    //storage[1] = newX;
    //storage[2] = newY;
    
   // printf("newx %d\n", newX);
   //storage[0] = newX;
   //storage[1] = newY;
   //printf("storage: %f\n",storage[0]);


    data_pack1[0] = newX;
    data_pack1[1] = newY;
   
    //printf("data: %d\n",data_pack1[0]);
   

}







/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
///////////// RECIEVE WAYPOINTS////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////




 void clientInit2()
 {
  printf("Connecting WP: \n");
 // Video Socket ////////////////////////////////////////////////////////////////////////
  
   h2 = gethostbyname(SOCKET_SERVER);
  if(h2==NULL) {
    printf("%s: unknown host '%s' \n", "program", "program");
	    exit(1);
	  }
	
	  printf("%s: sending data to '%s' (IP : %s) \n", "program", h->h_name,
	        inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
	
	  remoteServAddr_wp.sin_family = h->h_addrtype;
	  memcpy((char *) &remoteServAddr_wp.sin_addr.s_addr,
	         h->h_addr_list[0], h->h_length);
	  remoteServAddr_wp.sin_port = htons(REMOTE_SERVER_PORT_WP);
	
	  /* socket creation */
	  sd_wp = socket(PF_INET,SOCK_DGRAM,0);
	  if(sd_wp<0) {
    printf("%s: cannot open socket \n","program");
	    exit(1);
	  }
	
	  /* bind any port */
	  cliAddr_wp.sin_family = AF_INET;
	  cliAddr_wp.sin_addr.s_addr = htonl(INADDR_ANY);
	  cliAddr_wp.sin_port = htons(REMOTE_SERVER_PORT_WP);
	
	  rc_wp = bind(sd_wp, (struct sockaddr *) &cliAddr_wp, sizeof(cliAddr_wp));
	  if(rc_wp<0) {
	    printf("%s: cannot bind port\n", "program");
	    exit(1);
	  }
	
	/* BEGIN jcs 3/30/05 */
	
	  flags_wp = 0;
	
	  timeOut = 100; // m
 }






void *wayPoints(void)
{



}


void goToPos()
{
	//the waypoints array consist of the data sent from the iPad and converted at the server to Webots coords
	//waypoints[0]=uavID, [1]=wx, [2]=wy, ...
  
  //waypoints[0] = 1;
  
  //printf("X_wp: %f\n Y_wp: %f\n",waypoints[0],waypoints[1]);
  int m, n;
	float a;
  static float dist;
  int head_go = 0;
  static int start;
  //if (start == 0){
  //start = 1;
  //waypoints[0] = -7+19.9352;
  //waypoints[1] = -10+19.9403;
  //}
  

  
  if ((waypoint_active == 0) && (got_info >0)){
  //printf("X: %f\nY: %f\n",waypoints[0],waypoints[1]);
  dist = sqrt(pow(waypoints[0] - (coords[0]+19.9352), 2) + pow(waypoints[1] - (coords[2]+19.9403), 2));
  waypoint_active = 1;
  start_coords[0] = coords[0]; //x
  start_coords[1] = coords[2];  //y
	float m = waypoints[0] - (coords[0] + 19.9352);
	float n = waypoints[1] - (coords[2] + 19.9403);
  
  
  

	if (n < 0) {
		a = (atan(m/n) + pi);
	} else {
		a = atan(m/n);
	}
  
  
  //printf("
  

	//angle to turn

	turn = a - getAngle();
  last_heading = rot[3];
  rot[3] = a;
 // printf("Turn: %f\nA: %f\nM: %f\nN: %f\nAngle: %f\n",turn,a,m,n,getAngle());
 // printf("Going (%f,%f)\n",waypoints[0],waypoints[1]);
  
  /*
  if ( (m<0) && (n<0)){
    printf("Changing\n");
    turn = (2*pi - turn);
    }
    */
    
    
  
	
 //printf("A: %f\nM: %f\nN: %f\nTurn %f\nTravel Distance: %f\n",a,m,n,turn,dist);
 }
 // dofSelection(RIGHT, turn);
  //printf("My angle: %f\n",getAngle());
  
  //turning_state = 1;
 // printf("Turn State: %f\n",turning_state);
  //add a check whether to turn right or left

	//distance to travel to waypoint
  
	//if (turning_state != 0){
 // printf("Forward Motion\n");
   int i;
  //temp_wp[i++] = 1.3;
  if ((x_coord != 0) && (y_coord != 0)){
	dofSelection(FORWARD, dist);
  }

	//move head to the next waypoint
	//head += 3;

	return;
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
   clientInit2();

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
       
        
        
        float holder[2];
        char cVal[32];
        wb_supervisor_field_set_sf_vec3f(drone_trans_field,coords);
        wb_supervisor_field_set_sf_rotation(drone_rot_field, rot);
        
      
  do {
    
       data_pack1 =  wb_camera_get_image(cam1);
       pack++;
      receiveInfo();
      goToPos();
       mainUdp();
       //printf("Looping\n");
      
       //dofSelection(RIGHT,1.0);
       
       
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
