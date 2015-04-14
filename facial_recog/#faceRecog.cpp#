#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
//#include "opencv2/videoio.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <vector>

using namespace std;
using namespace cv;

void detectAndDisplay(Mat frame);
int file_count();
int img_count();
vector<int> total_imgs();
string exec_cmd(char* cmd);
vector<Mat> read_imgs(string dirName);
vector<Mat> read_stored_faces(string dirName);
vector<Mat> read_vary_faces(string rt_dir);
int count_dir_img(string dir);
string face_cascade_name = "haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string fr_window_name = "Face Recognition";
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

vector<int> total_imgs() {
  string dirs[] = {"faces/mike/cc", "faces/alvin/cc", "faces/ethan/cc", "faces/yuki/cc"};
  vector<int> counts;
  for(int i=0; i<4; i++) {
    counts.push_back(0);
  }
  printf("line %d in file %s\n", __LINE__, __FILE__);
  for(int i=0; i<5; i++) {
    int num = count_dir_img(dirs[i]);
    cout << "line 91, num: " << num << endl;
    counts[i] += num;
    printf("line %d in file %s\n", __LINE__, __FILE__);
  }
  printf("line %d in file %s\n", __LINE__, __FILE__);
  return counts;
}

int count_dir_img(string dir) {
  string cmd = "ls -IR " + dir + "/*.jpg | wc -l";
  cout << "line 96: " << cmd << endl;
  char *ccmd = new char[cmd.length() + 1];
  string result = "";
  strcpy(ccmd, cmd.c_str());
  result = exec_cmd(ccmd);
  int num = atoi(result.c_str());
  cout << ".jpg file num: " << num << endl;
  return num;
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

vector<Mat> read_vary_faces(string rt_dir) {
  string dirs[] = {"faces/alvin/cc", "faces/ethan/cc", "faces/yuki/cc"};
  vector<string> dirVec;
  // add real-time detect image
  dirVec.push_back(rt_dir);
  dirVec.push_back(dirs[0]);
  printf("line %d in file %s\n", __LINE__, __FILE__);
  // for(int i=0; i<3; i++) {
  //   dirVec.push_back(dirs[i]);
  // }
  vector< vector<Mat> > imgVecs;
  vector<Mat> allImgs;
  for(int i=0; i<2; i++) {
    imgVecs.push_back(read_imgs(dirVec[i]));
    printf("line %d in file %s\n", __LINE__, __FILE__);
  }
  //allImgs.reserve(5*(imgVecs[0].size()));
  for(int i=0; i<2; i++) {
  	printf("line %d in file %s\n", __LINE__, __FILE__);
    //cout << "image one row size: " << imgVecs[i].size() << endl;
    allImgs.insert(allImgs.end(), imgVecs[i].begin(), imgVecs[i].end());
  }
  return allImgs;
}

vector<Mat> read_imgs(string dirName) {
  Size size(255, 255);
  DIR *dir;
  dir = opendir(dirName.c_str());
  vector<Mat> imgVec;
  struct dirent *ent;
  //string img_n;
  if(dir != NULL) {
    while((ent = readdir(dir)) != NULL) {
      string cur_img(ent->d_name);
      if((cur_img).find(".jpg") != string::npos) {
        string imgPath(dirName + "/" + ent->d_name);
        Mat img = imread(imgPath);
        cvtColor(img, img, CV_BGR2GRAY);
        resize(img, img, size);
        imgVec.push_back(img);
      }
    }
    closedir(dir);
  } else {
    cout << "images not present" << endl;
  }
  //cout << "imgVec.size: " << imgVec.size() << endl;
  return imgVec;
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
  Mat img_resize;
  int if_predict = 0;
  int ifCrop = 0;
  int save_image = 1;
  int num_face = 0;
  int major_vote;
  int turns = 0;
  int if_train = 1;
  int pos_x, pos_y;
  string fn_haar = string(); // </path/to/haar_cascade>
  string subj = "kelly";
  string cur_img;
  vector<Mat> images;
  vector<int> labels;
  int predict_num[] = {0, 0, 0, 0, 0};
  int predicts [2] = {0, 0};
  vector<string> names;
  names.push_back("Kelly");
  names.push_back("Alvin");
  string display_name;
  string box_text;
  //Rect faceRect(10, 10, 100, 100);
  Rect myROI(10, 10, 100, 100);
  IplImage* img;
  Size size(255, 255);
  //CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
  cvtColor(frame, frame_gray, CV_BGR2GRAY);
  equalizeHist(frame_gray, frame_gray);
  Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
  // -- Detect faces
  face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
  for(size_t i = 0; i < faces.size(); i++) {
    Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
    Mat border(frame_gray.rows, frame_gray.cols, CV_8UC1, Scalar(0, 0, 0));
    Mat border_sq(frame_gray.rows, frame_gray.cols, CV_8UC1, Scalar(0, 0, 0));
    ellipse(border, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 255, 255), -1, 8);
    ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
    Mat faceROI = frame_gray(faces[i]);
    std::vector<Rect> eyes;
    // -- In each face, detect eyes
    eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30));
    if(eyes.size() == 2) {
      Rect sqFace(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
	  Mat border(frame_gray.rows, frame_gray.cols, CV_8UC1, Scalar(0, 0, 0));
      rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(0, 0, 255));
      sq_face = frame_gray(sqFace).clone();
      bitwise_and(frame_gray, border, res);
      imshow("square", sq_face);
      //imshow("res", res);
      int num = img_count();
      if(num < 20) {
      	printf("line %d in file !!!!!!!!!!!!!!!!!!!!!%s\n", __LINE__, __FILE__);
        stringstream ss;
        ss << num;
        string nums = ss.str();
        cur_img = "faces/rt_faces/kelly/" + subj + nums + ".jpg";
        resize(sq_face, img_resize, size);
        imwrite(cur_img, img_resize);
        turns += 1;
      } else if((num == 20) && (if_train == 1)) {
        printf("line %d in file %s\n", __LINE__, __FILE__);
        // train
        string cur_img_dir = "faces/rt_faces/kelly";
        images = read_vary_faces(cur_img_dir);
        int face_w = images[0].cols;
        int face_h = images[0].rows;
        // build label vector
        for(int t=0; t<20; t++) {
          labels.push_back(0);
        }
        vector<int> label_count;
        // label_count = total_imgs();
        // printf("line %d in file %s\n", __LINE__, __FILE__);
        //int num_label = sizeof(label_count)/sizeof(label_count[0]);
        //printf("line %d in file %s\n", __LINE__, __FILE__);
        // for(int m=0; m<label_count.size(); m++) {
        //   for(int k=0; k<label_count[m]; k++) {
        //     labels.push_back(m + 1);
        //   }
        // }
        // for(int m=0; m<11; m ++) {
        //   // 1 - mike
        //   labels.push_back(1);
        // }
        for(int m=0; m<20; m++) {
          // 1 - alvin, orl 26
          labels.push_back(1);
        }
        // for(int m=0; m<20; m++) {
        //   // 2 - ethan, ori 51
        //   labels.push_back(2);
        // }
        // for(int m=0; m<20; m++) {
        //   // 3 - yuki, ori 60
        //   labels.push_back(3);
        // }
        cout << "labels.size(): " << labels.size() << endl;
        for(vector<int>::const_iterator ii=labels.begin(); ii!=labels.end(); ++ii) {
        	cout << *ii << ' ';
        }
        cout << "images.size(): " << images.size() << endl;
        model->train(images, labels);
        CascadeClassifier haar_cascade;
        //haar_cascade.load(fn_haar);
        resize(sq_face, img_resize, size);
        printf("line %d in file %s\n", __LINE__, __FILE__);
        int predict = model->predict(img_resize);
        cout << "line 303 predict: " << predict << endl;
        printf("line %d in file %s\n", __LINE__, __FILE__);
        if(predict == 0) {
          // 0 means is kelly
          predict_num[0] += 1;
          display_name = names[0];
        } else {
          // 0 means is alvin
          predict_num[1] += 1;
          display_name = names[1];
        }
        if(predict_num[0] > 0) {
          major_vote = 0;
        }
        else {
          major_vote = 1;
        }
        if_predict = 1;
        turns += 1;
        if_train = 0;
        box_text = "Prediction = " + display_name;
        pos_x = faces[i].x;
        pos_y = faces[i].y - 5;
        putText(frame, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 0, 255), 2.0);
      } else if((num == 20) && (if_predict == 1) && (turns < 41)) {
        // printf("line %d in file %s\n", __LINE__, __FILE__);
        resize(sq_face, img_resize, size);
        printf("line %d in file %s\n", __LINE__, __FILE__);
        int predict = model->predict(img_resize);
        cout << "predict: " << predict << endl;
        if(predict == 0) {
          predict_num[0] += 1;
          display_name = names[0];
          box_text = "Prediction = " + display_name;
          pos_x = faces[i].x;
          pos_y = faces[i].y - 5;
          putText(frame, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 0, 255), 2.0);
        } else if(predict == 1) {
          predict_num[1] += 1;
          display_name = names[1];
          box_text = "Prediction = " + display_name;
          pos_x = faces[i].x;
          pos_y = faces[i].y - 5;
          putText(frame, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 0, 255), 2.0);
        } else if(predict == 2) {
        	predict_num[2] += 1;
        } else if(predict == 3) {
        	predict_num[3] += 1;
        } else {
        	predict_num[4] += 1;
        }
        major_vote = -1;
        for(int cc=0; cc<5; cc++) {
        	if(predict_num[cc] > major_vote) {
        		major_vote == predict_num[cc];
        	}
        }
        turns += 1;
      } else {
        if_predict = 2;
      }
    }
    for(size_t j = 0; j < eyes.size(); j++) {
      //      cout << typeid(eyes[j]).name() << endl;
      Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[i].y + eyes[j].height*0.5);
      int radius = cvRound((eyes[i].width + eyes[j].height)*0.25);
      circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
      //sq_face = frame(myROI);
      //printf("i: %zu, j: %zu\n", i, j);
      //croppedEyes = frame();
      //crop = frame(myROI);
    }
  }
  if(ifCrop == 1) { 
    cout << "ifCrop = 1" << endl;
    //imshow("cropped eye", croppedEyes);
    //imshow(window_name, crop);
  }
  //imshow(fr_window_name, rf_frame)
  imshow(window_name, frame);
}
