#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

string imageName("lotus.png");          // Image by default

void seuillage(const Mat src, Mat dest, int seuil);
vector<int> histogramme(const Mat image, int intervalle);
int calculeVariance(Mat* image, int seuil, vector<int> histogram); // modified signature to decrease complexity
int seuillageOtsu(Mat* image);
double* kmean(int* histo, int nbClasse);

int main( int argc, char** argv )
{
  Mat image;
  int seuil = -1;
  if( argc > 1)
  {
    imageName = argv[1];
  }

  image = imread(imageName.c_str(), IMREAD_GRAYSCALE); // Read the file
  if( image.empty() )                          // Check for invalid input
  {
    std::cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  if(argc > 2){
    sscanf(argv[2], "%d", &seuil);
  }
  
  const int HISTOGRAM_INTERVALS = 3;
  const int CLASSES = 3; // for kmeans ** MUST BE >= 2 **
  
  // seuillage
  string window_name = "Image originale";
  string window_modified = "Image convertie";
  Mat image_convertie = Mat(image.size(), CV_8UC1, Scalar(0));
  seuillage(image, image_convertie, seuil);
  namedWindow( window_name, WINDOW_AUTOSIZE );
  imshow( window_name, image );
  namedWindow( window_modified, WINDOW_AUTOSIZE );
  imshow(window_modified, image_convertie);
  waitKey(0);
  
  // histogramme
  int intervals[HISTOGRAM_INTERVALS] = { 1, 10, 20 };
  for(int i=0; i < HISTOGRAM_INTERVALS; ++i){
    vector<int> hist = histogramme(image, intervals[i]);
    cout << "Histogramme de " << imageName << " avec un intervalle de " << intervals[i] << " est :\n";
    cout << "Valeur  :\t";
    for(int k=0; k<256/intervals[i]+1; ++k){
      cout << k*intervals[i] << "-" << (k+1)*intervals[i]-1 << "\t";
    }
    cout << "\nQuantité:\t";
    for(int k=0; k<256/intervals[i]+1; ++k){
      cout << hist[k] << "\t";
    }
    cout << "\n";
  }
  
  // kmeans
  vector<int> hist = histogramme(image, 1);
  int hist_array[hist.size()];
  for(unsigned int i=0; i<hist.size(); ++i){
    hist_array[i] = hist[i];
  }
  double* kmeans = kmean(hist_array, CLASSES);
  for(int i=0; i < CLASSES; ++i){
  }
  int tresholds [CLASSES-1];
  for(int i=0; i < CLASSES-1; ++i){
    tresholds[i] = (kmeans[i] + kmeans[i+1]) / 2;
  }
  // build the image
  for(int i = 0; i < image.size().width; ++i){
    for(int j = 0; j < image.size().height; ++j){
      uchar intensity = image.at<uchar>(j, i);
      if(intensity <= tresholds[0]){
        image_convertie.at<uchar>(j,i) = 0;
      }
      for(int k=0; k < CLASSES-2; ++k){
        if(intensity > tresholds[k] && intensity <= tresholds[k+1]){
          image_convertie.at<uchar>(j,i) = 255 / (CLASSES - k);
        }
      }
    }
  }
  imwrite("kmeans.png", image_convertie);
  return 0;
}

void seuillage(const Mat src, Mat dest, int seuil){
  bool otsu = false;
  if(seuil < 0){
    Mat m = src;
    seuil = seuillageOtsu(&m);
    otsu = true;
  }
  for(int i = 0; i < src.size().width; ++i){
    for(int j = 0; j < src.size().height; ++j){
      uchar intensity = src.at<uchar>(j, i);
      if(intensity < seuil){
        dest.at<uchar>(j,i) = 0;
      }
      else{
        dest.at<uchar>(j,i) = 255;
      }
    }
  }
  if(otsu == true){
    imwrite(to_string(seuil) + imageName, dest);
  }
}

vector<int> histogramme(const Mat image, int intervalle){
  vector<int> hist = vector<int>(256/intervalle+1);
  for(int i = 0; i < image.size().width; ++i){
    for(int j = 0; j < image.size().height; ++j){
      uchar intensity = image.at<uchar>(j, i);
      for(int k=0; k < 256/intervalle+1; ++k){
        if(intensity >= k*intervalle && intensity <= (k+1)*intervalle-1){
          ++hist[k];
        }
      }
    }
  }
  return hist;
}

int calculeVariance(Mat* image, int seuil, vector<int> histogram){ // modified signature to decrease compexity
  float p1 = 0, p2 = 0, mean1 = 0, mean2 = 0; // Better precision for intermediate results
  int total_pixels = image->size().width * image->size().height;
  // calculate probabilities
  for(int i=0; i < seuil; ++i){
    p1 = p1 + ((float)histogram[i]) / total_pixels;
  }
  for(int i=seuil; i < 256; ++i){ // I always use interval 1 for histogram
    p2 = p2 + ((float)histogram[i]) / total_pixels;
  }
  // calculate means
  for(int i=0; i < seuil; ++i){
    mean1 = mean1 + ((i * (((float)histogram[i]) / total_pixels)) / p1);
  }
  for(int i=seuil; i < 256; ++i){
    mean2 = mean2 + ((i * (((float)histogram[i]) / total_pixels)) / p2);
  }
  return p1 * p2 * pow((mean1 - mean2), 2);
}

int seuillageOtsu(Mat* image){
  int variance, max_variance = 0, best_seuil = 0;
  vector<int> histogram = histogramme(*image, 1);
  for(int i=1; i < 255; i++){ // I skip the single class cases
    variance = calculeVariance(image, i, histogram);
    if(variance > max_variance){
      max_variance = variance;
      best_seuil = i;
    }
  }
  return best_seuil;
}

double* kmean(int* histo, int nbClasse){
  bool go_on = true;
  double* vals = new double[nbClasse];
  int limits [nbClasse+1];
  // initialise values
  srand(time(NULL));
  for(int i=0; i < nbClasse; ++i){
    vals[i] = rand() % 255;
  }
  sort(vals, vals + nbClasse);
  limits[0] = 0, limits[nbClasse] = 255; // edges of the histogram
  //main loop
  while (go_on){
    go_on = false;
    // find intraclass points
    for(int i=1; i < nbClasse; ++i){
      limits[i] = (vals[i-1] + vals[i]) / 2;
    }
    
    // find new means
    for(int i=0; i < nbClasse; ++i){  // for each class
      int num_pixels = 0;
      for(int j=limits[i]; j < limits[i+1]; ++j){
        num_pixels = num_pixels + histo[j];
      }
      double mean = 0;
      for(int j=limits[i]; j < limits[i+1]; ++j){
        mean = mean + (j * histo[j]);
      }
      mean = mean / num_pixels;
      if(vals[i] != mean){
        go_on = true;
      }
      vals[i] = mean;
    }
  }
  return vals;
}

