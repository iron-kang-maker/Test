/*
 * ml_nerualnetwork_learning.cpp
 *
 *  Created on: Sep 1, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"
#include "fmincg.h"

using namespace cv;
using namespace std;
using namespace arma;

#define epsilon 1e-4

mat log_mat(mat A);
mat sigmoid(mat A);
mat sigmoidGradient(mat A);

void checkNNGradients(double lambda);
void costFunc(COSTDATA *cost_data, double* costVal, mat* grad);
void computeNumericalGradient(void (*costFunc)(COSTDATA *cost_data, COST_FUNC_DATATYPE *cost, mat *grad), COSTDATA *cost_data, mat *grad);

mat log_mat(mat A)
{
	int i, j;

	for ( i = 0; i < (int)A.n_rows; i++ )
		for ( j = 0; j < (int)A.n_cols; j++ )
			A(i, j) = log(A(i,j));

	return A;
}

mat sigmoid(mat A)
{
	int i, j;

	for ( i = 0; i < (int)A.n_rows; i++ )
	{
		for ( j = 0; j < (int)A.n_cols; j++)
		{
			A(i, j) = 1/(1+exp(-A(i, j)));
		}
	}
	return A;
}

mat sigmoidGradient(mat A)
{
	return sigmoid(A)%(1-sigmoid(A));
}

void costFunc(COSTDATA *cost_data, double* costVal, mat* grad)
{
	int i, m;
	double J = 0.0;
	mat a_1, z_2, a_2, z_3, a_3;
	mat theta1 = reshape(cost_data->theta->submat(0, 0, 0, cost_data->hid_layer_size*(cost_data->in_layer_size + 1)-1), cost_data->hid_layer_size, cost_data->in_layer_size + 1);
	mat theta2 = reshape(cost_data->theta->submat(0, cost_data->hid_layer_size*(cost_data->in_layer_size + 1), 0, cost_data->theta->n_cols-1), cost_data->num_label, cost_data->hid_layer_size + 1);
	mat _y(cost_data->num_label, 1);
	mat padding_mat(1,1);
	mat hyp, sub_theta1, sub_theta2;
	mat delta_3, delta_2;
	mat theta1_grad, theta2_grad;

	theta1_grad.copy_size(theta1);
	theta2_grad.copy_size(theta2);

	padding_mat.fill(1);
	m = cost_data->x.n_rows;
	_y.zeros();
	theta1_grad.zeros();
	theta2_grad.zeros();

	/*---------------start calculate cost value--------------*/
	for ( i = 0; i < m; i++ )
	{

		_y(cost_data->y(i, 0)-1, 0) = 1.0;

		hyp = sigmoid(join_rows(padding_mat, sigmoid(cost_data->x.row(i)*theta1.t()))*theta2.t());

		J += accu(log_mat(hyp)*(-_y) - log_mat(mat(1 - hyp))*(mat(1-_y)));

		_y.zeros();
	}

	/* ignore theta(0) item */
	sub_theta1 = theta1.submat(0, 1, cost_data->hid_layer_size-1, cost_data->in_layer_size);
	sub_theta2 = theta2.submat(0, 1, cost_data->num_label-1, cost_data->hid_layer_size);

	/* cost value with regularization */
	J = J/m + ( accu(sub_theta1%sub_theta1) + accu(sub_theta2%sub_theta2) )*cost_data->lambda/(2*m);
	*costVal = J;
	/*-----------------cost value finish---------------------*/

	/*---------------start calculate gradient----------------*/
	for ( i = 0; i < m; i++ )
	{
		/* step 1 */
		a_1 = cost_data->x.row(i).t();      // 401x1
		z_2 = theta1*a_1;                   // 25x1
		a_2 = sigmoid(z_2);                 // 25x1
		a_2 = join_cols(padding_mat, a_2);  // 26x1
		z_3 = theta2*a_2;                   // 10x1
		a_3 = sigmoid(z_3);                 // 10x1

		/* step 2 */
		_y(cost_data->y(i, 0)-1, 0) = 1.0;
		delta_3 = a_3 - _y;                 // 10x1
		delta_2 = (theta2.t()*delta_3)%join_cols(padding_mat, sigmoidGradient(z_2)); // 26x1

		/* step 3 */
		theta1_grad = theta1_grad + delta_2.submat(1, 0, delta_2.n_rows-1, 0)*a_1.t();
		theta2_grad = theta2_grad + delta_3*a_2.t();

		_y.zeros();
	}

	/* gradient with regularization */
	padding_mat = mat(cost_data->hid_layer_size, 1);
	padding_mat.zeros();
	theta1_grad = theta1_grad/m + (cost_data->lambda/m)*(join_rows(padding_mat, theta1.submat(0, 1, theta1.n_rows-1, theta1.n_cols-1)));

	padding_mat = mat(cost_data->num_label, 1);
	padding_mat.zeros();
	theta2_grad = theta2_grad/m + (cost_data->lambda/m)*(join_rows(padding_mat, theta2.submat(0, 1, theta2.n_rows-1, theta2.n_cols-1)));

	/* unroll gradients */
	if (grad)
		*grad = join_rows(reshape(theta1_grad, 1, cost_data->hid_layer_size*(cost_data->in_layer_size+1)), reshape(theta2_grad, 1, cost_data->num_label*(cost_data->hid_layer_size+1)));
	/*-------------calculate gradients finish-------------------*/

}

void checkNNGradients(double lambda)
{
	double J, diff;
	int input_layer_size = 3;
	int hidden_layer_size = 5;
	int num_labels = 3;
	int m = 5, i;
	mat tmp, theta1, theta2, x, y;
	mat unroll, grad, numgrad;
	COSTDATA cost_data;

	tmp = mat(hidden_layer_size*(input_layer_size+1), 1);
	for ( i = 0; i < hidden_layer_size*(input_layer_size+1); i++ )
		tmp(i,0) = sin(i+1)/10;

	theta1 = reshape(tmp, hidden_layer_size, input_layer_size+1);

	tmp = mat(num_labels*(hidden_layer_size+1), 1);
	for ( i = 0; i < num_labels*(hidden_layer_size+1); i++ )
		tmp(i,0) = sin(i+1)/10;

	theta2 = reshape(tmp, num_labels, hidden_layer_size+1);

	tmp = mat(input_layer_size*m, 1);
	for ( i = 0; i < m*input_layer_size; i++ )
		tmp(i, 0) = sin(i+1)/10;

	x = reshape(tmp, m, input_layer_size);

	tmp = mat(m, 1);
	tmp.fill(1);
	x = join_rows(tmp, x);

	for ( i = 0; i < m; i++ )
		tmp(i, 0) = (i+1)%3 + 1;

	y = tmp;

	unroll = join_rows(reshape(theta1, 1, hidden_layer_size*(input_layer_size+1)), reshape(theta2, 1, num_labels*(hidden_layer_size+1)));

	cost_data.x = x;
	cost_data.y = y;
	cost_data.theta = &unroll;
	cost_data.hid_layer_size = hidden_layer_size;
	cost_data.in_layer_size = input_layer_size;
	cost_data.num_label = num_labels;
	cost_data.lambda = lambda;

	costFunc(&cost_data, &J, &grad);
	printf("cost val: %g\n", J);
	computeNumericalGradient(&costFunc, &cost_data, &numgrad);

	for ( i = 0; i < (int)grad.n_cols; i++ )
		printf("%g      %g\n", grad(0, i), numgrad(0, i));

	diff = sqrt(accu((numgrad - grad)%(numgrad - grad)))/sqrt(accu((numgrad + grad)%(numgrad + grad)));
//	printf("%g, %g\n", sqrt(accu((numgrad - grad)%(numgrad - grad))), sqrt(accu((numgrad + grad)%(numgrad + grad))));
	printf("If your backpropagation implementation is correct, then "
	       "the relative difference will be small (less than 1e-9). "
	       "\nRelative Difference: %g\n", diff);

}

void computeNumericalGradient(void (*costFunc)(COSTDATA *cost_data, COST_FUNC_DATATYPE *cost, mat *grad), COSTDATA *cost_data, mat *grad)
{
	mat numgrad, perturb, tmp_theta;
	double loss1, loss2;

	numgrad.copy_size(*cost_data->theta);
	perturb.copy_size(*cost_data->theta);

	numgrad.zeros();
	perturb.zeros();

	tmp_theta = *cost_data->theta;
	int i;
	for ( i = 0; i < (int)cost_data->theta->n_cols; i++ )
	{
		perturb(0, i) = epsilon;
		*cost_data->theta = tmp_theta - perturb;

		costFunc(cost_data, &loss1, NULL);

		*cost_data->theta = tmp_theta + perturb;
		costFunc(cost_data, &loss2, NULL);
		printf("%d: loss1 : %g, loss2 : %g\n", i, loss1, loss2);

		numgrad(0, i) = (loss2 - loss1)/(2*epsilon);
		perturb(0, i) = 0;
	}

	*grad = numgrad;
}


int main(void)
{
	FILE *raw_data_x, *raw_data_y, *raw_theta_1, *raw_theta_2;
	int in_layer = 400, hid_layer = 25, out_layer = 10;
	int i, j, k, num;
	int label_num = 10, sampleNum = 5000;
	double tmp;
	COSTDATA cost_data;

	mat theta_1(hid_layer, in_layer+1), theta_2(label_num, hid_layer+1), theta_unroll;
	mat raw_x(sampleNum, in_layer), raw_y(sampleNum, 1), x(sampleNum, in_layer+1);

	raw_data_x = fopen("data/raw_data_x.dat", "r");
	raw_data_y = fopen("data/raw_data_y.dat", "r");
	raw_theta_1 = fopen("data/raw_theta_1.dat", "r");
	raw_theta_2 = fopen("data/raw_theta_2.dat", "r");

	fstream fin;
	fin.open("data/raw_data_x.dat",ios::in);
//	cout.precision(20);
//	if (!fin.is_open())
//		cout<<"can't open file"<<endl;
//	double f_x;
//	while (fin>>f_x)
//		cout<<f_x<<endl;
//	return 0;
//	while(fin.getline(line,sizeof(line),'\n')){
//	        cout<<line<<endl;
//	    }
//	return 0;

	for ( i = 0; i < sampleNum; i++ )
	{
		x(i, 0) = 1;
		for ( j = 0; j < 400; j++)
		{

			fin>>tmp;
//			fscanf(raw_data_x, "%lf,", &tmp);
			raw_x(i, j) = tmp;
			x(i, j+1) = tmp;
//			cout<<j<<":"<<tmp<<endl;
		}
//		return 0;
		fscanf(raw_data_y, "%lf", &tmp);
		raw_y(i, 0) = tmp;
	}

	for ( i = 0; i < hid_layer; i++ )
	{
		for ( j = 0; j < in_layer+1; j++ )
		{
			fscanf(raw_theta_1, "%lf", &tmp);
			theta_1(i, j) = tmp;
		}
	}


	for ( i = 0; i < out_layer; i++ )
	{
		for ( j = 0; j < hid_layer+1; j++ )
		{
			fscanf(raw_theta_2, "%lf", &tmp);
			theta_2(i, j) = tmp;
		}
	}
	printf("extract data finish.....\n");

	/* check gradient with numerical */
//	checkNNGradients(0);
//	checkNNGradients(3);

	theta_unroll = join_rows(reshape(theta_1, 1, hid_layer*(in_layer+1)), reshape(theta_2, 1, label_num*(hid_layer+1)));
	cost_data.x = x;
	cost_data.y = raw_y;
	cost_data.theta = &theta_unroll;
	cost_data.lambda = 0;
	cost_data.in_layer_size = in_layer;
	cost_data.hid_layer_size = hid_layer;
	cost_data.num_label = label_num;
	double J;
	costFunc(&cost_data, &J, NULL);
	printf("cost val(lambda = 0) : %f\n", J);
	cost_data.lambda = 1;
	costFunc(&cost_data, &J, NULL);
	printf("cost val(lambda = 1) : %f\n", J);
	mat test;
	test<<1<<-0.5<<0<<0.5<<1<<endr;
	printf("Sigmoid gradient evaluated at [1 -0.5 0 0.5 1]:\n");
	test = sigmoidGradient(test);

	cost_data.lambda = 3;
	costFunc(&cost_data, &J, NULL);
	printf("this value should be about 0.576051 : %f\n", J);

	/*-------------- predict --------------*/
	theta_1 = randu(hid_layer, in_layer+1)*2*0.12 - 0.12;
	theta_2 = randu(label_num, hid_layer+1)*2*0.12 - 0.12;
	theta_unroll = join_rows(reshape(theta_1, 1, hid_layer*(in_layer+1)), reshape(theta_2, 1, label_num*(hid_layer+1)));
	cost_data.theta = &theta_unroll;
	cost_data.lambda = 1;
	fmincg(&costFunc, &cost_data, 50);
	theta_1 = reshape(cost_data.theta->submat(0, 0, 0, cost_data.hid_layer_size*(cost_data.in_layer_size + 1)-1), cost_data.hid_layer_size, cost_data.in_layer_size + 1);
	theta_2 = reshape(cost_data.theta->submat(0, cost_data.hid_layer_size*(cost_data.in_layer_size + 1), 0, cost_data.theta->n_cols-1), cost_data.num_label, cost_data.hid_layer_size + 1);

	/*------------predict--------------*/
	mat h1, h2, p, padding_mat;
	p = mat(sampleNum, 1);
	padding_mat = mat(sampleNum, 1);
	padding_mat.fill(1);
	h1 = sigmoid(x*theta_1.t());
	h2 = sigmoid(join_rows(padding_mat, h1)*theta_2.t());

	for ( i = 0; i < sampleNum; i++ )
	{
		tmp = 0;
		num = 0;
		for ( j = 0; j < label_num; j++ )
		{
			if ( h2(i, j) > tmp )
			{
				tmp = h2(i, j);
				num = j + 1;
			}
		}
		p(i, 0) = num;
	}

	/*----------calculate accuracy-----------*/
	mat error_pic(sampleNum, 1);
	error_pic.zeros();
	int err_id = 0;
	float correct_cnt = 0;
	for ( i = 0; i < sampleNum; i++ )
	{
		if ( p(i, 0) == raw_y(i, 0))
			correct_cnt++;
		else
			error_pic(err_id++, 0) = i;
	}
	printf("Training Set Accuracy: %f\n", (correct_cnt/sampleNum*100.0));

	/*-----------display error image----------*/
	cv::Mat img = cv::Mat( 20, 20, CV_64FC1);

	for ( i = 0; i < (sampleNum-correct_cnt); i++ )
	{
		for ( j = 0; j < 20; j++ )
			for ( k = 0; k < 20; k++ )
				img.at<double>(j, k) = raw_x((int)error_pic(i, 0), j+k*20);
		printf("p: %f, raw: %f\n", p((int)error_pic(i, 0),0), raw_y((int)error_pic(i, 0),0));

		imshow("error image",img);
		char key = (char) waitKey(20000);
		if(key == 27) break;
	}

	fclose(raw_data_x);
	fclose(raw_data_y);
	fclose(raw_theta_1);
	fclose(raw_theta_2);
	return 0;
}
