function [x,y]=plotpoints(x,y,fig,options,color)
%plotpoints - plot points on image given in x and y on figure fig using color (optional)
%
% Syntax:  [x,y]=plotpoints(x,y,fig,options,color)
%
% Inputs:
%    x,y        - Coordinates of points to be printed
%    fig        - Existing figure for newly plotted points
%    options    - Plot options
%    color      - Optional color paramter
%
% Outputs:
%    x,y        - image coordinates of x and y points
%
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
if nargin <5
    color=[];
end
%need to xfer all points to figure coordinates
x=(x-329703).*1.05;
y=501-(y-4321520).*(1/1.2);
if ~isempty(color) 
    figure(fig);hold on; plot(x,y,options,'color',color);hold off;
else
    figure(fig);hold on; plot(x,y,options);hold off;
end