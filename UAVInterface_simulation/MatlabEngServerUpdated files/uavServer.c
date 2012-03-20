#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <string.h> 
#include <pthread.h>

#include "matrix.h"
#include "engine.h"

#define LOCAL_SERVER_PORT0 1500
#define LOCAL_SERVER_PORT1 1501
#define LOCAL_SERVER_PORT2 1502
#define LOCAL_SERVER_PORT3 1503
#define LOCAL_SERVER_PORT4 1504
#define LOCAL_SERVER_PORT5 1505
#define LOCAL_SERVER_PORT6 1506
#define LOCAL_SERVER_PORT7 1507
#define LOCAL_SERVER_PORT8 1508
#define LOCAL_SERVER_PORT9 1509
#define LOCAL_SERVER_PORT10 1510 

#define dataSize 12290
#define dataSize_command 255

long long int send_interval1 = 201, send_interval2 = 225;
long long int send_time1 = 0, send_time2 = 0;

int gotCoords=0;
float IPAD_UNIT_X = 45.080468637;
float IPAD_UNIT_Y = 45.201098449;
float MAT_UNIT_X = 6.798030861;
float MAT_UNIT_Y = 8.35910725;
float X_OFFSET = 19.9352;
float Y_OFFSET = 19.9403;
float IPAD_2_MAT_X = 6.789;
float IPAD_2_MAT_Y = 5.5185;
int matCoords[4];

int x_head = 0;
int y_head = 0;

char data1[12294];
int posArrary[6];
int posArrary2[6];
int posArrary3[6];
char data1_command[255];
float wp_data[255];
float x_wp_data[12290];
float y_wp_data[12290];

int data1_send[100];
char data2[12294];
double data2_command[255];
int data2_send[255];

int thread_lock = 0;

int i = 0;
int wPoints[64];
int can_send1;
int can_send2;

int sd, rc, n, cliLen, flags;
struct sockaddr_in cliAddr, servAddr;
struct sockaddr_in cliAddr_command, servAddr_command;
struct sockaddr_in cliAddr_wp, servAddr_wp;
struct sockaddr_in cliAddr_webots, servAddr_webots;

int sd_command, rc_command, n_command, cliLen_command, flags_command;
int sd_webots, rc_webots, n_webots, cliLen_webots, flags_webots;
int rc_wp, n_wp, cliLen_wp, flags_wp;
int sd2, sd4;

/* flag to determine if we are using matlab or ipad coordinates*/
int IPAD_FLAG = 1; //1=waypoint, 2=QoS, 3=waypoint+QoS
int WP_FLAG1 = 0, WP_FLAG2 = 0;

/*variables for Matlab engine*/
Engine *ep;
mxArray *result = NULL, *sresult = NULL, *cpresult = NULL;
double alulFlag[0], start[0], targetFound[0], cell_pos[0];
mxArray *m_A, *m_S, *m_TF, *m_CP;
double *cresult;
char *cpresult2;
int tFound = 0;
int startEng = 0;
int x1, y11, x2, y2;
int lastx1, lasty1, lastx2, lasty2;
int count=0;


long long get_current_time() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return (long long) (t.tv_sec ) * 1000000 + (t.tv_usec);
} 


float toWebots(float unit, float val, float offset){
	float newVal;
	newVal = (val / unit) - offset;
	return newVal;
}


void toMatlab(int x, int y) {
	float MATX = 329723, MATY = 4321586.5;
	int MX = 95, MY = 130, MAXY = 270; 
	int newX = (int)( x / IPAD_2_MAT_X );
	int newY = (int)( y / IPAD_2_MAT_Y );

	matCoords[0] = newX+MX+MATX;
	matCoords[1] = newY+MY+MATY;

	//printf("toMatlab=> newX: %i, newY: %i matX: %i, matY: %i\n\n",newX,newY,matCoords[0],matCoords[1]);
}


void *waitThread(int argc, char *argv[]) {
	while (1) {
		;
	}
}

// thread to receive video data for uav1 from webots
void *videoThread() {
	
	/* socket creation */
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(LOCAL_SERVER_PORT0);
	rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
	if(rc<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT0);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT0); 

	flags = 0;

	while(1){
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen = sizeof(cliAddr);
				n = recvfrom(sd, data1, dataSize, flags, (struct sockaddr *) &cliAddr, &cliLen);
				
				//long long int curr_time = get_current_time()/1000;
				//printf("received packet from videoThread %lld \n", curr_time );

				if(n<0) {
					printf("%s [1]: cannot receive data \n","prg");
				}
				else{
					recData++;
				}
				break;
			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}

// thread to send position and image data to interface
void *dataThread(){

	sd_command=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_command<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr_command.sin_family = AF_INET;
	servAddr_command.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_command.sin_port = htons(LOCAL_SERVER_PORT1);
	rc_command = bind (sd_command, (struct sockaddr *) &servAddr_command,sizeof(servAddr_command));
	if(rc_command<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT1);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT1);

	cliLen_command = sizeof(cliAddr_command);
	n_command = recvfrom(sd_command, data1_command, dataSize_command, flags_command,
		(struct sockaddr *) &cliAddr_command, &cliLen_command);

	//long long int curr_time = get_current_time()/1000;
	//printf("received packet from dataThread %lld \n", curr_time );

	if(n<0) {
		printf("%s [2]: cannot receive data \n","prg");
	}
	else {
		//printf("Got connection!\n");
	}

	int recData = 0;

	while(1){
		recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				recData++;

				char datatest[12294];
				int rcP;
				memset(data1_send,126,100);
				data1[0] = 'A';
				data2[0] = 'B';

				posArrary[2] = posArrary2[0];
                                posArrary[3] = posArrary2[1];
				posArrary[4] = posArrary3[0];
                                posArrary[5] = posArrary3[1];
				
				long long int curr_time = get_current_time()/1000;
				if (curr_time > send_time1 + send_interval1) {
					send_time1 = get_current_time()/1000;
            				rcP = sendto(sd_command,data1,sizeof(datatest),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
				}
				curr_time = get_current_time()/1000;
				if (curr_time > send_time2 + send_interval2) {
					send_time2 = get_current_time()/1000;
            				rcP = sendto(sd_command,data2,sizeof(datatest),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);

            				rcP = sendto(sd_command,posArrary,sizeof(posArrary),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
				}

				//printf("sending packet to interface %lld \n", curr_time );

				int i=0;

				if(rcP<0) {
					printf("%s [1]: cannot send size data %d \n","program",i-1);
					close(sd);
				}else{
				}
				break;
			}
		}
	}
}

flags_command = 0;

// thread to receive video data for uav2
void *videoThread1(){

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr2.sin_family = AF_INET;
	servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT2);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));
	if(rc2<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT2);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT2);

	flags2 = 0;

	while(1){

		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, data2, dataSize, flags2, (struct sockaddr *) &cliAddr2, &cliLen2);

				//long long int curr_time = get_current_time()/1000;
				//printf("received packet from videoThread1 %lld \n", curr_time );

				if(n2<0) {
					printf("%s [3]: cannot receive data \n","prg");
				}
				else{
					recData++;
				}
				break;
			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}

// thread to receive waypoint data from interface
void *wayPoints() {

	char *p1x, *p2x, *p3x, *p4x, *wp;
	char *p1y, *p2y, *p3y, *p4y, *pID;
	char *algID;
	char *pIDx;

	int sd_wp, rc_wp, n_wp, cliLen_wp, flags_wp;
	struct sockaddr_in cliAddr_wp, servAddr_wp;

	/* socket creation */
	sd_wp=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_wp<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr_wp.sin_family = AF_INET;
	servAddr_wp.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_wp.sin_port = htons(LOCAL_SERVER_PORT6);
	rc_wp = bind (sd_wp, (struct sockaddr *) &servAddr_wp,sizeof(servAddr_wp));
	if(rc_wp<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT6);/*change to 1507*/
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT6);

	flags_wp = 0;
	float pack_size;

	while(1){
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen_wp = sizeof(cliAddr_wp);
				n_wp = recvfrom(sd_wp, wPoints, sizeof(wPoints), flags_wp,
					(struct sockaddr *) &cliAddr_wp, &cliLen_wp);

				if(n_wp<0) {
					printf("%s [5]: cannot receive data \n","prg");
				}	
				else{
					recData++;

					pIDx = strtok ( wPoints, "," ); //dummy
					pID = strtok ( NULL, "," );
					p1x = strtok ( NULL, "," );
					p1y = strtok ( NULL, "," );
					p2x = strtok ( NULL, "," );
					p2y = strtok ( NULL, "," );
					p3x = strtok ( NULL, "," );
					p3y = strtok ( NULL, "," );
					p4x = strtok ( NULL, "," );
					p4y = strtok ( NULL, "," );
					algID = strtok ( NULL, "," );
					wp = strtok ( NULL, "," );

					//printf("p1x:%s\np1y:%s\np2x:%s\np2y:%s\np3x:%s\np3y:%s\np4x:%s\np4y:%s\narg_ID:%s\nWP_ID:%s",p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y,algID,wp);

					int pID2;
					float int_p1x, int_p1y;
					float int_p2x, int_p2y;
					float int_p3x, int_p3y;
					float int_p4x, int_p4y;
					float int_wp_id, int_alg_id;
					float clone_int_p1x;
					float clone_int_p1y;

					///////////////////////////////////////

					if(sscanf(pID, "%i", &pID2) == EOF )  {
						printf("error converting string\n");
					}
					if(sscanf(p1x, "%f", &int_p1x) == EOF ) {
						printf("error converting string\n");
					}
					if(sscanf(p1y, "%f", &int_p1y) == EOF )  {
						printf("error converting string\n");
					}

					if (gotCoords > 0){
						if(sscanf(p2x, "%f", &int_wp_id) == EOF )  {
							printf("error converting string\n");
						}
						printf("wp: %i,%f,%f,%f ", pID2, int_p1x, int_p1y, int_wp_id); fflush(stdout);
						printf("%lld\n", get_current_time()/1000); fflush(stdout);
					} else{
						if(sscanf(wp, "%f", &int_wp_id) == EOF )   {
							printf("error converting string\n");
						}
						if(sscanf(algID, "%f", &int_alg_id) == EOF )  {
							printf("error converting string\n");
						}
						printf("algID: %f ", int_alg_id);
						printf("%lld\n", get_current_time()/1000);
					}

					if (gotCoords < 1){
						if(sscanf(p2x, "%f", &int_p2x) == EOF )   {
							printf("error converting string\n");
						}
						if(sscanf(p2y, "%f", &int_p2y) == EOF )  {
							printf("error converting string\n");
						}
						if(sscanf(p3x, "%f", &int_p3x) == EOF )  {
							printf("error converting string\n");
						}
						if(sscanf(p3y, "%f", &int_p3y) == EOF )   {
							printf("error converting string\n");
						}
						if(sscanf(p4x, "%f", &int_p4x) == EOF )  {
							printf("error converting string\n");
						}
						if(sscanf(p4y, "%f", &int_p4y) == EOF )  {
							printf("error converting string\n");
						}
						/*if(sscanf(algID, "%f", &int_alg_id) == EOF )  {
							printf("error converting string\n");
						}*/

						gotCoords++;
						pack_size = 8;
						
						if (int_alg_id == 1) { 
							IPAD_FLAG = 1;
							printf("ipad flag %i\n", IPAD_FLAG); fflush(stdout);
						} else if (int_alg_id == 2) { 
							IPAD_FLAG = 2;
							printf("ipad flag %i\n", IPAD_FLAG); fflush(stdout);
						} else if (int_alg_id == 3){
							IPAD_FLAG = 3;
							printf("ipad flag %i\n", IPAD_FLAG); fflush(stdout);
						}

						int_p1x = toWebots(IPAD_UNIT_X,int_p1x,X_OFFSET);
						int_p1y = toWebots(IPAD_UNIT_Y,int_p1y,Y_OFFSET);
						int_p2x = toWebots(IPAD_UNIT_X,int_p2x,X_OFFSET);
						int_p2y = toWebots(IPAD_UNIT_Y,int_p2y,Y_OFFSET);
						int_p3x = toWebots(IPAD_UNIT_X,int_p3x,X_OFFSET);
						int_p3y = toWebots(IPAD_UNIT_Y,int_p3y,Y_OFFSET);
						int_p4x = toWebots(IPAD_UNIT_X,int_p4x,X_OFFSET);
						int_p4y = toWebots(IPAD_UNIT_Y,int_p4y,Y_OFFSET);
						clone_int_p1x = int_p1x;
						clone_int_p1y = int_p1y;

						if (WP_FLAG1 == 1) {
							lastx1 = int_p1x, lasty1=int_p1y;
							printf("wp1=> lastx1: %i, lasty1: %i ",lastx1,lasty1);
						}
						if (WP_FLAG2 == 1) {
							lastx2 = int_p1x, lasty2=int_p1y;
							printf("wp2=> lastx2: %i, lasty2: %i ",lastx2,lasty2);
						}
					}
					else{
						pack_size = 2;
					}

					int_p1x = toWebots(IPAD_UNIT_X,int_p1x,X_OFFSET);
					int_p1y = toWebots(IPAD_UNIT_Y,int_p1y,Y_OFFSET);

					wp_data[0] = pID2;
					wp_data[1] = clone_int_p1x;
					wp_data[2] = clone_int_p1y;
					wp_data[3] = int_p2x;
					wp_data[4] = int_p2y;
					wp_data[5] = int_p3x;
					wp_data[6] = int_p3y;
					wp_data[7] = int_p4x;
					wp_data[8] = int_p4y;
					wp_data[9] = int_p1x;
					wp_data[10] = int_p1y;
					wp_data[11] = int_wp_id;
					
					//printf("PID: %i\n\n",pID2);////////////////////////////
					
					if (pID2 == 1){
						x_wp_data[x_head] = pID2;
						x_wp_data[x_head+1] = int_p1x;
						x_wp_data[x_head+2] = int_p1y;
						x_wp_data[3] = int_wp_id;
						x_head = x_head + 4;
						can_send1 = 1;
					}
					
					if ((pID2 == 2)){
						y_wp_data[y_head] = pID2;
						y_wp_data[y_head+1] = int_p1x;
						y_wp_data[y_head+2] = int_p1y;
						y_wp_data[3] = int_wp_id; /*changes  this index constantly*/
						y_head = y_head + 4;
						can_send2 = 1;
					}
				}
				break;
			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}

// thread to receive waypoints from matlab
void *wayPointsMatlab() {

	//capture MATLAB output
	char buffer[256+1];
	buffer[256] = '\0';
	engOutputBuffer(ep, buffer, 256);

	char *p1x, *p1y, *p2x, *p2y, *wp, *algID;

	while (1) {
		if (IPAD_FLAG != 1) {
			if (startEng == 0) {
				/* Open the MATLAB engine */
				if (!(ep = engOpen("\0"))) {
					printf("Can't start MATLAB engine\n");
					return EXIT_FAILURE;
				}

				engEvalString(ep, "clear *");
				engEvalString(ep, "close all");
		
				m_A=mxCreateDoubleMatrix(1, 1, mxREAL);
				memcpy((void *)mxGetPr(m_A), (void *)alulFlag, sizeof(double)*1);
				engPutVariable(ep, "alulFlag", m_A);
		
				m_S=mxCreateDoubleMatrix(1, 1, mxREAL);
				memcpy((void *)mxGetPr(m_S), (void *)start, sizeof(double)*1);
				engPutVariable(ep, "start", m_S);
			
				m_TF=mxCreateDoubleMatrix(1, 1, mxREAL);
				memcpy((void *)mxGetPr(m_TF), (void *)targetFound, sizeof(double)*1);
				engPutVariable(ep, "targetFound", m_TF);
		
				m_CP=mxCreateDoubleMatrix(1, 1, mxREAL);
				memcpy((void *)mxGetPr(m_CP), (void *)cell_pos, sizeof(double)*1);
				engPutVariable(ep, "cell_pos", m_CP);

				engEvalString(ep, "start=0");
				engEvalString(ep, "targetFound=0");
				engEvalString(ep, "alulFlag = 0;"); 
				
				engEvalString(ep, "load"); 
		
				startEng = 1;
			}

			/**********************************************************************************/

			while (1) {

				engEvalString(ep, "if (start==1) cameras=[our_cam_pos(1,1) dov; our_cam_pos(2,1) dov]; end");
				engEvalString(ep, "demoRunAlulRobot");

				cpresult = engGetVariable(ep,"cell_pos");
				cpresult2 = mxArrayToString(cpresult);
				printf("wp: %s ", cpresult2); fflush(stdout);
				printf("%lld\n", get_current_time()/1000); fflush(stdout);

				//printf("%s", buffer+2);
			/****************************************************************************/

			p1x = strtok ( cpresult2, "," );
			p1y = strtok ( NULL, "," );
			p2x = strtok ( NULL, "," );
			p2y = strtok ( NULL, "," );
			wp = strtok ( NULL, "," );

			float int_p1x, int_p1y, int_p2x, int_p2y, int_wp_id;

			if(sscanf(p1x, "%f", &int_p1x) == EOF ) {
				printf("error converting string\n");
			}
			if(sscanf(p1y, "%f", &int_p1y) == EOF )  {
				printf("error converting string\n");
			}
			if(sscanf(p2x, "%f", &int_p2x) == EOF ) {
				printf("error converting string\n");
			}
			if(sscanf(p2y, "%f", &int_p2y) == EOF )  {
				printf("error converting string\n");
			}
			if(sscanf(wp, "%f", &int_wp_id) == EOF )   {
				printf("error converting string\n");
			}

			int_p1x = toWebots(MAT_UNIT_X,int_p1x,X_OFFSET);
			int_p1y = toWebots(MAT_UNIT_Y,int_p1y,Y_OFFSET);

			int_p2x = toWebots(MAT_UNIT_X,int_p2x,X_OFFSET);
			int_p2y = toWebots(MAT_UNIT_Y,int_p2y,Y_OFFSET);

			//if (count%3==0) {
				x_wp_data[x_head] = 1;
				x_wp_data[x_head+1] = int_p1x;
				x_wp_data[x_head+2] = int_p1y;
				x_wp_data[3] = int_wp_id;
				x_head = x_head + 4;
				can_send1 = 1;
			//}
			//if (count%3==1) {
				y_wp_data[y_head] = 2;
				y_wp_data[y_head+1] = int_p2x;
				y_wp_data[y_head+2] = int_p2y;
				y_wp_data[3] = int_wp_id+1; 
				y_head = y_head + 4;
				can_send2 = 1;
				count++;

			//}

			} 

			/* Free memory, close MATLAB engine and exit. */
			mxDestroyArray(result);
			engClose(ep);

			return EXIT_SUCCESS;
		}
	}
}

// thread to send uav2 waypoints to webots
void *sendWaypoints2(){

	struct sockaddr_in cliAddr_wp2, servAddr_wp2;
	struct sockaddr_in cliAddr_webots2, servAddr_webots2;

	int sd_webots2, rc_webots2, n_webots2, cliLen_webots2, flags_webots2;
	int rc_wp2, n_wp2, cliLen_wp2, flags_wp2;
	/*uses local variables*/

	sd_webots2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_webots2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr_webots2.sin_family = AF_INET;
	servAddr_webots2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_webots2.sin_port = htons(LOCAL_SERVER_PORT7);
	rc_webots2 = bind (sd_webots2, (struct sockaddr *) &servAddr_webots2,sizeof(servAddr_webots2));
	if(rc_webots2<0) {
		printf("%s: cannot bind port number %d \n", "prg", LOCAL_SERVER_PORT7);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT7);

	cliLen_webots2 = sizeof(cliAddr_webots2);
	n_webots2 = recvfrom(sd_webots2, wp_data,sizeof(wp_data), flags_webots2,
		(struct sockaddr *) &cliAddr_webots2, &cliLen_webots2);
	if(n_webots2<0) {
		printf("%s [6]: cannot receive data \n","prg");
		/*continue;*/
	}
	else{
		printf("Got connection!\n"); fflush(stdout);
		printf("Waypoint Ready? %d\n",wp_data[0]);
	}

	int recData = 0;

	while(1){
		recData = 0;

		while (recData < 1)	{

			switch(recData)	{
			case 0:
				recData++;

				char datatest2[12294];
				int rcP2_2;

				if(thread_lock == 0){
				
				if (gotCoords > 0){
					if (wp_data[0] == 0){
						rcP2_2 = sendto(sd_webots2,wp_data,sizeof(wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
					}

					if (can_send2 == 1){
						rcP2_2 = sendto(sd_webots2,y_wp_data,sizeof(y_wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
						//printf("y_wp_data: %f %f %f %f\n", y_wp_data[0], y_wp_data[1], y_wp_data[2], y_wp_data[3]);
						//can_send2=0;
					}
				}
				thread_lock = 1;

				int i=0;

				if(rcP2_2<0) {
					printf("%s [3]: cannot send size data %d \n","program",i-1);
					close(sd);
				}
				}
				break;
			}
		}
	}
}

// thread to send uav2 waypoints to webots
void *sendWaypoints(){

	sd_webots=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_webots<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr_webots.sin_family = AF_INET;
	servAddr_webots.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_webots.sin_port = htons(LOCAL_SERVER_PORT5);
	rc_webots = bind (sd_webots, (struct sockaddr *) &servAddr_webots,sizeof(servAddr_webots));
	if(rc_webots<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT5);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT5);

	cliLen_webots = sizeof(cliAddr_webots);
	n_webots = recvfrom(sd_webots, wp_data,sizeof(wp_data), flags_webots,
		(struct sockaddr *) &cliAddr_webots, &cliLen_webots);
	if(n_webots<0) {
		printf("%s [7]: cannot receive data \n","prg");
	}
	else{
		//printf("Got connection!\n");
		printf("Waypoint Ready? %d\n",wp_data[0]);
	}

	int recData = 0;

	while(1){
		recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:

				recData++;

				char datatest2[12294];
				int rcP2;

				if(thread_lock == 1){

				if (gotCoords > 0){
					if (wp_data[0] == 0){
						rcP2 = sendto(sd_webots,wp_data,sizeof(wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);
					}

					if (can_send1 == 1) {
						rcP2 = sendto(sd_webots,x_wp_data,sizeof(x_wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);	
						//printf("x_wp_data: %f %f %f %f\n", x_wp_data[0], x_wp_data[1], x_wp_data[2], x_wp_data[3]);
						//can_send1=0;		
					}
				}
				thread_lock = 0;

				int i=0;

				if(rcP2<0) {
					printf("%s [4]: cannot send size data %d \n","program",i-1);
					close(sd);
				}
				}
				break;
			}
		}
	}
}

// thread to receive uav1 position data from webots
void *posThread() {

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr2.sin_family = AF_INET;
	servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT8);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));
	if(rc2<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT8);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT8);

	flags2 = 0;

	while(1){
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {

			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, posArrary, 6, flags2, (struct sockaddr *) &cliAddr2, &cliLen2);
				x1 = posArrary[0];
				y11 = posArrary[1];

				//long long int curr_time = get_current_time()/1000;
				//printf("received packet from posThread %lld \n", curr_time );

				if(n2<0) {
					printf("%s [8]: cannot receive data \n","prg");
				}
				else{
					recData++;
				}
				break;

			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}

// thread to receive uav2 position data from webots
void *posThread2() {

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr2.sin_family = AF_INET;
	servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT9);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));
	if(rc2<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT9);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",
		"prg",LOCAL_SERVER_PORT9);

	flags2 = 0;
	
	while(1){
		
		int recData = 0;

		while (recData < 1) {

			switch(recData) {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, posArrary2, sizeof(posArrary2), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				x2 = posArrary[0];
				y2 = posArrary[1];

				//long long int curr_time = get_current_time()/1000;
				//printf("received packet from posThread2 %lld \n", curr_time );

				if(n2<0) {
					printf("%s [9]: cannot receive data \n","prg");
				}
				else{
					recData++;
				}
				break;
			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}

// thread to receive target uav position data from webots
void *posThread3() {

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr2.sin_family = AF_INET;
	servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT10);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));
	if(rc2<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT10);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",
		"prg",LOCAL_SERVER_PORT10);

	flags2 = 0;
	
	while(1){
		
		int recData = 0;

		while (recData < 1) {

			switch(recData) {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, posArrary3, sizeof(posArrary3), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				//printf("target: %i, %i\n",posArrary3[0],posArrary3[1]);

				//long long int curr_time = get_current_time()/1000;
				//printf("received packet from posThread3 %lld \n", curr_time );

				if(n2<0) {
					printf("%s [10]: cannot receive data \n","prg");
				}
				else{
					recData++;
				}
				break;
			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	pthread_t video_pth; 
	pthread_t data_pth;
	pthread_t video_pth1; 
	pthread_t data_pth1; 
	pthread_t waypoint_pth; 
	pthread_t waypoint_matlab_pth;
	pthread_t send_waypoint_pth; 
	pthread_t send_waypoint2_pth; 
	pthread_t pos_pth; 
	pthread_t pos_pth2;
	pthread_t pos_pth3;
	pthread_t wait_pth; 

	pthread_create(&pos_pth,NULL,posThread,"Drone 1 pos info.. \n");
	pthread_create(&pos_pth2,NULL,posThread2,"Drone 2 pos info.. \n");
	pthread_create(&pos_pth3,NULL,posThread3,"Target Drone pos info.. \n");
	pthread_create(&data_pth,NULL,dataThread,"Data stuff.. \n");
	pthread_create(&send_waypoint2_pth,NULL,sendWaypoints2,"Send wp.. \n");
	pthread_create(&send_waypoint_pth,NULL,sendWaypoints,"Send wp.. \n");
	pthread_create(&video_pth,NULL,videoThread,"Getting video data.\n");
	pthread_create(&video_pth1,NULL,videoThread1,"Getting video data (Drone 2).\n");
	pthread_create(&waypoint_pth,NULL,wayPoints,"waypoint info.. \n");
	pthread_create(&waypoint_pth,NULL,wayPointsMatlab,"waypoint matlab info.. \n");
	pthread_create(&wait_pth,NULL,waitThread(argc,argv),"waiting.. \n");
}
