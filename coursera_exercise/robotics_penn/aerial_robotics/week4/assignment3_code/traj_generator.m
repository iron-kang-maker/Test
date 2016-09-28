function [ desired_state ] = traj_generator(t, state, waypoints)
% TRAJ_GENERATOR: Generate the trajectory passing through all
% positions listed in the waypoints list
%
% NOTE: This function would be called with variable number of input arguments.
% During initialization, it will be called with arguments
% trajectory_generator([], [], waypoints) and later, while testing, it will be
% called with only t and state as arguments, so your code should be able to
% handle that. This can be done by checking the number of arguments to the
% function using the "nargin" variable, check the MATLAB documentation for more
% information.
%
% t,state: time and current state (same variable as "state" in controller)
% that you may use for computing desired_state
%
% waypoints: The 3xP matrix listing all the points you much visited in order
% along the generated trajectory
%
% desired_state: Contains all the information that is passed to the
% controller for generating inputs for the quadrotor
%
% It is suggested to use "persistent" variables to store the waypoints during
% the initialization call of trajectory_generator.


%% Example code:
% Note that this is an example of naive trajectory generator that simply moves
% the quadrotor along a stright line between each pair of consecutive waypoints
% using a constant velocity of 0.5 m/s. Note that this is only a sample, and you
% should write your own trajectory generator for the submission.

persistent waypoints0 traj_time d0
if nargin > 2
    d = waypoints(:,2:end) - waypoints(:,1:end-1);
    d0 = 2 * sqrt(d(1,:).^2 + d(2,:).^2 + d(3,:).^2);
    traj_time = [0, cumsum(d0)];
    waypoints0 = waypoints;
else
    if(t > traj_time(end))
        t = traj_time(end);
    end
    t_index = find(traj_time >= t,1);
    total_t = t;
    %fprintf('t index: %dm t: %f\n', t_index, total_t);

    %if(t_index > 1)
    %    t = t - traj_time(t_index-1);
    %end
    %if(t == 0)
    %    desired_state.pos = waypoints0(:,1);
    %else
    %    scale = t/d0(t_index-1);
    %    desired_state.pos = (1 - scale) * waypoints0(:,t_index-1) + scale * waypoints0(:,t_index);
    %    %fprintf('t: %f, x:%f\n', t, waypoints0(:,t_index));
    %end
    desired_state.pos = zeros(3,1);
    desired_state.vel = zeros(3,1);
    desired_state.acc = zeros(3,1);
    
    if (total_t < traj_time(2))
        c1 = [-8.576183477927135e-05 0.001760966735090 -0.012924790541416 0.034150690732404 0 2.984674284785074e-14 0 0]';
        c_y1 = [-1.183467064489890e-04 0.002398259270949 -0.017090351877660 0.042287634242592 0 3.394745504068583e-14 0 0]';
        c_z1 = [-1.098665914558825e-04 0.002138481368501 -0.014858313625110 0.037320451186945 0 -4.317045192457109e-15 0 0]';
        pos1 = ori(total_t, c1);
        vel1 = deri_1st(total_t, c1);
        acc1 = deri_2nd(total_t, c1);
        pos_y1 = ori(total_t, c_y1);
        vel_y1 = deri_1st(total_t, c_y1);
        acc_y1 = deri_2nd(total_t, c_y1);
        pos_z1 = ori(total_t, c_z1);
        vel_z1 = deri_1st(total_t, c_z1);
        acc_z1 = deri_2nd(total_t, c_z1);
        desired_state.pos(1) = pos1;
        desired_state.vel(1) = vel1;
        desired_state.acc(1) = acc1;
        desired_state.pos(2) = pos_y1;
        desired_state.vel(2) = vel_y1;
        desired_state.acc(2) = acc_y1;
        desired_state.pos(3) = pos_z1;
        desired_state.vel(3) = vel_z1;
        desired_state.acc(3) = acc_z1;
        %fprintf('x1: %f(%f), pos1: %f\n', total_t, t, pos1);
    elseif (total_t < traj_time(3))
        c2 = [1.314075673770635e-05 -6.372925358278054e-04 0.011998639279742 -0.109744731340054 0.498468131601205 -1.036046072807811 1.196322400271224 -0.592025775254248]';
        c_y2 = [1.944411492975399e-05 -9.429890194172846e-04 0.017632902730318 -0.158187076236571 0.694464444570868 -1.443416569462734 1.666713112758673 -0.824808699129657]';
        c_z2 = [3.580328580579961e-05 -0.001393823784254 0.021850461213869 -0.174617660345905 0.734174812160946 -1.525952980084060 1.762017906103133 -0.871972318361838]';
        pos2 = ori(total_t, c2);
        vel2 = deri_1st(total_t, c2);
        acc2 = deri_2nd(total_t, c2);
        pos_y2 = ori(total_t, c_y2);
        vel_y2 = deri_1st(total_t, c_y2);
        acc_y2 = deri_2nd(total_t, c_y2);
        pos_z2 = ori(total_t, c_z2);
        vel_z2 = deri_1st(total_t, c_z2);
        acc_z2 = deri_2nd(total_t, c_z2);
        desired_state.pos(1) = pos2;
        desired_state.vel(1) = vel2;
        desired_state.acc(1) = acc2;
        desired_state.pos(2) = pos_y2;
        desired_state.vel(2) = vel_y2;
        desired_state.acc(2) = acc_y2;
        desired_state.pos(3) = pos_z2;
        desired_state.vel(3) = vel_z2;
        desired_state.acc(3) = acc_z2;
        %fprintf('x2: %f, pos2: %f\n', total_t, pos2);
    elseif (total_t < traj_time(4))
        c3 = [1.314075674100668e-05 -6.372925359878711e-04 0.011998639283069 -0.109744731378474 0.498468131867437 -1.036046073914894 1.196322402829034 -0.592025777786760]';
        c_y3 = [1.944411493434283e-05 -9.429890196398400e-04 0.017632902734944 -0.158187076289983 0.694464444940856 -1.443416571000124 1.666713116306019 -0.824808702634884]';
        c_z3 = [-3.580328580691729e-05 0.002078908761877 -0.050328895664438 0.658837373527899 -5.040168353523375 22.477529612210944 -53.671624792536555 53.993079588015640]';
        pos3 = ori(total_t, c3);
        vel3 = deri_1st(total_t, c3);
        acc3 = deri_2nd(total_t, c3);
        pos_y3 = ori(total_t, c_y3);
        vel_y3 = deri_1st(total_t, c_y3);
        acc_y3 = deri_2nd(total_t, c_y3);
        pos_z3 = ori(total_t, c_z3);
        vel_z3 = deri_1st(total_t, c_z3);
        acc_z3 = deri_2nd(total_t, c_z3);
        desired_state.pos(1) = pos3;
        desired_state.vel(1) = vel3;
        desired_state.acc(1) = acc3;
        desired_state.pos(2) = pos_y3;
        desired_state.vel(2) = vel_y3;
        desired_state.acc(2) = acc_y3;
        desired_state.pos(3) = pos_z3;
        desired_state.vel(3) = vel_z3;
        desired_state.acc(3) = acc_z3;
        %fprintf('x3: %f, pos3: %f\n', total_t, pos3);
    elseif (total_t <= traj_time(5))
        c4 = [-8.576183478821544e-05 0.006557485277656 -0.212312229135118 3.775431665058692 -39.877450532825740 2.507231496495301e+02 -8.709227075026888e+02 1.294168344863007e+03]';
        c_y4 = [-1.183467064614409e-04 0.009080755852700 -0.294876388775502 5.254630107316650 -55.557155572253340 3.493068098517986e+02 -1.213367146234815e+03 1.803031816516289e+03]';
        c_z4 = [1.098665914632288e-04 -0.008518006697005 0.280050077905571 -5.063491638191446 54.427991434967424 -3.483290445697433e+02 1.230839428831113e+03 -1.853010380779324e+03]';
        pos4 = ori(total_t, c4);
        vel4 = deri_1st(total_t, c4);
        acc4 = deri_2nd(total_t, c4);
        pos_y4 = ori(total_t, c_y4);
        vel_y4 = deri_1st(total_t, c_y4);
        acc_y4 = deri_2nd(total_t, c_y4);
        pos_z4 = ori(total_t, c_z4);
        vel_z4 = deri_1st(total_t, c_z4);
        acc_z4 = deri_2nd(total_t, c_z4);
        if (pos4 > 0)
            desired_state.pos(1) = pos4;
            desired_state.vel(1) = vel4;
            desired_state.acc(1) = acc4;
            desired_state.pos(2) = pos_y4;
            desired_state.vel(2) = vel_y4;
            desired_state.acc(2) = acc_y4;
            desired_state.pos(3) = pos_z4;
            desired_state.vel(3) = vel_z4;
            desired_state.acc(3) = acc_z4;
        end
        %fprintf('x4: %f(%f), pos4: %f\n', total_t, t, pos4);
    end
       
    
    
    desired_state.yaw = 0;
    desired_state.yawdot = 0;
end
%


%% Fill in your code here

% desired_state.pos = zeros(3,1);
% desired_state.vel = zeros(3,1);
% desired_state.acc = zeros(3,1);
% desired_state.yaw = 0;
end

