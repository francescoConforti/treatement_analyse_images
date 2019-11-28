#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>

using namespace cv;
using namespace std;

int aire(const Mat img);
int* boite_englobante(const Mat img);
int hauteur_boite_englobante(int* boite);
int largeur_boite_englobante(int* boite);
float diameter(const Mat img);
Point gravityCentre(const Mat img);
int perimeter(const Mat img);
float rapport_isoperimetrique(const Mat img);
float elongation(const Mat img);
int intersections(const Mat dau, float m, int dq = 1);
int crofton(const Mat img, float da = M_PI);
int momentGeometrique(const Mat img, int p, int q);
int momentCentre(const Mat img, int p, int q);
double momentCentreNormalise(const Mat img, int p, int q);

Mat DT4_chanfrein(const Mat src);
void forwardPass(const Mat src, Mat dst, const Mat mask);
void backwardPass(const Mat src, Mat dst, const Mat mask);
Mat invertImage(const Mat src);
bool insideMatrix(const Mat src, int y, int x);

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
  int lh = largeur * hauteur;
  float diam = diameter(image);
  Point gCentre = gravityCentre(image);
  int per = perimeter(image);
  float ri = rapport_isoperimetrique(image);
  float elong = elongation(image);
  int pCroft = crofton(image);
  int m = momentGeometrique(image, 1, 1);
  int mc = momentCentre(image, 1, 1);
  double mcn = momentCentreNormalise(image, 1, 1);
  
  cout << "aire: " << a << "\n";
  cout << "largeur: " << largeur << "\n";
  cout << "hauteur: " << hauteur << "\n";
  cout << "l*h: " << lh << "\n";
  cout << "diameter: " << diam << "\n";
  cout << "centre gravité: " << gCentre.x << ", " << gCentre.y << "\n";
  cout << "perimeter: " << per << "\n";
  cout << "rapport isopérimétrique: " << ri << "\n";
  cout << "elongation: " << elong << "\n";
  cout << "crofton perimeter: " << pCroft << "\n";
  cout << "moment géométrique (p=q=1): " << m << "\n";
  cout << "moment centrée (p=q=1): " << mc << "\n";
  cout << "moment centrée normalizé (p=q=1): " << mcn << "\n";
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
  vector<Point> perimeter;
  float diameter = 0;
  float distance;
  Mat dt = DT4_chanfrein(img);
  for(int i=0; i < dt.size().width; ++i){
    for(int j=0; j < dt.size().height; ++j){
      if(dt.at<uchar>(j,i) == 1){ // the pixel is part of the contour
        perimeter.push_back(Point(i,j));
      }
    }
  }
  for(unsigned int i=0; i < perimeter.size()-1; ++i){
    for(unsigned int j=i+1; j < perimeter.size(); ++j){
      Point p1 = perimeter[i];
      Point p2 = perimeter[j];
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

int perimeter(const Mat img){
  Mat dt = DT4_chanfrein(img);
  int perimeter = 0;
  for(int i=0; i < dt.size().width; ++i){
    for(int j=0; j < dt.size().height; ++j){
      if(dt.at<uchar>(j,i) == 1){ // the pixel is part of the contour
        ++perimeter;
      }
    }
  }
  return perimeter;
}

float rapport_isoperimetrique(const Mat img){
  int a = aire(img);
  int p = perimeter(img);
  return (pow(p,2) * 1.0) / (4.0 * M_PI * a);
}

float elongation(const Mat img){
  return 1 - rapport_isoperimetrique(img);
}

int intersections(const Mat dau, float m, int dq){
  int res = 0;
  for(int q=0; q <= dau.size().height; q+=dq){
    for(int x=0; x < dau.size().width; ++x){  // one line
      int y = m*x+q;
      if(insideMatrix(dau, y, x) && dau.at<uchar>(y,x) == 1){ // intersect contour
        ++res;
      }
    }
  }
  return res;
}

int crofton(const Mat img, float da/* = pi*/){
  Mat dau = DT4_chanfrein(img);
  int res = 0;
  for(int a=0; a <= 2 * M_PI; a+=da){
    res += intersections(dau, a);
  }
  return res;
}

int momentGeometrique(const Mat img, int p, int q){
  int res = 0;
  for(int i=0; i < img.size().width; ++i){
    for(int j=0; j < img.size().height; ++j){
      if(img.at<uchar>(j,i) == 0){
        res += pow(i, p) * pow(j, q);
      }
    }
  }
  return res;
}

int momentCentre(const Mat img, int p, int q){
  Point gCentre = gravityCentre(img);
  int res = 0;
  for(int i=0; i < img.size().width; ++i){
    for(int j=0; j < img.size().height; ++j){
      if(img.at<uchar>(j,i) == 0){
        res += pow(i - gCentre.x, p) * pow(j - gCentre.y, q);
      }
    }
  }
  return res;
}

double momentCentreNormalise(const Mat img, int p, int q){
  int sur = momentCentre(img, p, q);
  int sousse = momentCentre(img, 0, 0);
  return (sur * 1.0) / pow(sousse, ((p+q) / 2) +1);
}

/***************************************
 *    UTILS FOR FINDING CONTOURS
 * ************************************/
 
Mat DT4_chanfrein(const Mat src){
  Mat inverted = invertImage(src); // initialization
  Mat res = inverted;
  Mat mask = Mat(Size(3,3), CV_8UC1, Scalar(1));
  mask.at<uchar>(1, 1) = 0;
  // assign max value to the corners (wich should not be calculated)
  mask.at<uchar>(0, 0) = mask.at<uchar>(2, 2) = mask.at<uchar>(2, 0) = mask.at<uchar>(0, 2) = 255;
  forwardPass(inverted, res, mask);
  backwardPass(inverted, res, mask);
  return res;
}

void forwardPass(const Mat src, Mat dst, const Mat mask){
  for(int j=0; j < src.size().height; ++j){
    for(int i=0; i < src.size().width; ++i){
      for(int k=0; k < mask.size().width; ++k){
        for(int h=0; k+h < mask.size().width && h < mask.size().height-1; ++h){  // last row is not in half-mask; take only left portion of the matrix
          if(insideMatrix(src, j+h-(mask.size().height / 2), i+k-(mask.size().width / 2))){
            int candidateValue = dst.at<uchar>(j+h-(mask.size().height / 2), i+k-(mask.size().width / 2))
                                  + mask.at<uchar>(h, k);
            if(candidateValue < dst.at<uchar>(j, i) && candidateValue <= 255)  // prevent overflow in uchar
              dst.at<uchar>(j, i) = candidateValue;
          }
        }
      }
    }
  }
}

void backwardPass(const Mat src, Mat dst, const Mat mask){
  for(int j = src.size().height - 1; j >= 0 ; --j){
    for(int i = src.size().width - 1; i >= 0 ; --i){
      for(int h=1; h < mask.size().height; ++h){  // first row is not in half-mask
        for(int k=0; k < mask.size().width; ++k){
          if(h >= mask.size().width -1 -k && insideMatrix(src, j+h-(mask.size().height / 2), i+k-(mask.size().width / 2))){  // take only right portion of the matrix
            int candidateValue = dst.at<uchar>(j+h-(mask.size().height / 2), i+k-(mask.size().width / 2))
                                  + mask.at<uchar>(h, k);
            if(candidateValue < dst.at<uchar>(j, i) && candidateValue <= 255)  // prevent overflow in uchar
              dst.at<uchar>(j, i) = candidateValue;
          }
        }
      }
    }
  }
}

Mat invertImage(const Mat src){
  Mat res = Mat(src.size(), CV_8UC1, Scalar(0,0,0));
  for(int i=0; i < src.size().width; ++i){
    for(int j=0; j < src.size().height; ++j){
      if(src.at<uchar>(j, i) == 255){
        res.at<uchar>(j, i) = 0;
      }
      else{
        res.at<uchar>(j, i) = 255;
      }
    }
  }
  return res;
}

bool insideMatrix(const Mat src, int y, int x){
  return y >= 0 && y < src.size().height && x >= 0 && x < src.size().width;
}
