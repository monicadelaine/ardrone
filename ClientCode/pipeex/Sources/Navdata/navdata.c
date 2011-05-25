#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

#include <Navdata/navdata.h>
//#include <Navdata/parrotControl.h>
#include <UI/gamepad.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>


#define MAX_LINE 20






//#include <ardrone_api.h>

double d = 0; //distance
double nD = 0; // new distance
double turnState = 0;
double *rec_status;
int oT = 0; //oldtime

//int nTime = 0;

/* Initialization local variables before event loop  */
inline C_RESULT demo_navdata_client_init( void* data )
{
  //emer();
 // setup();
  return C_OK;
}

/* Receving navdata during the event loop */
inline C_RESULT demo_navdata_client_process( const navdata_unpacked_t* const navdata )
{

    static double firstStamp = 0;
	const navdata_demo_t*nd = &navdata->navdata_demo;
	const navdata_time_t*t = &navdata->navdata_time;
/*

	printf("=====================\nNavdata for flight demonstrations =====================\n\n");

	printf("Control state : %i\n",nd->ctrl_state);
	printf("Battery level : %i mV\n",nd->vbat_flying_percentage);
	printf("Orientation   : [Theta] %4.3f  [Phi] %4.3f  [Psi] %4.3f\n",nd->theta,nd->phi,nd->psi);
	printf("Altitude      : %i\n",nd->altitude);
	printf("Speed         : [vX] %4.3f  [vY] %4.3f  [vZPsi] %4.3f\n",nd->vx,nd->vy,nd->vz);


    rec_status  = flight(nd->vx, nd->vy, nd->vz, t->time, d, 0, oT, flightTime);
    printf("Time: %d \n",t->time);


    d = rec_status[0];
    turnState = rec_status[1];
    oT = rec_status[2];

    printf("d: %f \n",d);

*/
    if (firstStamp == 0)
    {
     firstStamp = t->time;
    }


    flightTime = ((t->time - firstStamp)/1000)*0.001;


   //const char* line;




  double buffer[255];
  buffer[0] = nd->theta;
  buffer[1] = nd->phi;
  buffer[2] = nd->psi;
  buffer[3] = nd->vx;
  buffer[4] = nd->vy;
  buffer[5] = nd->vz;
  buffer[6] = nd->vbat_flying_percentage;
  buffer[7] = t->time;
  buffer[8] = d;
  buffer[9] = oT;
  buffer[10] = flightTime;

  double* line = buffer;

  //printf("inside: %c \n", line[1]);
   mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWGRP;

   int pipe;
   //line[0] = 'a';

   // open a named pipe
   pipe = open("/home/navD", O_WRONLY | O_CREAT | O_TRUNC, mode);

   if (pipe == -1){
       printf("Error writing. \n");
   }

   // get a line to send
  // printf("Enter line: ");
   //fgets(line, MAX_LINE, stdin);

   // actually write out the data and close the pipe
   write(pipe, line, 255);

	// close the pipe
   close(pipe);

	printf("\033[8A");

  return C_OK;
}

/* Relinquish the local resources after the event loop exit */
inline C_RESULT demo_navdata_client_release( void )
{
  return C_OK;
}

/* Registering to navdata client */
BEGIN_NAVDATA_HANDLER_TABLE
  NAVDATA_HANDLER_TABLE_ENTRY(demo_navdata_client_init, demo_navdata_client_process, demo_navdata_client_release, NULL)
END_NAVDATA_HANDLER_TABLE

