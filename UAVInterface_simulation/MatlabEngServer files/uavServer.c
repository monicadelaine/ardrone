#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <pthread.h>

#include "matrix.h"
#include "engine.h"

#define LOCAL_SERVER_PORT 1500
#define LOCAL_SERVER_PORT2 1501
#define LOCAL_SERVER_PORT3 1502
#define LOCAL_SERVER_PORT4 1503
#define LOCAL_SERVER_PORT5 1504
#define LOCAL_SERVER_PORT6 1505
#define LOCAL_SERVER_PORT7 1506
#define LOCAL_SERVER_PORT8 1507
#define LOCAL_SERVER_PORT9 1508
#define LOCAL_SERVER_PORT10 1509

#define MAX_MSG 40000
#define dataSize 12290
#define waypointSize 128
#define dataSize_command 255

int gotCoords=0;
float IPAD_UNIT_X = 45.080468637;
float IPAD_UNIT_Y = 45.201098449;
float MAT_UNIT_X = 6.798030861;
float MAT_UNIT_Y = 6.191931294;
float X_OFFSET = 19.9352;
float Y_OFFSET = 19.9403;
float ipadCoords[4];

float drone1_X;
float drone1_Y;
float drone2_X;
float drone2_Y;

int x_head = 0;
int y_head = 0;

float init_x = 0;
float init_y = 0;

char data1[57600];
int posArrary[6];
int posArrary2[6];
char data1_command[255];
float wp_data[255];
float x_wp_data[1000];
float y_wp_data[1000];
int data1_send[100];
char* comData;
char* comData2;

char data2[57600];
double data2_command[255];
int data2_send[255];

int thread_lock = 0;

int i = 0;
int done = 0;
int done2 = 0;
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
int IPAD_FLAG = 1;
int recMatData = 0;

/*variables for Matlab engine*/
Engine *ep;
mxArray *result = NULL, *sresult = NULL, *cpresult = NULL;
double alulFlag[0], start[0], targetFound[0], cell_pos[0];
mxArray *m_A, *m_S, *m_TF, *m_CP;
double *cresult;
char *cpresult2;
int tFound = 0;
int startEng = 0;


float toWebots(float unit, float val, float offset){
	float newVal;
	newVal = (val / unit) - offset;

	return newVal;
}


void toIpad(float x, float y, float x2, float y2) {
	float newX = ((x + X_OFFSET)  * IPAD_UNIT_X);
	float newX2 = ((x2  + X_OFFSET) * IPAD_UNIT_X);
	float newY = ((y + Y_OFFSET) * IPAD_UNIT_Y) ;
	float newY2 = ((y2 + Y_OFFSET) * IPAD_UNIT_Y);

	ipadCoords[0] = newX;
	ipadCoords[1] = newY;
	ipadCoords[2] = newX2;
	ipadCoords[3] = newY2;
}


void toMatlab(float x, float y, float x2, float y2) {
	float newX = ((x + X_OFFSET)  * MAT_UNIT_X);
	float newX2 = ((x2  + X_OFFSET) * MAT_UNIT_X);
	float newY = ((y + Y_OFFSET) * MAT_UNIT_Y) ;
	float newY2 = ((y2 + Y_OFFSET) * MAT_UNIT_Y);

	ipadCoords[0] = newX;
	ipadCoords[1] = newY;
	ipadCoords[2] = newX2;
	ipadCoords[3] = newY2;
}


void *waitThread(int argc, char *argv[]) {
	while (1) {
		;
	}
}


void *videoThread(void) {
	/*////////////////////////////////////////////////Video Socket/////////////////////////////////*/
	
	/* socket creation */
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(LOCAL_SERVER_PORT);
	rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
	if(rc<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT);

	/* BEGIN jcs 3/30/05 */

	flags = 0;

	/*////////////////////////////////////////////////Video Socket Drone 1 /////////////////////////////////*/

	while(1){
		/* printf("vid.");*/
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen = sizeof(cliAddr);
				n = recvfrom(sd, data1, dataSize, flags, (struct sockaddr *) &cliAddr, &cliLen);

				if(n<0) {
					printf("%s [1]: cannot receive data \n","prg");
					/*continue;*/
				}
				else{
					/*printf("Got Packet %d\n",recData);*/
					recData++;
				}

				break;
			default:
				printf("Default case reached\n");
				break;
			}
		}

		memcpy(comData, data1,57600 *sizeof(uint8_t));
		/*printf("data: %d\n",data1[0]);*/

		drone1_X = data1[0];
		drone1_Y = data1[1];

		/*drone1_X = data1[0];
		drone1_Y = data1[1];

		printf("Drone1 X: %d\n",data1[0]);
		printf("Ipad Val: %c\n",comData[0]);
		*/
		done++;

		;
	}
}


void *dataThread(){
	/*/////////////////////////////////////////////////////// Commands and Data Comm 1//////////////////////*/

	sd_command=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_command<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr_command.sin_family = AF_INET;
	servAddr_command.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_command.sin_port = htons(LOCAL_SERVER_PORT2);
	rc_command = bind (sd_command, (struct sockaddr *) &servAddr_command,sizeof(servAddr_command));
	if(rc_command<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT2);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT2);

	cliLen_command = sizeof(cliAddr_command);
	n_command = recvfrom(sd_command, data1_command, dataSize_command, flags_command,
		(struct sockaddr *) &cliAddr_command, &cliLen_command);
	if(n<0) {
		printf("%s [2]: cannot receive data \n","prg");
		/*continue;*/
	}
	else{
		//printf("Got connection!\n");
		/* printf("Got Packet %d\n",recData);
		recData++;*/
		//printf("Test %c\n",data1_command[0]);
	}

	/*toIpad(-5,-10,-10,-10);
	printf("x1: %f\nx2: %f\nx3: %f\nx4: %f\n",ipadCoords[0], ipadCoords[1], ipadCoords[2], ipadCoords[3]);*/
	int recData = 0;

	while(1){
		/* printf(".");*/
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

				/*
				init_x = init_x + 0.003;
				init_y = init_y + 0.003;
				printf("Value: %f\n",init_x);*/
				data1[12290] = drone1_X;
				data1[12291] = drone1_Y;
				data1[12292] = drone2_X;
				data1[12293] = drone2_Y;
				data1[12294] = 999;
				/* sleep(1);*/

				/*while (thread_lock == 1) {
				;
				}
				thread_lock = 1;*/
				posArrary[2] = posArrary2[0];
                		posArrary[3] = posArrary2[1];
				posArrary[4] = posArrary2[2];
                		posArrary[5] = posArrary2[3];
				
                		rcP = sendto(sd_command,data1,sizeof(datatest),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);

                		rcP = sendto(sd_command,data2,sizeof(datatest),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);

                		rcP = sendto(sd_command,posArrary,sizeof(posArrary),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);


				int i=0;

				if(rcP<0) {
					printf("%s [1]: cannot send size data %d \n","program",i-1);
					close(sd);
					/*exit(1);*/
				}else{
					/*   printf("Data Sent\n");*/
				}
				break;
			}
		}
	}
}

flags_command = 0;


/*//////// Just for testing.... function will be created to reduce code later ////////////*/


void *videoThread1(void){
	/*////////////////////////////////////////////////Video Socket/////////////////////////////////*/

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	/* socket creation */
	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr2.sin_family = AF_INET;
	servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT3);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));
	if(rc2<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT3);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT3);

	flags2 = 0;

	/*////////////////////////////////////////////////Video Socket Drone 1 /////////////////////////////////*/

	while(1){
		/* printf("vid.");*/
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, data2, dataSize, flags2, (struct sockaddr *) &cliAddr2, &cliLen2);

				if(n2<0) {
					printf("%s [3]: cannot receive data \n","prg");
					/*continue;*/
				}
				else{
					/* printf("Got Packet %d\n",recData);*/
					recData++;
				}

				break;
			default:
				printf("Default case reached\n");
				break;
			}
		}

		memcpy(comData2, data2,57600 *sizeof(uint8_t));
		drone2_X = data2[0];
		drone2_Y = data2[1];

		done2++;

		;
	}
}


/*//////// Just for testing.... function will be created to reduce code later ////////////*/

void *dataThread1(){
	/*/////////////////////////////////////////////////////// Commands and Data Comm 1//////////////////////*/

	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr_command.sin_family = AF_INET;
	servAddr_command.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_command.sin_port = htons(LOCAL_SERVER_PORT4);
	rc_command = bind (sd2, (struct sockaddr *) &servAddr_command,sizeof(servAddr_command));
	if(rc_command<0) {
		printf("%s: cannot bind port number %d \n",	"prg", LOCAL_SERVER_PORT4);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n", "prg",LOCAL_SERVER_PORT4);

	cliLen_command = sizeof(cliAddr_command);
	n_command = recvfrom(sd2, data1_command, dataSize_command, flags_command,
		(struct sockaddr *) &cliAddr_command, &cliLen_command);
	if(n<0) {
		printf("%s [4]: cannot receive data \n","prg");
		/*continue;*/
	}
	else{
		//printf("Got connection!\n");
		/*   printf("Got Packet %d\n",recData);
		recData++;*/
		printf("Test %c\n",data1_command[0]);
	}

	int recData = 0;

	while(1){
		/*  printf(".");*/
		recData = 0;

		while (recData < 1)	{

			switch(recData)		{
			case 0:

				recData++;

				char datatest2[42000];
				int rcP2;
				/* memset(data1_send,126,100);*/
				data2[57600] = 2;

				while (thread_lock == 1) {
					;
				}
				thread_lock = 1;
				rcP2 = sendto(sd2,data2,sizeof(datatest2),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
				thread_lock = 0;

				int i=0;

				if(rcP2<0) {
					printf("%s [2]: cannot send size data %d \n","program",i-1);
					close(sd);
					/*exit(1);*/
				}else{
					/*    printf("Data Sent\n");*/
				}
				break;
			}
		}
	}
}


void *wayPoints(void) {

	char *p1x, *p2x, *p3x, *p4x, *wp;
	char *p1y, *p2y, *p3y, *p4y, *pID;
	char *algID;

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
	servAddr_wp.sin_port = htons(LOCAL_SERVER_PORT7);
	rc_wp = bind (sd_wp, (struct sockaddr *) &servAddr_wp,sizeof(servAddr_wp));
	if(rc_wp<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT7);/*change to 1507*/
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT7);

	flags_wp = 0;
	float pack_size;

	/*DATA Thread*/

	while(1){
		/* printf("vid.");*/
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
			//if (IPAD_FLAG ==1) {
				cliLen_wp = sizeof(cliAddr_wp);
				//printf("Entering receive\n");
				n_wp = recvfrom(sd_wp, wPoints, waypointSize, flags_wp,
					(struct sockaddr *) &cliAddr_wp, &cliLen_wp);
				//printf("Exit receive\n");

				if(n_wp<0) {
					printf("%s [5]: cannot receive data \n","prg");
					/*continue;*/
				}	
				else{
					/* printf("Got Packet %d\n",recData);*/
					recData++;
					//printf("Array ID: %c\n",wPoints[0]);

					pID = strtok ( wPoints, "," );
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

					float pID2;
					float int_p1x, int_p1y;
					float int_p2x, int_p2y;
					float int_p3x, int_p3y;
					float int_p4x, int_p4y;
					float int_wp_id, int_alg_id;
					float clone_int_p1x;
					float clone_int_p1y;

					int *i;

					/* put this in a FOR Loop later!!!!!*/
					///////////////////////////////////////

					if(sscanf(pID, "%f", &pID2) == EOF )  {
						printf("error converting string\n");
					}
					if(sscanf(p1x, "%f", &int_p1x) == EOF ) {
						printf("error converting string\n");
					}
					if(sscanf(p1y, "%f", &int_p1y) == EOF )  {
						printf("error converting string\n");
					}

					//////////////////////////////////////////////

					if (gotCoords > 0){
						if(sscanf(p2x, "%f", &int_wp_id) == EOF )  {
							printf("error converting string\n");
						}
					} else{
						if(sscanf(wp, "%f", &int_wp_id) == EOF )   {
							printf("error converting string\n");
						}
					}

					//printf("after \n");

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
						if(sscanf(algID, "%f", &int_alg_id) == EOF )  {
							printf("error converting string\n");
						}
						gotCoords++;
						pack_size = 8;

						if (int_alg_id == 2) { 
							IPAD_FLAG = 2;
							//printf("ipad flag %i\n", IPAD_FLAG);
						} else if (int_alg_id == 3){
							IPAD_FLAG = 3;
							//printf("ipad flag %i\n", IPAD_FLAG);
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
					}
					else{
						pack_size = 2;
					}

					/* for loop here ^^^^^^^^^^
					printf("\nBefore\nWebots x: %f \n Webots y: %f \n",int_p1x,int_p1y);*/

					int_p1x = toWebots(IPAD_UNIT_X,int_p1x,X_OFFSET);
					//printf("New ID %f\n",pID2);
					//printf("New P1x: %f\n",int_p1x);
					int_p1y = toWebots(IPAD_UNIT_Y,int_p1y,Y_OFFSET);
					//printf("New P1y: %f\n",int_p1y);

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
					
					printf("PID: %f\n",pID2);
					
					if (pID2 == 1){
						x_wp_data[x_head] = 1;
						x_wp_data[x_head+1] = int_p1x;
						x_wp_data[x_head+2] = int_p1y;
						x_wp_data[3] = int_wp_id;
						x_head = x_head + 4;
						can_send1 = 1;
					}
					
					if ((pID2 == 2)){
						y_wp_data[y_head] = 2;
						y_wp_data[y_head+1] = int_p1x;
						y_wp_data[y_head+2] = int_p1y;
						y_wp_data[3] = int_wp_id; /*changes  this index constantly*/
						y_head = y_head + 4;
						can_send2 = 1;
					}
					/*printf("\nWebots x: %f \n Webots y: %f \n",int_p1x,int_p1y);*/
				}
				//}
				break;

			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}


void *wayPointsMatlab(void) {
/*void wayPointsMatlab(void) {//for testing*/

	/*startEng = 0; IPAD_FLAG = 0; /*for testing*/

	char *p1x, *p1y, *p2x, *p2y, *wp, *algID;

	while (1) {
	if (IPAD_FLAG != 1) {
		
		if (startEng == 0) {
			/* Open the MATLAB engine */
			if (!(ep = engOpen("\0"))) {
				fprintf(stderr, "\nCan't start MATLAB engine\n");
				return EXIT_FAILURE;
			}

			/* testing ALULSimulation */
			printf("using AlulRobotSimulation\n");

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

			if (IPAD_FLAG == 2) 
				engEvalString(ep, "alulFlag = 0;"); 
			else if (IPAD_FLAG == 3)
				engEvalString(ep, "alulFlag = 1;"); 
			printf("ipad flag %i\n", IPAD_FLAG);
		
			startEng = 1;
		}

		/**********************************************************************************/

		//while (tFound == 0) {
		while (1) {

			engEvalString(ep, "if (start==1) cameras=[our_cam_pos(1,1) dov; our_cam_pos(2,1) dov]; end");
			engEvalString(ep, "demoRunAlulRobot");

			result = engGetVariable(ep,"targetFound");
			cresult = mxGetPr(result);

			cpresult = engGetVariable(ep,"cell_pos");
			cpresult2 = mxArrayToString(cpresult);

			//printf("%s\n", cpresult2);

			if (cresult[0]==1.00) {
				tFound = 1;
			}

			/**********************************************************************************/

			p1x = strtok ( cpresult2, "," );
			p1y = strtok ( NULL, "," );
			p2x = strtok ( NULL, "," );
			p2y = strtok ( NULL, "," );
			wp = strtok ( NULL, "," );

			//printf("p1x:%s\np1y:%s\np2x:%s\np2y:%s\nWP_ID:%s\n",p1x,p1y,p2x,p2y,wp);

			float int_p1x, int_p1y, int_p2x, int_p2y, int_wp_id;

			/* put this in a FOR Loop later!!!!!*/

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

			//printf("after \n");

			/* for loop here ^^^^^^^^^^
			printf("\nBefore\nWebots x: %f \n Webots y: %f \n",int_p1x,int_p1y);*/

			int_p1x = toWebots(MAT_UNIT_X,int_p1x,X_OFFSET);
			//printf("New P1x: %f\n",int_p1x);
			int_p1y = toWebots(MAT_UNIT_Y,int_p1y,Y_OFFSET);
			//printf("New P1y: %f\n",int_p1y);

			int_p2x = toWebots(MAT_UNIT_X,int_p2x,X_OFFSET);
			///printf("New P2x: %f\n",int_p2x);
			int_p2y = toWebots(MAT_UNIT_Y,int_p2y,Y_OFFSET);
			//printf("New P2y: %f\n",int_p2y);

			x_wp_data[x_head] = 1;
			x_wp_data[x_head+1] = int_p1x;
			x_wp_data[x_head+2] = int_p1y;
			x_wp_data[3] = int_wp_id;
			x_head = x_head + 4;
			can_send1 = 1;

			y_wp_data[y_head] = 2;
			y_wp_data[y_head+1] = int_p2x;
			y_wp_data[y_head+2] = int_p2y;
			y_wp_data[3] = int_wp_id+1; 
			y_head = y_head + 4;
			can_send2 = 1;

			/*printf("\nWebots x: %f \n Webots y: %f \n",int_p1x,int_p1y);*/
		}

		//printf("end testing AlulRobotSimulation\n");

		/* Free memory, close MATLAB engine and exit. */
		mxDestroyArray(result);
		engClose(ep);

		return EXIT_SUCCESS;
	}
	}
}


void *sendWaypoints2(void){

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

	/* bind local server port */
	servAddr_webots2.sin_family = AF_INET;
	servAddr_webots2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_webots2.sin_port = htons(LOCAL_SERVER_PORT8);
	rc_webots2 = bind (sd_webots2, (struct sockaddr *) &servAddr_webots2,sizeof(servAddr_webots2));
	if(rc_webots2<0) {
		printf("%s: cannot bind port number %d \n",	"prg", LOCAL_SERVER_PORT8);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT8);

	cliLen_webots2 = sizeof(cliAddr_webots2);
	n_webots2 = recvfrom(sd_webots2, wp_data,sizeof(wp_data), flags_webots2,
		(struct sockaddr *) &cliAddr_webots2, &cliLen_webots2);
	if(n_webots2<0) {
		printf("%s [6]: cannot receive data \n","prg");
		/*continue;*/
	}
	else{
		printf("Got connection!\n");
		/*   printf("Got Packet %d\n",recData);
		recData++;*/
		printf("Waypoint Ready? %d\n",wp_data[0]);
		//printf("Waypoint Ready? %d\n",wp_data[1]);
	}

	int recData = 0;

	while(1){
		/*  printf(".");*/
		recData = 0;

		while (recData < 1)	{

			switch(recData)	{
			case 0:
				recData++;

				char datatest2[42000];
				int rcP2_2;
				/* memset(data1_send,126,100);*/
				data2[57600] = 2;

				/*while ((thread_lock == 1) ) {
                 		}*/
				if(thread_lock == 0){
				//thread_lock = 1;
				if (gotCoords > 0){
					/*   printf("Sending webots coords\n");*/
					if (wp_data[0] == 0){
						rcP2_2 = sendto(sd_webots2,wp_data,sizeof(wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
					}
					/*
					if (wp_data[0] == 1){
					rcP2_2 = sendto(sd_webots2,x_wp_data,sizeof(x_wp_data),flags,(struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
					}
					*/
					/*printf("Sending: %f\n",wp_data[0]);*/
					if (can_send2 == 1){
						//printf("Servicing thread 2\n");
						rcP2_2 = sendto(sd_webots2,y_wp_data,sizeof(y_wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
					}
				}
				thread_lock = 1;

				int i=0;

				if(rcP2_2<0) {
					printf("%s [3]: cannot send size data %d \n","program",i-1);
					close(sd);
					/*exit(1);*/
				}else{
					/*   printf("Data Sent\n");*/
				}
				}
				break;
			}
		}
	}
}


void *sendWaypoints(void){

	sd_webots=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_webots<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr_webots.sin_family = AF_INET;
	servAddr_webots.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_webots.sin_port = htons(LOCAL_SERVER_PORT6);
	rc_webots = bind (sd_webots, (struct sockaddr *) &servAddr_webots,sizeof(servAddr_webots));
	if(rc_webots<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT6);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT6);

	cliLen_webots = sizeof(cliAddr_webots);
	n_webots = recvfrom(sd_webots, wp_data,sizeof(wp_data), flags_webots,
		(struct sockaddr *) &cliAddr_webots, &cliLen_webots);
	if(n_webots<0) {
		printf("%s [7]: cannot receive data \n","prg");
		/*continue;*/
	}
	else{
		//printf("Got connection!\n");
		/*   printf("Got Packet %d\n",recData);
		recData++;*/
		printf("Waypoint Ready? %d\n",wp_data[0]);
		//printf("Waypoint Ready? %d\n",wp_data[1]);
	}

	int recData = 0;

	while(1){
		/*  printf(".");*/
		recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:

				recData++;

				char datatest2[42000];
				int rcP2;
				/* memset(data1_send,126,100);*/
				data2[57600] = 2;
				if(thread_lock == 1){
				/*while ((thread_lock == 1) ) {
                 		}*/
				//thread_lock = 0;
				if (gotCoords > 0){
					/*  printf("Sending webots coords\n");*/
					if (wp_data[0] == 0){
						rcP2 = sendto(sd_webots,wp_data,sizeof(wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);
					}

					if (can_send1 == 1) {
						//printf("Servicing thread 1\n");
						rcP2 = sendto(sd_webots,x_wp_data,sizeof(x_wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);				
					}
					/*
					if (wp_data[0] == 2){
					rcP2 = sendto(sd_webots,y_wp_data,sizeof(y_wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);
					}
					*/
				}
				thread_lock = 0;

				int i=0;

				if(rcP2<0) {
					printf("%s [4]: cannot send size data %d \n","program",i-1);
					close(sd);
					/*exit(1);*/
				}else{
					/*     printf("Data Sent\n");*/
				}
				}
				break;
			}
		}
	}
}


void *posThread(void) {

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	/* socket creation */
	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
	servAddr2.sin_family = AF_INET;
	servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT9);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));
	if(rc2<0) {
		printf("%s: cannot bind port number %d \n",/* stub*/
			"prg", LOCAL_SERVER_PORT9);
		exit(1);
	}

	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT9);

	flags2 = 0;

	while(1){
		/* printf("vid.");*/
		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {

			case 0:
				cliLen2 = sizeof(cliAddr2);
				/*printf("Receiving\n");*/
				n2 = recvfrom(sd2, posArrary, 6, flags2, (struct sockaddr *) &cliAddr2, &cliLen2);

				if(n2<0) {
					printf("%s [8]: cannot receive data \n","prg");
					/*continue;*/
				}
				else{
					recData++;
					/* printf("Data: %d\nData2: %d",posArrary[1],posArrary[2]);*/
				}
				break;

			default:
				printf("Default case reached\n");
				break;
			}
		}
	}
}

void *posThread2(void) {

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	/* socket creation */
	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	/* bind local server port */
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
		/* printf("vid.");*/
		int recData = 0;

		while (recData < 1) {

			switch(recData) {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				/*printf("Receiving\n");*/
				n2 = recvfrom(sd2, posArrary2, sizeof(posArrary2), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				//printf("target: %i, %i\n",posArrary2[0],posArrary2[3]);

				if(n2<0) {
					printf("%s [9]: cannot receive data \n","prg");
					/*continue;*/
				}
				else{
					recData++;
					/* printf("Data: %d\nData2: %d",posArrary[1],posArrary[2]);*/
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
	/*wayPointsMatlab(); //for testing*/

	comData = malloc(57600 * sizeof(uint8_t));
	comData2 = malloc(57600 * sizeof(uint8_t));

	/*Threads*/
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
	pthread_t wait_pth; 

	pthread_create(&pos_pth,NULL,posThread,"pos info.. \n");
	pthread_create(&pos_pth2,NULL,posThread2,"pos info.. \n");
	pthread_create(&data_pth,NULL,dataThread,"Data stuff.. \n");
	pthread_create(&send_waypoint2_pth,NULL,sendWaypoints2,"Send wp.. \n");
	pthread_create(&send_waypoint_pth,NULL,sendWaypoints,"Send wp.. \n");
	pthread_create(&video_pth,NULL,videoThread,"Getting Pixbuf Data.\n");
	pthread_create(&data_pth1,NULL,dataThread1,"Data stuff (Drone 2).. \n");
	pthread_create(&video_pth1,NULL,videoThread1,"Getting Pixbuf Data (Drone 2).\n");
	pthread_create(&waypoint_pth,NULL,wayPoints,"waypoint info.. \n");
	pthread_create(&waypoint_pth,NULL,wayPointsMatlab,"waypoint matlab info.. \n");
	pthread_create(&wait_pth,NULL,waitThread(argc,argv),"waiting.. \n");
}
