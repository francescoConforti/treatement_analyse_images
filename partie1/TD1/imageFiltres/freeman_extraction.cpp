#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

class MyPoint{
  public:
    int x;
    int y;
    
  MyPoint(int x, int y){
    this->x=x;
    this->y=y;
  }
  
  bool equals(MyPoint p){
    if(p.x != x || p.y != y){
      return false;
    }
    return true;
  }
};

MyPoint applyFreeman(MyPoint p, int dir);

int main( int argc, char** argv ){
  bool fin = false;
  MyPoint next = MyPoint(0,0);
  MyPoint curr = MyPoint(0,0);
  int contour[100];
  int dir=4, k=0;
  Mat img;
  const char* filename = argc >=2 ? argv[1] : "lettre.pgm";
  img = imread( filename, IMREAD_GRAYSCALE );
  while(!fin){
    while(img.at<uchar>(next.y, next.x) != 255){
      dir = (dir+1)%8;
      next = applyFreeman(curr, dir);
    }
    int dir_init = (dir+5)%8;
    if(k==0){
      MyPoint p0 = new MyPoint(next.x, next.y);
    }
    else if(k==1){
      MyPoint p1 = new MyPoint(next.x, next.y);
    }
    else{
      fin = (curr.equals(p0) && next.equals(p1));
    }
    curr=next;
    next = applyFreeman(curr, dir_init)
    contour[k] = dir;
    ++k;
    dir = dir_init;
  }
}

MyPoint applyFreeman(MyPoint p, int dir){
  if(dir < 0 || dir > 7){
    return p;
  }
  MyPoint res = MyPoint(p.x, p.y);
  switch(dir){
    case 0:
      res.x = res.x +1;
      break;
    case 1:
      res.x = res.x +1;
      res.y = res.y +1;
      break;
    case 2:
      res.y = res.y +1;
      break;
    case 3:
      res.x = res.x -1;
      res.y = res.y +1;
      break;
    case 4:
      res.x = res.x -1;
      break;
    case 5:
      res.x = res.x -1;
      res.y = res.y -1;
      break;
    case 6:
      res.y = res.y -1;
      break;
    case 7:
      res.x = res.x +1;
      res.y = res.y -1;
      break;
  }
  return res;
}
