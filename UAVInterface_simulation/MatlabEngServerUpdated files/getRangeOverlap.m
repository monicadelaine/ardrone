function [newmiddle, newrange] = getRangeOverlap(middleA,middleB, range)
%getRangeOverlap - Determine if there is overlap between two camera FOVs
%
% Syntax:  [newmiddle, newrange] = getRangeOverlap(middleA,middleB, range)
%
% Inputs:
%    middleA    - middle of range A in degrees
%    middleB    - middle of range B in degrees
%    range      - fov of view for A (B is assumed to be 45)
%
% Outputs:
%    newmiddle  - Middle of overlap region
%    newrange   - Range of overlap region in degrees
%
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none
%
% See also: ALULSimulation

 
% Author: Monica Anderson
% University of Minnesota, Computer Science
% email address: mlapoint@cs.umn.edu  
% Website: http://www-users.cs.umn.edu/~mlapoint
% August 2006; Last revision: 14 Aug 2006

if isempty(middleB)
    newrange=range;
    newmiddle=middleA;
    return
end

if abs(middleA-middleB) > (range*2)
    newrange=0;
    newmiddle=[];
    return
end

minA=middleA-range;
maxA=middleA+range;

minB=middleB-22.5;
maxB=middleB+22.5;

if minA < minB
    minAngle = minB;
else
    minAngle = minA;
end

if maxA > maxB
    maxAngle = maxB;
else
    maxAngle = maxA;
end

newrange=(maxAngle-minAngle)./2;
newmiddle=(maxAngle+minAngle)./2;

