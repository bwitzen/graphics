/* *****************************************************************************
 *
 * Computer Graphics - University of Amsterdam
 * Assignment 5.2
 *
 * Lorenzo Liberatore <l.liberatore@gmail.com>
 * Ben Witzen <benwitzen@live.nl>
 * March 8th, 2013
 *
 * Part of assignment 5.
 *
 * ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "marching_tetrahedra.h"

/* Compute a linearly interpolated position where an isosurface cuts
   an edge between two vertices (p1 and p2), each with their own
   scalar value (v1 and v2) */

static vec3
interpolate_points(unsigned char iso, vec3 p1, vec3 p2, unsigned char v0, unsigned char v1)
{
  if (v0 < v1) {
    float a = (iso - v0) / (v1 -= v0);
    return v3_add(v3_multiply(p1, a), v3_multiply(p2, 1 - a));
  }
  else {
    float a = (iso - v1) / (v0 -= v1);
    return v3_add(v3_multiply(p1, 1 - a), v3_multiply(p2, a));
  }
}

int create_triangle(triangle *triangles, unsigned char isovalue, cell c, int v0, int v1,
                    int v2, int v3, int v4, int v5)
{
  // create triangle corners
  triangles->p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
  triangles->p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
  triangles->p[2] = interpolate_points(isovalue, c.p[v4], c.p[v5], c.value[v4], c.value[v5]);

  // surface normal = cross product between two vectors that are the face
  vec3 normal = v3_crossprod(v3_subtract(c.p[v1], c.p[v0]), v3_subtract(c.p[v3], c.p[v2]));

  // store normals
  triangles->n[0] = v3_multiply(v3_add(triangles->n[0], normal), 0.33);
  triangles->n[1] = v3_multiply(v3_add(triangles->n[1], normal), 0.33);
  triangles->n[2] = v3_multiply(v3_add(triangles->n[2], normal), 0.33);

  return 1;
}

/* Using the given iso-value generate triangles for the tetrahedron
   defined by corner vertices v0, v1, v2, v3 of cell c.

   Store the resulting triangles in the "triangles" array.

   Return the number of triangles created (either 0, 1, or 2).

   Note: the output array "triangles" should have space for at least
         2 triangles.
*/
static int
generate_tetrahedron_triangles(triangle *triangles, unsigned char iso, cell c, int v0, int v1, int v2, int v3)
{
  unsigned char bits = 0x0;

  // set bits
  if (c.value[v0] >= iso)
    bits += 0x01;
  if (c.value[v1] >= iso)
    bits += 0x02;
  if (c.value[v2] >= iso)
    bits += 0x04;
  if (c.value[v3] >= iso)
    bits += 0x08;

  // cases with 0 triangles
  if (bits == 0x00 || bits == 0x0F)
    return 0;
  // cases with 1 triangle
  if (bits == 0x01 || bits == 0x0E)
    return create_triangle(triangles, iso, c, v0, v1, v0, v2, v0, v3);
  if (bits == 0x02 || bits == 0x0D)
    return create_triangle(triangles, iso, c, v1, v2, v1, v3, v1, v0);
  if (bits == 0x04 || bits == 0x0B)
    return create_triangle(triangles, iso, c, v2, v1, v2, v3, v2, v0);
  if (bits == 0x08 || bits == 0x07)
    return create_triangle(triangles, iso, c, v3, v0, v3, v1, v3, v2);
  // cases with 2 triangles
  if (bits == 0x03 || bits == 0x0C) {
    create_triangle(triangles, iso, c, v1, v3, v1, v2, v3, v0);
    create_triangle(triangles, iso, c, v3, v0, v0, v2, v2, v1);
    return 2;
  }
  else if (bits == 0x05 || bits == 0x0A) {
    create_triangle(triangles, iso, c, v0, v3, v3, v2, v2, v1);
    create_triangle(triangles, iso, c, v0, v3, v0, v1, v1, v2);
    return 2;
  }
  else if (bits == 0x06 || bits == 0x09) {
    create_triangle(triangles, iso, c, v3, v2, v2, v0, v3, v1);
    create_triangle(triangles, iso, c, v0, v1, v0, v2, v3, v1);
    return 2;
  }

  printf("Error in marching... corrupted bit value?\n");
  return 0;

}


/* Generate triangles for a single cell for the given iso-value. This function
   should produce at most 6 * 2 triangles (for which the "triangles" array should
   have enough space).

   Use calls to generate_tetrahedron_triangles().

   Return the number of triangles produced
*/

int
generate_cell_triangles(triangle *triangles, cell c, unsigned char iso)
{
    int amt = 0;
    amt += generate_tetrahedron_triangles(triangles + amt, iso, c, 0, 1, 3, 7); // t1
    amt += generate_tetrahedron_triangles(triangles + amt, iso, c, 0, 2, 6, 7); // t2
    amt += generate_tetrahedron_triangles(triangles + amt, iso, c, 0, 1, 5, 7); // t3
    amt += generate_tetrahedron_triangles(triangles + amt, iso, c, 0, 2, 3, 7); // t4
    amt += generate_tetrahedron_triangles(triangles + amt, iso, c, 0, 4, 5, 7); // t5
    amt += generate_tetrahedron_triangles(triangles + amt, iso, c, 0, 4, 6, 7); // t6
    return amt;
}
