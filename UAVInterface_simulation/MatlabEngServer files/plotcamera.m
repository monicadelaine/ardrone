function plotcamera(x,y,fig,cam,angle,fov,dov)
%plotcamera - plots lines that represent the cameras FOV
%
% Syntax:  plotcamera(x,y,fig,cam,angle,fov)
%
% Inputs:
%    x,y        - Coordinates of points to be printed
%    fig        - Existing figure for plotted points
%    cam        - Plot options
%    fov        - Camera field of view
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none
%
% See also: ALULSimulation, plot

 
% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% August 2006; Last revision: 14 Aug 2006

optionsC=[1 .5 .5;.5 1 .5;.4 .4 .8;.2 .4 .4;0 0 0];

%need to determine where the lines are
fov_2 = fov/2;
bangle=angle-fov_2;
eangle=angle+fov_2;
if (bangle >180) bangle=360-bangle;end
if (eangle <-180) eangle=360+eangle;end
%normalized...between -180 and 180
dist=dov;%in meters

%need x and y of point dist meters out
xb=x+cosd(bangle)*dist;
yb=y+sind(bangle)*dist;
xe=x+cosd(eangle)*dist;
ye=y+sind(eangle)*dist;

%need to xfer all points to figure coordinates
x=([x]-329703).*1.05;
xb=([xb]-329703).*1.05;
xe=([xe]-329703).*1.05;
y=501-([y]-4321520).*(1/1.2);
yb=501-([yb]-4321520).*(1/1.2);
ye=501-([ye]-4321520).*(1/1.2);
figure(fig);hold on; plot([x;xb],[y;yb],'color',optionsC(cam,:),'linewidth',2);plot([x;xe],[y;ye],'color',optionsC(cam,:),'linewidth',2);hold off;end