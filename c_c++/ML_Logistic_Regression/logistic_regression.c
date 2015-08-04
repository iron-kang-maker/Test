/*
 * logistic_regression.c
 *
 *  Created on: Jul 30, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double sigmoid(double z)
{
	return (1/(1+exp(-z)));
}

double cost_func(float x[][3], float y[], double theta[], int length)
{
	double ret, tmp;
	int i, j;

	ret = tmp = 0;

	for ( i = 0; i < length; i++)
	{
		tmp = 0;
		for ( j = 0; j < 3; j++ )
			tmp += x[i][j]*theta[j];

		ret += ( -y[i]*log( sigmoid(tmp) ) - ( 1 - y[i] )*log( 1 - sigmoid(tmp) ) );

	}

	return (ret/length);
}

void gradient(double grad[], float x[][3], float y[], double theta[], int length, int size)
{
	double ret, tmp;
	int i, j, k;

	for ( i = 0; i < size; i++ )
	{
		ret = 0;

		for ( j = 0; j < length; j++ )
		{
			tmp = 0;
			for ( k = 0; k < 3; k++ )
				tmp += x[j][k]*theta[k];

			ret += (sigmoid(tmp) - y[j])*x[j][i];
		}
		grad[i] = ret/length;
	}

}

void preditc(float x[][3], double theta[], int pred[], int length, int size)
{
	double tmp;
	int i, j;

	for ( i = 0; i < length; i++ )
	{
		tmp = 0;
		for ( j = 0; j < size; j++ )
			tmp += theta[j]*x[i][j];

		if ( sigmoid(tmp) >= 0.5 )
			pred[i] = 1;
		else
			pred[i] = 0;
	}
}

int main(void)
{
	/******************* Logistic Regression ************************/
	FILE *raw_data_plot, *iter_plot;
	FILE *rawdata, *f, *iter, *logistic_data;
	int length, i, j;
	int iterations = 10000;
	double theta[3] = {0};
	double grad[3];
	float alpha = 0.0005;

	f = popen("wc -l data/ex2data1.txt", "r");
	fscanf(f, "%d", &length);
	pclose(f);

	raw_data_plot 	= popen ("gnuplot -persistent", "w");
	iter_plot		= popen ("gnuplot -persistent", "w");
	rawdata 		= fopen("data/ex2data1.txt", "r");
	logistic_data 	= fopen("data/logistic_data", "w");
	iter			= fopen("data/iter", "w");

	float x[length][3], y[length];
	int pred[length];

	for (i = 0; i < length; i++)
	{
		fscanf(rawdata, "%f %f %f", &x[i][1], &x[i][2], &y[i]);
		x[i][0] = 1;

	}

	printf("cost value: %e\n", cost_func(x, y, theta, length));
	gradient(grad, x, y, theta, length, 3);
	printf("grad(%f, %f, %f)\n", grad[0], grad[1], grad[2]);

	for (i = 0; i < iterations; i++)
	{
		gradient(grad, x, y, theta, length, 3);

		for ( j = 0; j < 3; j++ )
		{
			theta[j] -= alpha*grad[j];
		}

		fprintf(iter, "%d %e %f\n", i, cost_func(x, y, theta, length), theta[0]);
	}

	printf("theta(%f, %f, %f)\n", theta[0], theta[1], theta[2]);
	printf("we get cost value: %f\n", cost_func(x, y, theta, length));

	theta[0] = -25.161272;
	theta[1] = 0.206233;
	theta[2] = 0.201470;

	printf("target cost value: %f\n", cost_func(x, y, theta, length));

	for (i = 0; i < length; i++)
	{
		fprintf(logistic_data, "%f %f\n", x[i][1], -(theta[0] + theta[1]*x[i][1])/theta[2]);
	}

	printf("For a student with scores 45 and 85, we predict an admission probability of %f\n",
			sigmoid(theta[0] + theta[1]*45 + theta[2]*85));

	preditc(x, theta, pred, length, 3);
	int cnt = 0;

	for (i = 0; i < length; i++)
	{
		if ( pred[i] == (int)y[i])
			cnt++;
	}

	printf("Train Accuracy: %d\n", cnt);

	fprintf(raw_data_plot, "set title \"Logistic regression Raw data\"\n");
	fprintf(raw_data_plot, "set xlabel \"Exam 1 score\"\n");
	fprintf(raw_data_plot, "set ylabel \"Exam 2 score\" \n");
	fprintf(raw_data_plot, "set xr [30.0:100.0] \n");
	fprintf(raw_data_plot, "set yr [30.0:100.0] \n");
	fprintf(raw_data_plot, "plot \"< awk '{if($3 == 1) print}' data/ex2data1.txt\" u 1:2 t \"Admitted\" with points pointtype 1,");
	fprintf(raw_data_plot, "     \"< awk '{if($3 == 0) print}' data/ex2data1.txt\" u 1:2 t \"Not admitted\" with points,");
	fprintf(raw_data_plot, "     'data/logistic_data' using 1:2 t \"Decision Boundary\" with lines\n");

	fprintf(iter_plot, "set title \"Cost / iteration\"\n");
	fprintf(iter_plot, "set xlabel \"iteraton\"\n");
	fprintf(iter_plot, "set ylabel \"cost value\" \n");
	fprintf(iter_plot, "plot 'data/iter' using 1:2  with lines\n ");


	fflush(raw_data_plot);
	fflush(iter_plot);

	fclose(logistic_data);
	fclose(rawdata);
	fclose(iter);


	/****************** Regularized logistic regression *******************/
	FILE *raw_data2_plot;

	raw_data2_plot 	= popen ("gnuplot -persistent", "w");

	fprintf(raw_data2_plot, "set title \"Regularized logistic regression Raw data\"\n");
	fprintf(raw_data2_plot, "set xlabel \"Microchip Test 1\"\n");
	fprintf(raw_data2_plot, "set ylabel \"Microchip Test 2\" \n");
	fprintf(raw_data2_plot, "set xr [-1:1.5] \n");
	fprintf(raw_data2_plot, "set yr [-0.8:1.2] \n");
	fprintf(raw_data2_plot, "plot \"< awk '{if($3 == 1) print}' data/ex2data2.txt\" u 1:2 t \"y = 1\" with points pointtype 1,");
	fprintf(raw_data2_plot, "     \"< awk '{if($3 == 0) print}' data/ex2data2.txt\" u 1:2 t \"y = 0\" with points pointtype 7\n");
//	fprintf(raw_data2_plot, "     'data/logistic_data' using 1:2 t \"Decision Boundary\" with lines\n");


	fflush(raw_data2_plot);
	return 0;
}

