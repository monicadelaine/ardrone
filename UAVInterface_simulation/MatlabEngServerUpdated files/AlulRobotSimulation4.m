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
%    cameras            - camera positions and parameters
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
% See also: demoRunALUL, calculateALUL4ROI, returnCell,
%  plotcamera, plotpoints
% 

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

    rows=73;
    columns=109;
    
    posCnt = 2; 
        
    MX = 130;
    MY = 190;
    MAX_Y = 175;

    voteunseen=[];

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

    sim_start = 1;
end

    our_cam_pos = cameras;
    temp_pos=our_cam_pos;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % GET ACTIONS TO SEE A CELL 
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %disp 'get actions to see a cell'
    choices = [];
    allChoicesVisible = [];
    allChoicesUnseen = [];
    choiceCnt = 1;
    cnt=1;
                 
    % Determine which cells can be seen by each camera action
    for c = 1:numCameras   
        bestChoice = 1; 
        totalunseen = 0; 
        sortCnt = 1;
        orderedChoices = [];
        allChoices = [];
        choices = [choices; returnNeighboringCells(our_cam_pos(c,1),rows,columns,ucells)]; 
    
        for k = 1:numChoices
            temp_pos(c,1) = choices(choiceCnt,1);
            [v,l,s] = isVisible(ourcells,temp_pos,our_occlusion_status,our_occlusions);

            votevisible(:,choiceCnt) = l(:,c);
            totalvotesvisible = sum(votevisible(:,choiceCnt));
            allChoicesVisible = [allChoicesVisible; c choiceCnt totalvotesvisible choices(choiceCnt,1)];
          
            %unseen(choices(choiceCnt,1),1)   
            voteunseen(:,choiceCnt) = l(:,c) .*(2.^ unseen );
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
    
    %temp_pos
    %choices
    %allChoicesUnseen
    sortedAll=sortrows(allChoicesUnseen,-3);

    % Set camera choices  
    for j = 1:numCameras    
        for k = 1:numChoices*numCameras
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
    %disp 'ensure alul threshold is satisfied'
    totalChoices = 96; %numChoices*numChoices;  
    maxALUL_Results = zeros(totalChoices,1);
    maxALUL_Pos = zeros(totalChoices,numCameras);
    alulCnt = 1;
    AlulFound = 0;
        
    if alulFlag == 1
        for i = 1:numChoices
            for j = numChoices+1:numChoices*2
                for k = numChoices*2+1:numChoices*3 
					for m = numChoices*3+1:numChoices*4 %%
						if AlulFound==1        
							break;
						end
						temp_pos(1,1) = allChoices(i,1);
						temp_pos(2,1) = allChoices(j,1);    
						temp_pos(3,1) = allChoices(k,1); 
						temp_pos(4,1) = allChoices(m,1); 
						[v,l,s]=isVisible(ourcells,temp_pos,our_occlusion_status,our_occlusions);
						thisVisible = sum(l(:,1:numCameras),2) > 0; %visible by some camera
						[avgALUL, minALUL, maxALUL]=calculateALUL4ROI(thisVisible,109,73,ROI);
						maxALUL_Results(alulCnt,:) = maxALUL;
						maxALUL_Pos(alulCnt,:) = [allChoices(i,1) allChoices(j,1) allChoices(k,1) allChoices(m,1)]
						alulCnt = alulCnt + 1;
						maxALUL
						prevALUL
						% Check if this solution is valid
						if (maxALUL < ALUL_THRESHOLD) || (maxALUL <= prevALUL)
							AlulFound=1;
							break;
						end
					end
                end
            end
        end
    else
        temp_pos(1,1) = allChoices(1,1);
        temp_pos(2,1) = allChoices(numChoices+1,1);
        temp_pos(3,1) = allChoices(numChoices*2+1,1);
        temp_pos(4,1) = allChoices(numChoices*3+1,1);
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % SET CAMERAS TO THEIR CALCULATED MOVEMENT - min alul
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %disp 'set camera to calculated movement'
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
    cell_pos = [num2str(xc(1,1)) ',' num2str(MAX_Y-yc(1,1)) ',' num2str(xc(2,1)) ',' num2str(MAX_Y-yc(2,1)) ',' num2str(xc(3,1)) ',' num2str(MAX_Y-yc(3,1)) ',' num2str(xc(4,1)) ',' num2str(MAX_Y-yc(4,1)) ',' num2str(posCnt)]
    %cell_pos1 = [num2str(1) ',' num2str(xc(1,1)) ',' num2str(MAX_Y-yc(1,1)) ',' num2str(posCnt)]
    %cell_pos2 = [num2str(2) ',' num2str(xc(2,1)) ',' num2str(MAX_Y-yc(2,1)) ',' num2str(posCnt+1)]
    %cell_pos3 = [num2str(3) ',' num2str(xc(3,1)) ',' num2str(MAX_Y-yc(3,1)) ',' num2str(posCnt+2)]
    %cell_pos4 = [num2str(4) ',' num2str(xc(4,1)) ',' num2str(MAX_Y-yc(4,1)) ',' num2str(posCnt+3)]
    posCnt = posCnt + 4;
    
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
    prevALUL=alulReturn(t, 3);
    alul.setCurrentALUL(alulReturn(t, 3));

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
    
    t=t+1;

    %pause(1)
%end
