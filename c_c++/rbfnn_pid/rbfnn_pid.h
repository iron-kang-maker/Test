/*
 *
 *  Created on: Apr 14, 2015
 *      Author: iron
 */

#ifndef RBFNN_PID_H_
#define RBFNN_PID_H_

#include <stdbool.h>

typedef enum {
	RBFNN_PID_Result_Ok = 0x00,
	RBFNN_PID_Result_MemAllocFailed
} RBFNN_PID_Result;

typedef struct a{
	int i;
	int j;
} Test;

typedef struct rbfnn_pid {
	struct {
		float kp;
		float ki;
		float kd;
		float error_1;
		float error_2;
		float u_1;
		float k;
	} pid;

	struct {
		float *center;			/* center of neuro */
		float *center_1;
		float *center_2;
		float *dc;
		float *bias;			/* bias of gaussian */
		float *bias_1;
		float *bias_2;
		float *db;
		float *weight;			/* weight of hidden layer to output layer */
		float *weight_1;
		float *weight_2;
		float *dw;
		float *gaussian;		/* Gaussian function */
		float eta;				/* learning rate */
		float alpha;			/* factor */
		float jabi;
		int in_layer;			/* input layer number */
		int hid_layer;
	} rbfnn;
}RBFNN_PID;

RBFNN_PID_Result RBFNN_PID_init(RBFNN_PID *rbfnn_pid, int input_layer_num, int hidden_layer_num, float *bias, float *center, float *weight);
float RBFNN_PID_model(RBFNN_PID *rbfnn_pid, float *u, float out, float error);
void RBFNN_PID_exit(RBFNN_PID *rbfnn_pid);
void RBFNN_test();

#endif /* RBFNN_PID_H_ */
