#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace std;

string exec(string& cmd) {
  FILE* in;
  char buf[512];
  if(!(in = popen(cmd.c_str(), "r"))) {
    return "";
  }
  while(fgets(buf, sizeof(buf), in) != NULL) {
    return string(buf);
  }
  pclose(in);
  return "";
}

void x_move() {
  Display *dpy;
  Window root_window;
  cout << "test" << endl;
  cout <<"line %d" <<  __LINE__ << endl;
  dpy = XOpenDisplay(0);
  root_window = XRootWindow(dpy, 0);
  XSelectInput(dpy, root_window, KeyReleaseMask);
  int x = 0;
  int y = 0;
  int i = 0;
  XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, x, y);
  XFlush(dpy);
  cout << "line %d" << __LINE__ << endl;
  unsigned int microsec = 500;
  // sleep(10);
  // Display *dpy1;
  // Window root_window1;
  // dpy1 = XOpenDisplay(0);
  // root_window1 = XRootWindow(dpy1, 0);
  // XSelectInput(dpy1, root_window1, KeyReleaseMask);
  // XWarpPointer(dpy1, None, root_window1, 0, 0, 0, 0, 2560, 1440);
  // XFlush(dpy1);
  /* while(i < 100) {
    x += 1;
    y += 1;
    cout << "line %d" << __LINE__ << endl;
    dpy = XOpenDisplay(0);
    root_window = XRootWindow(dpy, 0);
    XSelectInput(dpy, root_window, KeyReleaseMask);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, x, y);
    sleep(1);
    XFlush(dpy);
    }*/
}

void set_cursor() {

}

void move_cursor() {
  string cmd;
  stringstream s;
  string i_str;
  for(int i = 0; i < 100; i++) {
    cmd = "xdotool mousemove ";
    s << i;
    i_str = s.str();
    cmd.append(i_str);
    cmd.append(" ");
    cmd.append(i_str);
    cout << "cmd: " << cmd << endl;
    exec(cmd);
    s.str("");
  }
} 

int main(int argc, char *argv[]) {
  //  move_cursor();
  x_move();
}
