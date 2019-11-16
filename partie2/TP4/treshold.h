#ifndef TRESHOLD_H
#define TRESHOLD_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

void seuillage(const Mat src, Mat dest, int seuil);
vector<int> histogramme(const Mat image, int intervalle);
int calculeVariance(Mat* image, int seuil, vector<int> histogram);
int seuillageOtsu(Mat* image);
double* kmean(int* histo, int nbClasse);

#endif
