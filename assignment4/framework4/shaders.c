/* *****************************************************************************
 *
 * Computer Graphics - University of Amsterdam
 * Assignment 4.3
 *
 * Lorenzo Liberatore <l.liberatore@gmail.com>
 * Ben Witzen <benwitzen@live.nl>
 * March 8th, 2013
 *
 * Implements the various shaders as required by the assignment:
 * - matte shader
 * - blinn-phong shader
 * - reflection shader
 *
 * ************************************************************************** */

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

/*
 * matte shader
 */

vec3
shade_matte(intersection_point ip)
{
	int i;
	vec3 a, b;
	float dot_prod, light_matte = scene_ambient_light;

	// calculate the light intensity in p for all the lights in the scene
	for (i = 0; i < scene_num_lights; i++) {

		// calculate the color of the light in i position
		a = v3_normalize(v3_subtract(scene_lights[i].position, ip.p));

		// use small offset in ray origin to avoid the problem of self-shadowing
		b = v3_add(ip.p, v3_multiply(a, 0.0001));

		// calculate the color of light in i position and if there is no shadow and
		// light is between [0, 1]
		if (!shadow_check(b, a) && light_matte >= 0 && light_matte <= 1) {
			// calculate the contribution of light
			dot_prod = v3_dotprod(ip.n, a);
			// check if the contribution of light it is not negative so not behind the
			// point
			if (dot_prod >= 0) {
				// sums the contribution of the all lights reaching that point 
				light_matte += fmax(0, scene_lights[i].intensity * dot_prod);
			}
		}
	}
	// returns the vector calculated
	return v3_create(light_matte, light_matte, light_matte);
}


/*
 * Calculates shading in a particular intersection point
 */

float diffuse (intersection_point ip, light l_in) {

  vec3 l = v3_normalize(v3_subtract(l_in.position, ip.p));

  // start a bit above surface to avoid self-shading
  if (shadow_check(v3_add(ip.p, v3_multiply(ip.n, 0.0001)), l))
    return 0.0;

  // calculate the angle
  float angle = v3_dotprod(ip.n, l);
  
  // return 0 if the lightsource is behind the surface
  if (angle < 0.0)
    return 0.0;

  // otherwise, return the value
  return l_in.intensity * angle;

}


/*
 * Calculates the Blinn-phong shading in a particular intersection point
 */

float specular (intersection_point ip, light l_in, float phong) {

  vec3 l = v3_normalize(v3_subtract(l_in.position, ip.p));

  // start a bit above surface to avoid self-shading
  if (shadow_check(v3_add(ip.p, v3_multiply(ip.n, 0.0001)), l))
    return 0.0;

  // calculate h
  vec3 h = v3_multiply(v3_add(ip.i, l), 1 / v3_length(v3_add(ip.i, l)));

  // note that h dot n is always positive, so no further checks are needed

  // return the value, raising angle to the power phong
  return l_in.intensity * pow(v3_dotprod(ip.n, h), phong);

}


/*
 * Note:
 * Our blinn-phong function also uses the functions diffuse() and specular(). As
 * explained by Jose Lagerberg on her slides.
 */

vec3
shade_blinn_phong(intersection_point ip)
{
    
  // RED: perform the diffuse function for each scene light  
  // ALL: perform the specular function for each scene light
  //      50 is the phong exponent  
  
  float light_red = scene_ambient_light;
  float light_all = 0;

  for (int i = 0; i < scene_num_lights; i++) {
    light_red += diffuse(ip, scene_lights[i]);
    light_all += specular(ip, scene_lights[i], 50);
  }

  // apply the weighing factors
  light_red *= 0.8;
  light_all *= 0.5;
  
  // add ALL to RED
  light_red += light_all;

  // done!
  return v3_create(light_red, light_all, light_all);    

}

vec3
shade_reflection(intersection_point ip)
{
	// return the product between two vectors and store it in a variable
	float dot = v3_dotprod(ip.i, ip.n);

	// getting the shading vectors calculated
	vec3 r, result, matte = shade_matte(ip);

	// applying the formula r = 2n(i · n) − i to calculate the reflected direction
	r = v3_subtract((v3_multiply(v3_multiply(ip.n, 2), dot)), ip.i);

	// return the surface color (fixed the problem of selfshading)
	result = ray_color(ip.ray_level + 1, v3_add(ip.p, v3_multiply(r, 0.01)), r);

	// the entire result of shading
	return v3_create(((matte.x * 0.75) + (result.x * 0.25)),
	                 ((matte.y * 0.75) + (result.y * 0.25)), 
	                 ((matte.z * 0.75) + (result.z * 0.25)));
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
