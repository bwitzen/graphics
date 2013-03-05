#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "types.h"

int     num_rays_shot;
int     num_shadow_rays_shot;
int     num_triangles_tested;
int     num_bboxes_tested;

int     find_first_intersection(intersection_point *ip,
            vec3 ray_origin, vec3 ray_direction);

int     shadow_check(vec3 ray_origin, vec3 ray_direction);

#endif
