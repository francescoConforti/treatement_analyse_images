#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;


string window_name("Image originale");
string window_name2("Image segmentée");
Mat imgcolor; // To show the image with colored germes
Mat image;    // working image
Mat dst;
vector<Point> germes;
int seuil = 10;

void germeSelect(int event, int x, int y, int flags, void* userdata);
bool predicat(const Mat img, Point g, Point p, int seuil);
void croissanceRegion(const Mat src, Mat dst, vector<Point> germes, int seuil);
vector<Point> findNeighbours(const Mat img, Point p);
bool insideImage(const Mat img, Point p);
void selectionnerSeuil(int, void*);

int main( int argc, char** argv )
{
  string imageName("rice.png");
  
  if( argc > 1)
  {
    imageName = argv[1];
  }

  imgcolor = imread(imageName.c_str(), IMREAD_COLOR); // color for germe highlighting
  if( imgcolor.empty() )
  {
    std::cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  if(argc > 2){
    sscanf(argv[2], "%d", &seuil);
  }
  
  cvtColor(imgcolor,image,COLOR_BGR2GRAY);
  
  namedWindow( window_name, WINDOW_AUTOSIZE );
  imshow( window_name, imgcolor );
  
  setMouseCallback(window_name, germeSelect );
  waitKey(0);
  
  dst = Mat(image.size(), CV_8UC1, Scalar(0));
  croissanceRegion(image, dst, germes, seuil);
  
  imshow(window_name, image);
  namedWindow(window_name2, WINDOW_AUTOSIZE);
  imshow(window_name2, dst);
  createTrackbar("seuil", window_name2, &seuil, 100, selectionnerSeuil);
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

bool predicat(const Mat img, Point g, Point p, int seuil){
  bool res = false;
  uchar intensity_p = img.at<uchar>(p.y, p.x);
  uchar intensity_g = img.at<uchar>(g.y, g.x);
  if(abs(intensity_p - intensity_g) < seuil){
    res = true;
  }
  return res;
}

// dst should be initialised to 0
void croissanceRegion(const Mat src, Mat dst, vector<Point> germes, int seuil){
  int color = 10;
  for(unsigned int i=0; i < germes.size(); ++i){
    dst.at<uchar>(germes[i].y, germes[i].x) = color;
    vector<Point> neighbours = findNeighbours(src, germes[i]);
    while(!neighbours.empty()){
      Point p = neighbours.front();
      neighbours.erase(neighbours.begin());
      if(dst.at<uchar>(p.y, p.x) == 0 && predicat(src, germes[i], p, seuil)){
        dst.at<uchar>(p.y, p.x) = color;
        vector<Point> tmp = findNeighbours(src, p);
        neighbours.insert(neighbours.end(), tmp.begin(), tmp.end());
      }
    }
    color += 10;
  }
}

vector<Point> findNeighbours(const Mat img, Point p){
  vector<Point> res;
  for(int x = -1; x <= 1; ++x){
    for(int y = -1; y <= 1; ++y){
      if(x != 0 && y != 0){ // already have this point
        Point newPoint = Point(p.x + x, p.y + y);
        if(insideImage(img, newPoint)){
          res.push_back(newPoint);
        }
      }
    }
  }
  return res;
}

bool insideImage(const Mat img, Point p){
  return p.x >= 0 && p.x < img.size().width && p.y >= 0 && p.y < img.size().height;
}

void selectionnerSeuil(int, void*){
  dst = Mat(image.size(), CV_8UC1, Scalar(0));
  croissanceRegion(image, dst, germes, seuil);
  imshow(window_name2, dst);
}
