/* *****************************************************************************
 *
 * Computer Graphics - University of Amsterdam
 * Assignment 3.1.2
 *
 * Lorenzo Liberatore (10415459) <l.liberatore@gmail.com>
 * Ben Witzen (10189459) <benwitzen@live.nl>
 * Feb 20, 2013
 *
 * Implements myOrtho().
 *
 * ************************************************************************** */

#include <GL/glut.h>   
#include <GL/gl.h>
#include <GL/glu.h>

 
#define sqr(x) ((x)*(x))

/* ANSI C/ISO C89 does not specify this constant (?) */
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

void myOrtho(GLdouble l, GLdouble r, GLdouble b,    // left, right, bottom
             GLdouble t, GLdouble n, GLdouble f) {  // top, near, far

  // scaling matrix
  GLfloat Mscale[16] = {
    2/(r-l), 0.0,     0.0,     0.0,
    0.0,     2/(t-b), 0.0,     0.0,
    0.0,     0.0,     2/(n-f), 0.0,
    0.0,     0.0,     0.0,     1.0
  };

  // have GL multiply with this matrix
  glMultMatrixf(Mscale);

  // translate to origin (note the transposed matrix; this is for OpenGL)
  GLfloat Mtrans[16] = {
    1.0,        0.0,        0.0,        0.0,
    0.0,        1.0,        0.0,        0.0,
    0.0,        0.0,        1.0,        0.0,
    -((l+r)/2), -((b+t)/2), -((n+f)/2), 1.0
  };

  // have GL multiply with this matrix
  glMultMatrixf(Mtrans);
}

