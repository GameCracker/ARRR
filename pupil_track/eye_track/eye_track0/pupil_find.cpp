#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

using namespace cv;
using namespace std;


int main(int argc, char** argv) {
  Mat src = imread("raw.jpg");
  if(src.empty()) {
    return -1;
  }
  Mat gray;
  cout << "!!!" << endl;
  cvtColor(~src, gray, CV_BGR2GRAY);
  imshow("image-gray", src);
  threshold(gray, gray, 220, 225, THRESH_BINARY);
  imshow("image-gray", gray);
  vector<vector<Point> > contours;
  findContours(gray.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
  drawContours(gray, contours, -1, CV_RGB(255, 255, 255), -1);
  /*  for(int i=0; i<contours.size(); i++) {
    double area = contourArea(contours[i]);
    Rect rect = boundingRect(contours[i]);
    int radius = rect.width/2;
    if(area >= 30 && abs(1 - ((double)rect.width/(double)rect.height)) <= 0.2 && abs(1 - area/(CV_PI*pow(radius, 2))) <= 0.2) {
      circle(src, Point(rect.x + radius, rect.y + radius), radius, CV_RGB(255, 0, 0), 2);
    }
    }*/
  //imshow("image", src);
  waitKey(0);
  return 0;
}
