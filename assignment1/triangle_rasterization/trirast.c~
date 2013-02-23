/* *****************************************************************************
 *
 * Computer Graphics - University of Amsterdam
 * Assignment 1.2
 *
 * Lorenzo Liberatore (10415459) <l.liberatore@gmail.com>
 * Ben Witzen (10189459) <benwitzen@live.nl>
 * Feb 8, 2013
 *
 * Implements triangle rasterization.
 *
 * ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "types.h"


/*
 * Function returns the smallest value amongst three floats.
 */

float get_min(float x0, float x1, float x2) {
  if(x1 < x0)
    x0 = x1;
  if(x2 < x0)
    x0 = x2;
  return x0;
}


/*
 * Function returns the largest value amongst three floats.
 */

float get_max(float x0, float x1, float x2) {
  if(x1 > x0)
    x0 = x1;
  if(x2 > x0)
    x0 = x2;
  return x0;
}


/*
 * Collection of functions that returns the barycentric coordinates for the
 * variables, as described in the handout.
 */

float f01(float x0, float y0, float x1, float y1, float x, float y) {
  return ((y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0);
}


float f12(float x1, float y1, float x2, float y2, float x, float y) {
  return ((y1 - y2) * x + (x2 - x1) * y  +x1 * y2 - x2 * y1);
}


float f20(float x0, float y0, float x2, float y2, float x, float y) {
  return ((y2 - y0) * x + (x0 - x2) * y + x2 * y0 - x0 * y2);
}


/*
 * Rasterize a single triangle. The triangle is specified by its corner 
 * coordinates (x0,y0), (x1,y1), (x2,y2) and is drawn in color red,green,blue.
 */

void draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
                   byte red, byte green, byte blue) {
  
  // estimate bounding box
  float xmin = floor(get_min(x0, x1, x2));
  float xmax = ceil(get_max(x0, x1, x2));
  float ymin = floor(get_min(y0, y1, y2));
  float ymax = ceil(get_max(y0, y1, y2));

  // alpha, beta, gamma
  float a, b, g;

  // drawing the triangle
  for (int y = ymin; y < ymax; y++) {
    for (int x = xmin; x < xmax; x++) {

      // calculate alpha, beta, gamma
      a = f12(x1, y1, x2, y2, x, y) / f12(x1, y1, x2, y2, x0, y0);
      b = f20(x0, y0, x2, y2, x, y) / f20(x0, y0, x2, y2, x1, y1); 
      g = f01(x0, y0, x1, y1, x, y) / f01(x0, y0, x1, y1, x2, y2);

      // draw pixel if it's valid
      if (a > 0 && b > 0 && g > 0)
        PutPixel(x, y, red, green, blue);
    }
  }
}


/*
 * Optimized version of the previous function. Call it in the same way.
 */

void draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, 
                             float y2, byte red, byte green, byte blue) {
  
  // estimate bounding box
  float xmin = floor(get_min(x0, x1, x2));
  float xmax = ceil(get_max(x0, x1, x2));
  float ymin = floor(get_min(y0, y1, y2));
  float ymax = ceil(get_max(y0, y1, y2));

  bool flag;
  float a, b, g, fa, fb, fg, a_off, b_off, g_off;
  float o = -1;
  
  //barycentric cordinates for alpha, beta and gamma
  fa = f12(x1, y1, x2, y2, x0, y0);
  fb = f20(x0, y0, x2, y2, x1, y1);
  fg = f01(x0, y0, x1, y1, x2, y2);
  
  // alpha, beta, gamma offscreen
  a_off = fa * f12(x1, y1, x2, y2, o, o);
  b_off = fb * f20(x0, y0, x2, y2, o, o);
  g_off = fg * f01(x0, y0, x1, y1, o, o);

  // drawing the triangle
  for(int y = ymin;y < ymax; y++) {

    // initially, no pixel is drawn
    flag = false;
    for(int x = xmin; x < xmax; x++) {

      // calculate alpha, beta, gamma
      a = f12(x1, y1, x2, y2, x, y) / fa;
      b = f20(x0, y0, x2, y2, x, y) / fb;
      g = f01(x0, y0, x1, y1, x, y) / fg;

      // draw pixel if it's valid
      if (a >= 0 && b >= 0 && g >= 0) {
        if ((a > 0 || a_off > 0) && (b > 0 || b_off > 0) && (g > 0 || g_off > 0))
        {
          PutPixel(x, y, red, green, blue);
          flag = true;
        }
      }
      // if all pixels in row are drawn
      else if (flag)
        break;
    }
  }
}

