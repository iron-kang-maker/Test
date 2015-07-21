/*
 * pid.h
 *
 *  Created on: Apr 9, 2015
 *      Author: iron
 */

#ifndef PID_H_
#define PID_H_

typedef struct pid {
	float kp;
	float ki;
	float kd;
	float error_1;
	float error_2;
	float u_1;
}PID;

float PID_controller(PID *pid, float error);
void  PID_reset(PID *pid);

#endif /* PID_H_ */
