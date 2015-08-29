/*
 * ml_nn_ff.cpp
 *
 *  Created on: Aug 27, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include <armadillo>
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;
using namespace arma;


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


int main(void)
{
	FILE *raw_data_x, *raw_data_y, *raw_theta_1, *raw_theta_2;

	double tmp;
	double *pred;
	int in_layer = 401, hid_layer = 25, out_layer = 10;
	int i, j, k, num;
	int label = 10, featureNum = 401, sampleNum = 5000;

	mat theta_1(hid_layer, featureNum), theta_2(label, hid_layer+1);
	mat raw_x(sampleNum, featureNum-1), raw_y(sampleNum, 1), x(sampleNum, featureNum);


	pred = (double *) malloc(sampleNum*sizeof(double));

	raw_theta_1 = fopen("data/raw_theta_1.dat", "r");
	raw_theta_2 = fopen("data/raw_theta_2.dat", "r");
	raw_data_x = fopen("data/raw_data_x.dat", "r");
	raw_data_y = fopen("data/raw_data_y.dat", "r");

	cout<<"Armadillo version: "<<arma_version::as_string()<<endl;


	for ( i = 0; i < sampleNum; i++ )
	{
		x(i, 0) = 1;
		for ( j = 0; j < 400; j++)
		{
			fscanf(raw_data_x, "%lf", &tmp);
			raw_x(i, j) = tmp;
			x(i, j+1) = tmp;
		}
		fscanf(raw_data_y, "%lf", &tmp);
		raw_y(i, 0) = tmp;
	}

	for ( i = 0; i < hid_layer; i++ )
	{
		for ( j = 0; j < in_layer; j++ )
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

	mat tmp_mat(1,1);
	tmp_mat(0, 0) = 1;
	double tmp_pro, max_pro, id;

	mat t;

	for ( i = 0; i < sampleNum; i++ )
	{
		max_pro = id = 0;
		for ( j = 0; j < label; j++)
		{
			tmp_pro = sigmoid(join_rows(tmp_mat, sigmoid(x.row(i)*theta_1.t()))*theta_2.t())(0, j);

			if ( tmp_pro > max_pro)
			{
				max_pro = tmp_pro;
				id = j + 1;
			}
		}
		pred[i] = id;
	}

	float correct_cnt = 0;
	for ( i = 0; i < sampleNum; i++ )
	{
		if ( pred[i] == raw_y(i, 0))
			correct_cnt++;
	}
	printf("Training Set Accuracy: %f\n", (correct_cnt/sampleNum*100.0));


	srand( (unsigned)time(NULL));

	cv::Mat img = cv::Mat( 20, 20, CV_64FC1);

	while (1)
	{
		num = (int)( rand() /(RAND_MAX+1.0)*sampleNum + 1);

		for ( i = 0; i < 20; i++ )
			for ( j = 0; j < 20; j++ )
				img.at<double>(i, j) = raw_x(num, i+j*20);

		imshow("image",img);
		max_pro = id = 0;

		for ( k = 0; k < label; k++)
		{
			tmp_pro = sigmoid(join_rows(tmp_mat, sigmoid(x.row(num)*theta_1.t()))*theta_2.t())(0, k);

			if ( tmp_pro > max_pro)
			{
				max_pro = tmp_pro;
				id = k + 1;
			}
		}
		printf("pridect: %d\n", ((int)id)%10);
		char key = (char) waitKey(20000);
		if(key == 27) break;
	}

	free(pred);
	fclose(raw_data_x);
	fclose(raw_data_y);
	fclose(raw_theta_1);
	fclose(raw_theta_2);

	return 0;
}

