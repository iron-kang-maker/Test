function [ A ] = diff_matrix(ta1, ta2)

A = [     ta2^7     ta2^6     ta2^5    ta2^4   ta2^3  ta2^2 ta2 1;
          ta1^7     ta1^6     ta1^5    ta1^4   ta1^3  ta1^2 ta1 1;      
        7*ta1^6   6*ta1^5   5*ta1^4  4*ta1^3 3*ta1^2  2*ta1  1  0;
       42*ta1^5  30*ta1^4  20*ta1^3 12*ta1^2 6*ta1    2      0  0;
      210*ta1^4 120*ta1^3  60*ta1^2 24*ta1     6      0      0  0;
      840*ta1^3 360*ta1^2 120*ta1   24         0      0      0  0;
     2520*ta1^2 720*ta1   120        0         0      0      0  0;
     5040*ta1   720         0        0         0      0      0  0];
    
end