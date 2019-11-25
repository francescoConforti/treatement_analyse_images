#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>
#include <stdlib.h>

using namespace cv;
using namespace std;

int aire(const Mat img);
int* boite_englobante(const Mat img);
int hauteur_boite_englobante(int* boite);
int largeur_boite_englobante(int* boite);
float diameter(const Mat img);
Point gravityCentre(const Mat img);

int main( int argc, char** argv ){
  string window_modified("image modified");
  string window_name("image originale");
  string imageName("../disque.pgm");
  if( argc > 1){
    imageName = argv[1];
  }

  Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
  if( image.empty() ){
    std::cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  
  int a = aire(image);
  int* boite = boite_englobante(image);
  int largeur = largeur_boite_englobante(boite);
  int hauteur = hauteur_boite_englobante(boite);
  float diam = diameter(image);
  Point gCentre = gravityCentre(image);
  
  cout << "aire: " << a << "\n";
  cout << "largeur: " << largeur << "\n";
  cout << "hauteur: " << hauteur << "\n";
  cout << "diameter: " << diam << "\n";
  cout << "centre gravité: " << gCentre.x << ", " << gCentre.y << "\n";
}

int aire(const Mat img){
  int count = 0;
  for(int i=0; i < img.size().width; ++i){
    for(int j=0; j < img.size().height; ++j){
      if(img.at<uchar>(j,i) == 0){
        ++count;
      }
    }
  }
  return count;
}

// return a 4 element array with minx maxx miny maxy
int* boite_englobante(const Mat img){
  int *values = (int*)malloc(4*sizeof(int));
  values[0] = img.size().width;
  values[1] = 0;
  values[2] = img.size().height;
  values[3] = 0;
  for(int i=0; i < img.size().width; ++i){
    for(int j=0; j < img.size().height; ++j){
      if(img.at<uchar>(j,i) == 0){
        if(i < values[0]){
          values[0] = i;
        }
        if(i > values[1]){
          values[1] = i;
        }
        if(j < values[2]){
          values[2] = j;
        }
        if(j > values[3]){
          values[3] = j;
        }
      }
    }
  }
  return values;
}

int hauteur_boite_englobante(int* boite){
  return boite[1] - boite[0];
}

int largeur_boite_englobante(int* boite){
  return boite[3] - boite[2];
}

//euclidean distance
float diameter(const Mat img){
  vector<vector<Point>> contours;
  findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
  if(contours.size() > 1){  // there is supposed to be only one shape
    return -1;
  }
  float diameter = 0;
  float distance;
  for(unsigned int i=0; i < contours[0].size()-1; ++i){
    for(unsigned int j=i+1; j < contours[0].size(); ++j){
      Point p1 = contours[0][i];
      Point p2 = contours[0][j];
      distance = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
      if(distance > diameter){
        diameter = distance;
      }
    }
  }
  return diameter;
}

Point gravityCentre(const Mat img){
  int numPixel = 0, sumX = 0, sumY = 0, x, y;
  for(int i=0; i < img.size().width; ++i){
    for(int j=0; j < img.size().height; ++j){
      if(img.at<uchar>(j,i) == 0){
        sumX += i;
        sumY += j;
        ++numPixel;
      }
    }
  }
  x = sumX / numPixel;
  y = sumY / numPixel;
  return Point(x,y);
}
