/************************************************************
                            INACHEVÉ
*************************************************************/

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int numChildren = 4;

struct GenericNode  // For trees and graphs
{
    GenericNode ** children;
    int childCount;
    int minX, maxX, minY, maxY; // inclusive
};

GenericNode * tree;

void defineNode(GenericNode& node, int minX, int maxX, int minY, int maxY);
bool splitNode(GenericNode& node);
bool homogeneous(const Mat& img, GenericNode region, int treshold);
void generateTree(const Mat& img, GenericNode& node, int treshold);

int main( int argc, char** argv ){
  string imageName("rice.png");
  int treshold = 10;
  if( argc > 1)
  {
    imageName = argv[1];
  }
  if(argc > 2){
    sscanf(argv[2], "%d", &treshold);
  }
  
  Mat img = imread(imageName.c_str(), IMREAD_COLOR);
  
  tree = new GenericNode();
  defineNode(*tree, 0, img.size().width, 0, img.size().height);
  generateTree(img, *tree, treshold);
  
  list<GenericNode> graph;
}

void defineNode(GenericNode& node, int minX, int maxX, int minY, int maxY){
  node.minX = minX;
  node.maxX = maxX;
  node.minY = minY;
  node.maxY = maxY;
}

// returns true if the node was split successfully
bool splitNode(GenericNode& node){
  bool res;
  int meanX = (node.minX + node.maxX) / 2;
  int meanY = (node.minY + node.maxY) / 2;
  if(meanX == node.minX || meanX == node.maxX ||
     meanY == node.minY || meanY == node.maxY){
    node.childCount = 0;
    node.children = NULL;
    res = false;   
  }
  else{
    node.childCount = numChildren;
    node.children = new GenericNode*[numChildren];
    for(int i=0; i < numChildren; ++i){
      node.children[i] = new GenericNode();
    }
    defineNode(*(node.children[0]), node.minX, meanX, node.minY, meanY);
    defineNode(*(node.children[1]), meanX+1, node.maxX, node.minY, meanY);
    defineNode(*(node.children[2]), node.minX, meanX, meanY+1, node.maxY);
    defineNode(*(node.children[3]), meanX+1, node.maxX, meanY+1, node.maxY);
    res = true;
  }
  return res;
}

bool homogeneous(const Mat& img, GenericNode region, int treshold){
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

void generateTree(const Mat& img, GenericNode& node, int treshold){
  if(!homogeneous(img, node, treshold)){
    splitNode(node);
  }
  if(node.childCount > 0){
    for(int i=0; i < node.childCount; ++i){
      generateTree(img, *(node.children[i]), treshold);
    }
  }
}
