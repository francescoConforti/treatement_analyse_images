#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

string imageName("lotus.png");          // Image by default
string window_name("image");
Mat imgcolor; // To show the image with colored germes
Mat image;    // working image
vector<Point> germes;

void germeSelect(int event, int x, int y, int flags, void* userdata);

int main( int argc, char** argv )
{
  if( argc > 1)
  {
    imageName = argv[1];
  }

  imgcolor = imread(imageName.c_str(), IMREAD_COLOR); // Read the file
  if( imgcolor.empty() )                          // Check for invalid input
  {
    std::cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  cvtColor(imgcolor,image,COLOR_BGR2GRAY);
  
  namedWindow( window_name, WINDOW_AUTOSIZE );
  imshow( window_name, imgcolor );
  
  setMouseCallback(window_name, germeSelect );
  waitKey(0);
}

void germeSelect(int event, int x, int y, int flags, void* userdata){
  if( event != EVENT_LBUTTONDOWN ){
    return;
  }
  germes.push_back(Point(x, y));
  circle(imgcolor, germes.back(), 3, Scalar(0, 255, 0), 5);
  imshow( window_name, imgcolor );
}
