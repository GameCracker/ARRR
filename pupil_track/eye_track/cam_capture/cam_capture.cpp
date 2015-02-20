#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, const char** argv) {
  CvCapture* capture = 0;
  Mat frame, frameCopy, image;
  capture = cvCaptureFromCAM(0); //0=default, -1=any camera, 1..99=your camera
  if(!capture)
    cout << "No camera detected" << endl;
  cvNamedWindow("result", 1);
  if(capture) {
    cout << "In capture ..." << endl;
    for(;;) {
      IplImage* iplImg = cvQueryFrame(capture);
      frame = iplImg;
      if(frame.empty())
	break;
      if(iplImg->origin == IPL_ORIGIN_TL)
	frame.copyTo(frameCopy);
      else
	flip(frame, frameCopy, 0);
      if(waitKey(10) >= 0)
	cvReleaseCapture(&capture);
    }
    waitKey(0);
    cvDestroyWindow("result");
    return 0;
  }
}


