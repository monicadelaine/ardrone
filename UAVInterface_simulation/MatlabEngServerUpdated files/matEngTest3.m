clear *
close all

start=0;
targetFound = 0;

%while targetFound == 0
while 1
    if start==1 
        cameras=[our_cam_pos(1,1) dov; our_cam_pos(2,1) dov; our_cam_pos(3,1) dov]
    end
    
    demoRunAlulRobot3
    
end
