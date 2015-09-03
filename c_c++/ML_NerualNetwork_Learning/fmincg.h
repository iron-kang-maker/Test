/*
 * fmincg.h
 *
 *  Created on: Aug 24, 2015
 *      Author: iron
 */

#ifndef FMINCG_H_
#define FMINCG_H_

#include <stdio.h>
#include <math.h>
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"

#define COST_FUNC_DATATYPE double
#define COST_FUNC_DATATYPE_MIN 2.2251e-308
#define RHO 0.01f
#define SIG 0.5f
#define INT 0.1f
#define EXT 3.0f
#define MAX 20
#define RATIO 100.0f

using namespace arma;

typedef struct cost_data {
	mat x;
	mat y;
	mat *theta;
	double lambda;
	int in_layer_size;
	int hid_layer_size;
	int num_label;
} COSTDATA;

int fmincg(void (*costFunc)(COSTDATA *cost_data, COST_FUNC_DATATYPE *cost, mat *grad), COSTDATA* cost_data, int maxCostCalls);


#endif /* FMINCG_H_ */
