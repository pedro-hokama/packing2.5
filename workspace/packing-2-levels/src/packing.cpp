#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

#include "cp25Packing.h"

using namespace std;


int main(int argc, char **argv) {
  //Verbose levels are:
  // 0 - none
  // 1 - minimal

  int n = 10;
  int w[n];
  int h[n];
  int p[n];
  int x[n];
  int y[n];
  int l[n];
  int W = 3;
  int H = 9;

  w[0] = 2; h[0] = 3; p[0] = 0;
  w[1] = 2; h[1] = 3; p[1] = 0;
  w[2] = 1; h[2] = 6; p[2] = 0;
  w[3] = 1; h[3] = 6; p[3] = 0;
  w[4] = 1; h[4] = 6; p[4] = 0;
  w[5] = 1; h[5] = 6; p[5] = 0;
  w[6] = 1; h[6] = 3; p[6] = 1;
  w[7] = 1; h[7] = 3; p[7] = 1;

  w[8] = 2; h[8] = 3; p[8] = 1;
  w[9] = 2; h[9] = 3; p[9] = 1;

  double time_used;
  int status;
  cp_25_simple_unloading(W, H, n, w, h, p, x, y, l, 3600, &time_used, &status, 3);

  return 0;

}
