function flag = triangle_intersection(P1, P2)
% triangle_test : returns true if the triangles overlap and false otherwise

%%% All of your code should be between the two lines of stars.
% *******************************************************************
n = size(P1, 1);

flag = true;

for k = 1:n
    A = [P1(k,1) 1; P1(mod(k,3)+1,1) 1];
    b = [P1(k,2); P1(mod(k,3)+1,2)];
    
    sigP1 = 0;
    sigP2 = 0;
    
    rk = rank(A);
    if (rk ~= 1)
        m = A\b;
    
        p1 = [P1(mod(k+1, 3)+1, 1) P1(mod(k+1, 3)+1, 2)];
    
        
        if p1(2) < (m(1)*p1(1) + m(2))
            sigP1 = sigP1 - 1;
        else
            sigP1 = sigP1 + 1;
        end
    
        
        for l = 1:n
            if P2(l,2) > (m(1)*P2(l,1) + m(2))
                sigP2 = sigP2 + 1;
            elseif P2(l,2) < (m(1)*P2(l,1) + m(2))
                sigP2 = sigP2 - 1;
            end
        end
    else
        p1 = P1(mod(k+1, 3)+1, 1);
        if p1 > P1(k,1)
            sigP1 = 1;
        else
            sigP1 = -1;
        end
        
        for l = 1:n
            if P2(l,1) > P1(k,1)
                sigP2 = sigP2 + 1;
            elseif P2(l,1) < P1(k,1)
                sigP2 = sigP2 - 1;
            end
        end
    end
    
    if (sigP1*sigP2) == -3
        flag = false;
        return;
    end
    
end

for k = 1:n
    A = [P2(k,1) 2; P2(mod(k,3)+1,1) 1];
    b = [P2(k,2); P2(mod(k,3)+1,2)];
    
    sigP1 = 0;
    sigP2 = 0;
    
    rk = rank(A);
    if (rk ~= 1)
        m = A\b;
    
        p2 = [P2(mod(k+1, 3)+1, 1) P2(mod(k+1, 3)+1, 2)];
    
        
        if p2(2) < (m(1)*p2(1) + m(2))
            sigP2 = sigP2 - 1;
        else
            sigP2 = sigP2 + 1;
        end
    
        
        for l = 1:n
            if P1(l,2) > (m(1)*P1(l,1) + m(2))
                sigP1 = sigP1 + 1;
            elseif P1(l,2) < (m(1)*P1(l,1) + m(2))
                sigP1 = sigP1 - 1;
            end
        end
    else
        p2 = P2(mod(k+1, 3)+1, 1);
        if p2 > P2(k,1)
            sigP2 = 1;
        else
            sigP2 = -1;
        end
        
        for l = 1:n
            if P1(l,1) > P2(k,1)
                sigP1 = sigP1 + 1;
            elseif P1(l,1) < P2(k,1)
                sigP1 = sigP1 - 1;
            end
        end
    end
    
    if (sigP1*sigP2) == -3
        flag = false;
        return;
    end
    
end

% *******************************************************************
end