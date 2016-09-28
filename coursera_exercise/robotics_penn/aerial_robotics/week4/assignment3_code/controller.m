function [F, M] = controller(t, state, des_state, params)
%CONTROLLER  Controller for the quadrotor
%
%   state: The current state of the robot with the following fields:
%   state.pos = [x; y; z], state.vel = [x_dot; y_dot; z_dot],
%   state.rot = [phi; theta; psi], state.omega = [p; q; r]
%
%   des_state: The desired states are:
%   des_state.pos = [x; y; z], des_state.vel = [x_dot; y_dot; z_dot],
%   des_state.acc = [x_ddot; y_ddot; z_ddot], des_state.yaw,
%   des_state.yawdot
%
%   params: robot parameters

%   Using these current and desired states, you have to compute the desired
%   controls


% =================== Your code goes here ===================

% Thrust
kd_z = 25;
kp_z = 200;
z_c_ddot = des_state.acc(3) + kd_z*(des_state.vel(3) - state.vel(3)) + kp_z*(des_state.pos(3) - state.pos(3));
F = params.mass*(params.gravity + z_c_ddot);

% Moment
M = zeros(3,1);
kd_x = 25;
kp_x = 5;
x_c_ddot = des_state.acc(1) + kd_x*(des_state.vel(1) - state.vel(1)) + kp_x*(des_state.pos(1) - state.pos(1));

kd_y = 25;
kp_y = 5;
y_c_ddot = des_state.acc(2) + kd_y*(des_state.vel(2) - state.vel(2)) + kp_y*(des_state.pos(2) - state.pos(2));

phi_c = (x_c_ddot*sin(des_state.yaw) - y_c_ddot*cos(des_state.yaw))/params.gravity;
theta_c = (x_c_ddot*cos(des_state.yaw) + y_c_ddot*sin(des_state.yaw))/params.gravity;

kd_phi = 1;
kp_phi = 1;
kd_theta = 1;
kp_theta = 1;
kd_psi = 1;
kp_psi = 1;

M(1) = kd_phi*(-state.omega(1)) + kp_phi*(phi_c - state.rot(1));
M(2) = kd_theta*(-state.omega(2)) + kp_theta*(theta_c - state.rot(2));
M(3) = kd_psi*(-state.omega(3)) + kp_psi*(des_state.yaw - state.rot(3));
% =================== Your code ends here ===================

end
