#include "treshold.h"

string imageName("lotus.png");          // Image by default

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

