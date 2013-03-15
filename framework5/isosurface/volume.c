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

#include <stdio.h>
#include <stdlib.h>

#include "volume.h"

/* The voxels of the volume dataset, stored as a one-dimensional array */
unsigned char   *volume;

/* The dimensions of the volume dataset */
int     nx, ny, nz;

/* The size of a voxel */
float   sizex, sizey, sizez;

/* Utility function to convert the index of a voxel
   into an index in the volume array above */
int
voxel2idx(int i, int j, int k)
{
    return (k*ny + j)*nx + i;
}

/* Extract a cell from the volume, so that datapoint 0 of the
   cell corresponds to voxel (i, j, k), datapoint 1 to voxel (i+1, j, k),
   etc. See the assignment. */
cell
get_cell(int i, int j, int k)
{
    cell c;

    // offsets needed to encapsulate in for-loop
    float i_array[8] = {i+0, i+1, i+0, i+1, i+0, i+1, i+0, i+1};
    float j_array[8] = {j+0, j+0, j+1, j+1, j+0, j+0, j+1, j+1};
    float k_array[8] = {k+0, k+0, k+0, k+0, k+1, k+1, k+1, k+1};

    // fill the cell
    for (int a = 0; a < 8; a++) {
      c.p[a] = v3_create(i_array[a], j_array[a], k_array[a]);
      c.n[a] = v3_crossprod(c.p[a], v3_set_component(c.p[a], 0, c.p[a].x + 1));
      c.value[a] = volume[voxel2idx(i_array[a], j_array[a], k_array[a])];
    }

    // return the cell
    return c;
}

/* Utility function to read a volume dataset from a VTK file.
   This will store the data in the "volume" array and update the dimension
   and size values. */

void
read_volume(const char *fname)
{
    FILE *f;
    char s[256];
    int nvoxels;

    printf("Reading %s\n", fname);
    f = fopen(fname, "rb");

    if (!f)
    {
        fprintf(stderr, "read_volume(): Could not open file '%s' for reading!\n", fname);
        exit(-1);
    }

    // header line
    fgets(s, 255, f);

    // comment line
    fgets(s, 255, f);

    // BINARY
    fgets(s, 255, f);

    // DATASET STRUCTURED_POINTS
    fgets(s, 255, f);

    // DIMENSIONS %d %d %d
    fscanf(f, "%s %d %d %d\n", s, &nx, &ny, &nz);
    printf("%d x %d x %d voxels\n", nx, ny, nz);

    // ASPECT_RATIO/SPACING %f %f %f
    fscanf(f, "%s %f %f %f\n", s, &sizex, &sizey, &sizez);
    printf("voxel sizes: %.3f, %.3f, %.3f\n", sizex, sizey, sizez);

    // ORIGIN ...
    fgets(s, 255, f);

    // POINT_DATA ...
    fgets(s, 255, f);

    // SCALARS ...
    fgets(s, 255, f);

    // LOOKUP_TABLE ...
    fgets(s, 255, f);

    // allocate memory to hold the volume data and read it from file
    nvoxels = nx * ny * nz;
    volume = (unsigned char*)malloc(nvoxels);

    if (fread(volume, 1, nvoxels, f) < (size_t)nvoxels)
    {
        printf("WARNING: not all data could be read!\n");
    }

    fclose(f);
}
