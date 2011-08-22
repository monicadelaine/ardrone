%FUNCTION_NAME - demoRunAlulRobot
% Loads sample data and runs simulations of surveillance of a defined ROI and
% randomly simulated targets
%
% Syntax:  demoRunAlulRobot
%
% Outputs:
%    data - list of time to detection and target tracking times 
%
%
% Other m-files required: AlulCspSimulation, ALULStats
% Subfunctions: none
% MAT-files required: ucells, occlusion_status, ROI, cameras
%
% See also: AlulRobotSimulation, ALULStats

% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% August 2006; Last revision: 14 Aug 2006

% only load files once
if start==0  
    load ucells
    load occlusions
    load occlusion_status
    load ROI
    %load cameras
    javaaddpath(pwd);
    f2=imread('croi_new.jpg');
    statSaveDir = 'sim/';

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
    
    % Target data
    %start_targets=uint32(rand(numTargets,1)*numCells);
    load start_targets10
    numTargets = size(start_targets,1);  
    
    %    maxUnseen          - max timesteps each viewable cell can go unseen
    %    steps              - number of timesteps to simulate
    %    alulval            - unknown?????
    %    dynamic_t          - flag for dynamic targets
    %    alulFlag           - flag to use ALUL
    
    maxUnseen = 150;
    steps = 100;
    alulval = 0;
    dynamic_t = 0;
    %alulFlag = 0; 
        
    ALUL_THRESHOLD = 15;
    
    t = 1;
    start = 1;
    sim_start = 0;
    
    dov = 20;
    cameras=[5131 dov; 5136 dov];
    %cameras=[5203 dov; 5241 dov; 2283 dov; 2321 dov];
end

AlulRobotSimulation;

if targetFound == 1
    fileName = strcat(statSaveDir, '_starvation-');
    fileName = strcat(fileName, int2str(maxUnseen));
    fileName = strcat(fileName, '_alulThreshold-');
    fileName = strcat(fileName, int2str(ALUL_THRESHOLD));

    fileName = strcat(fileName, '.csv');
    outFile = fopen(fileName,'w');

    % Write out parameters
    fprintf(outFile, 'Starvation Time,%d\n', maxUnseen);
    fprintf(outFile, 'ALUL Threshold,%d\n', ALUL_THRESHOLD);

    % Write out spacer
    fprintf(outFile, '\n\n\n');

    % Write headers
    fprintf(outFile, 'min,avg,max,cells unseen,max unseen\n');

    %Write data
    for i = 1:t-1
        fprintf(outFile, '%f,%f,%f,%d,%d\n', alulReturn(i,2), alulReturn(i,1), alulReturn(i,3), unseenReturn(i), maxUnseenReturn(i));
    end

    fprintf(outFile, '\n');
    fprintf(outFile, '%f,%f,%f,', mean(alulReturn(:,2)), mean(alulReturn(:,1)), mean(alulReturn(:,3)));
    fprintf(outFile, '%f,%f\n\n\n', mean(unseenReturn), mean(maxUnseenReturn));

    fclose(outFile);
end