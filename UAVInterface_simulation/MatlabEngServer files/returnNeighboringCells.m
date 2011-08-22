function [cells,r,c]=returnNeighboringCells(cell,rows,columns,ucells)

% this calculation gives the correct neighboring cell numbers but the c
% needs to be incremented to have an indexable r and c value
% this is because values start at 1, not zero

    c = int16(floor(cell/rows));
    r = mod(cell,rows);
    
    cells=[];
    if (c > 1 && abs(ucells(cell,3)-ucells(((c-1)*rows)+r,3)) < 2) cells=[cells; ((c-1)*rows)+r];end %sub col
    if (c < columns-1 && abs(ucells(cell,3)-ucells(((c+1)*rows)+r,3)) < 2) cells=[cells; ((c+1)*rows)+r];end % add col
    if (r > 1 && abs(ucells(cell,3)-ucells((c*rows)+r-1,3)) < 2) cells=[cells; (c*rows)+r-1];end %subtract row
    if (r < rows-1 && abs(ucells(cell,3)-ucells((c*rows)+r+1,3)) < 2) cells=[cells; (c*rows)+r+1];end % add row
    cells=double(cells);
end