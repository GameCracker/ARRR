#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdio>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;

int main() {
  Rect faceRect(10, 10, 100, 100);
  Mat sq_face;
  Mat img;
//faceRect = myROI(10, 10, 100, 100);
  CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 352);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 288);
  IplImage* frame;
  for(int i=0; i<25; i++) {
    frame = cvQueryFrame(capture);
  }
  img = image(frame)
  sq_face = img(faceRect).clone();
  printf("Image captured \n");
  cvSaveImage("test.jpg", sq_face);
  printf("Image saved \n");
  cvReleaseCapture(&capture);
  return 0;
}
