/*
 * gnuplot_test.c
 *
 *  Created on: Jul 17, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float cost_func(float *x, float y[], float theta[], int length, int features)
{
	float ret = 0;
	int i, j;
	float hyp = 0;

	for ( i = 0; i < length; i++)
	{
		hyp = 0;
		for ( j = 0; j < (features + 1); j++)
			hyp += theta[j] * (*(x+i*(features+1)+j));

		ret += pow(hyp - y[i], 2);
	}

	return (ret/(2*length));
}

float delta_func(float *x, float y[], float theta[], int length, int index, int features)
{
	float ret = 0;
	int i, j;
	float hyp = 0;

	for (i = 0; i < length; i++)
	{
		hyp = 0;
		for ( j = 0; j < (features + 1); j++)
			hyp += theta[j] * (*(x+i*(features+1)+j));

		ret = ret + (hyp - y[i])*(*(x+i*(features+1)+index));
	}


	return (ret/length);
}

void featureNormalize(float *x, float mean[], float std[], int length, int features)
{
	int i, j;

	for ( i = 0 ; i < length; i++ )
	{
		for ( j = 1; j < (features+1); j++ )
			*(x+i*(features+1)+j) = (*(x+i*(features+1)+j) - mean[j-1]) / std[j-1];
	}
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
			delta[j] = delta_func(x, y, theta, length, j, 1);
		}

		for (j = 0; j < 2; j++)
		{
			theta[j] -= alpha*delta[j];
		}
		theta_all[i][0] = theta[0];
		theta_all[i][1] = theta[1];

		fprintf(iter, "%d %f\n", i, cost_func(x, y, theta, length, 1));
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

			costVal = cost_func(x, y, theta, length, 1);

			fprintf(cost, "%f %f %f\n", theta0_step,theta1_step, costVal);

		}
		fprintf(cost, "\n");
	}


	fprintf(linearPlot, "set title \"Linear regression\"\n");
	fprintf(linearPlot, "set xlabel \"Population of City in 10,000s\"\n");
	fprintf(linearPlot, "set ylabel \"Profit in $10,000s\" \n");
	fprintf(linearPlot, "set xr [3.0:28.0] \n");
	fprintf(linearPlot, "set yr [-6.0:28.0] \n");
	fprintf(linearPlot, "plot 'data/ex1data1.txt' using 1:2 t \"Training Data\",");
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
	fprintf(contourPlot, "set cntrparam levels 20\n");
	fprintf(contourPlot, "set pm3d map\n");
	fprintf(contourPlot, "set label \"min(%f, %f)\" at %f,%f point pt 5 lc rgb \"#FF0000\"\n",
							optimal_theta[0], optimal_theta[1], optimal_theta[0], optimal_theta[1]);
	for (i = 0; i < iterations; i++)
	{
		fprintf(contourPlot, "set object %d circle at %f,%f radius char 0.1 fillstyle empty border lc rgb '#00FF00' lw 2\n",
				i+1, theta_all[i][0], theta_all[i][1]);
	}

	fprintf(contourPlot, "splot 'data/cost' using 1:2:3 notitle\n ");

	fprintf(iterPlot, "set title \"Cost / iteration\"\n");
	fprintf(iterPlot, "set xlabel \"iteraton\"\n");
	fprintf(iterPlot, "set ylabel \"cost value\" \n");
	fprintf(iterPlot, "plot 'data/iter' using 1:2 notitle\n ");





	/********************* Multi features **************************/
	FILE *iter_multi_plot, *iter_multi;
	float means[2] = { 0 };;
	float std[2] = { 0 };
	float theta_multi[3] = { 0 }, delta_multi[3] = { 0 };

	f = popen("wc -l data/ex1data2.txt", "r");
	fscanf(f, "%d", &length);

	data 	= fopen("data/ex1data2.txt", "r");
	iter_multi_plot= popen ("gnuplot -persistent", "w");
	iter_multi    = fopen("data/iter_multi", "w");

	float x_data2[length][3], y_data2[length];
	for (i = 0; i < length; i++)
	{
		fscanf(data, "%f %f %f", &x_data2[i][1], &x_data2[i][2], &y_data2[i]);
		x_data2[i][0] = 1;
		means[0] += x_data2[i][1];
		means[1] += x_data2[i][2];
//		printf("%.2f, %.2f, %.2f\n", x_data2[i][0], x_data2[i][1], x_data2[i][2]);
	}

	means[0] /= length;
	means[1] /= length;

	printf("mean(%f, %f)\n", means[0], means[1]);

	for ( i = 0; i < length; i++ )
	{
		std[0] += pow(x_data2[i][1] - means[0], 2);
		std[1] += pow(x_data2[i][2] - means[1], 2);
	}

	std[0] = sqrt( std[0]/(length - 1) );
	std[1] = sqrt( std[1]/(length - 1));

	printf("std (%f, %f)\n", std[0], std[1]);

	featureNormalize(x_data2, means, std, length, 2);

//	for ( i = 0; i < length; i++ )
//		printf("%f, %f, %f\n", x_data2[i][0], x_data2[i][1], x_data2[i][2]);

	alpha = 0.01;
	iterations = 400;

	for (i = 0; i < iterations; i++)
	{
		for (j = 0; j < 3; j++)
		{
			delta_multi[j] = delta_func(x_data2, y_data2, theta_multi, length, j, 2);
		}

		for (j = 0; j < 3; j++)
		{
			theta_multi[j] -= alpha*delta_multi[j];
		}

		fprintf(iter_multi, "%d %f\n", i, cost_func(x_data2, y_data2, theta_multi, length, 2));
	}

	printf("theta_multi[%.3f, %.3f, %.3f]\n", theta_multi[0], theta_multi[1], theta_multi[2]);
	fprintf(iter_multi_plot, "set title \"Muiti features\"\n");
	fprintf(iter_multi_plot, "set xlabel \"iteraton\"\n");
	fprintf(iter_multi_plot, "set ylabel \"cost value\" \n");
	fprintf(iter_multi_plot, "plot 'data/iter_multi' using 1:2 notitle\n ");


	fflush(linearPlot);
	fflush(costPlot);
	fflush(contourPlot);
	fflush(iterPlot);

	fflush(iter_multi_plot);

	pclose(f);
	fclose(data);
	fclose(iter_multi);
	fclose(record);
	fclose(cost);
	fclose(iter);

	return 0;
}

