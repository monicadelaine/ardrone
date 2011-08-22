function [x,y]=getCoords(xp,yp)
%FUNCTION_NAME - translates image coordinates to cell coordinates
%
% Syntax:  [x,y]=getCoords(xp,yp)
% Inputs:
%    xp,yp       - image coordinates
%
% Outputs:
%    x,y         - cell coordinates
%
%
% See also: selectROI, ALULSimulation
 
% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% December 1999; Last revision: 12-May-2004
    x=xp./1.05+329703;
    y=((501-yp).*1.2)+4321520;
end