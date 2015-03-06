#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdio>

using namespace std;
using namespace cv;

void detectAndDisplay(Mat frame);
int file_count();
int img_count();
string exec_cmd(char* cmd);
string face_cascade_name = "haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Eye & Face detection";
RNG rng(12345);

/** @function main */
int main(int argc, const char** argv) {
  //img_count();
  CvCapture* capture;
  Mat frame;
  if(!face_cascade.load(face_cascade_name)) {
    printf("--(!)Error loading face model\n");
    return -1;
  }
  if(!eyes_cascade.load(eyes_cascade_name)) {
      printf("--(!)Error loading eyes model\n");
      return -1;
  }
    // read video stream
  capture = cvCaptureFromCAM(-1);
  if(capture) {
    while(true) {
      frame = cvQueryFrame(capture);
      if(!frame.empty()) {
	detectAndDisplay(frame);
      }
      else {
	printf("--(!) No captured frame -- Break!");
	break;
      }
      int c = waitKey(10);
      if((char)c == 'c')
	break;
    }
  }
  return 0;
}

string exec_cmd(char* cmd) {
  FILE* pipe = popen(cmd, "r");
  if(!pipe) 
    return "ERROR";
  char buffer[128];
  string result = "";
  while(!feof(pipe)) {
    if(fgets(buffer, 128, pipe) != NULL) {
      result += buffer;
    }
  }
  pclose(pipe);
  cout << "result: " << result << endl;
  return result;
}

int img_count() {
  string cmd = "ls -IR faces/rt_faces/kelly/*.jpg | wc -l";
  char *ccmd = new char[cmd.length() + 1];
  string result = "";
  strcpy(ccmd, cmd.c_str());
  result = exec_cmd(ccmd);
  int num = atoi(result.c_str());
  cout << "num: " << num << endl;
  return num;
}

int file_count() {
  DIR *dp;
  int i;
  struct dirent *ep;
  dp = opendir("./faces/rt_faces/");
  if(dp != NULL) {
    while(ep = readdir(dp)) {
      i++;
    }
    (void) closedir(dp);
  }
  else
    perror("Couldn't open the directory.");
  printf("There's %d files in the current directory.\n", i);
  return i;
}

  /** @function detectAndDisplay */
void detectAndDisplay(Mat frame) {
  std::vector<Rect> faces;
  Mat frame_gray;
  Mat croppedEyes;
  Mat crop;
  //  Mat border;
  Mat res;
  Mat sq_face;
  int ifCrop = 0;
  int save_image = 1;
  int num_face = 0;
  //Rect faceRect(10, 10, 100, 100);
  Rect myROI(10, 10, 100, 100);
  IplImage* img;
  //CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
  cvtColor(frame, frame_gray, CV_BGR2GRAY);
  equalizeHist(frame_gray, frame_gray);
  // -- Detect faces
  face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
  for(size_t i = 0; i < faces.size(); i++) {
    Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
    Mat border(frame_gray.rows, frame_gray.cols, CV_8UC1, Scalar(0, 0, 0));
    Mat border_sq(frame_gray.rows, frame_gray.cols, CV_8UC1, Scalar(0, 0, 0));
    ellipse(border, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 255, 255), -1, 8);
    // if(eyes.size() == 2) {
    // printf("line %d\n", __LINE__);
    // bitwise_and(frame_gray, border, res);
    // imshow("res", res);
    //}
    //imshow("border", border);
    ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
    //bitwise_and(frame, border, res);
    //imshow("res", res);
    Mat faceROI = frame_gray(faces[i]);
    std::vector<Rect> eyes;
    // -- In each face, detect eyes
    eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30));
    if(eyes.size() == 2) {
      Rect sqFace(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
      sq_face = frame_gray(sqFace).clone();
      printf("line %d\n", __LINE__);
      bitwise_and(frame_gray, border, res);
      imshow("square", sq_face);
      imshow("res", res);
    }
    for(size_t j = 0; j < eyes.size(); j++) {
      //      cout << typeid(eyes[j]).name() << endl;
      Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[i].y + eyes[j].height*0.5);
      int radius = cvRound((eyes[i].width + eyes[j].height)*0.25);
      circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
      //sq_face = frame(myROI);
      printf("i: %zu, j: %zu\n", i, j);
      //croppedEyes = frame();
      //crop = frame(myROI);
    }
  }
  if(ifCrop == 1) { 
    cout << "ifCrop = 1" << endl;
    //imshow("cropped eye", croppedEyes);
    //imshow(window_name, crop);
  }
  //imshow(window_name, sq
  imshow(window_name, frame);
}
