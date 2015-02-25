#include <X11/Xlib.h>

int main(void) {
  Display *dpy;
  Window root_window;
  int i;
  dpy = XOpenDisplay(0);
  root_window = XRootWindow(dpy, 0);
  XSelectInput(dpy, root_window, KeyReleaseMask);
  for(i=0; i<800; ++i) {
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, i, i);
    XFlush(dpy);	// Flushes the output buffer
    usleep(50000);
  }
  return 0;
}
