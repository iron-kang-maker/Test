/*
 * rbfnn_pid.c
 *
 *  Created on: Apr 14, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rbfnn_pid.h"

RBFNN_PID_Result RBFNN_PID_init(RBFNN_PID *rbfnn_pid, int input_layer_num, int hidden_layer_num, float *bias, float *center, float *weight)
{
	int i;

	rbfnn_pid->rbfnn.in_layer = input_layer_num;
	rbfnn_pid->rbfnn.hid_layer = hidden_layer_num;

	rbfnn_pid->rbfnn.center = (float *) malloc(hidden_layer_num*input_layer_num*sizeof(float));
	rbfnn_pid->rbfnn.center_1 = (float *) malloc(hidden_layer_num*input_layer_num*sizeof(float));
	rbfnn_pid->rbfnn.center_2 = (float *) malloc(hidden_layer_num*input_layer_num*sizeof(float));

	rbfnn_pid->rbfnn.dw = (float *) malloc(rbfnn_pid->rbfnn.hid_layer*sizeof(float));
	rbfnn_pid->rbfnn.db = (float *) malloc(rbfnn_pid->rbfnn.hid_layer*sizeof(float));
	rbfnn_pid->rbfnn.dc = (float *) malloc(rbfnn_pid->rbfnn.in_layer*rbfnn_pid->rbfnn.hid_layer*sizeof(float));

	for(i=0; i<hidden_layer_num*input_layer_num; i++)
	{
		rbfnn_pid->rbfnn.center[i] = center[i];
		rbfnn_pid->rbfnn.center_1[i] = center[i];
		rbfnn_pid->rbfnn.center_2[i] = center[i];
	}

	rbfnn_pid->rbfnn.bias = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn_pid->rbfnn.bias_1 = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn_pid->rbfnn.bias_2 = (float *) malloc(hidden_layer_num*sizeof(float));

	rbfnn_pid->rbfnn.weight = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn_pid->rbfnn.weight_1 = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn_pid->rbfnn.weight_2 = (float *) malloc(hidden_layer_num*sizeof(float));

	for(i=0; i<hidden_layer_num; i++)
	{
		rbfnn_pid->rbfnn.bias[i] = bias[i];
		rbfnn_pid->rbfnn.bias_1[i] = bias[i];
		rbfnn_pid->rbfnn.bias_2[i] = bias[i];

		rbfnn_pid->rbfnn.weight[i] = weight[i];
		rbfnn_pid->rbfnn.weight_1[i] = weight[i];
		rbfnn_pid->rbfnn.weight_2[i] = weight[i];
	}

	rbfnn_pid->rbfnn.gaussian = (float *) malloc(hidden_layer_num*sizeof(float));

	if (!rbfnn_pid->rbfnn.center || !rbfnn_pid->rbfnn.center_1 || !rbfnn_pid->rbfnn.center_2 ||
		!rbfnn_pid->rbfnn.bias || !rbfnn_pid->rbfnn.bias_1 || !rbfnn_pid->rbfnn.bias_2 ||
		!rbfnn_pid->rbfnn.weight || !rbfnn_pid->rbfnn.weight_1 || !rbfnn_pid->rbfnn.weight_2 ||
		!rbfnn_pid->rbfnn.center || !rbfnn_pid->rbfnn.center_1 || !rbfnn_pid->rbfnn.center_2 ||
		!rbfnn_pid->rbfnn.gaussian)
		return RBFNN_PID_Result_MemAllocFailed;

	rbfnn_pid->pid.u_1 = 0;
	rbfnn_pid->pid.error_1 = 0;
	rbfnn_pid->pid.error_2 = 0;

	rbfnn_pid->rbfnn.jabi = 0;

	return RBFNN_PID_Result_Ok;
}

float RBFNN_PID_norm(RBFNN_PID *rbfnn_pid, float *u, int layer)
{
	float norm;
	int i;

	norm = 0;

	for(i=0; i<rbfnn_pid->rbfnn.in_layer; i++)
	{
		norm += pow(u[i]-rbfnn_pid->rbfnn.center[layer*rbfnn_pid->rbfnn.in_layer+i], 2);
	}

	return norm;
}

/* Formula:          ||X(k)-Cj(K)||^2
 * 		h(j) = exp(- ----------------- )
 *                  	2*bj(k)^2
 */
void RRBFNN_PID_gaussian(RBFNN_PID *rbfnn_pid, float *u)
{
	int i;
	for(i=0; i<rbfnn_pid->rbfnn.hid_layer; i++)
	{
		rbfnn_pid->rbfnn.gaussian[i] = exp(-RBFNN_PID_norm(rbfnn_pid, u, i)/(2*rbfnn_pid->rbfnn.bias[i]*rbfnn_pid->rbfnn.bias[i]));
	}
}

void RBFNN_PID_exit(RBFNN_PID *rbfnn_pid)
{
	free(rbfnn_pid->rbfnn.bias);
	free(rbfnn_pid->rbfnn.bias_1);
	free(rbfnn_pid->rbfnn.bias_2);
	free(rbfnn_pid->rbfnn.db);

	free(rbfnn_pid->rbfnn.center);
	free(rbfnn_pid->rbfnn.center_1);
	free(rbfnn_pid->rbfnn.center_2);
	free(rbfnn_pid->rbfnn.dc);

	free(rbfnn_pid->rbfnn.weight);
	free(rbfnn_pid->rbfnn.weight_1);
	free(rbfnn_pid->rbfnn.weight_2);
	free(rbfnn_pid->rbfnn.dw);
}

float RBFNN_PID_model(RBFNN_PID *rbfnn_pid, float *u, float out, float error)
{
	float rfb_out, err, eta, alpha, pid_out;
	float pid_term1, pid_term2, pid_term3;
	float kp, ki, kd;
	int i, j, k;

	rfb_out = 0;
	err = 0;
	eta = rbfnn_pid->rbfnn.eta;
	alpha = rbfnn_pid->rbfnn.alpha;

	/* RBFNN output*/
	RRBFNN_PID_gaussian(rbfnn_pid, u);
	for(i=0; i<rbfnn_pid->rbfnn.hid_layer; i++)
	{
		rfb_out += rbfnn_pid->rbfnn.weight[i]*rbfnn_pid->rbfnn.gaussian[i];
	}

	err = out - rfb_out;
	/* update weight, bias, center */
	for(i=0; i<rbfnn_pid->rbfnn.hid_layer; i++)
	{
		rbfnn_pid->rbfnn.dw[i] = eta*err*rbfnn_pid->rbfnn.gaussian[i];
		rbfnn_pid->rbfnn.db[i] = eta*err*rbfnn_pid->rbfnn.weight[i]*rbfnn_pid->rbfnn.gaussian[i]*RBFNN_PID_norm(rbfnn_pid, u, i)*pow(rbfnn_pid->rbfnn.bias[i],-3);
		for(j=0; j<rbfnn_pid->rbfnn.in_layer; j++) {
			rbfnn_pid->rbfnn.dc[i*rbfnn_pid->rbfnn.in_layer+j] =
					err*eta*rbfnn_pid->rbfnn.weight[i]*rbfnn_pid->rbfnn.gaussian[i]*(u[j]-rbfnn_pid->rbfnn.center[i*rbfnn_pid->rbfnn.in_layer+j])/(rbfnn_pid->rbfnn.bias[i]*rbfnn_pid->rbfnn.bias[i]);
		}
	}


	for(i=0; i<rbfnn_pid->rbfnn.hid_layer; i++)
	{
		rbfnn_pid->rbfnn.weight[i] = rbfnn_pid->rbfnn.weight_1[i] + rbfnn_pid->rbfnn.dw[i] + alpha*(rbfnn_pid->rbfnn.weight_1[i]-rbfnn_pid->rbfnn.weight_2[i]);
		rbfnn_pid->rbfnn.bias[i]   = rbfnn_pid->rbfnn.bias_1[i] + rbfnn_pid->rbfnn.db[i] + alpha*(rbfnn_pid->rbfnn.bias_1[i]-rbfnn_pid->rbfnn.bias_2[i]);

		for(j=0; j<rbfnn_pid->rbfnn.in_layer; j++) {
			k = i*rbfnn_pid->rbfnn.in_layer+j;
			rbfnn_pid->rbfnn.center[k] = rbfnn_pid->rbfnn.center_1[k] + rbfnn_pid->rbfnn.dc[k] + alpha*(rbfnn_pid->rbfnn.center_1[k]-rbfnn_pid->rbfnn.center_2[k]);
			rbfnn_pid->rbfnn.center_2[k] = rbfnn_pid->rbfnn.center_1[k];
			rbfnn_pid->rbfnn.center_1[k] = rbfnn_pid->rbfnn.center[k];
		}

		rbfnn_pid->rbfnn.weight_2[i] = rbfnn_pid->rbfnn.weight_1[i];
		rbfnn_pid->rbfnn.weight_1[i] = rbfnn_pid->rbfnn.weight[i];
		rbfnn_pid->rbfnn.bias_2[i] = rbfnn_pid->rbfnn.bias_1[i];
		rbfnn_pid->rbfnn.bias_1[i] = rbfnn_pid->rbfnn.bias[i];

	}

	/* calculate Jacobian:
	 *             cj1-x(1)
	 * J = ∑ wj*hj------------
	 *               bj^2
	 */
	rbfnn_pid->rbfnn.jabi = 0;
	for (i=0; i<rbfnn_pid->rbfnn.hid_layer; i++)
	{
		rbfnn_pid->rbfnn.jabi = rbfnn_pid->rbfnn.jabi + rbfnn_pid->rbfnn.weight[i]*rbfnn_pid->rbfnn.gaussian[i]*(-u[0]+rbfnn_pid->rbfnn.center[i*rbfnn_pid->rbfnn.in_layer])/(rbfnn_pid->rbfnn.bias[i]*rbfnn_pid->rbfnn.bias[i]);
	}

	/* calculate PID term */
	pid_term1 = error - rbfnn_pid->pid.error_1;
	pid_term2 = error;
	pid_term3 = error - 2*rbfnn_pid->pid.error_1 + rbfnn_pid->pid.error_2;

	/* update PID parameter */
	kp = rbfnn_pid->pid.kp + eta*error*rbfnn_pid->rbfnn.jabi*pid_term1;
	ki = rbfnn_pid->pid.ki + eta*error*rbfnn_pid->rbfnn.jabi*pid_term2;
	kd = rbfnn_pid->pid.kd + eta*error*rbfnn_pid->rbfnn.jabi*pid_term3;

	rbfnn_pid->pid.kp = kp;
	rbfnn_pid->pid.ki = ki;
	rbfnn_pid->pid.kd = kd;

	/* calculate PID output ∆u(k) = kp*(e(k)-e(k-1)) + ki*e(k) + kd(e(k)-2*e(k-1)+e(k-2))
	 * u(k) = u(k-1) + ∆u(k) */
	pid_out = rbfnn_pid->pid.u_1 + kp*pid_term1 + ki*pid_term2 + kd*pid_term3;
	rbfnn_pid->pid.u_1 = pid_out;

	/* update error term */
	rbfnn_pid->pid.error_2 = rbfnn_pid->pid.error_1;
	rbfnn_pid->pid.error_1 = error;


	return pid_out;
}

void RBFNN_test()
{
	printf("rbfnn pid test\n");
}

