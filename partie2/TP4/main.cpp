#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

void dilatation(const Mat src, Mat dst, const Mat element);
void erosion(const Mat src, Mat dst, const Mat element);
bool inside_mat(const Mat src, int x, int y);

int main( int argc, char** argv ){
  string imageName("lotus.png");
  string window_name("image_originale");
  string window_modified("image_modified");
  if( argc > 1)
  {
    imageName = argv[1];
  }

  Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
  if( image.empty() )
  {
    std::cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  
  Mat image_modified = Mat(image.size(), CV_8UC1, Scalar(0));
  Mat element = Mat::ones(3,3, CV_8UC1);
  element.at<uchar>(0,0)=0;
  element.at<uchar>(2,2)=0;
  element.at<uchar>(2,0)=0;
  element.at<uchar>(0,2)=0;
  
  dilatation(image, image_modified, element);
  imwrite("dilatation.png", image_modified);
  erosion(image, image_modified, element);
  imwrite("erosion.png", image_modified);
  return 0;
}

void dilatation(const Mat src, Mat dst, const Mat element){
  int xcentre = element.size().width / 2;
  int ycentre = element.size().height / 2;
  for(int i=0; i < src.size().width; ++i){
    for(int j=0; j < src.size().height; ++j){
      int max = -256;
      for(int k=0; k < element.size().width; ++k){
        for(int h=0; h < element.size().height; ++h){
          int xpos = i+k-xcentre;
          int ypos = j+h-ycentre;
          if(inside_mat(src, xpos, ypos)){
            uchar intensity = src.at<uchar>(ypos, xpos);
            if(intensity > max){
              max = intensity;
            }
          }
        }
      }
      dst.at<uchar>(j, i) = max;
    }
  }
}

void erosion(const Mat src, Mat dst, const Mat element){
  int xcentre = element.size().width / 2;
  int ycentre = element.size().height / 2;
  for(int i=0; i < src.size().width; ++i){
    for(int j=0; j < src.size().height; ++j){
      int min = 256;
      for(int k=0; k < element.size().width; ++k){
        for(int h=0; h < element.size().height; ++h){
          int xpos = i+k-xcentre;
          int ypos = j+h-ycentre;
          if(inside_mat(src, xpos, ypos)){
            uchar intensity = src.at<uchar>(ypos, xpos);
            if(intensity < min){
              min = intensity;
            }
          }
        }
      }
      dst.at<uchar>(j, i) = min;
    }
  }
}

bool inside_mat(const Mat mat, int x, int y){
  if(x < 0 || x >= mat.size().width ||
     y < 0 || y >= mat.size().height){
    return false;
   }
   return true;
}
