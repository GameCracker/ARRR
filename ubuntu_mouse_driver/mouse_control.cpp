#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

// get output of command, replace popen and pclose with _popen and _pclose
string exec(char* cmd) {
  FILE* pipe = popen(cmd, "r");
  if(!pipe)
    return "ERROR";
  char buf[128];
  string result = "";
  while(!feof(pipe)) {
    if(fgets(buf, 128, pipe) != NULL)
      result += buf;
  }
  pclose(pipe);
  return result;
}

int exec1(string& cmd) {
  FILE* in;
  char buf[512];
  if(!(in = popen(cmd.c_str(), "r"))) {
    return 1;
  }
  while(fgets(buf, sizeof(buf), in) != NULL) {
    cout << buf;
  }
  pclose(in);
  return 0;
}

int main(int argc, char *argv[]) {
  string str = "ls";
  getline(cin, str);
  cout << "Command entered: " << str << endl;
  exec1(str);
}


