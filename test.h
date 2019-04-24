//test.h

#ifndef TEST_H_
#define TEST_H_

#include <iostream>
#include <map>
#include <stdlib.h>
#include <string>
#include <iterator>
#include <fstream>
#include <time.h>
#include <math.h>
#include <random>

int calcTotal(std::map<char, std::map<char,int> >& g, char s);
char nextletter(std::map<char, std::map<char,int> >& g, char s);
float lerp(float a0, float a1, float w);
float dotGridGradient(int ix, int iy, float x, float y);
float perlin(float x, float y);
int IYMAX = 16;
int IXMAX = 16;
float PI = 3.14159;
extern float Gradient[][][2];


#endif
