function [ u ] = pd_controller(~, s, s_des, params)
%PD_CONTROLLER  PD controller for the height
%
%   s: 2x1 vector containing the current state [z; v_z]
%   s_des: 2x1 vector containing desired state [z; v_z]
%   params: robot parameters

kv = 18;
kp = 100;
e_v = s_des(2) - s(2);
e_p = s_des(1) - s(1);
u = params.mass*(kp * e_p + kv * e_v + params.gravity);


% FILL IN YOUR CODE HERE


end

