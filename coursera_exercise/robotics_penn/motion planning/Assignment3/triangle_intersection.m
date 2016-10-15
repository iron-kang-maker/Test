function flag = triangle_intersection(P1, P2)
% triangle_test : returns true if the triangles overlap and false otherwise

%%% All of your code should be between the two lines of stars.
% *******************************************************************
n = size(P1, 1);

flag = true;

for k = 1:n
    p1 = P1(k,:);
    p2 = P1(mod(k,3)+1,:);
    p = P1(mod(k+1,3)+1, :);
    
    sigP1 = 0;
    sigP2 = 0;
    
    if line_function(p1, p2, p) > 0
        sigP1 = 1;
    else
        sigP1 = -1;
    end
    
    for l = 1:n
        p = P2(l,:);
        if line_function(p1, p2, p) > 0
            sigP2 = sigP2 + 1;
        else
            sigP2 = sigP2 -1;
        end
    end
    
    if (sigP1*sigP2) == -3
        flag = false;
        return;
    end
    
end

for k = 1:n
    p1 = P2(k,:);
    p2 = P2(mod(k,3)+1,:);
    p = P2(mod(k+1,3)+1, :);
    
    sigP1 = 0;
    sigP2 = 0;
    
    if line_function(p1, p2, p) > 0
        sigP1 = 1;
    else
        sigP1 = -1;
    end
    
    for l = 1:n
        p = P1(l,:);
        if line_function(p1, p2, p) > 0
            sigP2 = sigP2 + 1;
        else
            sigP2 = sigP2 -1;
        end
    end
    
    if (sigP1*sigP2) == -3
        flag = false;
        return;
    end
    
end

% *******************************************************************
end