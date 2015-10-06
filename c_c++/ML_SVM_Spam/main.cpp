/*
 * main.c
 *
 *  Created on: Oct 1, 2015
 *      Author: iron
 */
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cctype>
#include <sys/types.h>
#include <regex.h>
#include "svm.h"
#include "stem.h"
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"

using namespace std;
using namespace arma;

void getVocabList(map<int, string> &vocab)
{
	FILE *fid;
	int num = 1899, i, index;
	char str[100];

	fid = fopen("data/vocab.txt", "r");

	for (i = 0; i < num; i++)
	{
		fscanf(fid, "%d\t%s", &index, str);
		vocab[i].assign(str);
	}

	fclose(fid);

}

void regular(string &file_contents, const char * pattern, string replace)
{
	int cflags = REG_EXTENDED | REG_ICASE ;;
	int status, index, length;
	const size_t nmatch = 1;
	regmatch_t pmatch[nmatch];
	regex_t reg;

	regcomp(&reg, pattern, cflags);
	index = 0;

	while ((file_contents.c_str() + index))
	{
		status = regexec(&reg, file_contents.c_str() + index, nmatch, pmatch, 0);

//		cout<<file_contents.c_str() + index<<endl;
		if (status == REG_NOMATCH){
			cout<<"No Match"<<endl;
			break;
		}
		else if (status == 0)
		{
			length = pmatch[0].rm_eo - pmatch[0].rm_so;

			file_contents.replace(index + pmatch[0].rm_so, length, "");
			file_contents.insert(index + pmatch[0].rm_so, replace);
			index += pmatch[0].rm_eo + replace.length() - length;
		}

	}

}

mat processEmail(string file_contents)
{
	int i;
	mat word_indices(1, 1);
	map<int, string> vocabList;

	getVocabList(vocabList);

	std::transform(file_contents.begin(), file_contents.end(), file_contents.begin(), ::tolower);
//	cout<<file_contents<<endl;

	/* Handle Numbers
	   Look for one or more characters between 0-9 */
	regular(file_contents, "[0-9]+", "number");
//	cout<<file_contents<<endl;

	/* Handle URLS
	   Look for strings starting with http:// or https:// */
	regular(file_contents, "(http|https)://[a-zA-Z0-9\./_]+", "httpaddr");
//	cout<<file_contents<<endl;

	/* Handle Email Addresses
	   Look for strings with @ in the middle */
	regular(file_contents, "[-a-zA-Z0-9_]+@[a-zA-Z0-9\._]+", "emailaddr");
//	cout<<file_contents<<endl;

	/* Handle $ sign */
	regular(file_contents, "[$]+", "dollar");
//	cout<<file_contents<<endl;

	char ori[file_contents.length()];
	char *str;
	char token[] = " :	,.'?>@$&?!(){}<_;%";//"@$/#.-:&*+=[]?!(){},''\">_<;%";
	int res = -1;
	mat index(1, 1);
	string tmp;
	struct stemmer * z = create_stemmer();

	strcpy(ori, file_contents.c_str());

	str = strtok(ori, token);

	word_indices.zeros();

	while (str)
	{
//		cout<<"str: "<<str<<"("<<strlen(str)<<")"<<endl;
		res = stem(z, str, strlen(str)-1);
		str[res+1] = '\0';
//		cout<<"stem: "<<str<<"("<<res<<")"<<endl;

		if (strlen(str) < 1)
			continue;

		for (i = 0; i < (int)vocabList.size(); i++)
		{
//			tmp.assign(str);
//			if (tmp == vocabList[i])
//				cout<<"the same: "<<i<<endl;

			if (strcmp(str, vocabList[i].c_str()) == 0)
			{
				if (word_indices(0, 0) == 0)
					word_indices(0, 0) = i + 1;
				else {
					index(0, 0) = i + 1;
					word_indices = join_cols(index, word_indices);
					cout<<"index: "<<i<<endl;
				}

			}
		}

		str = strtok(NULL, token);

	}
	free_stemmer(z);
	cout<<"size: "<<word_indices.n_rows<<endl;
	return word_indices;
}

mat emailFeatures(mat word_indices)
{
	int i;
	mat x(1899, 1);

	x.zeros();

	for (i = 0; i < (int)word_indices.n_rows; i++)
	{
		x(word_indices(i), 0) = 1;
	}


	return x;
}

int main(void)
{
	int i, j;
	string file_contents, tmp;
	fstream fin;
	char line[100];
	mat word_indices, features;

	fin.open("data/emailSample1.txt",ios::in);

	while(fin.getline(line,sizeof(line),'\n'))
	{
		tmp.assign(line);
		file_contents += tmp;
	}

	//==================== Feature Extraction ====================
	word_indices = processEmail(file_contents);
	features = emailFeatures(word_indices);

	cout<<"Number of non-zero entries: "<<accu(features)<<endl;

	//=========== Train Linear SVM for Spam Classification ========
	int sample_num = 4000, featrue_num = 1899;
	FILE *f_X, *f_y;
	mat X(sample_num, featrue_num), y(sample_num, 1);
	MODEL model;
	double C = 0.1, pred = 0, sum = 0;

	f_X = fopen("data/X.dat", "r");
	f_y = fopen("data/y.dat", "r");

	for (i = 0; i < sample_num; i++)
	{
		for (j = 0; j < featrue_num; j++)
			fscanf(f_X, "%lf", &X(i, j));
		fscanf(f_y, "%lf", &y(i, 0));
	}

	model = svmTrain(X, y, C, linear, 0.001, 5, 0);

	for (i = 0; i < sample_num; i++)
	{
		pred = svmPredict(model, X.row(i), linear, 0);

		if (pred >= 0)
			pred = 1;
		else
			pred = 0;

		if (pred == y(i, 0))
			sum++;
	}

	sum = sum/sample_num;
	cout<<"Training Accuracy: "<<sum*100<<endl;

	//=================== Test Spam Cl ===assification=============
	sample_num = 1000;
	X = mat(sample_num, featrue_num);
	y = mat(sample_num, 1);

	f_X = fopen("data/X.dat", "r");
	f_y = fopen("data/y.dat", "r");

	for (i = 0; i < sample_num; i++)
	{
		for (j = 0; j < featrue_num; j++)
			fscanf(f_X, "%lf", &X(i, j));
		fscanf(f_y, "%lf", &y(i, 0));
	}

	for (i = 0; i < sample_num; i++)
	{
		pred = svmPredict(model, X.row(i), linear, 0);

		if (pred >= 0)
			pred = 1;
		else
			pred = 0;

		if (pred == y(i, 0))
			sum++;
	}

	sum = sum/sample_num;
	cout<<"Test Accuracy: "<<sum*100<<endl;

	//=================  Top Predictors of Spam ====================
	mat weight;
	map<int, string> vocabList;

	getVocabList(vocabList);
	weight = sort(model.w, 0);

	for (i = 0; i < 15; i++)
	{
		for (j = 0; j < (int)weight.n_rows; j++)
		{
			if (model.w(j, 0) == weight(weight.n_rows-1-i, 0))
				cout<<vocabList[j]<<": "<<"("<<weight(weight.n_rows-1-i, 0)<<")"<<endl;
		}
	}

	//=================== Try Your Own Emails =====================
	pred = svmPredict(model, features.t(), linear, 0);

	if (pred >= 0)
		pred = 1;
	else
		pred = 0;

	printf("Spam Classification: %f\n", pred);
	return 0;
}

