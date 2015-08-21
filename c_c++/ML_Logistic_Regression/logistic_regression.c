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

void mapFeature(float *in_x, float *out_x, int length, int featureNum, int ori_featureNum, int polyno)
{
	int i, j, k, l;

	for ( i = 0; i < length; i++ )
	{
		l = 0;

		for ( j = 0; j <= polyno; j++ )
		{
			k = 0;
			while ( (k+j) <= polyno )
			{
				*(out_x + i*featureNum + l) = pow(*(in_x + i*(ori_featureNum+1) + 1), k) * pow(*(in_x + i*(ori_featureNum+1) + 2), j);
				l++;
				k++;
			}
		}
	}
}

double cost_func_reg(float *x, float y[], double theta[], int length, int featureNum, int lamba)
{
	double ret, tmp, reg;
	int i, j;

	ret = tmp = 0;

	for ( i = 0; i < length; i++)
	{
		tmp = 0;
		for ( j = 0; j < featureNum; j++ )
			tmp += *(x+i*featureNum+j)*theta[j];

		ret += ( -y[i]*log( sigmoid(tmp) ) - ( 1 - y[i] )*log( 1 - sigmoid(tmp) ) );

	}

	for ( i = 0; i < featureNum; i++ )
	{
		reg = reg + theta[i]*theta[i];
	}

	reg = reg*lamba/(2*length);

	return ((ret/length) + reg);
}

void gradient_reg(double grad[], float *x, float y[], double theta[], int length, int featureNum, int lamba)
{
	double ret, tmp, reg;
	int i, j, k;

	for ( i = 0; i < featureNum; i++ )
	{
		ret = 0;
		reg = 0;

		for ( j = 0; j < length; j++ )
		{
			tmp = 0;
			for ( k = 0; k < featureNum; k++ )
				tmp += *(x+j*featureNum+k)*theta[k];

			ret += (sigmoid(tmp) - y[j])*(*(x+j*featureNum+i));
		}

		if ( i >= 1 )
			reg = theta[i]*lamba/length;
//		printf("reg: %f\n", reg);

		grad[i] = ret/length + 0;
//		printf("grad: %f\n", grad[i]);
	}

}

int main(void)
{
	/******************* Logistic Regression ************************/
	FILE *raw_data_plot, *iter_plot, *boundary_plot;
	FILE *rawdata, *f, *iter, *logistic_data;
	int length, i, j;
	int iterations = 10000;
	double theta[3] = {0};
	double grad[3];
	float alpha = 0.0005;

	f = popen("wc -l data/ex2data1.txt", "r");
	fscanf(f, "%d", &length);

	raw_data_plot 	= popen ("gnuplot -persistent", "w");
	iter_plot		= popen ("gnuplot -persistent", "w");
	boundary_plot	= popen ("gnuplot -persistent", "w");
	rawdata 		= fopen("data/ex2data1.txt", "r");
	logistic_data 	= fopen("data/logistic_data", "w");
	iter			= fopen("data/iter", "w");

	float x[length][3], y[length];
	int pred[length];

	for ( i = 0; i < length; i++ )
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
	fprintf(iter_plot, "plot 'data/iter' using 1:2  notitle with lines\n ");


	fflush(raw_data_plot);
	fflush(iter_plot);

	fclose(logistic_data);
	fclose(rawdata);
	fclose(iter);


	/****************** Regularized logistic regression *******************/
	FILE *raw_data2_plot, *iter_reg_plot;
	FILE *rawdata2, *iter_reg;

	raw_data2_plot 	= popen ("gnuplot -persistent", "w");
	iter_reg_plot	= popen ("gnuplot -persistent", "w");

	fprintf(raw_data2_plot, "set title \"Regularized logistic regression Raw data\"\n");
	fprintf(raw_data2_plot, "set xlabel \"Microchip Test 1\"\n");
	fprintf(raw_data2_plot, "set ylabel \"Microchip Test 2\" \n");
	fprintf(raw_data2_plot, "set xr [-1:1.5] \n");
	fprintf(raw_data2_plot, "set yr [-0.8:1.2] \n");
	fprintf(raw_data2_plot, "plot \"< awk '{if($3 == 1) print}' data/ex2data2.txt\" u 1:2 t \"y = 1\" with points pointtype 1,");
	fprintf(raw_data2_plot, "     \"< awk '{if($3 == 0) print}' data/ex2data2.txt\" u 1:2 t \"y = 0\" with points pointtype 7\n");
//	fprintf(raw_data2_plot, "     'data/logistic_data' using 1:2 t \"Decision Boundary\" with lines\n");

	f = popen("wc -l data/ex2data2.txt", "r");
	fscanf(f, "%d", &length);

	rawdata2 = fopen("data/ex2data2.txt", "r");
	iter_reg = fopen("data/iter_reg", "w");

	float x_2[length][3], y_2[length];

	for ( i = 0; i < length; i++ )
	{
		fscanf(rawdata2, "%f %f %f", &x_2[i][1], &x_2[i][2], &y_2[i]);
		x_2[i][0] = 1;

		printf("%.3f, %.3f, %.3f\n", x_2[i][1], x_2[i][2], y_2[i]);

	}


	int featureNum = (1+7)*7/2;
	float x_2_map[length][featureNum];
	double theta_reg[featureNum], grad_reg[featureNum];;
	int lamba = 100;

	mapFeature(x_2, x_2_map, length, featureNum, 2, 6);

//	for ( i = 0; i < length; i++ )
//	{
//		for ( j = 0; j < featureNum; j++ )
//			printf("%.3f ", x_2_map[i][j]);
//		printf("\n");
//	}

	printf("Cost at initial theta (zeros): %f\n", cost_func_reg(x_2_map, y_2, theta_reg, length, featureNum, lamba));

	iterations = 200000;

	for (i = 0; i < iterations; i++)
	{
		gradient_reg(grad_reg, x_2_map, y_2, theta_reg, length, featureNum, lamba);

		for ( j = 0; j < featureNum; j++ )
		{
			theta_reg[j] -= alpha*grad_reg[j];
//			printf("%.3f ", theta_reg[j]);
		}
//		printf("\n");

		fprintf(iter_reg, "%d %e %.3f %.3f %.3f\n", i, cost_func_reg(x_2_map, y_2, theta_reg, length, featureNum, lamba), theta_reg[0], theta_reg[1], theta_reg[2]);
	}

	char equ_str[100];
	sprintf(equ_str, "%f+%f*x+%f*x**2+%f*x**3+%f*x**4+%f*x**5+%f*x**6+"
					 "%f*y+%f*x*y+%f*x**2*y+%f*x**3*y+%f*x**4*y+%f*x**5*y+"
					 "%f*y**2+%f*x*y**2+%f*x**2*y**2+%f*x**3*y**2+%f*x**4*y**2+"
					 "%f*y**3+%f*x*y**3+%f*x**2*y**3+%f*x**3*y**3+"
					 "%f*y**4+%f*x*y**4+%f*x**2*y**4+"
					 "%f*y**5+%f*x*y**5+"
					 "%f*y**6",
					theta_reg[0], theta_reg[1], theta_reg[2], theta_reg[3], theta_reg[4], theta_reg[5], theta_reg[6],
					theta_reg[7], theta_reg[8], theta_reg[9], theta_reg[10], theta_reg[11], theta_reg[12],
					theta_reg[13], theta_reg[14], theta_reg[15], theta_reg[16], theta_reg[17], theta_reg[18],
					theta_reg[19], theta_reg[20], theta_reg[21], theta_reg[22], theta_reg[23], theta_reg[24],
					theta_reg[25], theta_reg[26], theta_reg[27]);
//	printf("%s",equ_str);


	fprintf(iter_reg_plot, "set title \"Regu Cost / iteration\"\n");
	fprintf(iter_reg_plot, "set xlabel \"iteratons\"\n");
	fprintf(iter_reg_plot, "set ylabel \"cost value\" \n");
	fprintf(iter_reg_plot, "plot 'data/iter_reg' using 1:2 t \"cost\" with lines\n ");

	fprintf(boundary_plot, "set title \"lambda = 100\"\n");
	fprintf(boundary_plot, "set contour\n");
	fprintf(boundary_plot, "set cntrparam levels discrete 0\n");
	fprintf(boundary_plot, "set view map\n");
	fprintf(boundary_plot, "unset surface\n");
	fprintf(boundary_plot, "set isosamples 1000,1000\n");
	fprintf(boundary_plot, "set xlabel \"Microchip Test 1\"\n");
	fprintf(boundary_plot, "set ylabel \"Microchip Test 2\" \n");
	fprintf(boundary_plot, "set xr [-1:1.5] \n");
	fprintf(boundary_plot, "set yr [-0.8:1.2] \n");

	for (i = 0; i < length; i++)
	{
		if ( y_2[i] == 0)
			fprintf(boundary_plot, "set object %d circle at %f,%f radius char 0.1 fillstyle empty border lc rgb '#FFFF00' lw 2\n",
				i+1, x_2[i][1], x_2[i][2]);
		else
			fprintf(boundary_plot, "set object %d circle at %f,%f radius char 0.1 fillstyle empty border lc rgb '#FF0000' lw 2\n",
							i+1, x_2[i][1], x_2[i][2]);
	}

	fprintf(boundary_plot, "splot %s t \"Boundary\"\n", equ_str);

	fflush(raw_data2_plot);
	fflush(iter_reg_plot);
	fflush(boundary_plot);
	return 0;
}

