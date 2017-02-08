
// opencv librairy

#include <stdio.h>
#include <iostream>
#include "opencv\cv.h"

#include "targetver.h"
#include <opencv2/opencv.hpp>
#include "opencv\highgui.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>
using namespace cv;
using namespace std;

#define pi 3.1416;
#define MIN_INTENSITY 30
#define MIN_STATURATION 30
#define MIN_PIXELHUE 13 //20
#define MIN_PIXELINT 13 //30
#define ecartFenetre 30
#define hauteurFenetre 120
#define angleFenetre 45

void resetHistogram(long A[]);
void filterAverage(long histogram[], int radius, int size);
void validatePixel(Mat im, Mat *mask, int min_intensity, int min_staturation);
void showHistogram(long histogramH[], long histogramV[]);
void fonction_trapeze(Mat img, int d, int h, double alpha);
void calHistogram(Mat img, int d, int h, float alpha, long histogramH[], long histogramV[], Mat masque);
Mat detectObstacle(Mat im, long histogramH[], long histogramV[]);
void classifyHV(long histogramH[], long histogramV[], long hue_thresholds, long intensity_thresholds);