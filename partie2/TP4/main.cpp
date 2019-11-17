#include "./treshold.h"

void dilatation(const Mat src, Mat dst, const Mat element);
void erosion(const Mat src, Mat dst, const Mat element);
bool inside_mat(const Mat src, int x, int y);
void ouverture(const Mat src, Mat dst, const Mat element);
void fermeture(const Mat src, Mat dst, const Mat element);
void debuitage(const Mat src, Mat dst, const Mat element);
void gradient_interne(const Mat src, Mat dst, const Mat element);
void gradient_externe(const Mat src, Mat dst, const Mat element);
void gradient_morphologique(const Mat src, Mat dst, const Mat element);
Mat minima(const Mat gradient);
Mat lignePartegeEaux(const Mat src, const Mat element, string imageName);

int main( int argc, char** argv ){
  string imageName("tree.png");
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
  
  //ouverture(image, image_modified, element);
  namedWindow( window_name, WINDOW_AUTOSIZE );
  imshow( window_name, image );
  namedWindow( window_modified, WINDOW_AUTOSIZE );
  /*imshow( window_modified, image_modified );
  waitKey(0);
  fermeture(image, image_modified, element);
  imshow( window_modified, image_modified );
  waitKey(0);
  debuitage(image, image_modified, element);
  imshow( window_modified, image_modified );
  waitKey(0);
  gradient_interne(image, image_modified, element);
  imshow( window_modified, image_modified );
  waitKey(0);
  gradient_externe(image, image_modified, element);
  imshow( window_modified, image_modified );
  waitKey(0);
  gradient_morphologique(image, image_modified, element);
  imshow( window_modified, image_modified );
  waitKey(0);*/
  
  image_modified = lignePartegeEaux(image, element, imageName);
  imshow( window_modified, image_modified);
  waitKey(0);
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

void ouverture(const Mat src, Mat dst, const Mat element){
  Mat tmp = Mat(src.size(), CV_8UC1, Scalar(0));
  erosion(src, tmp, element);
  dilatation(tmp, dst, element);
}

void fermeture(const Mat src, Mat dst, const Mat element){
  Mat tmp = Mat(src.size(), CV_8UC1, Scalar(0));
  dilatation(src, tmp, element);
  erosion(tmp, dst, element);
}

void debuitage(const Mat src, Mat dst, const Mat element){
  Mat tmp = Mat(src.size(), CV_8UC1, Scalar(0));
  ouverture(src, tmp, element);
  fermeture(tmp, dst, element);
}

void gradient_interne(const Mat src, Mat dst, const Mat element){
  Mat tmp = Mat(src.size(), CV_8UC1, Scalar(0));
  erosion(src, tmp, element);
  for(int i=0; i < src.size().width; ++i){
    for(int j=0; j < src.size().height; ++j){
      uchar intensity_original = src.at<uchar>(j, i);
      uchar intensity_tmp = tmp.at<uchar>(j, i);
      dst.at<uchar>(j, i) = intensity_original - intensity_tmp;
    }
  }
}

void gradient_externe(const Mat src, Mat dst, const Mat element){
  Mat tmp = Mat(src.size(), CV_8UC1, Scalar(0));
  dilatation(src, tmp, element);
  for(int i=0; i < src.size().width; ++i){
    for(int j=0; j < src.size().height; ++j){
      uchar intensity_tmp = tmp.at<uchar>(j, i);
      uchar intensity_original = src.at<uchar>(j, i);
      dst.at<uchar>(j, i) = intensity_tmp - intensity_original;
    }
  }
}

void gradient_morphologique(const Mat src, Mat dst, const Mat element){
  Mat tmp_dilatation = Mat(src.size(), CV_8UC1, Scalar(0));
  Mat tmp_erosion = Mat(src.size(), CV_8UC1, Scalar(0));
  dilatation(src, tmp_dilatation, element);
  erosion(src, tmp_erosion, element);
  for(int i=0; i < src.size().width; ++i){
    for(int j=0; j < src.size().height; ++j){
      uchar intensity_dilatation = tmp_dilatation.at<uchar>(j, i);
      uchar intensity_erosion = tmp_erosion.at<uchar>(j, i);
      dst.at<uchar>(j, i) = intensity_dilatation- intensity_erosion;
    }
  }
}

Mat minima(const Mat gradient){
  Mat tmp = Mat(gradient.size(), CV_8UC1, Scalar(0));
  Mat marqueur = Mat(gradient.size(), CV_32SC1, Scalar(0,0,0));
  vector<vector<Point> > contours;
  seuillage(gradient, tmp, -1);
  
  findContours(tmp, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  drawContours(marqueur, contours, -1, Scalar(255, 255, 255), -1);
  return marqueur;
}

Mat lignePartegeEaux(const Mat src, const Mat element, string imageName){
  Mat marker, srcColor;
  cvtColor(src, srcColor, COLOR_GRAY2RGB);
  vector<Vec3b> colorTab;
  Mat gradient = Mat(src.size(), CV_8UC1, Scalar(0));
  gradient_morphologique(src, gradient, element);
  
  // this part substitutes the function minima; in here also
  // distanceTransform and normalize are calculated
  Mat tmp = Mat(gradient.size(), CV_8UC1, Scalar(0));
  Mat marqueur = Mat(gradient.size(), CV_32SC1, Scalar(0,0,0));
  vector<vector<Point> > contours;
  seuillage(gradient, tmp, -1);
  Mat dist;
  distanceTransform(tmp, dist, DIST_L2, 3);
  normalize(dist, dist, 0, 1.0, NORM_MINMAX);
  Mat dist_8u;
  dist.convertTo(dist_8u, CV_8U);
  findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  for (size_t i = 0; i < contours.size(); i++){
    drawContours(marqueur, contours, static_cast<int>(i), Scalar(static_cast<int>(i)+1), -1);
  }
  
  watershed(srcColor, marqueur);
  Mat dst = Mat::zeros(marqueur.size(), CV_8UC3);
  
  vector<Vec3b> colors;
  for (unsigned int i = 0; i < contours.size(); i++){
    int b = theRNG().uniform(0, 255);
    int g = theRNG().uniform(0, 255);
    int r = theRNG().uniform(0, 255);
    colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
  }
  for (int i = 0; i < marqueur.size().width; ++i){
    for (int j = 0; j < marqueur.size().height; ++j){
      int index = marqueur.at<int>(j,i);
      if(index == -1){
        dst.at<Vec3b>(j,i) = Vec3b(0, 0, 0);
      }else{
        dst.at<Vec3b>(j,i) = colors[index-1];
      }
    }
  }
  return dst;
}
