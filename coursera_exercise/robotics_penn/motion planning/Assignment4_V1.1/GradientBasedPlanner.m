function route = GradientBasedPlanner (f, start_coords, end_coords, max_its)
% GradientBasedPlanner : This function plans a path through a 2D
% environment from a start to a destination based on the gradient of the
% function f which is passed in as a 2D array. The two arguments
% start_coords and end_coords denote the coordinates of the start and end
% positions respectively in the array while max_its indicates an upper
% bound on the number of iterations that the system can use before giving
% up.
% The output, route, is an array with 2 columns and n rows where the rows
% correspond to the coordinates of the robot as it moves along the route.
% The first column corresponds to the x coordinate and the second to the y coordinate

[gx, gy] = gradient (-f);

%%% All of your code should be between the two lines of stars.
% *******************************************************************
route = start_coords;
pos = start_coords;
fprintf('iter: %d, start(%d, %d), end(%d, %d)\n', max_its, start_coords(1), start_coords(2), end_coords(1), end_coords(2));
%gx = gx./abs(gx);
%gy = gy./abs(gy);
%gx = gx/abs(max(gx(:)));
%gy = gy/abs(max(gy(:)));
for i=1:max_its
    %Interpolation
    %1D: f(n+a) = (1-a)*f(n) + a*f(n+1)
    %
    %                   f(n+1)
    %   f(n)   f(n+a)     |
    %    |       |        |
    %--------------------------
    %    n      n+a      n+1
    %================================================
    %2D: b3 = (1-a)*[(1-b)*a1 + b*a2] + a*[(1-b)*a3 + b*a4]]
    %
    %   a1 ------------a2
    %    |       |
    %    |       a
    %    |       |
    %    |---b---b3
    %    |
    %    |
    %   a3-------------a4
    gx_a1 = gx(floor(pos(2)), floor(pos(1)));
    gx_a2 = gx(floor(pos(2)), ceil(pos(1)));
    gx_a3 = gx(ceil(pos(2)), floor(pos(1)));
    gx_a4 = gx(ceil(pos(2)), ceil(pos(1)));
    b = pos(1) - floor(pos(1));
    a = pos(2) - floor(pos(2));
    gx_b3 = (1 - a)*((1 - b)*gx_a1 + b*gx_a2) + a*((1 - b)*gx_a3 + b*gx_a4);
    
    gy_a1 = gy(floor(pos(2)), floor(pos(1)));
    gy_a2 = gy(floor(pos(2)), ceil(pos(1)));
    gy_a3 = gy(ceil(pos(2)), floor(pos(1)));
    gy_a4 = gy(ceil(pos(2)), ceil(pos(1)));
    gy_b3 = (1 - a)*((1 - b)*gy_a1 + b*gy_a2) + a*((1 - b)*gy_a3 + b*gy_a4);

    %v = [gx_b3 gy_b3];
    v = [gx(round(pos(2)), round(pos(1))) gy(round(pos(2)), round(pos(1)))];
    v = v/norm(v);
    %v = [v(1)/norm(gx) v(2)/norm(gy)];
    pos = pos + v;
    %if (abs(round(pos(1)) - route(end, 1)) == 1) && (abs(round(pos(2)) - route(end, 2)) == 1) 
    %    fprintf('diagnoal, curr(%d, %d) pre(%d, %d)\n', round(pos(1)), round(pos(2)), route(end, 1), route(end, 2));
    %    %route = [route; round(pos(1)) route(end, 2)];
    %elseif (abs(round(pos(1)) - route(end, 1)) == 0) && (abs(round(pos(2)) - route(end, 2)) == 0) 
    %    fprintf('stop\n');
    %    %continue;
    %end
    
    fprintf('%d: now (%f, %f), prev (%f, %f), error: %f\n', i, pos(1), pos(2), route(end, 1), route(end, 2), norm(pos-route(end,:))); 
    route = [route; pos];
    
    
    if norm(pos - end_coords) < 2
        break
    end
end

fprintf('iter: %d, start(%f, %f), end(%f, %f), size: %d\n', i, route(1,1), route(1,2), route(end, 1), route(end, 2), size(route, 1));
%route
% *******************************************************************
end
