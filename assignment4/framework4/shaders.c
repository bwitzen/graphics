/* Computer Graphics and Game Technology, Assignment Ray-tracing
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "shaders.h"
#include "perlin.h"
#include "v3math.h"
#include "intersection.h"
#include "scene.h"
#include "quat.h"
#include "constants.h"

// shade_constant()
//
// Always return the same color. This shader does no real computations
// based on normal, light position, etc. As such, it merely creates
// a "silhouette" of an object.

vec3
shade_constant(intersection_point ip)
{
    return v3_create(1, 0, 0);
}

vec3
shade_matte(intersection_point ip)
{
	int i;
	vec3 a, b;
	float dot_prod, light_matte = scene_ambient_light;
	
	// Calculate the light intensity in p for all the lights in the scene
	for (i = 0; i < scene_num_lights; i++) {
	
		// Calculate the color of the light in i position
		a = v3_normalize(v3_subtract(scene_lights[i].position, ip.p));
	
		// Use small offset in ray origin to avoid the problem of self-shadowing
		b = v3_add(ip.p, v3_multiply(a, 0.1));
	
		// Calculate the color of light in i position and if there is no shadow and light is between [0, 1]
		if (!shadow_check(b, a) && light_matte >= 0 && light_matte <= 1) {
			//Calculate the contribution of light
			dot_prod = v3_dotprod(ip.n, a);
			//Check if the contribution of light it is not negative so not behind the point
			if (dot_prod >= 0) {
				//Sums the contribution of the all lights reaching that point 
				light_matte += fmax(0, scene_lights[i].intensity * dot_prod);
			}
		}
	}
	//returns the vector calculated
	return v3_create(light_matte, light_matte, light_matte);
}

vec3
shade_blinn_phong(intersection_point ip)
{
    return v3_create(1, 0, 0);
}

vec3
shade_reflection(intersection_point ip)
{
    return v3_create(1, 0, 0);
}

// Returns the shaded color for the given point to shade.
// Calls the relevant shading function based on the material index.
vec3
shade(intersection_point ip)
{
  switch (ip.material)
  {
    case 0:
      return shade_constant(ip);
    case 1:
      return shade_matte(ip);
    case 2:
      return shade_blinn_phong(ip);
    case 3:
      return shade_reflection(ip);
    default:
      return shade_constant(ip);

  }
}

// Determine the surface color for the first object intersected by
// the given ray, or return the scene background color when no
// intersection is found
vec3
ray_color(int level, vec3 ray_origin, vec3 ray_direction)
{
    intersection_point  ip;

    // If this ray has been reflected too many times, simply
    // return the background color.
    if (level >= 3)
        return scene_background_color;

    // Check if the ray intersects anything in the scene
    if (find_first_intersection(&ip, ray_origin, ray_direction))
    {
        // Shade the found intersection point
        ip.ray_level = level;
        return shade(ip);
    }

    // Nothing was hit, return background color
    return scene_background_color;
}
