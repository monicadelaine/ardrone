%FUNCTION_NAME - demoRunAlulRobot
% Runs simulation of surveillance of a defined ROI 
%
% Syntax:  demoRunAlulRobot
%
% Other m-files required: AlulRobotSimulation
%
% MAT-files required: ucells, occlusion_status, ROI
%
% See also: AlulRobotSimulation


% only load files once
if start==0  
    load ucells
    load occlusions
    load occlusion_status
    load ROI
    javaaddpath(pwd);
    f2=imread('croi_new2.jpg');

    % Cell data
    if ~isempty(ROI)
        ourcells=ucells(ROI,:);
        our_occlusion_status=occlusion_status(ROI,:);
        our_occlusions=occlusions(ROI,:);
        numCells = size(ourcells,1);
    else
        ourcells=ucells;
        our_occlusion_status=occlusion_status;
        our_occlusions=occlusions;
        numCells = size(ucells,1);
    end
     
    %    maxUnseen          - max timesteps each viewable cell can go unseen
    %    steps              - number of timesteps to simulate
    %    alulval            - unknown?????
    %    dynamic_t          - flag for dynamic targets
    %    alulFlag           - flag to use ALUL
    
    maxUnseen = 25;
    steps = 100;
    alulval = 0;
    dynamic_t = 0;
    alulFlag = 1; 
        
    ALUL_THRESHOLD = 15;
    
    t = 2;
    start = 1;
    sim_start = 0;
    
    dov = 11;
    %cameras=[5131 dov; 5136 dov];    
    cameras=[4992 dov; 4995 dov; 4773 dov; 4998 dov];  
end

AlulRobotSimulation4;

