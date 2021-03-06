#ifndef PLYMODEL_H
#define PLYMODEL_H

#include "types.h"

int         ply_num_triangles, ply_num_vertices, ply_num_normals;

triangle    *ply_triangles;
vec3        *ply_vertices;
vec3        *ply_normals;

void        read_ply_model(const char *fname);

#endif
