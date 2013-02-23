/* *****************************************************************************
 *
 * Computer Graphics - University of Amsterdam
 * Assignment 1.1
 *
 * Lorenzo Liberatore (10415459) <l.liberatore@gmail.com>
 * Ben Witzen (10189459) <benwitzen@live.nl>
 * Feb 8, 2013
 *
 * Implements an optimized version of Bresenham's Midpoint Line-Algorithm. See
 * lines 31-36 for instructions on how to call it to draw a line.
 *
 * ************************************************************************** */

#include "SDL.h"   
#include "init.h"
#include <stdbool.h>


/*
 * Function swaps two integers pointed at by A and B.
 */

void swap(int * a, int * b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}


/*
 * Function draws a line between two points with coordinates (x0,y0) and (x1,y1)
 * on surface pointed at by S with COLOUR. Uses incrementally optimized
 * Bresenham Midpoint Line Algorithm. Supports diagonal lines in any direction.
 */
void mla(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 colour) {

  // swap coordinates so that m is always positive
  bool x_swapped = false, y_swapped = false;
  if (x0 > x1) {
    swap(&x0, &x1);
    x_swapped = true;
  }
  if (y0 > y1) {
    swap(&y0, &y1);
    y_swapped = true;
  }
    
  // determine m (growth rate)  
  double m = ((double)y1 - (double)y0) / ((double)x1 - (double)x0);
  
  // determine whether line should progress left/up (-1) or right/down (+1)
  int var = 1;
  
  // swap around additional points if needed - this completes the swap between
  // x-axis and y-axis for certain octants - also inverts line progression
  if ((!x_swapped && y_swapped) || (x_swapped && !y_swapped)) {
    var = -1;
    if (m >= 1.0)
      swap(&x0, &x1);
    else
      swap(&y0, &y1);
  }

  // initiate variables
  int x = x0;
  int y = y0;
  double d;
  int * x_draw = &x;
  int * y_draw = &y;

  // depending on the octant, swap all variables in the while loop
  if (m >= 1.0) {
    if ((!x_swapped && y_swapped) || (x_swapped && !y_swapped))
      swap(&x0, &x1);
    swap(&x, &y);
    swap(&x0, &y0);
    swap(&x1, &y1);
    x_draw = &y;
    y_draw = &x;
  }
  else {
    if ((!x_swapped && y_swapped) || (x_swapped && !y_swapped))
      swap(&y0, &y1);
  }

  // calculate d
  d = -0.5 * ((double)x1 - (double)x0) + ((double)y0 - (double)y1);

  // perform drawing and pixel calculation
  while (x < x1) {
    PutPixel(s, *x_draw, *y_draw, colour);
    if (d < 0) {
      y = y + var;
      d = d + (x1 - x0) + (y0 - y1);
    }
    else {
      d = d + (y0 - y1);
    }
    x++;
  }
  return;
}

