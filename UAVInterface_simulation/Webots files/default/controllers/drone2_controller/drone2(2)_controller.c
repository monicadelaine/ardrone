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



#define TIMESTEP 256
/*
 * You may want to add defines macro here.
 */
#define TIME_STEP 64
//const unsigned char *data_pack1;
int width;
int height;
int size;



/*
 * You should put some helper functions here
 */

/*
 * This is the main program.
 * The arguments of the main function can be specified by the
 * "controllerArgs" field of the Robot node
 */

int main(int argc, char **argv)
{
  /* necessary to initialize webots stuff */
  //static WbFieldRef cam_field;
  
  wb_robot_init();
  //static WbDeviceTag cam1; 
   static WbDeviceTag cam2; 

  size = width * height * 3;
  //cam1 = wb_robot_get_device("camera1");
  cam2 = wb_robot_get_device("camera2");
 
 
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
      
       
      
       wb_camera_enable(cam2, TIMESTEP);
       //wb_camera_enable(cam2, TIMESTEP);
        
       
      
       // wb_camera_get_image(camera_2);
       // int r = wb_camera_image_get_red(data_pack1,width,20,20);
        // printf("Data: %d\n",r);
        
      
  do {
       const unsigned char *im =  wb_camera_get_image(cam2);
       printf("Data: %c\n",im[40]);
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
  
  return 0;
}
