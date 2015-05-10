#ifndef MOUSE_CONTROL_H
#define MOUSE_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <typeinfo>

using namespace std;

int move_cursor(int w, int h, double x, double y, int t);;
string trim(string str);
int click(int time);
string trim_left(string str);
string trim_right(string str);
string& trim_in_place(string& str);
string& trim_right_in_place(string& str);
string& trim_left_in_place(string& str);
string exec2(string& cmd);
int exec1(string& cmd);
string exec(char* cmd);

#endif

