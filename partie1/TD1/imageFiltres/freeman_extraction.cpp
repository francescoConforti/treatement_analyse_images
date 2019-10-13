#include <iostream>
#include <string>

using namespace std;

int main( int argc, char** argv ){
  bool fin = false;
  
}

class Point{
  public:
    double x;
    double y;
    
  Point(int x, int y){
    this->x=x;
    this->y=y;
  }
  
  bool equals(Point p){
    if(p.x != x || p.y != y){
      return false;
    }
    return true;
  }
};
