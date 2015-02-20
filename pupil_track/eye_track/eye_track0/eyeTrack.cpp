#include "cv.h"
#include "highgui.h"
#include "opencv2/opencv.hpp"

using namespace cv;

IplImage* GetThresholdedImage(IplImage* img) {
  IplImage *imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
  cvCvtColor(img, imgHSV, CV_BGR2HSV);
  IplImage *imgThresh = cvCreateImage(cvGetSize(img), 8, 1);
  cvInRangeS(imgHSV, cvScalar(0, 84, 0, 0), cvScalar(179, 256, 11, 0), imgThresh);
  cvReleaseImage(&imgHSV);
  return imgThresh;
}

int main(int, char **) {
  IplImage *imgScribble = NULL;
  char c = 0;
  CvCapture *capture;
  capture = cvCreateFileCapture("main.avi");
  if(!capture) {
    printf("Camera could not be initialized");
    exit(0);
  }
  cvNamedWindow("Simple");
  cvNamedWindow("Thresholded");
  while(c != 32) {
    IplImage *img = 0;
    img = cvQueryFrame(capture);
    if(!img)
      break;
    if(imgScribble == NULL)
      imgScribble = cvCreateImage(cvGetSize(img), 8, 3);
    IplImage *timg = GetThresholdedImage(img);
    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(timg, moments, 1);
    double moment10 = cvGetSpatialMoment(moments, 1, 0);
    double moment01 = cvGetSpatialMoment(moments, 0, 1);
    double area = cvGetCentralMoment(moments, 0, 0);
    static int posX = 0;
    static int posY = 0;
    int lastX = posX;
    int lastY = posY;
    posX = moment10/area;
    posY = moment01/area;
    printf("position (%d, %d)\n", posX, posY);
    if(lastX > 0 && lastY > 0 && posX > 0 && posY > 0) {
      cvLine(imgScribble, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0, 255, 255), 5);
    }
    cvAdd(img, imgScribble, img);
    cvShowImage("Simple", img);
    cvShowImage("Thresholded", timg);
    c = cvWaitKey(3);
    cvReleaseImage(&timg);
    delete moments;
  }
  cvReleaseImage(&img);
  cvDestroyWindow("Simple");
  cvDestroyWindow("Thresholded");
  return 0;
}
