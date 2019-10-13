#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

float maximum(float x, float y, float z);
void selectionerCouleur(int event, int x, int y, int , void* );
void selectionnerSeuil( int, void* );

using namespace cv;
using namespace std;

string window_name = "Image originale";
string window_modified = "Image convertie";
Mat image;
Mat last_image;
int trackbar_val = 10;
uchar blue_mouse;
uchar green_mouse;
uchar red_mouse;

int main( int argc, char** argv )
{
    string imageName("daisy.png");          // Image by default
    if( argc > 1)
    {
        imageName = argv[1];
    }

    image = imread(imageName.c_str(), IMREAD_COLOR); // Read the file
    if( image.empty() )                          // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    // grey image
    Mat image_grey;
    cvtColor(image, image_grey, COLOR_BGR2GRAY) ;

    //RGB
    Mat image_red = Mat(image.size(), CV_8UC3, Scalar(0,0,0));
    Mat image_blue = Mat(image.size(), CV_8UC3, Scalar(0,0,0));
    Mat image_green = Mat(image.size(), CV_8UC3, Scalar(0,0,0));
    for(int i = 0; i < image.size().width; ++i){
      for(int j = 0; j < image.size().height; ++j){
        Vec3b intensity = image.at<Vec3b>(j, i);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        image_blue.at<Vec3b>(j, i).val[0] = blue;
        image_green.at<Vec3b>(j, i).val[1] = green;
        image_red.at<Vec3b>(j, i).val[2] = red;
      }
    }
    
    //CMYB
    float K, R, G, B;
    Mat image_cmyb = Mat(image.size(), CV_32FC3, Scalar(0,0,0));
    for(int i = 0; i < image.size().width; ++i){
      for(int j = 0; j < image.size().height; ++j){
        Vec3b intensity = image.at<Vec3b>(j, i);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        R = red/255.0; G = green/255.0; B = blue/255.0;
        K = 1 - maximum (R, G, B);
        R = (1-R-K)/(1-K);
        G = (1-G-K)/(1-K);
        B = (1-B-K)/(1-K);
        image_cmyb.at<Vec3f>(j, i).val[0] = B;
        image_cmyb.at<Vec3f>(j, i).val[1] = G;
        image_cmyb.at<Vec3f>(j, i).val[2] = R;
      }
    }
    
    //YCbCr
    float Kr = 0.299, Kb = 0.114, Kg = 0.587, W = 0.5, R2, G2, B2;
    Mat image_ycbcr = Mat(image.size(), CV_32FC3, Scalar(0,0,0));
    for(int i = 0; i < image.size().width; ++i){
      for(int j = 0; j < image.size().height; ++j){
        Vec3b intensity = image.at<Vec3b>(j, i);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        R = red/255.0; G = green/255.0; B = blue/255.0;
        B2 = Kr*R + Kg*G + Kb*B;
        G2 = (W*(B-B2))/(1-Kb);
        R2 = (W*(R-B2))/(1-Kr);
        image_ycbcr.at<Vec3f>(j, i).val[0] = B2;
        image_ycbcr.at<Vec3f>(j, i).val[1] = G2;
        image_ycbcr.at<Vec3f>(j, i).val[2] = R2;
      }
    }
    
    // matrix mul
    Mat image_mat = Mat(image.size(), CV_32FC3, Scalar(0,0,0));
    float vals[] = { 0.6, 0.8, 0.9,
                     0.7, 0.8, 0.8,
                     0.9, 0, 0 };
    float rgb[3];
    Mat modifier = Mat(3, 3, CV_32FC1, vals) * (1/255.0);
    Mat rgb_mat, res_mat;
    for(int i = 0; i < image.size().width; ++i){
      for(int j = 0; j < image.size().height; ++j){
        Vec3b intensity = image.at<Vec3b>(j, i);
        rgb[0] = intensity.val[0];
        rgb[1] = intensity.val[1];
        rgb[2] = intensity.val[2];
        rgb_mat = Mat(3, 1, CV_32FC1, rgb);
        res_mat = modifier * rgb_mat;
        image_mat.at<Vec3f>(j, i).val[0] = res_mat.at<float>(0,0);
        image_mat.at<Vec3f>(j, i).val[1] = res_mat.at<float>(1,0);
        image_mat.at<Vec3f>(j, i).val[2] = res_mat.at<float>(2,0);
      }
    }

    namedWindow( window_name, WINDOW_AUTOSIZE );  // Create a window for display.
    imshow( window_name, image );                 // Show our image inside it.
    namedWindow( window_modified, WINDOW_AUTOSIZE );
    createTrackbar("seuil", window_name, &trackbar_val, 100, selectionnerSeuil);
    selectionnerSeuil(0,0);
    
    // mouse behaviour
    setMouseCallback(window_name, selectionerCouleur );
    
    bool cycle = true;
    while(cycle){
      int key = waitKey(20);
      
      
      switch (key){
        case 'b': // blue
          imshow( window_modified, image_blue );
          last_image = image_blue;
          break;
        case 'g': // green
          imshow( window_modified, image_green );
          last_image = image_green;
          break;
        case 'r': // red
          imshow( window_modified, image_red );
          last_image = image_red;
          break;
        case 'y': // grey
          imshow( window_modified, image_grey );
          last_image = image_grey;
          break;
        case 'c': // CMYB
          imshow( window_modified, image_cmyb );
          last_image = image_cmyb;
          break;
        case 'k': // YCbCr
          imshow( window_modified, image_ycbcr );
          last_image = image_ycbcr;
          break;
        case 'm': // matrix multiplication
          imshow( window_modified, image_mat );
          last_image = image_mat;
          break;
        case 's': // save image
          imwrite("resultat.png", last_image);
          break;
        default:
          if(key != -1){
            cycle = false;
          }
          break;
      }
    }
    
    return 0;
} // END MAIN

bool check_seuil(uchar intensity_mouse, uchar intensity){
  bool res = false;
  if(abs(intensity_mouse - intensity) <= trackbar_val){
    res = true;
  }
  return res;
}

void selectionerCouleur(int event, int x, int y, int , void* ){
  if( event != EVENT_LBUTTONDOWN ){
    return;
  }
  Mat image_mouse = Mat(image.size(), CV_8UC3, Scalar(0,0,0));
  Vec3b intensity_mouse = image.at<Vec3b>(y, x);
  blue_mouse = intensity_mouse.val[0];
  green_mouse = intensity_mouse.val[1];
  red_mouse = intensity_mouse.val[2];
  for(int i = 0; i < image.size().width; ++i){
    for(int j = 0; j < image.size().height; ++j){
      Vec3b intensity = image.at<Vec3b>(j, i);
      uchar blue = intensity.val[0];
      uchar green = intensity.val[1];
      uchar red = intensity.val[2];
      if(check_seuil(blue_mouse, blue) && 
         check_seuil(green_mouse, green) && 
         check_seuil(red_mouse, red)){
        image_mouse.at<Vec3b>(j, i).val[0] = blue;
        image_mouse.at<Vec3b>(j, i).val[1] = green;
        image_mouse.at<Vec3b>(j, i).val[2] = red;
      }
      else{
        image_mouse.at<Vec3b>(j, i).val[0] = 0;
        image_mouse.at<Vec3b>(j, i).val[1] = 0;
        image_mouse.at<Vec3b>(j, i).val[2] = 0;
      }
      imshow( window_modified, image_mouse );
      last_image = image_mouse;
    }
  }
}

void selectionnerSeuil( int, void* ){
  Mat image_trackbar = Mat(image.size(), CV_8UC3, Scalar(0,0,0));
  for(int i = 0; i < image.size().width; ++i){
    for(int j = 0; j < image.size().height; ++j){
      Vec3b intensity = image.at<Vec3b>(j, i);
      uchar blue = intensity.val[0];
      uchar green = intensity.val[1];
      uchar red = intensity.val[2];
      if(check_seuil(blue_mouse, blue) && 
         check_seuil(green_mouse, green) && 
         check_seuil(red_mouse, red)){
        image_trackbar.at<Vec3b>(j, i).val[0] = blue;
        image_trackbar.at<Vec3b>(j, i).val[1] = green;
        image_trackbar.at<Vec3b>(j, i).val[2] = red;
      }
      else{
        image_trackbar.at<Vec3b>(j, i).val[0] = 0;
        image_trackbar.at<Vec3b>(j, i).val[1] = 0;
        image_trackbar.at<Vec3b>(j, i).val[2] = 0;
      }
      imshow( window_modified, image_trackbar );
      last_image = image_trackbar;
    }
  }
}

float maximum(float x, float y, float z) {
  float max = x; /* assume x is the largest */

  if (y > max) { /* if y is larger than max, assign y to max */
    max = y;
  } /* end if */

  if (z > max) { /* if z is larger than max, assign z to max */
    max = z;
  } /* end if */

  return max; /* max is the largest value */
}
