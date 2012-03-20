function [visible, list, counts] = isVisible(cells,cameras,occlusion_status,occlusions)
%isVisible - determines if cells are visible given camera angles
%
% Syntax:  [visible, list, counts] = isVisible(cells,cameras, occlusion_status,occlusions)
%
% Inputs:
%    cells              - list of cell coordinates and cell heights                        
%    cameras            - camera positions and parameters
%    occlusion_status   - list of cell visibility to each camera
%                           (created in recordOcclusions)
%
% Outputs:
%    visible            - visibility of each cell for the supplied config
%    lists              - visibility of each cell by camera
%    counts             - stats on coverage
%
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none
%
% See also: demoRunALUL, recordOcclusions, calcAngles, ALULSimulation

 
% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% August 2006; Last revision: 14 Aug 2006


%cameras - 1-cell position and 2-dov

%occlusion_status - cells x cameras - 0 = not occluded, 1 = occluded 

%visible - cells x status - where status is 0 = not visible, 1 = visible

ncells=size(cells,1);
ncameras=size(cameras,1);
    
%create structure to track visibility
visible=false(ncells,1); % initialize all cells to not visible
pvisible=~ occlusion_status(:,1:ncameras);

for x=1:ncameras
    pos_cell=cameras(x,1);
    dov=cameras(x,2);
    %get the visibility and distance for this cell
    viscell=occlusions(:,pos_cell);
    %find all cells that are less than dov
    list(:,x) = pvisible(:,x) & viscell < dov;
end 

visible=max(list(:,1:ncameras),[],2) > 0;
list(:,x+1)=(sum(occlusion_status(:,1:ncameras),2) ==ncameras) ;
nvisible=sum(list,1);
allvisible=sum(max(list(:,1:ncameras),[],2));
counts=[nvisible allvisible sum(sum(list(:,1:ncameras),2) > 1)];

end