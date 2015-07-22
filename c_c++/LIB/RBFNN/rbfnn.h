/*
 * rbfnn.h
 *
 *  Created on: Apr 9, 2015
 *      Author: iron
 */

#ifndef RBFNN_H_
#define RBFNN_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct rbfnn {
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
	int in_layer;			/* input layer number */
	int hid_layer;			/* hidden layer number */
}RBFNN;

void RBFNN_init(RBFNN *rbfnn, int input_layer_num, int hidden_layer_num, float *bias, float *center, float *weight);
void RBFNN_exit(RBFNN *rbfnn);
float RBFNN_model(RBFNN *rbfnn, float *u, float out);

#endif /* RBFNN_H_ */
