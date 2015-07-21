/*
 * back1.c
 *
 *  Created on: Feb 28, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define	Ncycle 	10000		//學習循環次數
#define	Ntrain	4			//學習樣本個數
#define Ntest	4
#define Ninp	2			//#輸入節點
#define	Nhid	4			//#隱藏層節點
#define Nout	1			//#輸出節點
#define eta		0.5			//學習速率
#define alpha	0.2			//慣性因子
#define	train_file	"/home/iron/program/eclipse/NN_back1/file/xor.txt"	//訓練樣本
#define weight_file "/home/iron/program/eclipse/NN_back1/file/xor.wei"	//儲存訓練完的加權及偏權值
#define mse_file	"/home/iron/program/eclipse/NN_back1/file/xor.mse"	//儲存學習中推論輸出值與目標輸出值的平方誤差和
#define test_file	"/home/iron/program/eclipse/NN_back1/file/xor.txt"
#define recall_file	"/home/iron/program/eclipse/NN_back1/file/xor.rec"

float random_value(void);

void memory()
{
	FILE *fp1, *fp2, *fp3;
	float X[Ninp], T[Nout], H[Nhid], Y[Nout];
	float W_xh[Ninp][Nhid], W_hy[Nhid][Nout];			//W_xh: 輸入層與隱藏層的加權矩陣, W_hy:隱藏層與輸出層的加權矩陣
	float dW_xh[Ninp][Nhid], dW_hy[Nhid][Nout];
	float Q_h[Nhid], Q_y[Nout];							//Q_h: 隱藏層的偏壓, Q_y:輸出層的偏壓
	float dQ_h[Nhid], dQ_y[Nout];
	float delta_h[Nhid], delta_y[Nout];
	float sum, mse;
	int	Icycle;
	int i, j;
	int Itrain;
	int h;

	/*---------- open files ------------*/
	fp1 = fopen(train_file, "r");
	fp2 = fopen(weight_file, "w");
	fp3 = fopen(mse_file, "w");

	if(fp1 == NULL)
	{
		puts("Training file not exist !!");
		exit(1);
	}

	/*---------- initialize weights ----*/
	srand((int)time(0));
	for(h=0;h<Nhid;h++)
		for(i=0;i<Ninp;i++)
		{
			W_xh[i][h] = random_value();
			dW_xh[i][h] = 0;
		}

	for(j=0;j<Nout;j++)
		for(h=0;h<Nhid;h++)
		{
			W_hy[h][j] = random_value();
			dW_hy[h][j] = 0;
		}

	for(h=0;h<Nhid;h++)
	{
		Q_h[h] = 0;//random_value();
		dQ_h[h] = 0;
		delta_h[h] = 0;
	}

	for(j=0;j<Nout;j++)
	{
		Q_y[j] = random_value();
		dQ_y[j] = 0;
		delta_y[j] = 0;
	}

	/*--------- Start Learning --------*/
	for(Icycle=0; Icycle<Ncycle; Icycle++)
	{
		mse = 0.0;
		/*... input one training example ...*/
		fseek(fp1, 0, 0);
		for(Itrain=0; Itrain<Ntrain; Itrain++)
		{
			for(i=0; i<Ninp; i++)
				fscanf(fp1, "%f", &X[i]);
			for(j=0; j<Nout; j++)
				fscanf(fp1, "%f", &T[j]);

			/*...... compute H,Y ......*/
			for(h=0; h<Nhid; h++)
			{
				sum = 0.0;
				for(i=0; i<Ninp; i++)
					sum = sum + X[i]*W_xh[i][h];	// i-th hidden input
				H[h] = (float)1.0/(1.0+exp(-(sum-Q_h[h])));
			}

			for(j=0; j<Nout; j++)
			{
				sum = 0.0;
				for(h=0; h<Nhid; h++)
					sum = sum + H[h]*W_hy[h][j];
				Y[j] = (float)1.0/(1.0 + exp(-(sum-Q_y[j])));
			}

			/*....... compute delta .......*/
			for(j=0; j<Nout; j++)
				delta_y[j] = Y[j]*(1.0 - Y[j])*(T[j] - Y[j]);	//輸出層差距量

			for(h=0; h<Nhid; h++)
			{
				sum=0.0;
				for(j=0; j<Nout; j++)
					sum = sum + W_hy[h][j]*delta_y[j];
				delta_h[h] = H[h]*(1.0 - H[h])*sum;				//隱藏層差距量
			}

			/*....... compute dW, dQ ......*/
			for(j=0 ;j<Nout; j++)
				for(h=0; h<Nhid; h++)
					dW_hy[h][j] = eta*delta_y[j]*H[h] + alpha*dW_hy[h][j];	//dW_hy

			for(j=0; j<Nout ; j++)
				dQ_y[j] = -eta*delta_y[j] + alpha*dQ_y[j];					//dQ_y

			for(h=0; h<Nhid; h++)
				for(i=0; i<Ninp; i++)
					dW_xh[i][h] = eta*delta_h[h]*X[i] + alpha*dW_xh[i][h];	//dW_xh

			for(h=0; h<Nhid; h++)
				dQ_h[h] = -eta*delta_h[h] + alpha*dQ_h[h];					//dQ_h


			/*........ compute new W, Q .........*/
			for(j=0; j<Nout; j++)
				for(h=0; h<Nhid; h++)
					W_hy[h][j] = W_hy[h][j]+dW_hy[h][j];								//new W_hy

			for(j=0; j<Nout; j++)
				Q_y[j] = Q_y[j]+dQ_y[j];											//new Q_y

			for(h=0; h<Nhid; h++)
				for(i=0; i<Ninp; i++)
					W_xh[i][h] = W_xh[i][h]+dW_xh[i][h];								//new W_xh

			for(h=0; h<Nhid; h++)
				Q_h[h] = Q_h[h]+dQ_h[h];											//new Q_h

			/*........ compute the mean_square_error .......*/
			for(j=0; j<Nout; j++)
				mse += (T[j] - Y[j])*(T[j] - Y[j]);
		}	/* end of 1 learning  cycle */

		/*...... Write the mse_value to mse_file .......*/
		mse = mse/Ntrain;
		if((Icycle%10) == 9)
		{
			printf("\nIcycle=%3d mse=%-8.4f\n", Icycle, mse);
			fprintf(fp3, "%3d %-8.4f\n", Icycle, mse);
		}

	}	/* end of total learning cycle */

	/*------- Write the weights to weight_file ---------*/
	printf("\n");
	for(h=0; h<Nhid; h++)
	{
		for(i=0; i<Ninp; i++)
		{
			printf("W_xh[%2d][%2d]=%-8.2f", i, h, W_xh[i][h]);
			fprintf(fp2, "%-8.2f", W_xh[i][h]);
		}
		printf("\n");
		fprintf(fp2, "\n");
	}
	printf("\n");
	fprintf(fp2,"\n");
	for(j=0; j<Nout; j++)
	{
		for(h=0; h<Nhid; h++)
		{
			printf("W_hy[%2d][%2d]=%-8.2f", h, j, W_hy[h][j]);
			fprintf(fp2, "%-8.2f", W_hy[h][j]);
		}
		printf("\n");
		fprintf(fp2, "\n");
	}
	printf("\n");
	fprintf(fp2, "\n");

	for(h=0; h<Nhid; h++)
	{
		printf("Q_h[%2d]=%-8.2f", h, Q_h[h]);
		fprintf(fp2, "%-8.2f", Q_h[h]);
	}
	printf("\n\n");
	fprintf(fp2, "\n\n");

	for(j=0; j<Nout; j++)
	{
		printf("Q_y[%2d]=%-8.2f", j, Q_y[j]);
		fprintf(fp2, "%-8.2f", Q_y[j]);
	}
	printf("\n\n");
	fprintf(fp2, "\n\n");

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

}

void recall()
{
	FILE *fp1, *fp2, *fp3;
	float X[Ninp], T[Nout], H[Nhid], Y[Nout];
	float W_xh[Ninp][Nhid], W_hy[Nhid][Nout];
	float Q_h[Nhid], Q_y[Nout];
	float sum, mse;
	int Itest;
	int i, j, h;


	/*-------------- open file ------------------*/
	fp1 = fopen(weight_file, "r");
	fp2 = fopen(test_file, "r");
	fp3 = fopen(recall_file, "w");

	if(fp1 == NULL)
	{
		printf("weight file not exist !!!\n");
		exit(1);
	}

	if(fp2 == NULL)
	{
		printf("test file not exist !!!\n");
		exit(1);
	}


	/*------------ input weights from weight_file ------*/
	fseek(fp1, 0, 0);
	for(h=0; h<Nhid; h++)
		for(i=0; i<Ninp; i++)
			fscanf(fp1, "%f", &W_xh[i][h]);

	for(j=0; j<Nout; j++)
		for(h=0; h<Nhid; h++)
			fscanf(fp1, "%f", &W_hy[h][j]);

	for(h=0; h<Nhid; h++)
		fscanf(fp1, "%f", &Q_h[h]);

	for(j=0; j<Nout; j++)
		fscanf(fp1, "%f", &Q_y[j]);


	/*----------- start testing --------------*/
	fseek(fp2, 0, 0);
	for(Itest=0; Itest<Ntest; Itest++)
	{
		/*...... input one testing example ........*/
		for(i=0; i<Ninp; i++)
			fscanf(fp2, "%f", &X[i]);
		for(j=0; j<Nout; j++)
			fscanf(fp2, "%f", &T[j]);


		/*....... compute H, Y ........*/
		for(h=0; h<Nhid; h++)
		{
			sum = 0.0;
			for(i=0; i<Ninp; i++)
				sum = sum + X[i]*W_xh[i][h];

			H[h] = (float) 1.0/(1.0+exp(-(sum-Q_h[h])));
		}

		for(j=0; j<Nout; j++)
		{
			sum = 0.0;
			for(h=0; h<Nout; h++)
				sum = sum + H[h]*W_hy[h][j];
			Y[j] = (float)1.0/(1.0+exp(-(sum-Q_y[j])));
		}

		/*........ compute the mean_square_error ......*/
		mse = 0.0;
		for(j=0; j<Nout; j++)
			mse += (T[j] - Y[j])*(T[j] - Y[j]);

		/*--------- write the results to recall_file -------*/
		printf("T[j] = ");
		fprintf(fp3, "T[j] = ");
		for(j=0; j<Nout; j++)
		{
			printf("%-8.2f", T[j]);
			fprintf(fp3, "%-8.2f", T[j]);
		}

		printf("Y[j] = ");
		fprintf(fp3, "Y[j] = ");
		for(j=0; j<Nout; j++)
		{
			printf("%-8.2f", Y[j]);
			fprintf(fp3, "%-8.2f", Y[j]);
		}
		printf(" mse = %-8.4f\n\n", mse);
		fprintf(fp3," mse = %-8.4f", mse);
		fprintf(fp3, "\n");
	}	/* end of recalling for total test examples */

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

}

int main(void) {

	memory();
	recall();
	return 0;
}	/* end of the program */

float random_value(void)
{
	return ((rand()/(RAND_MAX+1.0)) - 0.5);
}
