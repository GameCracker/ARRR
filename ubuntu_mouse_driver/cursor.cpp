#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>


int main() {
  struct input_event event, event_end;
  int fd = open("/dev/input/event3", O_RDWR);
  if(fd < 0) {
    printf("Error open mouse:%s\n", strerror(errno));
    return -1;
  }
  memset(&event, 0, sizeof(event));
  memset(&event, 0, sizeof(event_end));
  gettimeofday(&event.time, NULL);
  event.type = EV_REL;
  event.code = REL_X;
  event.value = 100;
  gettimeofday(&event_end.time, NULL);
  event_end.type = EV_SYN;
  event_end.code = SYN_REPORT;
  event_end.value = 0;
  for(int i=0; i<5; i++) {
    write(fd, &event, sizeof(event));
    write(fd, &event_end, sizeof(event_end));
    sleep(1);
  }
  close(fd);
  return 0;
}
