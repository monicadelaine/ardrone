function [alul,minalul,maxalul] = calculateALUL4ROI(thisVisible,rows,cols,ROI)
%calculateALUL4ROI - calculate ALUL for the cells in the ROI
%
% Syntax:  [alul,minalul,maxalul] = calculateALUL4ROI(thisVisible,rows,cols,ROI)
%
% Inputs:
%    thisVisible    - list instanteous visibility for cells listed in ROI
%    rows           - number of rows (thisVisible is expected to be square or rect) 
%    cols           - number of cols (thisVisible is expected to be square or rect) 
%    ROI            - cell indexes for region of interest
%
% Outputs:
%    alul           - average linear uncovered length
%    minalul        - minimum linear uncovered length
%    maxalul        - maximum linear uncovered length
%
%
% Other m-files required: none
% Subfunctions: previousX, previousY, nextX, and nextY
% MAT-files required: none
%
% See also: demoRunALUL,  ALULSimulation

 
% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% August 2006; Last revision: 14 Aug 2006

    numCells = size(thisVisible,1);
       % size(x,n) returns size of nth dim of x
    minalul=numCells;
    maxalul=0;

    holes=[];
    if size(thisVisible,1) ~= size(ROI,1)
        disp 'problem with visible and ROI not matching'
        alul=-1;
        return % bad input args, so quit early
    end
    if nargin <=3 % num args input
        ROI=1:numCells;
    end

    %type_corner=1; not used
    %type_edge=2; not used
    %type_center=3; not used
    alul=0;

    %seenCells=[]; not used until later

    %holeIdx=1; not used until later
    thisHole=[];
    for in=1:numCells %foreach cell in ROI, calculate thisHole
        %{
            thisHole has three values
                idx is number of adjacent cells in ROI and visible
                adj is index into ROI for visible adjacent cells in ROI
                edges is number of edges touching cells not visible or not
                    in ROI
        %}
        cellIdx=0;
        i=ROI(in);
        if thisVisible(in)==1 %if cell is visible...
            thisHole{in}.idx=cellIdx; %set thisHole to visible
            thisHole{in}.adj=[];
            thisHole{in}.edges=0;
            continue; % go on to next cell calculation
        end
        pX=previousX(i,rows,cols); % get surrounding 4 cells
        pY=previousY(i,rows,cols);
        nX=nextX(i,rows,cols);
        nY=nextY(i,rows,cols);
        [tf,loc]=ismember(pX,ROI); % [is pX in ROI, loc of pX in ROI or 0]
        if pX ~=0 && tf && thisVisible(loc) == 0 %if px in ROI and visible
            cellIdx=cellIdx+1;
            thisHole{in}.adj(cellIdx)=loc;
        end
        [tf,loc]=ismember(pY,ROI); % same as above with pY
        if pY ~=0  && tf && thisVisible(loc) == 0
            cellIdx=cellIdx+1;
            thisHole{in}.adj(cellIdx)=loc;
        end
        [tf,loc]=ismember(nX,ROI); % same as above with nX
        if nX ~=0  && tf && thisVisible(loc) == 0 
           cellIdx=cellIdx+1;
           thisHole{in}.adj(cellIdx)=loc;
        end
        [tf,loc]=ismember(nY,ROI); % same as above with nY
        if nY ~=0  && tf && thisVisible(loc) == 0 
            cellIdx=cellIdx+1;
            thisHole{in}.adj(cellIdx)=loc;
        end
        if cellIdx==0 % if no contiguous cells are in ROI and visible
            thisHole{in}.idx=1;
            thisHole{in}.edges=4;
            thisHole{in}.adj(1)=in;
        else
            thisHole{in}.idx=cellIdx;
            thisHole{in}.edges=4-cellIdx;
        end

    end

    % now I just need to figure out who is connected to whom
    holeIdx=0;
    seenCells=[];
    i=0;
%    dbstop at 56
    while i < numCells
        i=i+1;
        seenCells=unique(seenCells); % remove all duplicates
        % find next visible cell in ROI that has not been seen
        while (thisHole{i}.idx == 0 || ismember(i,seenCells)) && i < numCells
            i=i+1;
        end
        if (i < numCells)
            seenCells=[seenCells i]; % add this cell to seen
            i; %output i?
            queue=thisHole{i}.adj; % look at all adjacent cells in ROI
            holeIdx=holeIdx+1;
            holes{holeIdx}.cells=thisHole{i}.adj;

            while size(queue,2) >0
                %process queue
                %get first cell from queue and check for cells to add to queue
                %and seenCells
                idx=queue(1);queue(1)=[];
                if thisHole{idx}.idx >0
                    seenCells=[seenCells idx];
                    queue = [queue thisHole{idx}.adj];
                    holes{holeIdx}.cells=[holes{holeIdx}.cells thisHole{idx}.adj];
                    [c,ia,ib]=intersect(queue,seenCells);
                    queue(ia)=[];
                end
            end
            holes{holeIdx}.cells=unique(holes{holeIdx}.cells);
            holes{holeIdx}.count = size(holes{holeIdx}.cells,2);
            holes{holeIdx}.idx = ROI(holes{holeIdx}.cells');
            holes{holeIdx}.alul = 0;
            perimeter=0;
            for idx=holes{holeIdx}.cells
                perimeter=perimeter+thisHole{idx}.edges;
            end 
            holes{holeIdx}.alul=pi.*holes{holeIdx}.count./perimeter;
            alul = alul + holes{holeIdx}.alul;
            if minalul > holes{holeIdx}.alul
                minalul=holes{holeIdx}.alul;
            end
            if maxalul < holes{holeIdx}.alul
                maxalul=holes{holeIdx}.alul;
            end
            holes{holeIdx}.perimeter = perimeter;
        end
    end
    
    alul; % semi-colon masks print, so what are these for?
    alul=alul./holeIdx; % elem-by-elem division B./C => B[1,1]/C[1,1]
    alul;
    minalul;
    maxalul;
    
function previous = previousX(index,rows,cols)
    previous=index;
    previous=previous-1;
    if previous < 0
        previous=0;
    end

function previous = previousY(index,rows,cols)
    previous=index;
    previous=previous-cols;
    if previous < 0
        previous=0;
    end

function next = nextX(index,rows,cols)
    next=index;
    next=next+1;
    if next > rows*cols
        next=0;
    end

function next = nextY(index,rows,cols)
    next=index;
    next=next+cols;
    if next > rows*cols
        next=0;
    end

