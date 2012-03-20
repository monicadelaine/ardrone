function [h]= getHeight(cells,x,y)


[r,c]=size(x);
for i=1:r
    idx=returnCell(cells,x(i),y(i));
    h(i)=cells(idx,3);
end

h=h';