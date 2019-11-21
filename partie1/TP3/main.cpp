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
Mat axeMedian(const Mat dau);

string window_modified("image modified");
int WAITTIME = 2;
  
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
  imwrite("distanceAuFond.pgm", distanceAuFond);
  imshow( window_modified, distanceAuFond );
  waitKey(0);
  Mat axe = axeMedian(distanceAuFond);
  imshow( window_modified, axe );
  waitKey(0);
}

Mat DT8_chanfrein(const Mat src){
  Mat inverted = invertImage(src); // initialization
  Mat res = inverted;
  Mat mask = Mat(Size(3,3), CV_8UC1, Scalar(1));
  mask.at<uchar>(1, 1) = 0;
  forwardPass(inverted, res, mask);
  backwardPass(inverted, res, mask);
  normalizeChanfrein(res);
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
  normalizeChanfrein(res);
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
            cout << j << " " << i << " " <<  candidateValue << "\n";
            imshow(window_modified, dst);
            //waitKey(WAITTIME);
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
            cout << j << " " << i << " " <<  candidateValue << "\n";
            imshow(window_modified, dst);
            //waitKey(WAITTIME);
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
      cout << image.at<uchar>(j, i) << " ";
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
Mat axeMedian(const Mat dau){
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
      if(numBigger != 1){
        res.at<uchar>(j,i) = 0;
      }
    }
  }
  return res;
}

// dau is distance au fond
/*Mat axeMedian(const Mat dau){
  uchar maxVal = 0;
  Point max = Point(0,0);
  for(int i=0; i < dau.size().width; ++i){
    for(int j=0; j < dau.size().height; ++j){
      if(dau.at<uchar>(j,i) > max){
        maxVal = dau.at<uchar>(j,i);
        max = Point(i,j);
      }
    }
  }
  Mat res = Mat(dau.size(), CV_8UC1, Scalar(0));
  vector<Point> axe;
  axe.push_back(max);
  while(!axe.empty()){
    vector<Point> voisinage;
    Point p = axe.back();
    axe.pop_back();
    res.at<uchar>(p.y, p.x) = 255;
    for(int i=-1; i<3; ++i){
      for(int j=-1; j<3; ++j){
        if((j!=0 && i!=0) && insideMatrix(dau, p.y + j, p.x + i)){
          voisinage.push_back(Point(p.y + j, p.x + i));
        }
      }
    }
    sort(voisinage.begin(), voisinage.end());
    voisinage.pop_back();
    axe.push_back(voisinage.back());  // i take the second because the first is the original point
  }
}*/
