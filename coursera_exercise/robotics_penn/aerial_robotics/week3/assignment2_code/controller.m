function [ u1, u2 ] = controller(~, state, des_state, params)
%CONTROLLER  Controller for the planar quadrotor
%
%   state: The current state of the robot with the following fields:
%   state.pos = [y; z], state.vel = [y_dot; z_dot], state.rot = [phi],
%   state.omega = [phi_dot]
%
%   des_state: The desired states are:
%   des_state.pos = [y; z], des_state.vel = [y_dot; z_dot], des_state.acc =
%   [y_ddot; z_ddot]
%
%   params: robot paramters

%   Using these current and desired states, you have to compute the desired
%   controls

kv_z = 0.8;
kp_z = 117;
kv_y = 5; %1.1
kp_y = 0.639; %2
kv_phi = 5.9; %5
kp_phi = 611.14; %600

u1 = params.mass*(params.gravity + des_state.acc(2) + kv_z*(des_state.vel(2) - state.vel(2)) + kp_z*(des_state.pos(2) - state.pos(2)));
%fprintf('u1: %f, p: (%f, %f), v:(%f, %f)\n', u1, des_state.pos(2), state.pos(2), des_state.vel(2), state.vel(2));
fprintf('p(%f, %f, %f) v(%f, %f)\n', state.pos(1), des_state.pos(1), state.pos(2), state.vel(1), state.vel(2));
phi_c = -(des_state.acc(1) + kv_y*(des_state.vel(1) - state.vel(1)) + kp_y*(des_state.pos(1) - state.pos(1)))/params.gravity;

u2 = params.Ixx*(kv_phi*(0 - state.omega) + kp_phi*(phi_c - state.rot));
%fprintf('p: %f, v: %f, del_phi: %f(%f, %f), del_p: %f, del_v: %f, u2: %.8f\n', state.pos(1), state.vel(1), phi_c - state.rot, phi_c, state.rot, des_state.pos(1) - state.pos(1), des_state.vel(1) - state.vel(1), u2);
%fprintf('y: %f(%f), a: %f, v: %f, phi: %f, u2: %f\n', des_state.pos(1), state.pos(1), des_state.acc(1), des_state.vel(1), phi_c, u2);
% FILL IN YOUR CODE HERE

end

