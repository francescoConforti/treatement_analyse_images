#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

struct QuadNode
{
    QuadNode ** children;
    int childCount;
    int minX, maxX, minY, maxY; // inclusive
};

QuadNode * tree;

bool homogeneous(const Mat& img, QuadNode region, int treshold);

int main( int argc, char** argv ){
  string imageName("rice.png");
  if( argc > 1)
  {
    imageName = argv[1];
  }
}

bool homogeneous(const Mat& img, QuadNode region, int treshold){
  uchar minIntensity = 255, maxIntensity = 0;
  bool res = false;
  for(int i = region.minX; i <= region.maxX; ++i){
    for(int j = region.minY; j <= region.maxY; ++j){
      uchar intensity = img.at<uchar>(j, i);
      if(intensity < minIntensity){
        minIntensity = intensity;
      }
      if(intensity > maxIntensity){
        maxIntensity = intensity;
      }
    }
  }
  if(maxIntensity - minIntensity < treshold){
    res = true;
  }
  return res;
}

/*
class quadNode{
  private:
    int minX, maxX, minY, maxY;
    quadNode children [4];
    
  public:
    quadNode(){}
    quadNode(int minX, int maxX, int minY, int maxY){
      this->minX = minX;
      this->maxX = maxX;
      this->minY = minY;
      this->maxY = maxY;
    }
    int getMinX(){
      return minX;
    }
    int getMaxX(){
      return maxX;
    }
    int getMinY(){
      return minY;
    }
    int getmaxY(){
      return maxY;
    }
};

class quadTree{
  private: quadNode node;
  
  public:
    quadTree(const quadNode& root){
      node = root;
    }
    quadNode getRoot(){
      return node;
    }
};
*/
