/*
 * pid_test.c
 *
 *  Created on: Apr 9, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../LIB/PID/pid.h"

int main(void) {
	FILE * gnuplotPipe, *data;
	PID pid;
	int k=0, r;
	float ts = 0.001;
	float yout, yout_1, u_1, error;

	pid.kp = 0.03;
	pid.ki = 0.01;
	pid.kd = 0.03;

	yout = yout_1 = u_1 = 0;

	PID_reset(&pid);
	gnuplotPipe = popen ("gnuplot -persistent", "w");
	data = fopen("data", "w");

	for (k=1;k<2000; k++)
	{
		if (k<=500 || (k>1000 && k<=1500))
			r = 1;
		else
			r = -1;

		yout = (-0.2*yout_1+0.5*u_1)/(1.5+yout_1*yout_1);
		error = r - yout;

		yout_1 = yout;
		u_1 = PID_controller(&pid, error);
		printf("(%d) u_1: %f, error: %f\n", k, u_1, error);
		fprintf(data, "%f %d %f %f\n", k*ts , r, yout, error);
	}

	fprintf(gnuplotPipe, "set title \"PID test\"\n");
	fprintf(gnuplotPipe, "set xlabel \"time(s)\"\n");
	fprintf(gnuplotPipe, "set ylabel \"r, yout\" \n");

	fprintf(gnuplotPipe, "plot 'data' u 1:2 t \"r\" with lines,");
	fprintf(gnuplotPipe, "     'data' u 1:3 t \"yout\" with lines\n");

	fclose(data);
	fflush(gnuplotPipe);


	return 0;
}

