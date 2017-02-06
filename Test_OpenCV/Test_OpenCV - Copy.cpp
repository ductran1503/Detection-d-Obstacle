// Test_OpenCV.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv\cv.h"


#include <opencv2/opencv.hpp>
#include "opencv\highgui.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>


#define pi 3.1416;
#define MIN_INTENSITY 30
#define MIN_STATURATION 30
#define MIN_PIXELHUE 13 //20
#define MIN_PIXELINT 13 //30
#define ecartFenetre 30
#define hauteurFenetre 120
#define angleFenetre 45
using namespace cv;
using namespace std;

void resetHistogram(long A[]);

void filterAverage(long histogram[], int radius, int size)
{
	long filtred_histogram[361];
	if (size < 2 * radius + 1)
	{
		cout << "error" << endl;
		return;
	}
	resetHistogram(filtred_histogram);
	for (int i = 1; i <= radius; i++)
	{
		for (int j = 1; j <= i + radius; j++)
		{
			filtred_histogram[i] = filtred_histogram[i] + histogram[j];
		}
		filtred_histogram[i] = filtred_histogram[i] / (2 * radius + 1);
	}
	for (int i = size - radius + 1; i <= radius; i++)
	{
		for (int j = i - radius; j <= size; j++)
		{
			filtred_histogram[i] = filtred_histogram[i] + histogram[j];
		}
		filtred_histogram[i] = filtred_histogram[i] / (2 * radius + 1);
	}
	for (int i = radius + 1; i <= size - radius; i++)
	{
		for (int j = i - radius; j <= i + radius; j++)
		{
			filtred_histogram[i] = filtred_histogram[i] + histogram[j];
		}
		filtred_histogram[i] = filtred_histogram[i] / (2 * radius + 1);
	}
	for (int i = 1; i <= size; i++)
	{
		histogram[i] = filtred_histogram[i];
	}
	return;
}

void validatePixel(Mat im, Mat *mask, int min_intensity, int min_staturation)
{
	int nbrows = im.rows;
	int nbcols = im.cols;
	int i, j;
	for (i = 0; i < nbrows; i++)
	{
		for (j = 0; j < nbcols; j++)
		{
			Vec3b pixel = im.at<Vec3b>(i, j);
			mask->at<uchar>(i, j) = (pixel.val[2] > min_intensity) + ((pixel.val[1] > min_staturation) << 1);
		}
	}

}
// show the histograme of the interested zone
void showHistogram(long histogramH[], long histogramV[])
{
	int i;
	for (i = 1; i < 256; i++)
	{
		printf("%d   %d  \n", histogramH[i], histogramV[i]);
	}
}

void resetHistogram(long A[])
{

	int j;
	for (j = 0; j <361; j++)
	{
		A[j] = 0;
	}
}

//
void fonction_trapeze(Mat img, int d, int h, double alpha)
{
	int posX = img.rows;
	int posY = img.cols;

	int linetype = 4;
	Point trapeze[1][4];
	alpha = alpha * 0.01744;
	trapeze[0][0] = Point(d, posX - 10);
	trapeze[0][1] = Point(d + cos(alpha)*h, posX - 10 - sin(alpha)*h);
	trapeze[0][2] = Point(posY - d - cos(alpha)*h, posX - 10 - sin(alpha)*h);
	trapeze[0][3] = Point(posY - d, posX - 10);
	const Point* pts[1] = { trapeze[0] };
	int npts[] = { 4 };
	polylines(img, pts, npts, 1, 1, Scalar(0, 255, 0), 1, 8, 0);

}

// histograme calculation in the interestd zone. 
void calHistogram(Mat img, int d, int h, float alpha, long histogramH[], long histogramV[], Mat masque, long hue_thresholds, long intensity_thresholds)
{
	alpha = alpha * 0.01744;
	int nbr_rows = img.rows;
	int nbr_cols = img.cols;
	int X0 = d;
	int Y0 = nbr_rows - 10;
	int Y1 = nbr_rows - 10 - sin(alpha)*h;
	int X3 = nbr_cols - d;
	int i, j;
	Mat img_ext(nbr_rows, nbr_cols, img.type());
	resetHistogram(histogramH);
	resetHistogram(histogramV);

	//calcule the apparence nombre of each value H,V
	for (j = Y1; j<Y0; j++)
	{
		int first_x = X0 + (nbr_rows - 10 - j) / tan(alpha);
		int last_x = X3 - (nbr_rows - 10 - j) / tan(alpha);
		for (i = first_x; i <= last_x; i++)
		{
			Vec3b data = img.at<Vec3b>(Point(i, j));
			histogramH[data.val[0]] = histogramH[data.val[0]] + (masque.at<uchar>(j,i) == 3);
			histogramV[data.val[2]]++;
			img_ext.at<Vec3b>(Point(i, j)) = data;
		}
	}
	imshow("image extraite", img_ext);
	for (int i = 0; i < 361; i++)
	{
		histogramH[i] = (histogramH[i] < hue_thresholds);
		histogramV[i] = (histogramV[i] < intensity_thresholds);
	}
}

// obstacle detection
void detectObstacle(Mat im, long histogramH[], long histogramV[])
{
	Mat detection(im.rows, im.cols, CV_8UC1);
	int nbrows = im.rows;
	int nbcols = im.cols;
	int i, j;
	for (i = 0; i < nbrows; i++)
	{
		for (j = 0; j < nbcols; j++)
		{
			Vec3b pixel = im.at<Vec3b>(i,j);
			detection.at<uchar>(i, j) = 255* histogramH[pixel.val[0]]*histogramV[pixel.val[2]];
		}
	}
	
	imshow("detection", detection);
}

int main(int argc, const char** argv)
{
	int d1;
	Mat imgHSV, img_Filter;
	int n = 5;
	long histogramH[361];
	long histogramV[361];
	int sigma = (n - 1) / 6;
	Mat img = imread("image5.jpg", CV_LOAD_IMAGE_UNCHANGED);

	if (img.empty())
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
		//system("pause"); //wait for a key press
		return -1;
	}

	GaussianBlur(img, img_Filter, Size(n, n), sigma, 0);

	cvtColor(img_Filter, imgHSV, COLOR_BGR2HSV);

	int posX = img.rows;
	int posY = img.cols;

	//generer une masque des pixels validés
	Mat masque(img.rows, img.cols, CV_8UC2);

	validatePixel(imgHSV, &masque, MIN_INTENSITY, MIN_STATURATION);

	fonction_trapeze(img_Filter, ecartFenetre, hauteurFenetre, angleFenetre);

	resetHistogram(histogramH);
	resetHistogram(histogramV);

	calHistogram(imgHSV, ecartFenetre, hauteurFenetre, angleFenetre, histogramH, histogramV, masque, MIN_PIXELHUE, MIN_PIXELINT);

	imshow("Image initiale", img_Filter);

	imshow("Image HSV", imgHSV);

	showHistogram(histogramH, histogramV);

	cout << "\nafter filtring " << endl;

	filterAverage(histogramH, 1, 360);
	filterAverage(histogramV, 1, 350);
	detectObstacle(imgHSV, histogramH, histogramV);
	//showHistogram(histogramH, histogramV);

	waitKey(0); //wait infinite time for a keypress

	return 0;
}