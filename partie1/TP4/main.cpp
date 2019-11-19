#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

Mat DT8_chanfrein(const Mat src);
void printInTerminal(const Mat image);
Mat invertImage(const Mat src);
bool insideMatrix(const Mat src, int y, int x);
void forwardPass(const Mat src, Mat dst, const Mat mask);
void backwardPass(const Mat src, Mat dst, const Mat mask);
void normalizeChanfrein(Mat img);

string window_modified("image modified");
int WAITTIME = 5;
  
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
  
  namedWindow( window_modified, WINDOW_AUTOSIZE );
  Mat modified = DT8_chanfrein(image);
  imwrite("inverted.pgm", modified);
  imshow( window_modified, modified );
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
            waitKey(WAITTIME);
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
            waitKey(WAITTIME);
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
