function cell = returnCell(ucells,x,y)
% returnCell - get index of cell containing coordinates x,y
%
% Syntax:   cell = returnCell(ucells,x,y)
%
% Inputs:
%    input1 - list of cell coordinates and cell heights
%    x,y    - Point in cell
%
% Outputs:
%    cell - Index of cell in ucells
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

[val,cell]=min(abs(ucells(:,1)-x) + abs(ucells(:,2)-y));

