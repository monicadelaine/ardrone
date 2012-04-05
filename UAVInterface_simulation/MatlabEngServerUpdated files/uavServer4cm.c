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
#define LOCAL_SERVER_PORT14 1514 
#define LOCAL_SERVER_PORT15 1515
#define LOCAL_SERVER_PORT16 1516 

#define dataSize 12294
#define dataSize_command 255

long long int send_interval1 = 200, send_time1 = 0;

int gotCoords=0;
float IPAD_UNIT_X = 45.080468637;
float IPAD_UNIT_Y = 45.201098449;
float MAT_UNIT_X = 4.47238872;
float MAT_UNIT_Y = 5.41793988;
float X_OFFSET = 19.9352;
float Y_OFFSET = 19.9403;
int MX = 130, MY = 190, MAXY = 175; 
int matCoords[4];

char data1[12294];
char data2[12294];
char data3[12294];
char data4[12294];
char data1_command[255];
int posArray[6];
int posArray2[6];
int posArray3[6];
int posArray4[6];
int tposArray[20];
int bArray[400];
int bArray2[400][2];
int rArray[8];
int bflag = 0;
int r1_flag = 0, r1_ind = 0;
int r2_flag = 0, r2_ind = 0;
int r3_flag = 0, r3_ind = 0;
int r4_flag = 0, r4_ind = 0;

int w_head = 0;
int x_head = 0;
int y_head = 0;
int z_head = 0;
float wp_data[255];
float w_wp_data[4000];
float x_wp_data[4000];
float y_wp_data[4000];
float z_wp_data[4000];

int i = 0, s = 1;
int wPoints[64];
int can_send1, can_send2, can_send3, can_send4;

int sd, rc, n, cliLen, flags;
int sd_command, rc_command, n_command, cliLen_command, flags_command;
int sd_webots, rc_webots, n_webots, cliLen_webots, flags_webots;
struct sockaddr_in cliAddr, servAddr;
struct sockaddr_in cliAddr_command, servAddr_command;
struct sockaddr_in cliAddr_webots, servAddr_webots;

/* flag to determine if we are using matlab or ipad coordinates*/
int ALG_FLAG = 1; //1=manual, 2=auto, 3=semiauto
int WP_FLAG1 = 0, WP_FLAG2 = 0, WP_FLAG3 = 0, WP_FLAG4 = 0;
int dflag = 0;

/*variables for Matlab engine*/
Engine *ep;
mxArray *cpresult = NULL, *c1result = NULL, *c2result = NULL, *c3result = NULL, *c4result = NULL, *dresult = NULL, *bresult = NULL;
double alulFlag[0], start[0], cell_pos[0], cell1[0], cell2[0], cell3[0], cell4[0], mdata[0], bcells[400][2];
mxArray *m_A, *m_S, *m_CP, *m_C1, *m_C2, *m_C3, *m_C4, *m_D, *m_B;
char *cpresult2, *dresult2;
double *cresult, *bresult2;
double *ptr;
int startEng = 0;
int x1, y11, x2, y2, x3, y3, x4, y4;
int wp_x1[50][2], wp_y1[50][2], wp_x2[50][2], wp_y2[50][2], wp_x3[50][2], wp_y3[50][2], wp_x4[50][2], wp_y4[50][2];
int wpCnt1 = 0, wpCnt2 = 0, wpCnt3 = 0, wpCnt4 = 0, reachCnt1 = 0, reachCnt2 = 0, reachCnt3 = 0, reachCnt4 = 0;


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
	
	int newX = (int)( (x + X_OFFSET) * MAT_UNIT_X );
	int newY = (int)( (y + Y_OFFSET) * MAT_UNIT_Y );
	matCoords[0] = newX+MX+MATX;
	matCoords[1] = MAXY-newY+MY+MATY;
	//printf("toMatlab=> x: %i y: %i, newX: %i, newY: %i matX: %i, matY: %i\n",x,y,newX,newY,matCoords[0],matCoords[1]);
}

int toInterface(float unit, float val, float offset) {
	int interfaceCoords = (int)( (val + offset) * unit );
	return interfaceCoords;
	//printf("toInterface=> x: %f y: %f, intX: %i, intY: %i\n",x,y,interfaceCoords[0],interfaceCoords[1]);
}

void *waitThread(int argc, char *argv[]) {
	while (1) { ; }
}

void boundary(double *bresult2, mwSize rows, mwSize cols) {
	printf("bcells %i %i \n", rows, cols);
	int a, b, i, j, k=0, l=2;
	if (rows<400) {
		/*for (a=0; a<cols*rows; a++) {
			printf("%f ",bresult2[a]);
		}*/
		bflag=1;
		bArray[0] = rows, bArray[1] = cols;
		for (i=0; i<cols; i++) {
			for (j=0; j<rows; j++) {
				if (i==0 && bresult2[k] != -1 && bresult2[k] != -2 ) {
					bresult2[k] = toWebots(MAT_UNIT_X,bresult2[k],X_OFFSET);
					bresult2[k] = toInterface(IPAD_UNIT_X,bresult2[k],X_OFFSET);
				}
				if (i==1 && bresult2[k] != -1 && bresult2[k] != -2 ) {
					bresult2[k] = toWebots(MAT_UNIT_Y,bresult2[k],Y_OFFSET);
					bresult2[k] = toInterface(IPAD_UNIT_Y,bresult2[k],Y_OFFSET);
				}
				bArray[l] = bresult2[k];
				//bArray2[j][i] = bresult2[k];
				k+=1; l+=1;
			}
		}
		
		/*for (i=0; i<rows; i++) {
			for (j=0; j<cols; j++) {
				printf("%i ",bArray2[i][j]);
			}
			printf("\n");
		}*/
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
				data4[0] = 'D';
				
				//add the uav pos and route info to the target array
				tposArray[2] = posArray[0];
                                tposArray[3] = posArray[1];
				tposArray[4] = posArray2[0];
                                tposArray[5] = posArray2[1];	
				tposArray[6] = posArray3[0];
                                tposArray[7] = posArray3[1];	
				tposArray[8] = posArray4[0];
                                tposArray[9] = posArray4[1];
				tposArray[10] = rArray[0];
                                tposArray[11] = rArray[1];
				tposArray[12] = rArray[2];
                                tposArray[13] = rArray[3];	
				tposArray[14] = rArray[4];
                                tposArray[15] = rArray[5];	
				tposArray[16] = rArray[6];
                                tposArray[17] = rArray[7];
				tposArray[18] = -2;
				tposArray[19] = -2;
				//printf("r: %d, %d p3: %d, %d\n",tposArray[10],tposArray[11],tposArray[6],tposArray[7]);
				
				// send data to interface every ??? ms
				long long int curr_time = get_current_time()/1000;
				if (curr_time > send_time1 + send_interval1) {
					send_time1 = get_current_time()/1000;
					rcP = sendto(sd_command,data1,sizeof(data1),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
            				rcP = sendto(sd_command,data2,sizeof(data2),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
            				rcP = sendto(sd_command,data3,sizeof(data3),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
            				rcP = sendto(sd_command,data4,sizeof(data4),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
					rcP = sendto(sd_command,tposArray,sizeof(tposArray),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
					if (bflag==1) {
						rcP = sendto(sd_command,bArray,sizeof(bArray),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);
					}
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

// thread to receive video data for uav1 from webots
void *videoThread1() {
	
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

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

// thread to receive video data for uav4
void *videoThread4(){

	int sd4, rc4, n4, cliLen4, flags4;
	struct sockaddr_in cliAddr4, servAddr4;

	sd4=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd4<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr4.sin_family = AF_INET;
	servAddr4.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr4.sin_port = htons(LOCAL_SERVER_PORT14);
	rc4 = bind (sd4, (struct sockaddr *) &servAddr4,sizeof(servAddr4));
	if(rc4<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT14);
		exit(1);
	}
	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT14);

	flags4 = 0;

	while(1){

		int recData = 0;

		while (recData < 1)    {

			switch(recData)    {
			case 0:
				cliLen4 = sizeof(cliAddr4);
				n4 = recvfrom(sd4, data4, dataSize, flags4, (struct sockaddr *) &cliAddr4, &cliLen4);

				if(n4<0) {
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
	char *algID, *numUAVs, *pIDx;

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
					float ix, iy;

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
						ix=int_p1x, iy=int_p1y;
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
							ALG_FLAG = 1;
						} else if (int_alg_id == 2) { 
							ALG_FLAG = 2;
						} else if (int_alg_id == 3){
							ALG_FLAG = 3;
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
					
					if (pID2 == 1){
						x_wp_data[x_head] = pID2;
						x_wp_data[x_head+1] = int_p1x;
						x_wp_data[x_head+2] = int_p1y;
						x_wp_data[/*x_head+*/3] = int_wp_id;
						x_head = x_head + 4;
						can_send1 = 1; 
						if (ALG_FLAG == 3 && dflag == 1) {
							WP_FLAG1 = 1; 
							wp_x1[wpCnt1][0] = int_p1x; wp_x1[wpCnt1][1] = ix;
							wp_y1[wpCnt1][0] = int_p1y; wp_y1[wpCnt1][1] = iy;
							printf("manual wp1=> x1: %i, y1: %i \n",wp_x1[wpCnt1][0],wp_y1[wpCnt1][0]);
							wpCnt1++;	
						}
						dflag = 1;
					} else if (pID2 == 2) {
						y_wp_data[y_head] = pID2;
						y_wp_data[y_head+1] = int_p1x;
						y_wp_data[y_head+2] = int_p1y;
						y_wp_data[/*y_head+*/3] = int_wp_id; 
						y_head = y_head + 4;
						can_send2 = 1;				
						if (ALG_FLAG == 3) {
							WP_FLAG2 = 1; 
							wp_x2[wpCnt2][0] = int_p1x; wp_x2[wpCnt2][1] = ix;
							wp_y2[wpCnt2][0] = int_p1y; wp_y2[wpCnt2][1] = iy;
							printf("manual wp2=> x2: %i, y2: %i \n",wp_x2[wpCnt2][0],wp_y2[wpCnt2][0]);
							wpCnt2++;
						}
					} else if (pID2 == 3) {
						z_wp_data[z_head] = pID2;
						z_wp_data[z_head+1] = int_p1x;
						z_wp_data[z_head+2] = int_p1y;
						z_wp_data[/*z_head+*/3] = int_wp_id; 
						z_head = z_head + 4;
						can_send3 = 1;
						if (ALG_FLAG == 3) {
							WP_FLAG3 = 1; 
							wp_x3[wpCnt3][0] = int_p1x; wp_x3[wpCnt3][1] = ix;
							wp_y3[wpCnt3][0] = int_p1y; wp_y3[wpCnt3][1] = iy;
							printf("manual wp3=> x3: %i, y3: %i \n",wp_x3[wpCnt3][0],wp_y3[wpCnt3][0]);
							wpCnt3++;
						}
					} else if (pID2 == 4) {
						w_wp_data[w_head] = pID2;
						w_wp_data[w_head+1] = int_p1x;
						w_wp_data[w_head+2] = int_p1y;
						w_wp_data[/*w_head+*/3] = int_wp_id; 
						w_head = w_head + 4;
						can_send4 = 1;
						if (ALG_FLAG == 3) {
							WP_FLAG4 = 1; 
							wp_x4[wpCnt4][0] = int_p1x; wp_x4[wpCnt4][1] = ix;
							wp_y4[wpCnt4][0] = int_p1y; wp_y4[wpCnt4][1] = iy;
							printf("manual wp4=> x4: %i, y4: %i \n",wp_x4[wpCnt4][0],wp_y4[wpCnt4][0]);
							wpCnt4++;
						}
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

	char *p1x, *p1y, *p2x, *p2y, *p3x, *p3y, *p4x, *p4y, *wp, *algID;
	char *es0, *es1, *es2, *es3, *es4, *es5, *d;

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

			m_CP=mxCreateDoubleMatrix(1, 1, mxREAL);
			memcpy((void *)mxGetPr(m_CP), (void *)cell_pos, sizeof(double)*1);
			engPutVariable(ep, "cell_pos", m_CP);

			m_C1=mxCreateDoubleMatrix(1, 1, mxREAL);
			memcpy((void *)mxGetPr(m_C1), (void *)cell1, sizeof(double)*1);
			engPutVariable(ep, "cell1", m_C1);
	
			m_C2=mxCreateDoubleMatrix(1, 1, mxREAL);
			memcpy((void *)mxGetPr(m_C2), (void *)cell2, sizeof(double)*1);
			engPutVariable(ep, "cell2", m_C2);

			m_C3=mxCreateDoubleMatrix(1, 1, mxREAL);
			memcpy((void *)mxGetPr(m_C3), (void *)cell3, sizeof(double)*1);
			engPutVariable(ep, "cell3", m_C3);
	
			m_C4=mxCreateDoubleMatrix(1, 1, mxREAL);
			memcpy((void *)mxGetPr(m_C4), (void *)cell4, sizeof(double)*1);
			engPutVariable(ep, "cell4", m_C4);

			m_D=mxCreateDoubleMatrix(1, 1, mxREAL);
			memcpy((void *)mxGetPr(m_D), (void *)mdata, sizeof(double)*1);
			engPutVariable(ep, "mdata", m_D);

			m_B=mxCreateDoubleMatrix(2, 400, mxREAL);
			memcpy((void *)mxGetPr(m_B), (void *)bcells, sizeof(double)*400*2);
			engPutVariable(ep, "bcells", m_B);
			engEvalString(ep,"bcells=bcells'");

			engEvalString(ep, "start=0");
			engEvalString(ep, "alulFlag = 1;"); 

			engEvalString(ep, "loadMat4"); 

			startEng = 1;
		}
		if (ALG_FLAG != 1) {
			/**********************************************************************************/
			while (1) {
				///////////////////////////////
				if (reachCnt1 < wpCnt1 && r1_flag==0) { 			 
					rArray[0] = wp_x1[r1_ind][1], rArray[1] = wp_y1[r1_ind][1]; 
					r1_flag=1;
				}
				if (reachCnt2 < wpCnt2 && r2_flag==0) { 			 
					rArray[2] = wp_x2[r2_ind][1], rArray[3] = wp_y2[r2_ind][1]; 
					r2_flag=1;
				}
				if (reachCnt3 < wpCnt3 && r3_flag==0) { 			 
					rArray[4] = wp_x3[r3_ind][1], rArray[5] = wp_y3[r3_ind][1]; 
					r3_flag=1;
				}
				if (reachCnt4 < wpCnt4 && r4_flag==0) { 			 
					rArray[6] = wp_x4[r4_ind][1], rArray[7] = wp_y4[r4_ind][1]; 
					r4_flag=1;
				}	
				//////////////////////////
				if (reachCnt1 < wpCnt1) {
					float dist1 = sqrt( pow((wp_x1[reachCnt1][0] - x1),2) + pow((wp_y1[reachCnt1][0] - y11),2));
					printf("wp_x1:%i wp_y1:%i, x1:%i y1:%i, dist1:%f\n",wp_x1[reachCnt1][0],wp_y1[reachCnt1][0],x1,y11,dist1);
					if (dist1 <= 1)  { reachCnt1++; r1_flag=0; r1_ind++; }
				} else if (reachCnt1 >= wpCnt1) { WP_FLAG1 = 0; }
				if (reachCnt2 < wpCnt2) {
					float dist2 = sqrt( pow((wp_x2[reachCnt2][0] - x2),2) + pow((wp_y2[reachCnt2][0] - y2),2) );
					printf("wp_x2:%i wp_y2:%i, x2:%i y2:%i, dist2:%f\n",wp_x2[reachCnt2],wp_y2[reachCnt2],x2,y2,dist2);			
					if (dist2 <= 1) { reachCnt2++; r2_flag=0; r2_ind++;}
				} else if (reachCnt2 >= wpCnt2) { WP_FLAG2 = 0; }
				if (reachCnt3 < wpCnt3) {
					float dist3 = sqrt( pow((wp_x3[reachCnt3][0] - x3),2) + pow((wp_y3[reachCnt3][0] - y3),2));
					printf("wp_x3:%i wp_y3:%i, x3:%i y3:%i, dist3:%f\n",wp_x3[reachCnt3],wp_y3[reachCnt3],x3,y3,dist3);
					if (dist3 <= 1) { reachCnt3++; r3_flag=0; r3_ind++; }
				} else if (reachCnt3 >= wpCnt3) { WP_FLAG3 = 0; }
				if (reachCnt4 < wpCnt4) {
					float dist4 = sqrt( pow((wp_x4[reachCnt4][0] - x4),2) + pow((wp_y4[reachCnt4][0] - y4),2) );
					printf("wp_x4:%i wp_y4:%i, x4:%i y4:%i, dist4:%f\n",wp_x4[reachCnt4],wp_y4[reachCnt4],x4,y4,dist4);		
					if (dist4 <= 1) { reachCnt4++; r4_flag=0; r4_ind++;}
				} else if (reachCnt4 >= wpCnt4) { WP_FLAG4 = 0; }
				///////////////////////////////////////////////
				if (WP_FLAG1 == 0 && WP_FLAG2 == 0 && WP_FLAG3 == 0 && WP_FLAG4 == 0) { //auto
					engEvalString(ep, "if (start==1) cameras=[our_cam_pos(1,1) dov; our_cam_pos(2,1) dov; our_cam_pos(3,1) dov; our_cam_pos(4,1) dov]; end");
					engEvalString(ep, "demoRunAlulRobot4");
					cpresult = engGetVariable(ep,"cell_pos");
					cpresult2 = mxArrayToString(cpresult);
					printf("auto: %s %lld\n", cpresult2, get_current_time()/1000); fflush(stdout);
					dresult = engGetVariable(ep,"mdata");
					dresult2 = mxArrayToString(dresult);
					printf("data: %s %lld\n", dresult2, get_current_time()/1000); fflush(stdout);
					bresult = engGetVariable(ep,"bcells");
					mwSize rows = mxGetM(bresult);
					mwSize cols = mxGetN(bresult);
					bresult2 = mxGetData(bresult);
					boundary(bresult2,rows,cols);
				} else if (WP_FLAG1 == 1 || WP_FLAG2 == 1 || WP_FLAG3 == 1 || WP_FLAG4 == 1) { 
					if (WP_FLAG1 == 1) { //wp1
						printf("1 => ");
						toMatlab(x1,y11);
						int x = matCoords[0], y = matCoords[1]; //pos coming from webots
						char buf1[50];
						char *s1 = "cell1 = returnCell(ucells,";
						char *s2 = ")";
						snprintf(buf1, sizeof(buf1), "%s%d,%d%s", s1, x, y, s2);
						//printf("%s\n", buf1);	
						engEvalString(ep, buf1);
						c1result = engGetVariable(ep,"cell1");
						cresult = mxGetPr(c1result);
						//printf("cell1: %f\n", cresult[0]);
						es1 =  "cameras(1,:)=[cell1 dov];";
					} else if (WP_FLAG1 == 0) {
						es1 = "cameras(1,:)=[our_cam_pos(1,1) dov];";
					}
					if (WP_FLAG2 == 1) { //wp2
						printf("2 => ");
						toMatlab(x2,y2);
						int x = matCoords[0], y = matCoords[1]; 
						char buf2[50];
						char *s1 = "cell2 = returnCell(ucells,";
						char *s2 = ")";
						snprintf(buf2, sizeof(buf2), "%s%d,%d%s", s1, x, y, s2);
						//printf("%s\n", buf2);	
						engEvalString(ep, buf2);
						c2result = engGetVariable(ep,"cell2");
						cresult = mxGetPr(c2result);
						//printf("cell2: %f\n", cresult[0]);
						es2 =  "cameras(2,:)=[cell2 dov];";
					} else if (WP_FLAG2 == 0) {
						es2 = "cameras(2,:)=[our_cam_pos(2,1) dov];";
					}
					if (WP_FLAG3 == 1) { //wp3
						printf("3 => ");
						toMatlab(x3,y3);
						int x = matCoords[0], y = matCoords[1]; 
						char buf3[50];
						char *s1 = "cell3 = returnCell(ucells,";
						char *s2 = ")";
						snprintf(buf3, sizeof(buf3), "%s%d,%d%s", s1, x, y, s2);
						//printf("%s\n", buf3);	
						engEvalString(ep, buf3);
						c3result = engGetVariable(ep,"cell3");
						cresult = mxGetPr(c3result);
						//printf("cell3: %f\n", cresult[0]);
						es3 =  "cameras(3,:)=[cell3 dov];";
					} else if (WP_FLAG3 == 0) {
						es3 = "cameras(3,:)=[our_cam_pos(3,1) dov];";
					}
					if (WP_FLAG4 == 1) { //wp4
						printf("4 => ");
						toMatlab(x4,y4);
						int x = matCoords[0], y = matCoords[1]; //pos coming from webots
						char buf4[50];
						char *s1 = "cell4 = returnCell(ucells,";
						char *s2 = ");";
						snprintf(buf4, sizeof(buf4), "%s%d,%d%s", s1, x, y, s2);
						//printf("%s\n", buf4);	
						engEvalString(ep, buf4);
						c4result = engGetVariable(ep,"cell4");
						cresult = mxGetPr(c4result);
						//printf("cell4: %f\n", cresult[0]);
						es4 = "cameras(4,:)=[cell4 dov];";
					} else if (WP_FLAG4 == 0) {
						es4 = "cameras(4,:)=[our_cam_pos(4,1) dov];";
					}
					char buf5[200];
					es0 = "if (start==1)";
					es5 = "end";
					snprintf(buf5, sizeof(buf5), "%s %s %s %s %s %s", es0, es1, es2, es3, es4, es5);
					//printf("%s\n", buf5);
					engEvalString(ep, buf5);
					engEvalString(ep, "demoRunAlulRobot4");
					cpresult = engGetVariable(ep,"cell_pos");
					cpresult2 = mxArrayToString(cpresult);
					printf("man: %s %lld\n", cpresult2, get_current_time()/1000); fflush(stdout);
					dresult = engGetVariable(ep,"mdata");
					dresult2 = mxArrayToString(dresult);
					printf("data: %s %lld\n", dresult2, get_current_time()/1000); fflush(stdout);
					/*bresult = engGetVariable(ep,"bcells");
					bresult2 = mxGetPr(bresult);
					printf("bcells: %f %f \n", bresult2[0], bresult2[1]); fflush(stdout);*/
					//printf("buffer: %s\n", buffer+2);
				}

				/****************************************************************************/

				p1x = strtok ( cpresult2, "," );
				p1y = strtok ( NULL, "," );
				p2x = strtok ( NULL, "," );
				p2y = strtok ( NULL, "," );
				p3x = strtok ( NULL, "," );
				p3y = strtok ( NULL, "," );
				p4x = strtok ( NULL, "," );
				p4y = strtok ( NULL, "," );
				wp = strtok ( NULL, "," );

				float int_p1x, int_p1y, int_p2x, int_p2y, int_p3x, int_p3y, int_p4x, int_p4y, int_wp_id;

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
				if(sscanf(p4x, "%f", &int_p4x) == EOF ) {
					printf("error converting string\n");
				}
				if(sscanf(p4y, "%f", &int_p4y) == EOF )  {
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
				
				int_p4x = toWebots(MAT_UNIT_X,int_p4x,X_OFFSET);
				int_p4y = toWebots(MAT_UNIT_Y,int_p4y,Y_OFFSET);	

				printf("p1: %f %f, p2: %f %f, p3: %f %f, p4: %f %f %lld \n\n",int_p1x, int_p1y, int_p2x, int_p2y, int_p3x, int_p3y, int_p4x, int_p4y, get_current_time()/1000);

				if (s == 1) {
					if (WP_FLAG1 == 0) {
						x_wp_data[x_head] = 1;
						x_wp_data[x_head+1] = int_p1x;
						x_wp_data[x_head+2] = int_p1y;
						x_wp_data[/*x_head+*/3] = int_wp_id;
						x_head = x_head + 4;
						can_send1 = 1;
						rArray[0] = toInterface(IPAD_UNIT_X,int_p1x,X_OFFSET);
						rArray[1] = toInterface(IPAD_UNIT_Y,int_p1y,Y_OFFSET);
						//rArray[0] = interfaceCoords[0], rArray[1] = interfaceCoords[1];
					}
					s=2;					
				}
				if (s == 2) {
					if (WP_FLAG2 == 0) {
						y_wp_data[y_head] = 2;
						y_wp_data[y_head+1] = int_p2x;
						y_wp_data[y_head+2] = int_p2y;
						y_wp_data[/*y_head+*/3] = int_wp_id+1; 
						y_head = y_head + 4;
						can_send2 = 1;
						//toInterface(int_p2x,  int_p2y);
						//rArray[2] = interfaceCoords[0], rArray[3] = interfaceCoords[1];
						rArray[2] = toInterface(IPAD_UNIT_X,int_p2x,X_OFFSET);
						rArray[3] = toInterface(IPAD_UNIT_Y,int_p2y,Y_OFFSET);
					}
					s=3;
				}			
				if (s == 3) {
				if (WP_FLAG3 == 0) {
						z_wp_data[z_head] = 3;
						z_wp_data[z_head+1] = int_p3x;
						z_wp_data[z_head+2] = int_p3y;
						z_wp_data[/*z_head+*/3] = int_wp_id+2; 
						z_head = z_head + 4;
						can_send3 = 1;
						//toInterface(int_p3x, int_p3y);
						//rArray[4] = interfaceCoords[0], rArray[5] = interfaceCoords[1];
						rArray[4] = toInterface(IPAD_UNIT_X,int_p3x,X_OFFSET);
						rArray[5] = toInterface(IPAD_UNIT_Y,int_p3y,Y_OFFSET);
					}
					s=4;
				}		
				if (s == 4) {
					if (WP_FLAG4 == 0) {
						w_wp_data[w_head] = 4;
						w_wp_data[w_head+1] = int_p4x;
						w_wp_data[w_head+2] = int_p4y;
						w_wp_data[/*w_head+*/3] = int_wp_id+3; 
						w_head = w_head + 4;
						can_send4 = 1;
						//toInterface(int_p4x, int_p4y);
						//rArray[6] = interfaceCoords[0], rArray[7] = interfaceCoords[1];
						rArray[6] = toInterface(IPAD_UNIT_X,int_p4x,X_OFFSET);
						rArray[7] = toInterface(IPAD_UNIT_Y,int_p4y,Y_OFFSET);
					}
					s=1;
				}
			} 
			/* Free memory, close MATLAB engine and exit. */
			mxDestroyArray(cpresult);
			engClose(ep);
			//return EXIT_SUCCESS;
		}
	}
}

// thread to send uav4 waypoints to webots
void *sendWaypoints4(){

	struct sockaddr_in cliAddr_wp4, servAddr_wp4;
	struct sockaddr_in cliAddr_webots4, servAddr_webots4;

	int sd_webots4, rc_webots4, n_webots4, cliLen_webots4, flags_webots4;

	sd_webots4=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_webots4<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr_webots4.sin_family = AF_INET;
	servAddr_webots4.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr_webots4.sin_port = htons(LOCAL_SERVER_PORT15);
	rc_webots4 = bind (sd_webots4, (struct sockaddr *) &servAddr_webots4,sizeof(servAddr_webots4));
	
	if(rc_webots4<0) {
		printf("%s: cannot bind port number %d \n", "prg", LOCAL_SERVER_PORT15);
		exit(1);
	}
	printf("%s: waiting for data on port UDP %u\n",	"prg",LOCAL_SERVER_PORT15);

	cliLen_webots4 = sizeof(cliAddr_webots4);
	n_webots4 = recvfrom(sd_webots4, wp_data,sizeof(wp_data), flags_webots4,
		(struct sockaddr *) &cliAddr_webots4, &cliLen_webots4);

	if(n_webots4<0) {
		printf("%s [6]: cannot receive data \n","prg");
	}
	else{
		printf("Waypoint4 Ready? %d\n",wp_data[0]);
	}

	int recData = 0;

	while(1){
		recData = 0;
		while (recData < 1)	{
			switch(recData)	{
			case 0:
				recData++;
				int rcP2_4;
				
					if (gotCoords > 0){
						if (wp_data[0] == 0){
							rcP2_4 = sendto(sd_webots4,wp_data,sizeof(wp_data),flags, (struct sockaddr *)&cliAddr_webots4,cliLen_webots4);
						}

						if (can_send4 == 1){
							//printf("sending_wp4_data: %f %f %f %f\n", w_wp_data[w_head-4], w_wp_data[w_head+1-4], w_wp_data[w_head+2-4], w_wp_data[w_head+3-4]);
							rcP2_4 = sendto(sd_webots4,w_wp_data,sizeof(w_wp_data),flags, (struct sockaddr *)&cliAddr_webots4,cliLen_webots4);					
						}
					}

					if(rcP2_4<0) {
						printf("%s [3]: cannot send size data %d \n","program",i-1);
						close(sd);
					}
				break;
			}
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
			
					if (gotCoords > 0){
						if (wp_data[0] == 0){
							rcP2_3 = sendto(sd_webots3,wp_data,sizeof(wp_data),flags, (struct sockaddr *)&cliAddr_webots3,cliLen_webots3);
						}

						if (can_send3 == 1){
							//printf("sending_wp3_data: %f %f %f %f\n", z_wp_data[z_head-4], z_wp_data[z_head+1-4], z_wp_data[z_head+2-4], z_wp_data[z_head+3-4]);
							rcP2_3 = sendto(sd_webots3,z_wp_data,sizeof(z_wp_data),flags, (struct sockaddr *)&cliAddr_webots3,cliLen_webots3);
							
						}
					}

					if(rcP2_3<0) {
						printf("%s [3]: cannot send size data %d \n","program",i-1);
						close(sd);
					}
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
				
					if (gotCoords > 0){
						if (wp_data[0] == 0){
							rcP2_2 = sendto(sd_webots2,wp_data,sizeof(wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
						}

						if (can_send2 == 1){
							rcP2_2 = sendto(sd_webots2,y_wp_data,sizeof(y_wp_data),flags, (struct sockaddr *)&cliAddr_webots2,cliLen_webots2);
							//printf("sending_wp2_data: %f %f %f %f\n", y_wp_data[y_head-4], y_wp_data[y_head+1-4], y_wp_data[y_head+2-4], y_wp_data[y_head+3-4]);
						}
					}

					if(rcP2_2<0) {
						printf("%s [3]: cannot send size data %d \n","program",i-1);
						close(sd);
					}
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

					if (gotCoords > 0){
						if (wp_data[0] == 0){
							rcP2 = sendto(sd_webots,wp_data,sizeof(wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);
						}

						if (can_send1 == 1) {
							rcP2 = sendto(sd_webots,x_wp_data,sizeof(x_wp_data),flags,(struct sockaddr *)&cliAddr_webots,cliLen_webots);	
							//printf("sending_wp1_data: %f %f %f %f\n", x_wp_data[x_head-4], x_wp_data[x_head+1-4], x_wp_data[x_head+2-4], x_wp_data[x_head+3-4]);	
						}
					}

					if(rcP2<0) {
						printf("%s [4]: cannot send size data %d \n","program",i-1);
						close(sd);
					}
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
				n2 = recvfrom(sd2, posArray, 6, flags2, (struct sockaddr *) &cliAddr2, &cliLen2);
				x1 = toWebots(IPAD_UNIT_X,posArray[0],X_OFFSET), y11 = toWebots(IPAD_UNIT_Y,posArray[1],Y_OFFSET);

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
				n2 = recvfrom(sd2, posArray2, sizeof(posArray2), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				x2 = toWebots(IPAD_UNIT_X,posArray2[0],X_OFFSET), y2 = toWebots(IPAD_UNIT_Y,posArray2[1],Y_OFFSET);

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
				n2 = recvfrom(sd2, posArray3, sizeof(posArray3), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				x3 = toWebots(IPAD_UNIT_X,posArray3[0],X_OFFSET), y3 = toWebots(IPAD_UNIT_Y,posArray3[1],Y_OFFSET);

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

// thread to receive uav4 position data from webots
void *posThread4() {

	int sd2, rc2, n2, cliLen2, flags2;
	struct sockaddr_in cliAddr2, servAddr2;

	sd2=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd2<0) {
		printf("%s: cannot open socket \n","prg");
		exit(1);
	}

	servAddr2.sin_family = AF_INET;
	servAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr2.sin_port = htons(LOCAL_SERVER_PORT16);
	rc2 = bind (sd2, (struct sockaddr *) &servAddr2,sizeof(servAddr2));

	if(rc2<0) {
		printf("%s: cannot bind port number %d \n","prg", LOCAL_SERVER_PORT16);
		exit(1);
	}
	printf("%s: waiting for data on port UDP %u\n","prg",LOCAL_SERVER_PORT16);

	flags2 = 0;
	
	while(1){
		int recData = 0;
		while (recData < 1) {
			switch(recData) {
			case 0:
				cliLen2 = sizeof(cliAddr2);
				n2 = recvfrom(sd2, posArray4, sizeof(posArray4), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				x4 = toWebots(IPAD_UNIT_X,posArray4[0],X_OFFSET), y4 = toWebots(IPAD_UNIT_Y,posArray4[1],Y_OFFSET);

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
				n2 = recvfrom(sd2, tposArray, sizeof(tposArray), flags2,(struct sockaddr *) &cliAddr2, &cliLen2);
				//printf("target: %i, %i\n",tposArray[0],tposArray[1]);

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
	pthread_t video4_pth; 
	pthread_t pos4_pth;
	pthread_t send_waypoint4_pth;

	pthread_create(&pos1_pth,NULL,posThread1,"Drone 1 pos info.. \n");
	pthread_create(&pos2_pth,NULL,posThread2,"Drone 2 pos info.. \n");
	pthread_create(&pos3_pth,NULL,posThread3,"Drone 3 pos info.. \n");
	pthread_create(&pos4_pth,NULL,posThread4,"Drone 4 pos info.. \n");
	pthread_create(&tpos_pth,NULL,tposThread,"Target Drone pos info.. \n");
	pthread_create(&data_pth,NULL,dataThread,"Data stuff.. \n");
	pthread_create(&send_waypoint4_pth,NULL,sendWaypoints4,"Send wp 4.. \n");
	pthread_create(&send_waypoint3_pth,NULL,sendWaypoints3,"Send wp 3.. \n");
	pthread_create(&send_waypoint2_pth,NULL,sendWaypoints2,"Send wp 2.. \n");
	pthread_create(&send_waypoint1_pth,NULL,sendWaypoints1,"Send wp 1.. \n");
	pthread_create(&video1_pth,NULL,videoThread1,"Getting video1 data..\n");
	pthread_create(&video2_pth,NULL,videoThread2,"Getting video2 data..\n");
	pthread_create(&video3_pth,NULL,videoThread3,"Getting video3 data..\n");
	pthread_create(&video4_pth,NULL,videoThread4,"Getting video4 data..\n");
	pthread_create(&wp_interface_pth,NULL,wpInterface,"waypoint interface info.. \n");
	pthread_create(&wp_matlab_pth,NULL,wpMatlab,"waypoint matlab info.. \n");
	pthread_create(&wait_pth,NULL,waitThread(argc,argv),"waiting.. \n");
}
