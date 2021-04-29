#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <array>

#include "LookupTable.h"

struct GridCell
{
    glm::vec3 pnt[8];
    float val[8];
    int x, y, z;
};

struct VertexPoint
{
    glm::vec3 position;
    glm::vec3 normal;
};

struct TriFace
{
    glm::vec3 points[3];
    glm::vec3 normal;
};

class MarchingCubes
{
private:
    GLubyte *raw_data_ptr = nullptr;
    int data_dims[3];
    std::vector<GridCell> gridcells;

public:
    void set_3d_model(GLubyte *data, const glm::vec3 &dims);

    void compute_grid(int num_slices);

    std::vector<TriFace> compute_faces(GLfloat isoLevel);

    std::vector<VertexPoint> compute_verts(GLfloat isoLevel);

    void clear_raw_data();

private:
    float trilinear_interpolation(float x, float y, float z);

    float get_raw_data(int x, int y, int z);

    int get_index(int x, int y, int z, int num_x, int num_y);

    glm::vec3 vertex_interpolation(float isoLevel, GridCell cell, int p1, int p2);

    void threshold_iso_level(GLfloat &isoLevel);

    std::vector<float> get_grid_points(const glm::vec3 &n_slices, const glm::vec3 &deltas);

    void fill_cells(const glm::vec3 &n_slices, const glm::vec3 &deltas, const std::vector<float> &points);

    int compute_cube_index(const GridCell &cell, float isoLevel);

    std::array<glm::vec3, 12> compute_vertex_list(const GridCell &cell, float isoLevel, int cubeIndex);
};