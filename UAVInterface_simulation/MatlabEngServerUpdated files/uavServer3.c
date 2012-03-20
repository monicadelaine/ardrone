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
#define LOCAL_SERVER_PORT11 1511 
#define LOCAL_SERVER_PORT12 1512
#define LOCAL_SERVER_PORT13 1513 

#define dataSize 12294
#define dataSize_command 255


long long int send_interval1 = 201, send_interval2 = 210, send_interval3 = 217;
long long int send_time1 = 0, send_time2 = 0, send_time3 = 0;

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_lock = 0;

int gotCoords=0;
float IPAD_UNIT_X = 45.080468637;
float IPAD_UNIT_Y = 45.201098449;
float MAT_UNIT_X = 4.47238872;
float MAT_UNIT_Y = 5.41793988;
float X_OFFSET = 19.9352;
float Y_OFFSET = 19.9403;
float IPAD_2_MAT_X = 6.789; //?
float IPAD_2_MAT_Y = 5.5185; //?
int MX = 130, MY = 190, MAXY = 175; 
int matCoords[4];

char data1[12294];
char data2[12294];
char data3[12294];
char data1_command[255];
int posArrary[6];
int posArrary2[6];
int posArrary3[6];
int tposArrary[10];

int x_head = 0;
int y_head = 0;
int z_head = 0;
float wp_data[255];
float x_wp_data[12294];
float y_wp_data[12294];
float z_wp_data[12294];

int i = 0;
int wPoints[64];
int can_send1, can_send2, can_send3;

int sd, rc, n, cliLen, flags;
int sd_command, rc_command, n_command, cliLen_command, flags_command;
int sd_webots, rc_webots, n_webots, cliLen_webots, flags_webots;
struct sockaddr_in cliAddr, servAddr;
struct sockaddr_in cliAddr_command, servAddr_command;
struct sockaddr_in cliAddr_webots, servAddr_webots;

/* flag to determine if we are using matlab or ipad coordinates*/
int IPAD_FLAG = 1; //1=waypoint, 2=QoS, 3=waypoint+QoS
int WP_FLAG1 = 0, WP_FLAG2 = 0, WP_FLAG3 = 0;

/*variables for Matlab engine*/
Engine *ep;
mxArray *result = NULL, *sresult = NULL, *cpresult = NULL;
double alulFlag[0], start[0], targetFound[0], cell_pos[0];
mxArray *m_A, *m_S, *m_TF, *m_CP;
double *cresult;
char *cpresult2;
int tFound = 0;
int startEng = 0;
int x1, y11, x2, y2, x3, y3;
int lastx1, lasty1, lastx2, lasty2, lastx3, lasty3;
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
	
	int newX = (int)( x / IPAD_2_MAT_X );
	int newY = (int)( y / IPAD_2_MAT_Y );

	matCoords[0] = newX+MX+MATX;
	matCoords[1] = newY+MY+MATY;

	//printf("toMatlab=> newX: %i, newY: %i matX: %i, matY: %i\n\n",newX,newY,matCoords[0],matCoords[1]);
}


void *waitThread(int argc, char *argv[]) {
	while (1) { ; }
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
	if(n<0) {
		printf("%s [2]: cannot receive data \n","prg");
	}
	else {
		//printf("Got connection!\n");
	}

	int recData = 0;
	flags_command = 0;

	while(1){
		recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				recData++;

				int rcP;
				data1[0] = 'A';
				data2[0] = 'B';
				data3[0] = 'C';

				tposArrary[2] = posArrary[0];
                                tposArrary[3] = posArrary[1];
				tposArrary[4] = posArrary2[0];
                                tposArrary[5] = posArrary2[1];	
				tposArrary[6] = posArrary3[0];
                                tposArrary[7] = posArrary3[1];	
				//printf("p2: %d, %d p3: %d, %d\n",tposArrary[4],tposArrary[5],tposArrary[6],tposArrary[7]);
				
				// send data to interface every ??? ms
				long long int curr_time = get_current_time()/1000;
				if (curr_time > send_time1 + send_interval1) {
					send_time1 = get_current_time()/1000;
					rcP = sendto(sd_command,data1,sizeof(data1),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
				}
				if (curr_time > send_time2 + send_interval2) {
					send_time2 = get_current_time()/1000;
            				rcP = sendto(sd_command,data2,sizeof(data1),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
            				rcP = sendto(sd_command,tposArrary,sizeof(tposArrary),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
				}

				if (curr_time > send_time3 + send_interval3) {
					send_time3 = get_current_time()/1000;
            				rcP = sendto(sd_command,data3,sizeof(data1),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
				}

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
//flags_command = 0;

// thread to receive video data for uav1 from webots
void *videoThread1() {
	
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

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT0); //fflush(stdout);

	flags = 0;

	while(1){
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen = sizeof(cliAddr);
				n = recvfrom(sd, data1, dataSize, flags, (struct sockaddr *) &cliAddr, &cliLen);

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

// thread to receive video data for uav2
void *videoThread2(){

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
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT2);
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

// thread to receive video data for uav3
void *videoThread3(){

	int sd3, rc3, n3, cliLen3, flags3;
	struct sockaddr_in cliAddr3, servAddr3;

	sd3=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd3<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr3.sin_family = AF_INET;
	servAddr3.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr3.sin_port = htons(LOCAL_SERVER_PORT11);
	rc3 = bind (sd3, (struct sockaddr *) &servAddr3,sizeof(servAddr3));
	if(rc3<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT11);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT11);

	flags3 = 0;

	while(1){

		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen3 = sizeof(cliAddr3);
				n3 = recvfrom(sd3, data3, dataSize, flags3, (struct sockaddr *) &cliAddr3, &cliLen3);

				if(n3<0) {
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
void *wpInterface() {

	char *p1x, *p2x, *p3x, *p4x, *wp;
	char *p1y, *p2y, *p3y, *p4y, *pID;
	char *algID, *numUAVs;
	char *pIDx;

	int sd_wp, rc_wp, n_wp, cliLen_wp, flags_wp;
	struct sockaddr_in cliAddr_wp, servAddr_wp;

	sd_wp=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_wp<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr_wp.sin_family = AF_INET;
	servAddr_wp.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_wp.sin_port = htons(LOCAL_SERVER_PORT6);
	rc_wp = bind (sd_wp, (struct sockaddr *) &servAddr_wp,sizeof(servAddr_wp));
	if(rc_wp<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT6);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT6);

	flags_wp = 0;

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

					pIDx = strtok ( wPoints, "," ); //dummy value
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
					numUAVs = strtok ( NULL, "," );
					wp = strtok ( NULL, "," );

					//printf("p1x:%s\np1y:%s\np2x:%s\np2y:%s\np3x:%s\np3y:%s\np4x:%s\np4y:%s\nargID:%s\nnumUAV:%s\nwpID:%s",p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y,algID,numUAVs,wp);

					int pID2;
					float int_p1x, int_p1y;
					float int_p2x, int_p2y;
					float int_p3x, int_p3y;
					float int_p4x, int_p4y;
					float int_wp_id, int_alg_id, int_num_uav;
					float clone_int_p1x;
					float clone_int_p1y;

					///////////////////////////////////////

					// init -> 0,0,p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y,algID,numUAV,254
					// wp   -> 1,uavID,x,y,cnt

					if(sscanf(pID, "%i", &pID2) == EOF )  { 
						printf("error converting string\n");
					}
					if(sscanf(p1x, "%f", &int_p1x) == EOF ) { 
						printf("error converting string\n");
					}
					if(sscanf(p1y, "%f", &int_p1y) == EOF )  {
						printf("error converting string\n");
					}

					if (gotCoords > 0){ //wp data
						if(sscanf(p2x, "%f", &int_wp_id) == EOF )  {
							printf("error converting string\n");
						}
						printf("wp: %i,%f,%f,%f ", pID2, int_p1x, int_p1y, int_wp_id); fflush(stdout);
						printf("%lld\n", get_current_time()/1000); fflush(stdout);
					} else{ //init coord
						if(sscanf(numUAVs, "%f", &int_num_uav) == EOF )   {
							printf("error converting string\n");
						}
						if(sscanf(algID, "%f", &int_alg_id) == EOF )  {
							printf("error converting string\n");
						}
						printf("algID: %f numUAVs: %f ", int_alg_id, int_num_uav);
						printf("%lld\n", get_current_time()/1000);
					}

					if (gotCoords < 1){ //init coord
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

						gotCoords++;
						
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
						if (WP_FLAG3 == 1) {
							lastx3 = int_p1x, lasty3=int_p1y;
							printf("wp3=> lastx3: %i, lasty3: %i ",lastx3,lasty3);
						}
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
					} else if (pID2 == 2) {
						y_wp_data[y_head] = pID2;
						y_wp_data[y_head+1] = int_p1x;
						y_wp_data[y_head+2] = int_p1y;
						y_wp_data[3] = int_wp_id; /*changes this index constantly*/
						y_head = y_head + 4;
						can_send2 = 1;
					} else if (pID2 == 3) {
						z_wp_data[z_head] = pID2;
						z_wp_data[z_head+1] = int_p1x;
						z_wp_data[z_head+2] = int_p1y;
						z_wp_data[3] = int_wp_id; 
						z_head = z_head + 4;
						can_send3 = 1;
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
void *wpMatlab() {

	//capture MATLAB output
	char buffer[256+1];
	buffer[256] = '\0';
	engOutputBuffer(ep, buffer, 256);

	char *p1x, *p1y, *p2x, *p2y, *p3x, *p3y, *wp, *algID;

	while (1) {
		
		if (startEng == 0) {
			/* Open the MATLAB engine */
			if (!(ep = engOpen("\0"))) {
				printf("Can't start MATLAB engine\n");
				//return EXIT_FAILURE;
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
			engEvalString(ep, "alulFlag = 1;"); 

			engEvalString(ep, "loadMat3"); 

			startEng = 1;
		}
		if (IPAD_FLAG != 1) {
			/**********************************************************************************/
			while (1) {

				engEvalString(ep, "if (start==1) cameras=[our_cam_pos(1,1) dov; our_cam_pos(2,1) dov; our_cam_pos(3,1) dov]; end");
				engEvalString(ep, "demoRunAlulRobot3");

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
				p3x = strtok ( NULL, "," );
				p3y = strtok ( NULL, "," );
				wp = strtok ( NULL, "," );

				float int_p1x, int_p1y, int_p2x, int_p2y, int_p3x, int_p3y, int_wp_id;

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
				if(sscanf(p3x, "%f", &int_p3x) == EOF ) {
					printf("error converting string\n");
				}
				if(sscanf(p3y, "%f", &int_p3y) == EOF )  {
					printf("error converting string\n");
				}
				if(sscanf(wp, "%f", &int_wp_id) == EOF )   {
					printf("error converting string\n");
				}

				int_p1x = toWebots(MAT_UNIT_X,int_p1x,X_OFFSET);
				int_p1y = toWebots(MAT_UNIT_Y,int_p1y,Y_OFFSET);

				int_p2x = toWebots(MAT_UNIT_X,int_p2x,X_OFFSET);
				int_p2y = toWebots(MAT_UNIT_Y,int_p2y,Y_OFFSET);

				int_p3x = toWebots(MAT_UNIT_X,int_p3x,X_OFFSET);
				int_p3y = toWebots(MAT_UNIT_Y,int_p3y,Y_OFFSET);

				printf("p1: %f %f, p2: %f %f, p3: %f %f \n",int_p1x, int_p1y, int_p2x, int_p2y, int_p3x, int_p3y);

				x_wp_data[x_head] = 1;
				x_wp_data[x_head+1] = int_p1x;
				x_wp_data[x_head+2] = int_p1y;
				x_wp_data[3] = int_wp_id;
				x_head = x_head + 4;
				can_send1 = 1;

				//sleep(0.5);

				y_wp_data[y_head] = 2;
				y_wp_data[y_head+1] = int_p2x;
				y_wp_data[y_head+2] = int_p2y;
				y_wp_data[3] = int_wp_id+1; 
				y_head = y_head + 4;
				can_send2 = 1;

				/*z_wp_data[z_head] = 2;
				z_wp_data[z_head+1] = int_p3x;
				z_wp_data[z_head+2] = int_p3y;
				z_wp_data[3] = int_wp_id+2; 
				z_head = z_head + 4;
				can_send3 = 1;*/

				count++;
			} 

			/* Free memory, close MATLAB engine and exit. */
			mxDestroyArray(result);
			engClose(ep);

			//return EXIT_SUCCESS;
		}
	}
}

// thread to send uav3 waypoints to webots
void *sendWaypoints3(){

	struct sockaddr_in cliAddr_wp3, servAddr_wp3;
	struct sockaddr_in cliAddr_webots3, servAddr_webots3;

	int sd_webots3, rc_webots3, n_webots3, cliLen_webots3, flags_webots3;

	sd_webots3=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_webots3<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr_webots3.sin_family = AF_INET;
	servAddr_webots3.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_webots3.sin_port = htons(LOCAL_SERVER_PORT12);
	rc_webots3 = bind (sd_webots3, (struct sockaddr *) &servAddr_webots3,sizeof(servAddr_webots3));
	if(rc_webots3<0) {
		printf("%s: cannot bind port number %d \n", "prg", LOCAL_SERVER_PORT12);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT12);

	cliLen_webots3 = sizeof(cliAddr_webots3);
	n_webots3 = recvfrom(sd_webots3, wp_data,sizeof(wp_data), flags_webots3,
		(struct sockaddr *) &cliAddr_webots3, &cliLen_webots3);

	if(n_webots3<0) {
		printf("%s [6]: cannot receive data \n","prg");
		/*continue;*/
	}
	else{
		printf("Waypoint3 Ready? %d\n",wp_data[0]);
	}

	int recData = 0;

	while(1){

		recData = 0;

		while (recData < 1)	{

			switch(recData)	{
			case 0:
				recData++;

				int rcP2_3;
			
				//if(thread_lock == 0){
					//pthread_mutex_lock(&count_mutex);
				
					if (gotCoords > 0){
						if (wp_data[0] == 0){
							rcP2_3 = sendto(sd_webots3,wp_data,sizeof(wp_data),flags, (struct sockaddr *)&cliAddr_webots3,cliLen_webots3);
						}

						if (can_send3 == 1){
							//printf("sending_wp3_data: %f %f %f %f\n", z_wp_data[0], z_wp_data[1], z_wp_data[2], z_wp_data[3]);
							rcP2_3 = sendto(sd_webots3,z_wp_data,sizeof(z_wp_data),flags, (struct sockaddr *)&cliAddr_webots3,cliLen_webots3);
							
							//can_send3=0;
						}
					}

					if(rcP2_3<0) {
						printf("%s [3]: cannot send size data %d \n","program",i-1);
						close(sd);
					}
					//thread_lock = 1;
					//pthread_mutex_unlock(&count_mutex);
				//}
				break;
			}
		}
	}
}

// thread to send uav2 waypoints to webots
void *sendWaypoints2(){

	struct sockaddr_in cliAddr_wp2, servAddr_wp2;
	struct sockaddr_in cliAddr_webots2, servAddr_webots2;

	int sd_webots2, rc_webots2, n_webots2, cliLen_webots2, flags_webots2;

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
		printf("Waypoint2 Ready? %d\n",wp_data[0]);
	}

	int recData = 0;

	while(1){
		recData = 0;

		while (recData < 1)	{

			switch(recData)	{
			case 0:
				recData++;

				int rcP2_2;
			
				//if(thread_lock == 0){//
					//pthread_mutex_lock(&count_mutex);
				
					if (gotCoords > 0){
						if (wp_data[0] == 0){
							rcP2_2 = sendto(sd_webots2,wp_data,sizeof(wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
						}

						if (can_send2 == 1){
							rcP2_2 = sendto(sd_webots2,y_wp_data,sizeof(y_wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
							//printf("sending_wp2_data: %f %f %f %f\n", y_wp_data[0], y_wp_data[1], y_wp_data[2], y_wp_data[3]);
							//can_send2=0;
						}
					}

					if(rcP2_2<0) {
						printf("%s [3]: cannot send size data %d \n","program",i-1);
						close(sd);
					}
					//thread_lock = 1;
					//pthread_mutex_unlock(&count_mutex);
				//}//
				break;
			}
		}
	}
}

// thread to send uav1 waypoints to webots
void *sendWaypoints1(){

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
		printf("Got connection!\n"); fflush(stdout);
		printf("Waypoint1 Ready? %d\n",wp_data[0]);
	}

	int recData = 0;

	while(1){
		recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:

				recData++;

				int rcP2;
								//if(thread_lock == 1){//
				//pthread_mutex_lock(&count_mutex);

					if (gotCoords > 0){
						if (wp_data[0] == 0){
							rcP2 = sendto(sd_webots,wp_data,sizeof(wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);
						}

						if (can_send1 == 1) {
							rcP2 = sendto(sd_webots,x_wp_data,sizeof(x_wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);	
							//printf("sending_wp1_data: %f %f %f %f\n", x_wp_data[0], x_wp_data[1], x_wp_data[2], x_wp_data[3]);
							//can_send1=0;		
						}
					}

					if(rcP2<0) {
						printf("%s [4]: cannot send size data %d \n","program",i-1);
						close(sd);
					}
					//thread_lock = 0;
					//pthread_mutex_unlock(&count_mutex);
				//}//
				break;
			}
		}
	}
}

// thread to receive uav1 position data from webots
void *posThread1() {

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
				x1 = posArrary[0], y11 = posArrary[1];
				//printf("p1: %d %d ",x1,y11);

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

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT9);

	flags2 = 0;
	
	while(1){
		
		int recData = 0;

		while (recData < 1) {

			switch(recData) {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, posArrary2, sizeof(posArrary2), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				x2 = posArrary2[0], y2 = posArrary2[1];
				//printf("p2: %d %d ",x2,y2);

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

// thread to receive uav3 position data from webots
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
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT13);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));
	if(rc2<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT13);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT13);

	flags2 = 0;
	
	while(1){
		
		int recData = 0;

		while (recData < 1) {

			switch(recData) {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, posArrary3, sizeof(posArrary3), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				x3 = posArrary3[0], y3 = posArrary3[1];
				//printf("p3: %d %d ",x3,y3);

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
void *tposThread() {

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

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT10);

	flags2 = 0;
	
	while(1){
		
		int recData = 0;

		while (recData < 1) {

			switch(recData) {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, tposArrary, sizeof(tposArrary), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				//printf("target: %i, %i\n",tposArrary[0],tposArrary[1]);

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

	pthread_t video1_pth; 
	pthread_t data_pth;
	pthread_t video2_pth; 
	pthread_t wp_interface_pth; 
	pthread_t wp_matlab_pth;
	pthread_t send_waypoint1_pth; 
	pthread_t send_waypoint2_pth; 
	pthread_t pos1_pth; 
	pthread_t pos2_pth;
	pthread_t tpos_pth;
	pthread_t wait_pth; 
	pthread_t video3_pth; 
	pthread_t pos3_pth;
	pthread_t send_waypoint3_pth; 

	pthread_create(&pos1_pth,NULL,posThread1,"Drone 1 pos info.. \n");
	pthread_create(&pos2_pth,NULL,posThread2,"Drone 2 pos info.. \n");
	pthread_create(&pos3_pth,NULL,posThread3,"Drone 3 pos info.. \n");
	pthread_create(&tpos_pth,NULL,tposThread,"Target Drone pos info.. \n");
	pthread_create(&data_pth,NULL,dataThread,"Data stuff.. \n");
	pthread_create(&send_waypoint3_pth,NULL,sendWaypoints3,"Send wp 3.. \n");
	pthread_create(&send_waypoint2_pth,NULL,sendWaypoints2,"Send wp 2.. \n");
	pthread_create(&send_waypoint1_pth,NULL,sendWaypoints1,"Send wp 1.. \n");
	pthread_create(&video1_pth,NULL,videoThread1,"Getting video1 data..\n");
	pthread_create(&video2_pth,NULL,videoThread2,"Getting video2 data..\n");
	pthread_create(&video3_pth,NULL,videoThread3,"Getting video3 data..\n");
	pthread_create(&wp_interface_pth,NULL,wpInterface,"waypoint interface info.. \n");
	pthread_create(&wp_matlab_pth,NULL,wpMatlab,"waypoint matlab info.. \n");
	pthread_create(&wait_pth,NULL,waitThread(argc,argv),"waiting.. \n");
}
