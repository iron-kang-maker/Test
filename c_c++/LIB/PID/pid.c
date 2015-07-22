/*
 * pid.c
 *
 *  Created on: Apr 9, 2015
 *      Author: iron
 */
#include "pid.h"

void PID_reset(PID *pid) {
	pid->u_1 = 0;
	pid->error_1 = 0;
	pid->error_2 = 0;
}

float PID_controller(PID *pid, float error) {
	float output = 0.0;
	/* 增量式PID */
	/* ∆u(k) = kp*(e(k)-e(k-1)) + ki*e(k) + kd(e(k)-2*e(k-1)+e(k-2))
	 */
	output = pid->u_1 + pid->kp*(error - pid->error_1) + pid->ki*error + pid->kd*(error-2*pid->error_1+pid->error_2);
	pid->u_1 = output;
	pid->error_1 = error;
	pid->error_2 = pid->error_1;

	return output;
}
