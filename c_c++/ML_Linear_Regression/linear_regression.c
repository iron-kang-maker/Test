/*
 * gnuplot_test.c
 *
 *  Created on: Jul 17, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float cost_func(float x[][2], float y[], float theta[], int length)
{
	float ret = 0;
	int i;

	for ( i = 0; i < length; i++)
	{
		ret += pow(theta[0]*x[i][0] + theta[1] *x[i][1] - y[i], 2);
	}

	return (ret/(2*length));
}

float delta_func(float x[][2], float y[], float theta[], int length, int index)
{
	float ret = 0;
	int i;

	for (i = 0; i < length; i++)
	{
		ret = ret + (theta[0]*x[i][0] + theta[1]*x[i][1] - y[i])*x[i][index];
	}


	return (ret/length);
}

int main(void)
{
	FILE *linearPlot, *costPlot, *contourPlot, *iterPlot;
	FILE *data, *record, *f, *cost, *iter;
	int iterations = 1500;
	float alpha = 0.01;
	float theta[2], delta[2], optimal_theta[2], theta_all[iterations][2];
	float costVal;
	char str[20];
	int length, i, j;
	float theta0_step, theta1_step ;


	linearPlot 	= popen ("gnuplot -persistent", "w");
	costPlot 	= popen ("gnuplot -persistent", "w");
	contourPlot = popen ("gnuplot -persistent", "w");
	iterPlot 	= popen ("gnuplot -persistent", "w");

	f = popen("wc -l data/ex1data1.txt", "r");

	data 	= fopen("data/ex1data1.txt", "r");
	record 	= fopen("data/record", "w");
	cost 	= fopen("data/cost", "w");
	iter    = fopen("data/iter", "w");


//	fgets(str, sizeof(str)-1, f);
//
//	printf("%s\n", str);
	fscanf(f, "%d %s", &length, str);

	float x[length][2], y[length];

	for (i = 0; i < length; i++)
	{
		fscanf(data, "%f %f 0", &x[i][1], &y[i]);
		x[i][0] = 1;
	}

	for (i = 0; i < iterations; i++)
	{
		for (j = 0; j < 2; j++)
		{
			delta[j] = delta_func(x, y, theta, length, j);
		}

		for (j = 0; j < 2; j++)
		{
			theta[j] -= alpha*delta[j];
		}
		theta_all[i][0] = theta[0];
		theta_all[i][1] = theta[1];

		fprintf(iter, "%d %f\n", i, cost_func(x, y, theta, length));
	}

	optimal_theta[0] = theta[0];
	optimal_theta[1] = theta[1];

	for (i = 0; i < length; i++)
	{
		fprintf(record, "%f %f %f\n", x[i][1], y[i], theta[0]*x[i][0] + theta[1]*x[i][1]);
	}

	for (theta0_step = -10; theta0_step <= 10; (theta0_step+=0.2))
	{
		for (theta1_step = -1; theta1_step <= 4; (theta1_step+=0.05))
		{
			theta[0] = theta0_step;
			theta[1] = theta1_step;

			costVal = cost_func(x, y, theta, length);

			fprintf(cost, "%f %f %f\n", theta0_step,theta1_step, costVal);

		}
		fprintf(cost, "\n");
	}
//	getchar();
//
//	float predict1 = theta[0] + 3.5*theta[1];
//	printf("For population = 35,000, we predict a profit of %f\n",predict1*10000);
//	float predict2 = theta[0] + 7*theta[1];
//	printf("For population = 70,000, we predict a profit of %f\n",predict2*10000);



	fprintf(linearPlot, "set title \"Linear regression\"\n");
	fprintf(linearPlot, "set xlabel \"Population of City in 10,000s\"\n");
	fprintf(linearPlot, "set ylabel \"Profit in $10,000s\" \n");
	fprintf(linearPlot, "set xr [3.0:28.0] \n");
	fprintf(linearPlot, "set yr [-6.0:28.0] \n");
	fprintf(linearPlot, "plot 'data/record' using 1:2 t \"Training Data\" ,");
	fprintf(linearPlot, "     'data/record' using 1:3 t \"Linear regression\" with linespoints\n");


	fprintf(costPlot, "set isosamples 20,20\n");
	fprintf(costPlot, "set hidden3d\n");
//	fprintf(costPlot, "set contour\n");
//	fprintf(costPlot, "set cntrparam levels 20\n");
	fprintf(costPlot, "set pm3d\n");
	fprintf(costPlot, "splot 'data/cost' using 1:2:3 t \"Cost Function\" with lines\n");


	fprintf(contourPlot, "set title \"Cost Function Contour\"\n");
	fprintf(contourPlot, "set contour base\n");
	fprintf(contourPlot, "unset surface\n");
	fprintf(contourPlot, "set cntrparam levels 10\n");
	fprintf(contourPlot, "set pm3d map\n");
	fprintf(contourPlot, "set label \"min(%f, %f)\" at %f,%f point pt 5 lc rgb \"#FF0000\"\n",
							optimal_theta[0], optimal_theta[1], optimal_theta[0], optimal_theta[1]);
	for (i = 0; i < iterations; i++)
	{
		fprintf(contourPlot, "set object %d circle at %f,%f radius char 0.5 fillstyle empty border lc rgb '#00FF00' lw 2\n",
				i+1, theta_all[i][0], theta_all[i][1]);
	}
	fprintf(contourPlot, "splot 'data/cost' using 1:2:3 notitle\n ");

	fprintf(iterPlot, "set title \"Cost / iteration\"\n");
	fprintf(iterPlot, "set xlabel \"iteraton\"\n");
	fprintf(iterPlot, "set ylabel \"cost value\" \n");
	fprintf(iterPlot, "plot 'data/iter' using 1:2 notitle\n ");

	fflush(linearPlot);
	fflush(costPlot);
	fflush(contourPlot);
	fflush(iterPlot);

	pclose(f);
	fclose(record);
	fclose(data);
	fclose(cost);
	fclose(iter);
	return 0;
}

