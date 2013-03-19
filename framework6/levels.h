#pragma once

/*
 * Every vertex has a 2D position.
 */
typedef struct {
    float x, y;
} point_t;

/*
 * A polygon has a bunch of vertices.
 */
typedef struct {
    unsigned int num_verts;
    point_t *verts;
} poly_t;

/*
 * Represents a level with start and end positions, and a bunch of polygons.
 */
typedef struct {
    unsigned int num_polygons;
    poly_t *polygons;
    point_t start, end;
} level_t;


int load_levels(level_t **levels);
level_t load_level(const char *level_name);
