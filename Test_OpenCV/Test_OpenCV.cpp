// Test_OpenCV.cpp : définit le point d'entrée pour l'application console.
//

//#include "stdafx.h"

#include "DetectionObstacle.h"


int main(int argc, const char** argv)
{
	int d1;
	Mat imgHSV, img_Filter;
	int n = 5;
	long histogramH[361];
	long histogramV[361];
	int sigma = (n - 1) / 6;
	//Mat img = imread("image5.jpg", CV_LOAD_IMAGE_UNCHANGED);
	IplImage* iplImg = cvLoadImage("image5.jpg");
	Mat img(iplImg);
	if (img.empty())
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
		//system("pause"); //wait for a key press
		return -1;
	}

	GaussianBlur(img, img_Filter, Size(n, n), sigma, 0);

	cvtColor(img_Filter, imgHSV, COLOR_RGB2HSV);

	int posX = img.rows;
	int posY = img.cols;

	//generer une masque des pixels validés
	Mat masque(img.rows, img.cols, CV_8UC2);

	validatePixel(imgHSV, &masque, MIN_INTENSITY, MIN_STATURATION);

	fonction_trapeze(imgHSV, ecartFenetre, hauteurFenetre, angleFenetre);

	resetHistogram(histogramH);
	resetHistogram(histogramV);

	calHistogram(imgHSV, ecartFenetre, hauteurFenetre, angleFenetre, histogramH, histogramV, masque);
	
	showHistogram(histogramH, histogramV);

	filterAverage(histogramH, 1, 360);
	filterAverage(histogramV, 1, 350);

	classifyHV(histogramH, histogramV, MIN_PIXELHUE, MIN_PIXELINT);
	
	imshow("Image initiale", img_Filter);

	imshow("Image HSV", imgHSV);

	
	Mat detection = detectObstacle(imgHSV, histogramH, histogramV);
	
	IplImage* ipl_detection = cvCloneImage(&(IplImage)detection);
	Mat detection2(ipl_detection);
	imshow("detection", detection2);

	waitKey(0); //wait infinite time for a keypress

	return 0;
}