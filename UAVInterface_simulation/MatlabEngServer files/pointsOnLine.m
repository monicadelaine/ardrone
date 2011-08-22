function [x,y,z] = pointsOnLine(orgX,orgY,orgZ,destX,destY,destZ)
%FUNCTION_NAME - %iterate over the cells (samples between two points)
%
% Syntax:  [x,y,z] = pointsOnLine (1,2,3,6,7,8)
%
% Inputs:
%    orgX,orgY,orgZ         - 3D coordinates of point of origin
%    destX,destY,destZ      - 3D coordinates of destination point
%
% Outputs:
%    x,y,z                  - List of points 
%
% See also: recordOcclusions

 
% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% December 1999; Last revision: 12-May-2004



at=-orgX+destX;
bt=-orgY+destY;
ct=-orgZ+destZ;
dist=sqrt(at^2+bt^2+ct^2);

%dist=dist/100;%reduce resolution for timing reasons

at=at/dist;
bt=bt/dist;
ct=ct/dist;

t=1:dist;

x=[];
y=[];
z=[];

x(1,:)=(at.*t)+orgX;
y(1,:)=(bt.*t)+orgY;
z(1,:)=(ct.*t)+orgZ;
  
z(find(z(:)<0))=0; %make sure z can't be zero

