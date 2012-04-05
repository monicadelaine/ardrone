// File:         
// Date:         
// Description:  
// Author:       
// Modifications:

// You may need to add webots include files such as
// <webots/DistanceSensor.hpp>, <webots/LED.hpp>, etc.
// and/or to add some other includes
#include <webots/Robot.hpp>

// All the webots classes are defined in the "webots" namespace
using namespace webots;

// Here is the main class of your controller.
// This class defines how to initialize and how to run your controller.
// Note that this class derives Robot and so inherits all its functions
class drone_controller : public Robot {
  
  // You may need to define your own functions or variables, like
  //  LED* led;
  
  public:
    
    // drone_controller constructor
    drone_controller(): Robot() {
      
      // You should insert a getDevice-like function in order to get the
      // instance of a device of the robot. Something like:
      //  led = getLED("ledName");
      
      WbDeviceTag rotor1 = wb_robot_get_device("rotor1");
      //WbDeviceTag rotor1 = wb_robot_get_device("upper_rotor");

      
      wb_servo_set_position(rotor1, WB_SERVO_INFINITY);
      //wb_servo_set_position(rotor0, -WB_SERVO_INFINITY);
      
      
      
    }

    // drone_controller destructor
    virtual ~drone_controller() {
      
      // Enter here exit cleanup code
      
    }
    
    // User defined function for initializing and running
    // the drone_controller class
    void run(){
      
      // Main loop
      do {
        
        // Read the sensors:
        // Enter here functions to read sensor data, like:
        //  double val = distanceSensor->getValue();
        
        // Process sensor data here
        
        // Enter here functions to send actuator commands, like:
        //  led->set(1);

        // Perform a simulation step of 64 milliseconds
        // and leave the loop when the simulation is over
      } while (step(64) != -1);
    }
};

// This is the main program of your controller.
// It creates an instance of your Robot subclass, launches its
// function(s) and destroys it at the end of the execution.
// Note that only one instance of Robot should be created in
// a controller program.
// The arguments of the main function can be specified by the
// "controllerArgs" field of the Robot node
int main(int argc, char **argv)
{
  drone_controller* controller = new drone_controller();
  controller->run();
  delete controller;
  return 0;
}
