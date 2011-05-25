
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>

#include <ardrone_api.h>
#include <VP_Os/vp_os_print.h>
#include "parrotControl.h"



/*
int takeOff_status = 0;
double distance = 0;
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
*/


void setup(void)
{
  ardrone_at_set_toy_configuration("general:navdata_demo","FALSE");

/*
 pthread_t thread;
 int rc;
 long t = 1;
 rc = pthread_create(&thread, NULL, getKey, (void *)t);
 if (rc){
     printf("Error \n");


 }
*/
}

int emer(void)
{

 ardrone_tool_set_ui_pad_select(1);
 return 0;

}


double *flight(float x, float y, float z, int time, double dTraveled, int command, int oldT, double fTime)
{

    target_d = meters - offset;
    newTime = ((time - oldT)/factor)*m;

    if (takeOff_status == 0)
    {


    takeOff_status = 1;
    static int32_t start = 0;
    start ^= 1;
    ardrone_tool_set_ui_pad_start(start);

   ardrone_at_set_flat_trim();


    ardrone_at_set_progress_cmd(1,0,-0.05,0,0);
    //ardrone_tool_set_ui_pad_xy(0,1);

     //turn1(2,1);
     }



    rDistance = (x * newTime)* m;
    dTraveled = dTraveled + rDistance;
   // printf("Distance: %f rDistance: %f \n", dTraveled, rDistance);


   if ( fabs(dTraveled) > 2.3)
   // if ( fTime > 20)
    { ardrone_tool_set_ui_pad_start(0);
        }

  status[0] = dTraveled;
  status[1] = turnS;
  status[2] = time;

  return status;

}



void *getKey(void *threadid)
{

    while (1){

    char c;
    int kfd = 0;
       if(read(kfd, &c, 1) < 0)
        {
       perror("read():");
       exit(-1);
       }



    if (c == stop)
    {
      ardrone_tool_set_ui_pad_start(0);

    }
    }

}


void turn1(double angleNum, double rotation)
{
 double scale = 8;
 double degree = (angleNum/scale)*rotation;
 printf("deg: %f \n", degree);
 ardrone_at_set_progress_cmd(1,0,0,0,degree);

}



