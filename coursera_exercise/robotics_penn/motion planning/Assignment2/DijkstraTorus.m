function route = DijkstraTorus (input_map, start_coords, dest_coords)
% Run Dijkstra's algorithm on a grid.
% Inputs : 
%   input_map : a logical array where the freespace cells are false or 0 and
%      the obstacles are true or 1
%   start_coords and dest_coords : Coordinates of the start and end cell
%       respectively, the first entry is the row and the second the column.
% Output :
%   route : An array containing the linear indices of the cells along the
%    shortest route from start to dest or an empty array if there is no
%    route.

% set up color map for display
% 1 - white - clear cell
% 2 - black - obstacle
% 3 - red = visited
% 4 - blue  - on list
% 5 - green - start
% 6 - yellow - destination

cmap = [1 1 1; ...
        0 0 0; ...
        1 0 0; ...
        0 0 1; ...
        0 1 0; ...
        1 1 0];

colormap(cmap);


[nrows, ncols] = size(input_map);

% map - a table that keeps track of the state of each grid cell
map = zeros(nrows,ncols);

map(~input_map) = 1;  % Mark free cells
map(input_map)  = 2;  % Mark obstacle cells

% Generate linear indices of start and dest nodes
start_node = sub2ind(size(map), start_coords(1), start_coords(2));
dest_node  = sub2ind(size(map), dest_coords(1),  dest_coords(2));

map(start_node) = 5;
map(dest_node)  = 6;

% Initialize distance array
distances = Inf(nrows,ncols);

% For each grid cell this array holds the index of its parent
parent = zeros(nrows,ncols);

distances(start_node) = 0;
fd = fopen('log.txt', 'w');
% Main Loop
while true
    
    % Draw current map
    map(start_node) = 5;
    map(dest_node) = 6;
    
    image(1.5, 1.5, map);
    grid on;
    axis image;
    drawnow;
    
    % Find the node with the minimum distance
    [min_dist, current] = min(distances(:));
    
    if ((current == dest_node) || isinf(min_dist))
        break;
    end;
    
    % Update map
    map(current) = 3;         % mark current node as visited
    distances(current) = Inf; % remove this node from further consideration
    
    % Compute row, column coordinates of current node
    [i, j] = ind2sub(size(distances), current);
    fprintf(fd, '(%d, %d)\n', i, j);
    
    % Visit each neighbor of the current node and update the map, distances
    % and parent tables appropriately.
   
    %%% All of your code should be between the two lines of stars. 
    % *******************************************************************
    adj = [i j];
    %if (j > 1)
        if (j -1 == 0)
            id = ncols-1;
        else
            id = j - 1;
        end
        left = [i id];
        if (map(left(1), left(2)) ~= 2)
            adj = [adj;left];
        end
    %end
    
    %if (j+1 <= ncols)
        if (j+1 == ncols)
            id = 1;
        else
            id = j + 1;
        end
        right = [i id];
        if (map(right(1), right(2)) ~= 2)
            adj = [adj;right];
        end
    %end
    
    %if (i > 1)
        if (i - 1 == 0)
            id = nrows-1;
        else
            id = i - 1;
        end
        up = [id j];
        if (map(up(1), up(2)) ~= 2)
            adj = [adj;up];
        end
    %end
    
    %if (i+1 <= nrows)
        if (i+1 == nrows)
            id = 1;
        else
            id = i + 1;
        end
        down = [id j];
        if (map(down(1), down(2)) ~= 2)
            adj = [adj;down];
        end
    %end
    
    %adj = [left; right; up; down];
  
    for k=2:size(adj, 1)
        
        neighbor = adj(k, :);
        
        dist_val = distances(neighbor(1), neighbor(2));
        length = abs(i - neighbor(1)) + abs(j - neighbor(2));
        %fprintf('adj: %d, %d, map: %d\n', neighbor(1), neighbor(2), map(neighbor(1), neighbor(2)));
        if (map(neighbor(1), neighbor(2)) ~= 4) && dist_val > (min_dist + length) && (map(neighbor(1), neighbor(2)) ~= 3) && (map(neighbor(1), neighbor(2)) ~= 5)
            distances(neighbor(1) , neighbor(2)) = min_dist + length; 
            if (map(neighbor(1), neighbor(2)) ~= 6)
                map(neighbor(1), neighbor(2)) = 4;
            end
            
            
            parent(neighbor(1), neighbor(2)) = current; 
            
        end
    end
    
    % *******************************************************************
end
fclose(fd);
if (isinf(distances(dest_node)))
    route = [];
else
    route = [dest_node];
    
    while (parent(route(1)) ~= 0)
        route = [parent(route(1)), route];
    end
end

    function update (i,j,d,p)
        if ( (map(i,j) ~= 2) && (map(i,j) ~= 3) && (map(i,j) ~= 5) && (distances(i,j) > d) )
            distances(i,j) = d;
            map(i,j) = 4;
            parent(i,j) = p;
        end
    end

end
