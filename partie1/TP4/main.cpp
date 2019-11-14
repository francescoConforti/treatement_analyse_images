/* ***********************************************
 *      bisogna invertire il valore delle immagini:
 *      la figura è a 0 e il resto != 0
 * ***************************************************/

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

int main( int argc, char** argv ){
  string window_name("image originale");
  string window_modified("image modified");
  string imageName("disque.pgm");
  if( argc > 1){
    imageName = argv[1];
  }

  Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
  if( image.empty() ){
    std::cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  
  Mat modified = DT8_chanfrein(image);
  imwrite("inverted.pgm", modified);
  /*namedWindow( window_name, WINDOW_AUTOSIZE );  // Create a window for display.
  imshow( window_name, image );
  waitKey(0);*/
}

Mat DT8_chanfrein(const Mat src){
  Mat res = invertImage(src); // initialization
  // centro la maschera sul centro (0) e per tutti i punti intorno faccio il min della somma
  return res;
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
