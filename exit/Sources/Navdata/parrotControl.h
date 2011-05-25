#ifndef _PARROTCONTROL_H_
#define _PARROTCONTROL_H_

#include <stdlib.h>
#include <ardrone_api.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define stop 0x01

double *flight(float, float, float, int, double, int, int, double);
int emer(void);
void *getKey(void *);
void setup();
void turn1(double, double);


#endif // _PARROTCONTROL_H_
