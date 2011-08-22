% AlulRobotSimulation
% AlulRobotSimulation - Run simulation of moving cameras/robots to maximize
% coverage
%
% Syntax:  AlulRobotSimulation
%
% Inputs:
%    ucells             - list of cell coordinates and cell heights                        
%    ROI                - list of ucell indexes that denote our ROI
%                           (created in selectROI)
%    occlusion_status   - list of cell visibility to each camera
%                           (created in recordOcclusions)
%                           true if occluded, false otherwise
%    start_targets      - starting positions for targets
%                           start_targets=uint32(rand(numTargets,1)*numCells);
%    cameras            - camera positions and parameters
%    type               - type of surveillance and tracking (1,2 or 3)
%    heights            - data file that contains the heights for java classes
%    arl.matlab.simulation.*   - Java classes for target, terrain, camera, alul and gui
%    alulval            - Maximum alul system value
%
%           ADDED BY ALD
%    maxUnseen          - max timesteps each viewable cell can go unseen
%    steps              - number of timesteps to simulate
%    ourcells           - cells in ROI
%    our_occlusion_status
%                       - occlusion status of cells in ROI
%    numCells           - number of cells in ROI
%
%
% Outputs:
%    maxalulV           - Calculated maximum ALUL values by step
%    maxUnseen          - "Most" unseen cell
%
% Other m-files required: calculateALUL4ROI, isVisible
% Subfunctions: none
% MAT-files required: none
%
% See also: demoRunALUL,  selectROI, recordOcclusions, calcAngles,
% calculateALUL4ROI, returnCell, plotcamera, plotpoints, getRangeOverlap
% 

% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% August 2006; Last revision: 14 Aug 2006

if sim_start == 0
    close all;

    colors{1}=java.awt.Color.BLUE;
    colors{2}=java.awt.Color(.2,.3,.2);
    colors{2}.darker();
    colors{3}=java.awt.Color.CYAN;
    colors{4}=java.awt.Color.GRAY;
    colors{4}.darker();
    colors{5}=java.awt.Color.YELLOW;
    colors{6}=java.awt.Color.MAGENTA;
    colors{7}=java.awt.Color.ORANGE;
    colors{8}=java.awt.Color.PINK;
    colors{9}=java.awt.Color.GREEN;
    colors{9}.brighter();
    colors{10}=java.awt.Color.BLACK;

    numTargets = size(start_targets,1);

    rows=73;
    columns=109;
    
    posCnt = 1; 
    
    MX = 95;
    MY = 155;
    MAX_Y = 200;

    ttIndex=1;
    vote=[];
    voteunseen=[];
    totalLostBlind=0;
    totalLostROI=0;
    totalLostCamera=0;

    numCameras=size(cameras,1);
    numChoices=4;

    our_occlusion_status=occlusion_status(ROI,:);
    cam_occlusion_status=our_occlusion_status;

    pvisible = ~(sum(our_occlusion_status(:,1:numCameras),2) == numCameras); % all cells in our ROI possibly visible == 1
    pAllvisible = ~(sum(occlusion_status(:,1:numCameras),2) == numCameras); % all cells possibly visible == 1

    [tempa,minvalALU,tempb]=calculateALUL4ROI(pvisible,109,73,ROI);

    %worst we will get since it is all one hole
    [tempa,tempb,maxvalALU]=calculateALUL4ROI(zeros(numCells,1),109,73,ROI);
    prevALUL=maxvalALU;

    unseen=zeros(numCells,1);

    %Initialize java objects
    gui = javaObject('arl.matlab.surveil.TargetGUI');
    alul = javaObject('arl.matlab.surveil.ALUL',gui);
    alul.setAlul(alulval);
    alul.getALUL();

    terrain = javaObject('arl.matlab.surveil.Terrain','heights', 73,109,5,329723,4321586.5);
       
    for nt=1:numTargets 
        %dynamic targets
        if dynamic_t==1
            tgt(nt)= javaObject('arl.matlab.surveil.Target',ourcells(start_targets(nt,1),1),ourcells(start_targets(nt,1),2),terrain,gui,strcat('Target_',int2str(nt)), nt*3);
            % javaObject('arl.matlab.surveil.Target',ourcells(start_targets(nt,1),1),ourcells(start_targets(nt,1),2),terrain,gui,strcat('Target_',int2str(nt)));     
            tgt(nt).setAlulThreshold(15);           
        else
            tgt(nt)= start_targets(nt,1);
        end
        red=colors{nt}.getRed();
        blue=colors{nt}.getBlue();
        green=colors{nt}.getGreen();
        rgb(nt,1:3)=[red green blue]; 
        if (dynamic_t==1) tgt(nt).setLabelColor(red,green,blue); end
    end

    rgb=rgb./255;

    %gui.setVisible(true);

    targetPredAge = zeros(numTargets,1);
    targetFound = 0;
    sim_start = 1;
end

%for t=1:steps
%while targetFound == 0
    our_cam_pos = cameras;
    
    % Get cells visible at this time step
    [vt,lt,st] = isVisible(ourcells,our_cam_pos,our_occlusion_status,our_occlusions);
    
    %get state information
    alulThreshold = alul.getALUL();
    
    temp_pos=our_cam_pos;
    vote=zeros(numCells,4);
    alulvals=[];idx=1;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % TARGET ACQUISITION 
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    tgtPredCells = zeros(numTargets, 1);
    for tIdx = 1:numTargets
        if dynamic_t==1
            predCell = returnCell(ucells, tgt(tIdx).getPredictedX(), tgt(tIdx).getPredictedY());
            tgt(tIdx).step()
            tgtCCell=returnCell(ucells,tgt(tIdx).getCurrentX(),tgt(tIdx).getCurrentY); %this cell may not be in the ROI...
        else
            tgtCCell=returnCell(ucells,ourcells(start_targets(tIdx,1),1),ourcells(start_targets(tIdx,1),2));
        end
        
        CinROI=ismember(tgtCCell,ROI);
        
        if vt(find(ROI==tgtCCell))== 0
            targetPredAge(tIdx) = targetPredAge(tIdx) + 1;
        else
            if (dynamic_t==1) tgtPredCells(tIdx) = predCell; end
            targetPredAge(tIdx) = 0; 
            targetFound = 1;
            str = ['target found at cell ' num2str(tgtCCell)];
        end
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % GET CAMERA ACTIONS TO SEE A CELL AND CALCULATE TOP WEIGHT
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    choices = [];
    allChoicesVisible = [];
    allChoicesUnseen = [];
    topWeight = 0;
    choiceCnt = 1;
    cnt=1;
                 
    % Determine which cells can be seen by each camera action
    for c = 1:numCameras   
        bestChoice = 1; 
        totalunseen = 0; 
        sortCnt = 1;
        orderedChoices = [];
        allChoices = [];
        choices = [choices; returnNeighboringCells(our_cam_pos(c,1),rows,columns,ucells)]; %make 8 choices
    
        for k = 1:numChoices
            temp_pos(c,1) = choices(choiceCnt,1);
            [v,l,s] = isVisible(ourcells,temp_pos,our_occlusion_status,our_occlusions);

            votevisible(:,choiceCnt) = l(:,c);
            totalvotesvisible = sum(votevisible(:,choiceCnt));
            allChoicesVisible = [allChoicesVisible; c choiceCnt totalvotesvisible choices(choiceCnt,1)];
          
            voteunseen(:,choiceCnt) = l(:,c) .*(2.^ unseen);
            totalvotesunseen = sum(voteunseen(:,choiceCnt));
            allChoicesUnseen = [allChoicesUnseen; c choiceCnt totalvotesunseen choices(choiceCnt,1)];
            
            if totalvotesunseen > totalunseen
               totalunseen = totalvotesunseen; 
               bestChoice = choiceCnt;
            end            
            choiceCnt = choiceCnt + 1;
            
        end
        temp_pos(c,1) = choices(bestChoice,1);
    end   
    
    %choices
    %allChoicesUnseen
    sortedAll=sortrows(allChoicesUnseen,-3);

    % Set camera choices  
    for j = 1:numCameras    
        for k = 1:numChoices*2
            if sortedAll(k,1) == j
                allChoices(cnt,1) = sortedAll(k,4);
                cnt = cnt + 1;
            else
                continue;
            end            
        end
    end
    %allChoices
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % ENSURE ALUL THRESHOLD IS SATISFIED
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    totalChoices = numChoices*numChoices;  
    maxALUL_Results = zeros(totalChoices,1);
    maxALUL_Pos = zeros(totalChoices,numCameras);
    alulCnt = 1;
    AlulFound = 0;
        
    if alulFlag == 1
        for i = 1:numChoices
            for j = numChoices+1:numChoices*2
                if AlulFound==1        
                    break;
                end
                temp_pos(1,1) = allChoices(i,1);
                temp_pos(2,1) = allChoices(j,1);    
                [v,l,s]=isVisible(ourcells,temp_pos,our_occlusion_status,our_occlusions);
                thisVisible = sum(l(:,1:numCameras),2) > 0; %visible by some camera
                [avgALUL, minALUL, maxALUL]=calculateALUL4ROI(thisVisible,109,73,ROI);
                maxALUL_Results(alulCnt,:) = maxALUL;
                maxALUL_Pos(alulCnt,:) = [allChoices(i,1) allChoices(j,1)];
                alulCnt = alulCnt + 1;

                % Check if this solution is valid
                if maxALUL < ALUL_THRESHOLD
                    AlulFound=1;
                    break;
                end
            end
        end
    else
        temp_pos(1,1) = allChoices(1,1);
        temp_pos(2,1) = allChoices(numChoices+1,1);
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % SET CAMERA'S TO THEIR CALCULATED ANGLES - min alul
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    if(maxALUL_Results(totalChoices) < ALUL_THRESHOLD) % Good result found last time
        our_cam_pos=temp_pos;
    else
        % Find min ALUL and use it
        testNumber = find(maxALUL_Results==min(maxALUL_Results),1);
        for i = 1:numCameras
            our_cam_pos(i,1) = maxALUL_Pos(testNumber, i);
        end
    end
    %our_cam_pos
    
    for j = 1:numCameras 
        xc(j,1)=(ucells(our_cam_pos(j,1),1)-329723)-MX;
        yc(j,1)=(ucells(our_cam_pos(j,1),2)-4321586.5)-MY;
    end
    cell_pos = [num2str(xc(1,1)) ',' num2str(MAX_Y-yc(1,1)) ',' num2str(xc(2,1)) ',' num2str(MAX_Y-yc(2,1)) ',' num2str(posCnt)]
    %cell_pos1 = [num2str(1) ',' num2str(xc(1,1)) ',' num2str(MAX_Y-yc(1,1)) ',' num2str(posCnt)]
    %cell_pos2 = [num2str(2) ',' num2str(xc(2,1)) ',' num2str(MAX_Y-yc(2,1)) ',' num2str(posCnt+1)]
    posCnt = posCnt + 2;
    
    [v,l,s]=isVisible(ourcells,our_cam_pos,our_occlusion_status,our_occlusions);
    %l(:,5) - 1 means totally occluded by all cameras
    %l(:,1:4) - 1 means visible...
    %occlusion_status - 1 means not visible

    %calculate unseen cells for this step
    thisVisible = sum(l(:,1:numCameras),2) > 0; %visible by some camera
    
    %increment all unseen cells by 1
    index = find(~thisVisible & pvisible); %get index of all currently invisble but could be seen cells
    unseen(index,1) = unseen(index,1) + 1;
    
    %zero out all cells that are visible now....
    unseen(find(thisVisible==1),1) = 0;
    unseenReturn(t) = size(find(unseen ~= 0), 1);
    %disp unseen
    unseenReturn(t);
    
    % Get max unseen cell
    maxUnseenReturn(t) = max(unseen);
    %disp maxUnseen
    maxUnseenReturn(t);
    
    %figure(2);hist(unseen');
    %axis([0 steps 0 numCells]);
    %axis([0 50 0 numCells]);
    [alulReturn(t,1),alulReturn(t,2),alulReturn(t,3)]=calculateALUL4ROI(thisVisible,109,73,ROI);
    prevALUL=alulReturn(t, 1);
    alul.setCurrentALUL(alulReturn(t, 1));

    %if t > 1
    %    figure(1);
    %    hold on;
    %    plot([t-1;t],[alulReturn(t-1,1);alulReturn(t,1)],'k-'); % avg
    %    plot([t-1;t],[alulReturn(t-1,2);alulReturn(t,2)],'g'); % min
    %    plot([t-1;t],[alulReturn(t-1,3);alulReturn(t,3)],'r'); % max
    %end
    figure(3);
    %hold off;
    imagesc(f2);
    for j = 1:numCameras
    	plotcamera(ucells(our_cam_pos(j,1),1),ucells(our_cam_pos(j,1),2),3,j,90,180,dov);
    end

   %plot seen cells and starving cells
    plotpoints(ourcells(find(thisVisible==1),1),ourcells(find(thisVisible==1),2),3,'.',[1 1 1]);
    plotpoints(ourcells(find(unseen > maxUnseen == 1),1),ourcells(find(unseen > maxUnseen == 1),2),3,'.',[0 0 0]);

    for tgtCnt=1:numTargets
        if dynamic_t==1
            plotpoints(tgt(tgtCnt).getCurrentX(),tgt(tgtCnt).getCurrentY(),3,'s',rgb(tgtCnt,1:3));
        else
            plotpoints(ourcells(start_targets(tgtCnt),1),ourcells(start_targets(tgtCnt),2),3,'s',rgb(tgtCnt,1:3));
        end
    end
    
    t=t+1;
    %pause(1)
%end
