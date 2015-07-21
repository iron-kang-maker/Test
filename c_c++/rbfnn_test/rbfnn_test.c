/*
 * rbfnn_test.c
 *
 *  Created on: Apr 13, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../RBFNN/rbfnn.h"

float random_value(void)
{
	return ((rand()/(RAND_MAX+1.0))-0.5);
}

int main(void)
{
	FILE *gnuplotPipe, *gnuplotPipe2, *data;
	RBFNN rbfnn;
	float ts = 0.001;
	float yout, yout_1, ynout;
	int in_layer = 2;
	int hid_layer = 4;
	int i, k;
	float bias[4] = { 1.5, 1.5, 1.5, 1.5 };
	float center[8] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
	float weight[4];
	float x[2] = { 0, 0 }, r;


	gnuplotPipe = popen ("gnuplot -persistent", "w");
	gnuplotPipe2 = popen ("gnuplot -persistent", "w");
	data = fopen("data", "w");

	for(i=0; i<hid_layer; i++)
	{
		//printf("random: %f\n", random_value());
		weight[i] = random_value();
	}


	rbfnn.eta = 0.5;
	rbfnn.alpha = 0.05;

	RBFNN_init(&rbfnn, in_layer, hid_layer, bias, center, weight);
	printf("RBFNN init success\n");

	for (k=1;k<2000; k++) {
		/* 輸入訊號 */
		r = -0.5*sin(2*M_PI*k*ts);

		/* plant model*/
		yout = pow(r,3) + yout_1/(1+yout_1*yout_1);
		yout_1 = yout;

		ynout = RBFNN_model(&rbfnn, x, yout);

		x[0] = r;
		x[1] = yout;
		fprintf(data, "%f %f %f %f\n", k*ts , yout, ynout, yout-ynout);
	}

	RBFNN_exit(&rbfnn);

	printf("finish RBFNN\n");
	fprintf(gnuplotPipe, "set title \"RBFNN test\"\n");
	fprintf(gnuplotPipe, "set xlabel \"time(s)\"\n");
	fprintf(gnuplotPipe, "set ylabel \"yout, ynout\" \n");

	fprintf(gnuplotPipe, "plot 'data' u 1:2 t \"yout\" with lines,");
	fprintf(gnuplotPipe, "     'data' u 1:3 t \"ynout\" with lines\n");

	fprintf(gnuplotPipe2, "set title \"Error\"\n");
	fprintf(gnuplotPipe2, "set xlabel \"time(s)\"\n");
	fprintf(gnuplotPipe2, "set ylabel \"error\" \n");

	fprintf(gnuplotPipe2, "plot 'data' u 1:4 t \"error\" with lines\n");


	fclose(data);
	fflush(gnuplotPipe);
	fflush(gnuplotPipe2);


	return 0;
}



