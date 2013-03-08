/* *****************************************************************************
 *
 * Computer Graphics - University of Amsterdam
 * Assignment 4.3
 *
 * Lorenzo Liberatore <l.liberatore@gmail.com>
 * Ben Witzen <benwitzen@live.nl>
 * March 8th, 2013
 *
 * Implements BVH traversal to test for intersection. Optimized according to the
 * notes in the PDF.
 *
 * ************************************************************************** */

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "intersection.h"
#include "v3math.h"
#include "constants.h"
#include "scene.h"
#include "bvh.h"

// A few counters for gathering statistics on the number and types
// of ray shot

// The total number of rays
int num_rays_shot = 0;

// Number of shadow rays
int num_shadow_rays_shot = 0;

// Number of triangles tested for intersection
int num_triangles_tested = 0;

// Number of bounding boxes tested for intersection
int num_bboxes_tested = 0;

// Forward declarations

static int  find_first_intersected_bvh_triangle(intersection_point* ip,
                vec3 ray_origin, vec3 ray_direction);

// Checks if the given triangle is intersected by ray with given
// origin and direction.
//
// Returns 1 if there is an intersection, or 0 otherwise.
//
// When an intersection is found the fields of 'ip' will be filled in
// with the relevant values.
//
// Note: this routine does NOT return an intersection for triangles
// whose back side faces the ray (by definition a triangle normal
// points to the triangle's front side).
// I.e. we do back-face culling here ...
//
// Code based on Moller & Trumbore, 1997, "Fast, minimum storage
// ray/triangle intersection"

static int
ray_intersects_triangle(intersection_point* ip, triangle tri,
    vec3 ray_origin, vec3 ray_direction)
{
    vec3    edge1, edge2;
    vec3    tvec, pvec, qvec;
    double  det, inv_det;
    double  t, u, v;        // u, v are barycentric coordinates
    // t is ray parameter

    num_triangles_tested++;

    edge1 = v3_subtract(scene_vertices[tri.v[1]], scene_vertices[tri.v[0]]);
    edge2 = v3_subtract(scene_vertices[tri.v[2]], scene_vertices[tri.v[0]]);

    pvec = v3_crossprod(ray_direction, edge2);

    det = v3_dotprod(edge1, pvec);

    if (det < 1.0e-6)
        return 0;

    tvec = v3_subtract(ray_origin, scene_vertices[tri.v[0]]);

    u = v3_dotprod(tvec, pvec);
    if (u < 0.0 || u > det)
        return 0;

    qvec = v3_crossprod(tvec, edge1);

    v = v3_dotprod(ray_direction, qvec);
    if (v < 0.0 || u+v > det)
        return 0;

    t = v3_dotprod(edge2, qvec);

    if (t < 0.0)
        return 0;

    inv_det = 1.0 / det;
    t *= inv_det;
    u *= inv_det;
    v *= inv_det;

    // We have a triangle intersection!
    // Return the relevant intersection values.

    // Compute the actual intersection point
    ip->t = t;
    ip->p = v3_add(ray_origin, v3_multiply(ray_direction, t));

    // Compute an interpolated normal for this intersection point, i.e.
    // we use the barycentric coordinates as weights for the vertex normals
    ip->n = v3_normalize(v3_add(
        v3_add(
            v3_multiply(tri.vn[0], 1.0-u-v),
            v3_multiply(tri.vn[1], u)
        ),
        v3_multiply(tri.vn[2], v)));

    ip->i = v3_normalize(v3_negate(ray_direction));
    ip->material = tri.material;

    return 1;
}

// Check if the given sphere is intersected by the given ray.
// See Shirley et.al., section 10.3.1
// Returns 1 if there is an intersection (and sets the appropriate
// fields of ip), or 0 otherwise.
static int
ray_intersects_sphere(intersection_point* ip, sphere sph,
    vec3 ray_origin, vec3 ray_direction)
{
    float   A, B, C, D;
    vec3    diff;
    float   t_hit;

    A = v3_dotprod(ray_direction, ray_direction);

    diff = v3_subtract(ray_origin, sph.center);
    B = 2.0 * v3_dotprod(diff, ray_direction);
    C = v3_dotprod(diff, diff) - sph.radius * sph.radius;

    D = B*B - 4*A*C;

    if (D < 0.0)
        return 0;

    D = sqrt(D);

    // We're only interested in the first hit, i.e. the one with
    // the smallest t_hit, so we check -B-D first, followed by -B+D

    t_hit = (-B - D)/(2*A);

    if (t_hit < 0.0)
    {
        t_hit = (-B + D)/(2*A);
        if (t_hit < 0.0)
            return 0;
    }

    ip->t = t_hit;
    ip->p = v3_add(ray_origin, v3_multiply(ray_direction, t_hit));
    ip->n = v3_normalize(v3_subtract(ip->p, sph.center));
    ip->i = v3_normalize(v3_negate(ray_direction));
    ip->material = sph.material;

    return 1;
}


/*
 * Evaluates the node itself and recursively all children of this node. If a
 * triangle intersection was found, returns 1 and sets the appropriate fields
 * in ip. Else returns 0.
 */
 
int bvh_recursive(bvh_node *node, intersection_point *ip,
                  vec3 ray_origin, vec3 ray_direction, float tmin, float tmax)
{
  // if this is a leaf node, we've reached the bottom of this bvh tree branch
  if (node->is_leaf) {
    // create a temporary ip
    // t is used to find the closest hit, so we'll initiate it to a very high
    // value so that it will always evaluate worse than the first actual hit
    intersection_point current;
    intersection_point * ip_copy = ip; // back up old t value
    ip->t = 9999999;
    // return value
    int found = 0;
    
    // search each triangle in this particular bbox
    for (int i = 0; i < leaf_node_num_triangles(node); i++) {
      // test for intersection
      if (ray_intersects_triangle(&current, leaf_node_triangles(node)[i], ray_origin, ray_direction)) {
        // test if that intersection is closer than the currently closest
        if (current.t < ip->t) {
          // if so, update ip accordingly
          found = 1;
          *ip = current;
        }
      }
    }
    
    // restore ip values if no hit was found
    if (found == 0)
      ip->t = ip_copy->t;
    
    // immediately return, skipping any other bboxes that might not have been
    // tested yet - this implements the optimalization as required by note 3
    return found;

  }

  // if this bbox isn't intersected, just return
  if (!bbox_intersect(&tmin, &tmax, node->bbox, ray_origin, ray_direction, tmin, tmax))
    return 0;

  intersection_point left, right;
  float nearest = 0;
  bool nearest_right = false;
  bool nearest_left  = false;
    
  // check both children for intersection and determine who's intersecion is
  // closest to the camera
  if (bvh_recursive(inner_node_left_child(node), &left, ray_origin, ray_direction, tmin, tmax)) {
    nearest = left.t;
    nearest_right = false;
    nearest_left  = true;
  }

  if (bvh_recursive(inner_node_right_child(node), &right, ray_origin, ray_direction, tmin, tmax) && ((!nearest || right.t < nearest) && right.t > 0)) {
    nearest = right.t;
    nearest_right = true;
    nearest_left  = false;
  }
  
  // set ip to left or right, according to which one was the nearest
  // return 0 if neither ip was set
  if (nearest_left)
    *ip = left;
  else if (nearest_right)
    *ip = right;
  else
    return 0;
    
  // return 1, because ip was set to either left or right
  return 1;
}


/*
 * Function returns 1 if intersection found, in which case the fields of ip will
 * be set accordingly. Returns 0 if there are no intersections. Note that this
 * is merely a handle to initiate our recursive search.
 */

static int
find_first_intersected_bvh_triangle(intersection_point* ip,
                                    vec3 ray_origin, vec3 ray_direction)
{
    return bvh_recursive(bvh_root, ip, ray_origin, ray_direction, 0, 100);
}

// Returns the nearest hit of the given ray with objects in the scene
// (either a sphere or a triangle).
//
// Returns 1 and sets the intersection point values if there
// is an intersection, returns 0 otherwise.
int
find_first_intersection(intersection_point *ip, vec3 ray_origin, vec3 ray_direction)
{
    int     have_hit;
    float   t_nearest;
    intersection_point  ip2;

    num_rays_shot++;

    // We have found no hit yet
    t_nearest = C_INFINITY;
    have_hit = 0;

    // First check against spheres in the scene
    for (int s = 0; s < scene_num_spheres; s++)
    {
        // We need a second set of p and n variables, as there's the
        // possibility that we'll overwrite a closer intersection already
        // found
        if (ray_intersects_sphere(&ip2, scene_spheres[s], ray_origin, ray_direction))
        {
            if (ip2.t < t_nearest)
            {
                *ip = ip2;
                t_nearest = ip2.t;
                have_hit = 1;
            }
        }
    }

    // Then check against triangles in the scene

    if (use_bvh)
    {
        // Use the BVH to speed up intersection testing
        if (find_first_intersected_bvh_triangle(&ip2, ray_origin, ray_direction))
        {
            if (ip2.t < t_nearest)
            {
                *ip = ip2;
                t_nearest = ip2.t;
                have_hit = 1;
            }
        }
    }
    else
    {
        // Simply iterate over all the triangles in the scene and check for intersection
        for (int t = 0; t < scene_num_triangles; t++)
        {
            if (ray_intersects_triangle(&ip2, scene_triangles[t], ray_origin, ray_direction))
            {
                if (ip2.t < t_nearest)
                {
                    *ip = ip2;
                    t_nearest = ip2.t;
                    have_hit = 1;
                }
            }
        }
    }

    return have_hit;
}

// Optimized routine for tracing a shadow ray.
//
// This routine doesn't return the nearest intersection, but simply
// checks if there is any intersection.
int
shadow_check(vec3 ray_origin, vec3 ray_direction)
{
    intersection_point  ip;

    num_rays_shot++;
    num_shadow_rays_shot++;

    for (int s = 0; s < scene_num_spheres; s++)
    {
        if (ray_intersects_sphere(&ip, scene_spheres[s], ray_origin, ray_direction))
            return 1;
    }

    if (use_bvh)
    {
        // Use the BVH for speedy intersection testing
        if (find_first_intersected_bvh_triangle(&ip, ray_origin, ray_direction))
            return 1;
    }
    else
    {
        // Simply iterate over all the triangles in the scene and check for intersection
        for (int t = 0; t < scene_num_triangles; t++)
        {
            if (ray_intersects_triangle(&ip, scene_triangles[t], ray_origin, ray_direction))
                return 1;
        }
    }

    return 0;
}

