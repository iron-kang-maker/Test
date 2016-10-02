%
% TestScript for Assignment 1
%

%% Define a small map
map = false(10);

% Add an obstacle
%map (1:5, 6) = true;
map (1:10, 5:8) = true;
map (6, 1:10) = false;
%map (1:5, 5:8) = true;

start_coords = [10, 10];%[6,2];%[10, 10];%[6, 2];
dest_coords  = [1, 1];%[1 1];%[1, 1];%[8, 9];

%%
close all;
[route, numExpanded] = DijkstraGrid (map, start_coords, dest_coords);
% Uncomment following line to run Astar
%[route, numExpanded] = AStarGrid (map, start_coords, dest_coords);
