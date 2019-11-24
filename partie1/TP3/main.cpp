#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

Mat DT8_chanfrein(const Mat src);
Mat DT4_chanfrein(const Mat src);
void printInTerminal(const Mat image);
Mat invertImage(const Mat src);
bool insideMatrix(const Mat src, int y, int x);
void forwardPass(const Mat src, Mat dst, const Mat mask);
void backwardPass(const Mat src, Mat dst, const Mat mask);
void normalizeChanfrein(Mat img);
Mat axeMedian_DT8(const Mat dau);
Mat axeMedian_DT4(const Mat dau);
int yokoi_DT4(const Mat img, int y, int x);
int yokoi_DT8(const Mat img, int y, int x);
int isForeground(uchar pixelValue);
bool isSimplePoint(const Mat img, int y, int x, bool DT8);
bool reduceDistanceVal(Mat mat);
Mat skeletonization(const Mat dau, bool DT8);
Mat skeletonize(Mat image, cv::Size size, int structuring);

string window_modified("image modified");
int WAITTIME = 100;
  
int main( int argc, char** argv ){
  string window_name("image originale");
  string imageName("disque.pgm");
  if( argc > 1){
    imageName = argv[1];
  }

  Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
  if( image.empty() ){
    std::cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  
  namedWindow(window_name, WINDOW_AUTOSIZE);
  imshow(window_name, image);
  namedWindow( window_modified, WINDOW_AUTOSIZE );
  //Mat distanceAuFond = DT8_chanfrein(image);
  Mat distanceAuFond = DT4_chanfrein(image);
  Mat distanceNormalized = distanceAuFond.clone();
  normalizeChanfrein(distanceNormalized);
  imwrite("distanceAuFond.pgm", distanceNormalized);
  imshow( window_modified, distanceNormalized);
  //printInTerminal(distanceNormalized);
  waitKey(0);
  //Mat axe = axeMedian_DT8(distanceAuFond);
  Mat axe = axeMedian_DT4(distanceAuFond);
  imshow( window_modified, axe );
  waitKey(0);
  bool DT8 = false;
  Mat skel = skeletonization(distanceAuFond, DT8);
  imshow( window_modified, skel );
  waitKey(0);
}

Mat DT8_chanfrein(const Mat src){
  Mat inverted = invertImage(src); // initialization
  Mat res = inverted;
  Mat mask = Mat(Size(3,3), CV_8UC1, Scalar(1));
  mask.at<uchar>(1, 1) = 0;
  forwardPass(inverted, res, mask);
  backwardPass(inverted, res, mask);
  return res;
}

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
            /*cout << j << " " << i << " " <<  candidateValue << "\n";
            imshow(window_modified, dst);
            waitKey(WAITTIME);*/
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
            /*cout << j << " " << i << " " <<  candidateValue << "\n";
            imshow(window_modified, dst);
            waitKey(WAITTIME);*/
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

void printInTerminal(const Mat image){
  for(int j=0; j < image.size().height; ++j){
    for(int i=0; i < image.size().width; ++i){
      int val = image.at<uchar>(j, i);
      cout << val << " ";
    }
    cout << "\n";
  }  
}

bool insideMatrix(const Mat src, int y, int x){
  return y >= 0 && y < src.size().height && x >= 0 && x < src.size().width;
}

void normalizeChanfrein(Mat img){
  int max = 0;
  for(int i=0; i < img.size().width; ++i){
    for(int j=0; j < img.size().height; ++j){
      if(img.at<uchar>(j, i) == 255){
        img.at<uchar>(j, i) = 0;
      }else if(img.at<uchar>(j, i) > max){
        max = img.at<uchar>(j, i);
      }
    }
  }
  float scaleFactor = 255.0 / max;
  for(int i=0; i < img.size().width; ++i){
    for(int j=0; j < img.size().height; ++j){
      img.at<uchar>(j, i) = img.at<uchar>(j, i) * scaleFactor;
    }
  }
}

// dau is distance au fond
Mat axeMedian_DT8(const Mat dau){
  Mat res = Mat(dau.size(), CV_8UC1, Scalar(255));
  for(int i=0; i < dau.size().width; ++i){
    for(int j=0; j < dau.size().height; ++j){
      uchar pixelVal = dau.at<uchar>(j,i);
      int numBigger = 0;
      for(int k=-1; k<3; ++k){
        for(int h=-1; h<3; ++h){
          if(insideMatrix(dau, j+h, i+k) && dau.at<uchar>(j+h, i+k) > pixelVal){
            ++numBigger;
          }
        }
      }
      if(numBigger > 0 || pixelVal < 2){
        res.at<uchar>(j,i) = 0;
      }
    }
  }
  return res;
}

Mat axeMedian_DT4(const Mat dau){
  Mat res = Mat(dau.size(), CV_8UC1, Scalar(255));
  for(int i=0; i < dau.size().width; ++i){
    for(int j=0; j < dau.size().height; ++j){
      uchar pixelVal = dau.at<uchar>(j,i);
      int numBigger = 0;
      for(int k=-1; k<3; ++k){
        for(int h=-1; h<3; ++h){
          if(insideMatrix(dau, j+h, i+k) && dau.at<uchar>(j+h, i+k) > pixelVal){
            ++numBigger;
          }
        }
      }
      if(numBigger > 1 || pixelVal < 2){
        res.at<uchar>(j,i) = 0;
      }
    }
  }
  return res;
}

int yokoi_DT4(const Mat img, int y, int x){
  if(!insideMatrix(img, y-1, x-1) || !insideMatrix(img, y+1, x+1)){
    return 1;
  }
  int x0 = isForeground(img.at<uchar>(y-1, x));
  int x1 = isForeground(img.at<uchar>(y-1, x+1));
  int x2 = isForeground(img.at<uchar>(y, x+1));
  int x3 = isForeground(img.at<uchar>(y+1, x+1));
  int x4 = isForeground(img.at<uchar>(y+1, x));
  int x5 = isForeground(img.at<uchar>(y+1, x-1));
  int x6 = isForeground(img.at<uchar>(y, x-1));
  int x7 = isForeground(img.at<uchar>(y-1, x-1));
  int x8 = x0;
  return x0 * (1 - x1 * x2) +
         x2 * (1 - x3 * x4) +
         x4 * (1 - x5 * x6) +
         x6 * ( 1 - x7 * x8);
}

int yokoi_DT8(const Mat img, int y, int x){
  if(!insideMatrix(img, y-1, x-1) || !insideMatrix(img, y+1, x+1)){
    return 1;
  }
  int x0 = 1 - isForeground(img.at<uchar>(y-1, x));
  int x1 = 1 - isForeground(img.at<uchar>(y-1, x+1));
  int x2 = 1 - isForeground(img.at<uchar>(y, x+1));
  int x3 = 1 - isForeground(img.at<uchar>(y+1, x+1));
  int x4 = 1 - isForeground(img.at<uchar>(y+1, x));
  int x5 = 1 - isForeground(img.at<uchar>(y+1, x-1));
  int x6 = 1 - isForeground(img.at<uchar>(y, x-1));
  int x7 = 1 - isForeground(img.at<uchar>(y-1, x-1));
  int x8 = x0;
  return x0 * (1 - x1 * x2) +
         x2 * (1 - x3 * x4) +
         x4 * (1 - x5 * x6) +
         x6 * ( 1 - x7 * x8);
}

// returns 1 if foreground or 0 if background
int isForeground(uchar pixelValue){
  if(pixelValue > 0){
    return 1;
  } else{
    return 0;
  }
}

bool isSimplePoint(const Mat img, int y, int x, bool DT8){
  bool res;
  if(DT8){
    res = yokoi_DT8(img, y, x) == 1;
  } else{
    res = yokoi_DT4(img, y, x) == 1;
  }
  return res;
}

//returns true if at least 1 pixel was modified
bool reduceDistanceVal(Mat mat){
  bool modified = false;
  for(int i=0; i < mat.size().width; ++i){
    for(int j=0; j < mat.size().height; ++j){
      uchar val = mat.at<uchar>(j,i);
      if(val > 1){
        mat.at<uchar>(j,i) = val - 1;
        modified = true;
      }
    }
  }
  return modified;
}
/*
Mat skeletonization(const Mat dau, bool DT8){
  Mat skel = dau.clone();
  vector<Point> simples;
  for(int i=0; i < dau.size().width; ++i){
    for(int j=0; j < dau.size().height; ++j){
      if(isSimplePoint(dau, j, i, DT8)){
        simples.push_back(Point(i,j));
      }
    }
  }
  bool done = false;
  while(!simples.empty() && !done){
    vector<Point> next;
    for(Point p : simples){
      if(isSimplePoint(skel, p.y, p.x, DT8)){
        skel.at<uchar>(p.y, p.x) = 0;
        for(int k=-1; k<3; ++k){
          for(int h=-1; h<3; ++h){
            if(!(k==0 && h==0) && insideMatrix(skel, p.y + h, p.x + k)){
              next.push_back(Point(p.x + k, p.y + h));
            }
          }
        }
      }
    }
    simples = {};
    for(Point p : next){
      if(isSimplePoint(skel, p.y, p.x, DT8)){
        simples.push_back(Point(p.x, p.y));
      }
    }
    reduceDistanceVal(skel);
  }
  for(int i=0; i < skel.size().width; ++i){
    for(int j=0; j < skel.size().height; ++j){
      if(skel.at<uchar>(j,i) > 0){
        skel.at<uchar>(j,i) = 255;
      }
    }
  }
  return skel;
}*/

Mat skeletonization(const Mat dau, bool DT8){
  Mat skel = dau.clone();
  for(int n=1; n<=255; ++n){
    for(int reps=0; reps < 2; ++reps){
    for(int i=0; i < skel.size().width; ++i){
      for(int j=0; j < skel.size().height; ++j){
        if(skel.at<uchar>(j,i) == n && isSimplePoint(skel, j, i, DT8)){
          skel.at<uchar>(j,i) = 0;
        }
      }
    }
  }
}
  for(int i=0; i < skel.size().width; ++i){
    for(int j=0; j < skel.size().height; ++j){
      if(skel.at<uchar>(j,i) > 0){
        skel.at<uchar>(j,i) = 255;
      }
    }
  }
  return skel;
}
