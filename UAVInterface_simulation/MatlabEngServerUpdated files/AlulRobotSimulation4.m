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
						maxALUL_Pos(alulCnt,:) = [allChoices(i,1) allChoices(j,1) allChoices(k,1) allChoices(m,1)];
						alulCnt = alulCnt + 1;
						maxALUL
						%prevALUL
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

    %check for duplicate positions
    if size((unique(temp_pos,'rows')),1) ~= 4
	mat=temp_pos(:,1);
	[newmat,index] = unique(mat,'rows','first');
	repeatIndex = setdiff(1:size(mat,1),index);
	for i=1:size(repeatIndex,2)
	     a=repeatIndex(1,i);
	     temp_pos(a,1) = allChoices(((a-1)*numChoices)+2,1);
	end
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
    posCnt = posCnt + 4;
    
    %l(:,5) - 1 means totally occluded by all cameras
    %l(:,1:4) - 1 means visible...
    %occlusion_status - 1 means not visible
    [v,l,s]=isVisible(ourcells,our_cam_pos,our_occlusion_status,our_occlusions);
    thisVisible = sum(l(:,1:numCameras),2) > 0; %visible by some camera
    [avgALUL, minALUL, maxALUL]=calculateALUL4ROI(thisVisible,109,73,ROI);
    prevALUL=maxALUL;
    alul.setCurrentALUL(maxALUL);    
    %increment all unseen cells by 1
    index = find(~thisVisible & pvisible); %get index of all currently invisble but could be seen cells
    unseen(index,1) = unseen(index,1) + 1;
    unseenSum = sum(unseen);
    unseenMax = max(unseen);
    mdata = [num2str(maxALUL) ',' num2str(unseenMax) ',' num2str(unseenSum)]
    
    %zero out all cells that are visible now....
    unseen(find(thisVisible==1),1) = 0;  

    figure(3);
    imagesc(f2);
    for j = 1:numCameras
    	plotcamera(ucells(our_cam_pos(j,1),1),ucells(our_cam_pos(j,1),2),3,j,90,180,dov);
    end

   %plot seen cells and starving cells
    plotpoints(ourcells(find(thisVisible==1),1),ourcells(find(thisVisible==1),2),3,'.',[1 1 1]);
    plotpoints(ourcells(find(unseen > maxUnseen == 1),1),ourcells(find(unseen > maxUnseen == 1),2),3,'.',[0 0 0]);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % FIND BOUNDARY CELLS 
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    j=1; k=1;
    hull=[];
    for i=28:-1:1
        for l=1:26
           if unseen(j,1) > 3%maxUnseen
                hull(i,l) = 1;
                j = j + 1; k = k+1;
           else
                hull(i,l) = 0;
                j = j + 1;
           end
        end
    end 
    %hull
    
    st=1;
    boundary=[];
    allboundary = [];
    [B,L,N,A] = bwboundaries(hull);
    figure(1); imshow(hull); hold on;
    for k=1:length(B),
        if(~sum(A(k,:)))
            boundary = B{k};
            plot(boundary(:,2),boundary(:,1),'r','LineWidth',2);
            sz=size(boundary,1);
            allboundary(st:st+sz-1,:) = boundary;
            allboundary(st+sz,1) = -1; allboundary(st+sz,2) = -1;
            st=st+sz+1;        
            for l=find(A(:,k))'
                boundary = B{l};
                plot(boundary(:,2),boundary(:,1),'g','LineWidth',2);
                sz=size(boundary,1);
                allboundary(st,1) = -2; allboundary(st,2) = -2;
                allboundary(st+1:st+1+sz-1,:) = boundary;
                allboundary(st+1+sz,1) = -1; allboundary(st+1+sz,2) = -1;
                st=st+1+sz+1; 
            end
            %boundary   
        end
    end
    %allboundary
    
    %first check
    if size(allboundary,1)>0
        temp = 0; cnt = 2;
        lastx = 0; lasty = 0;
        outer = [];
        outer(1,1) = allboundary(1,1);
        outer(1,2) = allboundary(1,2);
	for i=1:size(allboundary,1)
            x = allboundary(i,1);
            y = allboundary(i,2);
            if (x == -1 && y == -1) && i<size(allboundary,1)
                outer(cnt,1) = lastx;
                outer(cnt,2) = lasty;
                outer(cnt+1,1) = x;
                outer(cnt+1,2) = y; 
                if allboundary(i+1,1)==-2 && allboundary(i+1,2)==-2
                    outer(cnt+2,1) = -2;
                    outer(cnt+2,2) = -2;
                    outer(cnt+3,1) = allboundary(i+2,1);
                    outer(cnt+3,2) = allboundary(i+2,2);
                    cnt=cnt+4;
                else
                    outer(cnt+2,1) = allboundary(i+1,1);
                    outer(cnt+2,2) = allboundary(i+1,2);
                    cnt=cnt+3;
                end
                temp = 0; lastx = 0; lasty = 0;       
            elseif (x == -2 && y == -2)
               	continue
            elseif x == lastx 
                lastx=x; lasty=y;
            else %x ~= lastx 
                if temp == 0 
                    temp = 1;
                    lastx=x; lasty=y;
                else
                    if outer(cnt-1,1) == lastx && outer(cnt-1,2) == lasty
                        outer(cnt-1,:) = [];
                        cnt = cnt-1;
                    end
                    outer(cnt,1) = lastx;
                    outer(cnt,2) = lasty;
                    outer(cnt+1,1) = x;
                    outer(cnt+1,2) = y;
                    lastx=x; lasty=y;
                    cnt=cnt+2; 
                end
            end
        end

        %second check
	for i=1:size(outer,1)
            x = outer(i,1);
            y = outer(i,2);
            if (x == -1 && y == -1) && i<size(outer,1)
                outer2(cnt,1) = lastx;
                outer2(cnt,2) = lasty; 
                outer2(cnt+1,1) = x;
                outer2(cnt+1,2) = y; 
                if outer(i+1,1)==-2 && outer(i+1,2)==-2
                    outer2(cnt+2,1) = -2;
                    outer2(cnt+2,2) = -2;
                    outer2(cnt+3,1) = outer(i+2,1);
                    outer2(cnt+3,2) = outer(i+2,2);
                    cnt=cnt+4;
                else
                    outer2(cnt+2,1) = outer(i+1,1);
                    outer2(cnt+2,2) = outer(i+1,2);
                    cnt=cnt+3;
                end
                temp = 0; lastx = 0; lasty = 0;  
         	elseif (x == -2 && y == -2)
               	continue
            elseif  y == lasty
                lastx=x; lasty=y;
            else 
                if temp == 0 
                    temp = 1;
                    lastx=x; lasty=y;
                else
                    if outer2(cnt-1,1) == lastx && outer2(cnt-1,2) == lasty
                        outer2(cnt-1,:) = [];
                        cnt = cnt-1;
                    end
                    outer2(cnt,1) = lastx;
                    outer2(cnt,2) = lasty;
                    outer2(cnt+1,1) = x;
                    outer2(cnt+1,2) = y;
                    lastx=x; lasty=y;
                    cnt=cnt+2;
                end
            end
        end
        %outer
        %outer2  
         
        j=1;
        bcells=[];
        for i=1:size(outer2,1)
            if outer2(i,1) >= 0 && outer2(i,2) >= 0 
                a=(outer2(i,1)-1) * 26 + outer2(i,2);
                bcells(j,1)=(ourcells(a,1)-329723)-MX;
                bcells(j,2)=(ourcells(a,2)-4321586.5)-MY;
                j=j+1;
            elseif outer2(i,1) == -2 && outer2(i,2) == -2
                bcells(j,1)=-2;
                bcells(j,2)=-2;
                j=j+1;
            else 
                bcells(j,1)=-1;
                bcells(j,2)=-1;
                j=j+1;
            end
        end
        bcells
    end

    %t=t+1;

%end
