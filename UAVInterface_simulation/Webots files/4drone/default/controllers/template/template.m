% MATLAB controller for Webots
% File:          template.m
% Date:          
% Description:   
% Author:        
% Modifications: 

% add path to Webots MATLAB API files
addpath([getenv('WEBOTS_HOME') '/lib/matlab']);

% initialize communication with Webots
wb_robot_init();

% uncomment the next two lines if you want to use
% MATLAB's desktop to interact with the controller:
%desktop;
%keyboard;

TIME_STEP = 64;

% get and enable devices, e.g.:
%  camera = wb_robot_get_device('camera');
%  wb_camera_enable(camera, TIME_STEP);

% main loop:
% perform simulation steps of TIME_STEP milliseconds
% and leave the loop when Webots signals the termination
%
while wb_robot_step(TIME_STEP) ~= -1

  % read the sensors, e.g.:
  %  rgb = wb_camera_get_image(camera);
  
  % Process here sensor data, images, etc.

  % send actuator commands, e.g.:
  %  wb_differential_wheels_set_speed(500, -500);
  
  % if your code plots some graphics, it needs to flushed like this:
  drawnow;

end

% cleanup code goes here: write data to files, etc.
