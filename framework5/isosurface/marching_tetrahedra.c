/* Computer Graphics, Assignment, Volume rendering with cubes/points/isosurface
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 * (always fill in these fields before submitting!!)
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "marching_tetrahedra.h"

/* Compute a linearly interpolated position where an isosurface cuts
   an edge between two vertices (p1 and p2), each with their own
   scalar value (v1 and v2) */

static vec3
interpolate_points(unsigned char isovalue, vec3 p1, vec3 p2, unsigned char v1, unsigned char v2)
{
    /* Initially, simply return the midpoint between p1 and p2.
       So no real interpolation is done yet */

    return v3_add(v3_multiply(p1, 0.5), v3_multiply(p2, 0.5));
}

/* Using the given iso-value generate triangles for the tetrahedron
   defined by corner vertices v0, v1, v2, v3 of cell c.

   Store the resulting triangles in the "triangles" array.

   Return the number of triangles created (either 0, 1, or 2).

   Note: the output array "triangles" should have space for at least
         2 triangles.
*/

static int
generate_tetrahedron_triangles(triangle *triangles, unsigned char isovalue, cell c, int v0, int v1, int v2, int v3)
{
  int bits[4] = {0, 0, 0, 0}; // easier than to work with hex atm

  // check cell values against isovalue
  if (c.value[v0] > isovalue)
    bits[0] = 1;
  if (c.value[v1] > isovalue)
    bits[1] = 1;
  if (c.value[v2] > isovalue)
    bits[2] = 1;
  if (c.value[v3] > isovalue)
    bits[3] = 1;

  // no triangles are returned if all bits are the same
  if (bits[0] == 0 && bits[1] == 0 && bits[2] == 0 && bits[3] == 0)
    return 0;

  if (bits[0] == 1 && bits[1] == 1 && bits[2] == 1 && bits[3] == 1)
    return 0;

  // handle the other 7 cases
  vec3 midpoints[6];
  int amt_of_triangles = 0;
  if ((bits[0] == 0 && bits[1] == 0 && bits[2] == 0 && bits[3] == 1) ||
      (bits[0] == 1 && bits[1] == 1 && bits[2] == 1 && bits[3] == 0)) {
    midpoints[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], 0, 0);
    midpoints[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], 0, 0);
    midpoints[2] = interpolate_points(isovalue, c.p[v0], c.p[v3], 0, 0);
    amt_of_triangles = 1;
  }
  else if ((bits[0] == 0 && bits[1] == 0 && bits[2] == 1 && bits[3] == 0) ||
           (bits[0] == 1 && bits[1] == 1 && bits[2] == 0 && bits[3] == 1)) {
    midpoints[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], 0, 0);
    midpoints[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], 0, 0);
    midpoints[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], 0, 0);
    amt_of_triangles = 1;
  }
  else if ((bits[0] == 0 && bits[1] == 1 && bits[2] == 0 && bits[3] == 0) ||
           (bits[0] == 1 && bits[1] == 0 && bits[2] == 1 && bits[3] == 1)) {
    midpoints[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], 0, 0);
    midpoints[1] = interpolate_points(isovalue, c.p[v2], c.p[v1], 0, 0);
    midpoints[2] = interpolate_points(isovalue, c.p[v2], c.p[v3], 0, 0);
    amt_of_triangles = 1;
  }
  else if ((bits[0] == 1 && bits[1] == 0 && bits[2] == 0 && bits[3] == 0) ||
           (bits[0] == 0 && bits[1] == 1 && bits[2] == 1 && bits[3] == 1)) {
    midpoints[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], 0, 0);
    midpoints[1] = interpolate_points(isovalue, c.p[v2], c.p[v1], 0, 0);
    midpoints[2] = interpolate_points(isovalue, c.p[v2], c.p[v3], 0, 0);
    amt_of_triangles = 1;
  }
  else if ((bits[0] == 0 && bits[1] == 0 && bits[2] == 1 && bits[3] == 1) ||
           (bits[0] == 1 && bits[1] == 1 && bits[2] == 0 && bits[3] == 0)) {
    midpoints[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], 0, 0);
    midpoints[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], 0, 0);
    midpoints[2] = interpolate_points(isovalue, c.p[v0], c.p[v3], 0, 0);
    midpoints[3] = interpolate_points(isovalue, c.p[v1], c.p[v0], 0, 0);
    midpoints[4] = interpolate_points(isovalue, c.p[v1], c.p[v2], 0, 0);
    midpoints[5] = interpolate_points(isovalue, c.p[v1], c.p[v3], 0, 0);
    amt_of_triangles = 2;
  }
  else if ((bits[0] == 0 && bits[1] == 1 && bits[2] == 0 && bits[3] == 1) ||
           (bits[0] == 1 && bits[1] == 0 && bits[2] == 1 && bits[3] == 0)) {
    midpoints[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], 0, 0);
    midpoints[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], 0, 0);
    midpoints[2] = interpolate_points(isovalue, c.p[v0], c.p[v3], 0, 0);
    midpoints[3] = interpolate_points(isovalue, c.p[v2], c.p[v0], 0, 0);
    midpoints[4] = interpolate_points(isovalue, c.p[v2], c.p[v1], 0, 0);
    midpoints[5] = interpolate_points(isovalue, c.p[v2], c.p[v3], 0, 0);
    amt_of_triangles = 2;
  }
  else if ((bits[0] == 0 && bits[1] == 1 && bits[2] == 1 && bits[3] == 0) ||
           (bits[0] == 1 && bits[1] == 0 && bits[2] == 0 && bits[3] == 1)) {
    midpoints[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], 0, 0);
    midpoints[1] = interpolate_points(isovalue, c.p[v2], c.p[v1], 0, 0);
    midpoints[2] = interpolate_points(isovalue, c.p[v2], c.p[v3], 0, 0);
    midpoints[3] = interpolate_points(isovalue, c.p[v3], c.p[v0], 0, 0);
    midpoints[4] = interpolate_points(isovalue, c.p[v3], c.p[v1], 0, 0);
    midpoints[5] = interpolate_points(isovalue, c.p[v3], c.p[v2], 0, 0);
    amt_of_triangles = 2;
  }

  // set the triangle vectors
  triangles[0].p[0] = midpoints[0];
  triangles[0].p[1] = midpoints[1];
  triangles[0].p[2] = midpoints[2];
  triangles[1].p[0] = midpoints[3];
  triangles[1].p[1] = midpoints[4];
  triangles[1].p[2] = midpoints[5];
  
  return amt_of_triangles;
}

/* Generate triangles for a single cell for the given iso-value. This function
   should produce at most 6 * 2 triangles (for which the "triangles" array should
   have enough space).

   Use calls to generate_tetrahedron_triangles().

   Return the number of triangles produced
*/

int
generate_cell_triangles(triangle *triangles, cell c, unsigned char isovalue)
{
    int amt_of_triangles = 0;
    amt_of_triangles += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 3, 7); // t1
    amt_of_triangles += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 6, 7); // t2
    amt_of_triangles += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 5, 7); // t3
    amt_of_triangles += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 3, 7); // t4
    amt_of_triangles += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 5, 7); // t5
    amt_of_triangles += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 6, 7); // t6
    return amt_of_triangles;
}
