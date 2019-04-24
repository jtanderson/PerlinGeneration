#include <iostream>
#include <map>
#include <stdlib.h>
#include <string>
#include <iterator>
#include <fstream>
#include <time.h>
#include <math.h>
#include <random>
#include <algorithm>
#include <limits>
#include <cmath>
#include <vector>
#include <stdio.h>
#include "geometry.h"

int calcTotal(std::map<char, std::map<char,int>>& g, char s);
char nextletter(std::map<char, std::map<char,int>>& g, char s);
float lerp(float a0, float a1, float w);

// to do blocks of 64px in a 1024x1024 square image
// maxes are grid size plus one because dotgrad is dumb

//const int width = 1024;
//const int height = 1024;

//const int num_grid = 8;
//const float grid_width = width / num_grid;
//const float grid_height = width / num_grid;

float perlin(float x, float y, float***);
float PI = 3.14159;
float dotGridGradient(int ix, int iy, float x, float y, float***);
//float*** Gradient;

struct vect{};

//vect operator*(vect v, int i){ }

std::vector<Vec3f> makeBuffer(int width, int height, int num_grad, float*** G){
    std::vector<Vec3f> framebuffer(width*height);

    const float grid_width = width / num_grad;
    const float grid_height = width / num_grad;

    float x, y;
    for(size_t j = 0; j < height; ++j) {
      for(size_t i = 0; i < width; ++i) {
        x = float(i)/grid_width;
        //printf("%f\n",x);
        y = float(j)/grid_height;
        framebuffer[i+j*width] = Vec3f(perlin(x,y,G),0, 0);
      }
    }

    return framebuffer;
}

void render(std::vector<Vec3f> framebuffer, int height, int width) {
    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm");
    ofs << "P6\n" << width << " " << height << "\n255\n";

    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

// height and width, number of gradient each direction
float*** makeGradGrid(int h, int w){
  //float Gradient[IYMAX][IXMAX][2];
  float*** Gradient = new float**[h];//[IXMAX][2];
  for(int i=0; i<h; i++){
    Gradient[i] = new float*[w];
    for(int j=0; j<w; j++){
      Gradient[i][j] = new float[2];
    }
  }
  //srand(time(0));
  std::mt19937_64 rng(time(0));
  float t;
  float tc;             // cos(t)
  float ts;             // sin(t)

  std::uniform_real_distribution<float> unif(0.0, 2.0*PI);
  for(int i = 0; i < h; i++) {
    // std::cout << "Here" << std::endl;
    for(int j = 0; j < w; j++){
      // std::cout << "Here2" << std::endl;
      t = unif(rng);
      tc = cos(t);
      ts = sin(t);
      //tc = round(tc);
      //ts = round(ts);
      Gradient[i][j][0] = tc;
      Gradient[i][j][1] = ts;
    }
  }
  return Gradient;
}

int main () {
  int pix_w = 1<<10;
  int pix_h = 1<<10;

  int num_grad = 8;

  int grad_height = num_grad+1;
  int grad_width = num_grad+1;

  float*** grad1 = makeGradGrid(grad_height, grad_width);
  auto buf1 = makeBuffer(pix_w, pix_h, num_grad, grad1);

  num_grad = 128;

  grad_height = num_grad+1;
  grad_width = num_grad+1;

  float*** grad2 = makeGradGrid(grad_height, grad_width);
  auto buf2= makeBuffer(pix_w, pix_h, num_grad, grad2);

  auto buf3 = buf1;
  for(int i=0; i<buf3.size(); i++){
    buf3[i][0] += buf2[i][0];
    buf3[i][1] += buf2[i][1];
    buf3[i][2] += buf2[i][2];
  }

  render(buf3, pix_h, pix_w);
  return 0;

  std::map<char, std::map<char,int> > graph;

  graph['^'] = std::map<char,int>();
  graph['$'] = std::map<char,int>();

  std::string name;
  std::ifstream myfile ("name.txt");

  if (myfile.is_open())
  {
    while ( std::getline (myfile,name) )
    {
      std::transform(name.begin(),name.end(),name.begin(),::tolower);
      if(graph['^'].count(name[0]) > 0){      //dealing with seeing the same letter again
        graph['^'][name[0]]++;
      } else {                                //first case for the letter
        graph['^'][name[0]] = 1;
      }

      //Create an empty map for the last letter, book keeping to get the last letter
      //to point to the terminal character
      if(graph.count(name[name.length()-1]) == 0){
        graph[name[name.length()-1]] = std::map<char,int>();
      }

      if(graph[name[name.length()-1]].count('$') > 0){
        graph[name[name.length()-1]]['$']++;
      } else {
        graph[name[name.length()-1]]['$'] = 1;
      }

      //creating the map for each character within the name
      for(int i = 0; i < name.length()-1; i++) {
        if( graph.count(name[i]) == 0){
          graph[name[i]] = std::map<char,int>();
        }

        //checking adjacent letter and creating the edge between it and the
        //current letter
        if( graph[name[i]].count(name[i+1]) == 0){
          graph[name[i]][name[i+1]] = 1;
        } else {
          graph[name[i]][name[i+1]]++;
        }
        // if(isalpha(name[i+1]))
        //   node.emplace(name[i+1], 1);
        //   graph.emplace(name[i],node);
      }

      //std::cout << name << '\n';
      //std::cout << name.length() << '\n';
    }
    myfile.close();

    for(auto i=graph.begin(); i != graph.end(); i++){
      std::cout << "Edges of " << i->first << std::endl;
      for(auto j=i->second.begin(); j !=i->second.end(); j++){
        std::cout << j->first << ": " << j->second << std::endl;
      }
      std::cout << "============\n";
    }

    std::string newName = "";
    char t = '^';

    while(t != '$') {

      t = nextletter(graph, t);
      newName.push_back(t);
    }

    std::cout << newName << std::endl;

  } else {
    std::cout << "Unable to open file";
  }

  return 0;
}

int calcTotal(std::map<char, std::map<char,int>>& g, char s){
  int total = 0;
  for( auto i = g[s].begin(); i != g[s].end(); i++){
    total += i->second;
  }
  return total;
}

char nextletter(std::map<char, std::map<char,int>>& g, char s){
  int total = calcTotal(g, s);
  int num = rand() % total + 1; // or something fancier

  int subtotal = 0;
  for( auto i = g[s].begin(); i != g[s].end(); i++){
    subtotal += i->second;
    if( num <= subtotal ){
      return i->first;
    }
  }
  return ' ';
}

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 *
 * as an alternative, this slightly faster equivalent function (macro) can be used:
 * #define lerp(a0, a1, w) (a0 + w*(a1 - a0))
 */
float lerp(float a0, float a1, float w) {
    return (1.0f - w)*a0 + w*a1;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y, float*** Gradient) {

    // Precomputed (or otherwise) gradient vectors at each grid node
    //extern float Gradient[IYMAX][IXMAX][2];
    
    //printf("dotGrad at (%0.6f,%0.6f)...\n",x,y);
    //printf("dotGrad at (%d,%d)...\n",ix,iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx*Gradient[iy][ix][0] + dy*Gradient[iy][ix][1]);
}

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y, float*** G) {

    // Determine grid cell coordinates
    int x0 = (int)x;
    int x1 = x0 + 1;
    int y0 = (int)y;
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    //printf("%0.6f\n", x);
    n0 = dotGridGradient(x0, y0, x, y, G);
    n1 = dotGridGradient(x1, y0, x, y, G);
    ix0 = lerp(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y, G);
    n1 = dotGridGradient(x1, y1, x, y, G);
    ix1 = lerp(n0, n1, sx);

    value = lerp(ix0, ix1, sy);
    return value;
}
