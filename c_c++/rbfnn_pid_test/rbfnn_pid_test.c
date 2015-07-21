/*
 * rbfnn_pid_test.c
 *
 *  Created on: Apr 14, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include "../rbfnn_pid/rbfnn_pid.h"

int main(void)
{
	FILE * gnuplotPipe, *data, *jacobPlot, *kpPlot, *kiPlot, *kdPlot, *u1Plot;
	RBFNN_PID rbfnn_pid;

	int k=0, ref, i;
	float x[3] = { 0, 0, 0 };
	float yout, yout_1, u_1, error;
	float ts = 0.001;
	float bias[6] = {1};
	float center[18] = {1};
	float weight[6] = {1};
	int in_layer = 3;
	int hid_layer = 6;

	for (i=0; i<6; i++)
		bias[i] = 40;

	for (i=0; i<18; i++)
		center[i] = 30;

	for (i=0; i<6; i++)
		weight[i] = 0.5;

	rbfnn_pid.rbfnn.eta = 0.25;
	rbfnn_pid.rbfnn.alpha = 0;

	rbfnn_pid.pid.kp = 0.03;
	rbfnn_pid.pid.ki = 0.5;
	rbfnn_pid.pid.kd = 0.03;

	yout_1 = 0;
	u_1 = 0;

	gnuplotPipe = popen ("gnuplot -persistent", "w");
	jacobPlot = popen ("gnuplot -persistent", "w");
	kpPlot = popen ("gnuplot -persistent", "w");
	kiPlot = popen ("gnuplot -persistent", "w");
	kdPlot = popen ("gnuplot -persistent", "w");
	u1Plot = popen ("gnuplot -persistent", "w");
	data = fopen("data", "w");

	if (RBFNN_PID_init(&rbfnn_pid, in_layer, hid_layer, bias, center, weight) != RBFNN_PID_Result_Ok)
		printf("BRFNN init fail\n");
	else
		printf("RBFNN init success\n");

	for (k=1;k<4000; k++)
	{
		if (k<=500 || (k>1000 && k<=1500) || (k>2000 && k<=2500) || (k>3000 && k<=3500))
			ref = 1;
		else
			ref = -1;

		yout = (-0.2*yout_1+0.5*u_1)/(1.5+yout_1*yout_1);
		error = ref - yout;

//		x[1] = yout;

		u_1 = RBFNN_PID_model(&rbfnn_pid, x, yout, error);
		x[2] = yout_1;
		yout_1 = yout;

		x[0] = u_1;
		x[1] = yout;
//		x[2] = yout;;
		fprintf(data, "%f %d %f %f %f %f %f %f\n", k*ts , ref, yout, rbfnn_pid.rbfnn.jabi, u_1, rbfnn_pid.pid.kp, rbfnn_pid.pid.ki, rbfnn_pid.pid.kd);
	}


	RBFNN_PID_exit(&rbfnn_pid);
	printf("finish RBFNN PID\n");
	fprintf(gnuplotPipe, "set title \"RBFNN PID\"\n");
	fprintf(gnuplotPipe, "set xlabel \"time(s)\"\n");
	fprintf(gnuplotPipe, "set ylabel \"ref, yout\" \n");
//	fprintf(gnuplotPipe, "plot 'data' u 1:2 t \"r\" with lines,");
	fprintf(gnuplotPipe, "plot 'data' u 1:2 t \"ref\" with lines,");
	fprintf(gnuplotPipe, "     'data' u 1:3 t \"yout\" with lines\n");
//	fprintf(gnuplotPipe, "     'data' u 1:4 t \"rbfnn_pid.pid.jabi\" with lines,");
//	fprintf(gnuplotPipe, "     'data' u 1:5 t \"u_1\" with lines\n");

	fprintf(jacobPlot, "set title \"Jacob Value\"\n");
	fprintf(jacobPlot, "set xlabel \"time(s)\"\n");
	fprintf(jacobPlot, "set ylabel \"Jacob Value\" \n");
	fprintf(jacobPlot, "plot 'data' u 1:4 t \"rbfnn_pid.pid.jabi\" with lines\n");

	fprintf(u1Plot, "set title \"u1 Value\"\n");
	fprintf(u1Plot, "set xlabel \"time(s)\"\n");
	fprintf(u1Plot, "set ylabel \"u1 Value\" \n");
	fprintf(u1Plot, "plot 'data' u 1:5 t \"u_1\" with lines\n");

	fprintf(kpPlot, "set title \"KP Value\"\n");
	fprintf(kpPlot, "set xlabel \"time(s)\"\n");
	fprintf(kpPlot, "set ylabel \"Kp Value\" \n");
	fprintf(kpPlot, "plot 'data' u 1:6 t \"rbfnn_pid.pid.kp\" with lines\n");

	fprintf(kiPlot, "set title \"KI Value\"\n");
	fprintf(kiPlot, "set xlabel \"time(s)\"\n");
	fprintf(kiPlot, "set ylabel \"Ki Value\" \n");
	fprintf(kiPlot, "plot 'data' u 1:7 t \"rbfnn_pid.pid.ki\" with lines\n");

	fprintf(kdPlot, "set title \"KD Value\"\n");
	fprintf(kdPlot, "set xlabel \"time(s)\"\n");
	fprintf(kdPlot, "set ylabel \"Kd Value\" \n");
	fprintf(kdPlot, "plot 'data' u 1:8 t \"rbfnn_pid.pid.kd\" with lines\n");

	fflush(gnuplotPipe);
	fflush(jacobPlot);
	fflush(kpPlot);
	fflush(kiPlot);
	fflush(kdPlot);

	fclose(data);

	return 0;
}

