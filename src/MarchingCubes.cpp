#include "MarchingCubes.h"

void MarchingCubes::set_3d_model(GLubyte *data, const glm::ivec3 &dims)
{
    raw_data_ptr = data;
    data_dims[0] = dims[0];
    data_dims[1] = dims[1];
    data_dims[2] = dims[2];
}

void MarchingCubes::compute_grid(int num_slices)
{
    float x_delta = 1.0f * (data_dims[0]) / (num_slices - 1);
    float y_delta = 1.0f * (data_dims[1]) / (num_slices - 1);
    float z_delta = 1.0f * (data_dims[2]) / (num_slices - 1);
    glm::vec3 deltas = glm::vec3(x_delta, y_delta, z_delta);

    glm::ivec3 n_slices = glm::vec3(num_slices + 2, num_slices + 2, num_slices + 2);
    // +2 to have space at Top and Bottom i.e for Grid Boundary cases
    glm::ivec3 gridcell_dims = n_slices - 1;
    auto points = get_grid_points(n_slices, deltas);
    fill_cells(n_slices, deltas, points);
}


std::vector<TriFace> MarchingCubes::compute_faces(float isoLevel)
{
    threshold_iso_level(isoLevel);

    GLfloat xmu = 1.0f / data_dims[0];
    GLfloat ymu = 1.0f / data_dims[1];
    GLfloat zmu = 1.0f / data_dims[2];
    glm::vec3 mu(xmu, ymu, zmu);

    std::vector<TriFace> faces;

    for (size_t i = 0; i < gridcells.size(); ++i)
    {
        int cubeIndex = compute_cube_index(gridcells[i], isoLevel);
        if (McLookup::EdgeTable[cubeIndex] == 0)
        {
            continue; // De-generate Case
        }
        auto vertex_list = compute_vertex_list(gridcells[i], isoLevel, cubeIndex);
        
        // Construct a Triangular Face using the TriTable
        for (size_t k = 0; McLookup::TriTable[cubeIndex][k] != -1; k += 3)
        {
            TriFace triangle;
            for (size_t j = 0; j < 3; j++)
            {
                int vertId = McLookup::TriTable[cubeIndex][k + j];
                triangle.points[j] = vertex_list[vertId] * mu;
            }
            glm::vec3 edge_01 = triangle.points[1] - triangle.points[0];
            glm::vec3 edge_02 = triangle.points[2] - triangle.points[0];
            glm::vec3 normal_vec = glm::cross(edge_01, edge_02);
            normal_vec = glm::normalize(normal_vec);
            triangle.normal = normal_vec;
            faces.push_back(triangle);
        }
    }
    return faces;
}



std::vector<VertexPoint> MarchingCubes::compute_verts(float isoLevel)
{
    threshold_iso_level(isoLevel);

    GLfloat xmu = 1.0f / data_dims[0];
    GLfloat ymu = 1.0f / data_dims[1];
    GLfloat zmu = 1.0f / data_dims[2];
    glm::vec3 mu(xmu, ymu, zmu);

    std::vector<VertexPoint> vertices;

    for (size_t i = 0; i < gridcells.size(); ++i)
    {
        int cubeIndex = compute_cube_index(gridcells[i], isoLevel);
        if (McLookup::EdgeTable[cubeIndex] == 0)
        {
            continue; // De-generate Case
        }
        auto vertex_list = compute_vertex_list(gridcells[i], isoLevel, cubeIndex);
        std::array<std::vector<glm::vec3>, 12> points_normals;

        // Construct a Triangular Face Normal using the TriTable
        for (size_t k = 0; McLookup::TriTable[cubeIndex][k] != -1; k += 3)
        {
            glm::vec3 trig_points[3];
            for (size_t j = 0; j < 3; j++)
            {
                int vertId = McLookup::TriTable[cubeIndex][k + j];
                trig_points[j] = vertex_list[vertId];
            }
            glm::vec3 edge_01 = trig_points[1] - trig_points[0];
            glm::vec3 edge_02 = trig_points[2] - trig_points[0];
            glm::vec3 normal_vec = glm::cross(edge_01, edge_02);
            normal_vec = glm::normalize(normal_vec);

            for (int t = 0; t < 3; t++)
            {                
                int vertId = McLookup::TriTable[cubeIndex][k + t];
                points_normals[vertId].push_back(normal_vec);
            }
        }

        // Average out all normal vector for all 12 points
        std::array<glm::vec3, 12> points_avg_normals;        
        for(int t = 0; t < points_normals.size(); t++)
        {
            glm::vec3 avg_normal = glm::vec3(0,0,0);
            if(points_normals[t].empty())
            {
                continue;
            }
            for(const auto& v : points_normals[t])
            {
                avg_normal += v;
            }
            avg_normal /= points_normals[t].size();
            points_avg_normals[t] = avg_normal;
        }

        for (size_t k = 0; McLookup::TriTable[cubeIndex][k] != -1; k += 3)
        {            
            for (size_t j = 0; j < 3; j++)
            {   
                VertexPoint pt;
                int vertId = McLookup::TriTable[cubeIndex][k + j];
                pt.position = vertex_list[vertId] * mu;
                pt.normal = points_avg_normals[vertId];
                vertices.push_back(pt);
            }
        }
    }

    return vertices;
}



void MarchingCubes::clear_raw_data()
{
    if (raw_data_ptr)
    {
        delete[] raw_data_ptr;
    }
}


float MarchingCubes::trilinear_interpolation(float x, float y, float z)
{
    int v0x = floor(x);
    int v0y = floor(y);
    int v0z = floor(z);
    if (v0x >= data_dims[0] - 1)
        v0x = data_dims[0] - 2;
    if (v0y >= data_dims[1] - 1)
        v0y = data_dims[1] - 2;
    if (v0z >= data_dims[2] - 1)
        v0z = data_dims[2] - 2;
    float lx = x - v0x;
    float ly = y - v0y;
    float lz = z - v0z;

    float vxyz = get_raw_data(v0x, v0y, v0z) * (1 - lx) * (1 - ly) * (1 - lz) +
                   get_raw_data(v0x + 1, v0y, v0z) * lx * (1 - ly) * (1 - lz) +
                   get_raw_data(v0x + 1, v0y + 1, v0z) * lx * ly * (1 - lz) +
                   get_raw_data(v0x + 1, v0y + 1, v0z + 1) * lx * ly * lz +
                   get_raw_data(v0x, v0y + 1, v0z + 1) * (1 - lx) * ly * lz +
                   get_raw_data(v0x, v0y, v0z + 1) * (1 - lx) * (1 - ly) * lz +
                   get_raw_data(v0x + 1, v0y, v0z + 1) * lx * (1 - ly) * lz +
                   get_raw_data(v0x, v0y + 1, v0z) * (1 - lx) * ly * (1 - lz);

    return vxyz / 255.0f;
}

GLfloat MarchingCubes::get_raw_data(int x, int y, int z)
{
    int index = z * data_dims.x * data_dims.y + y * data_dims.x + x;
    return raw_data_ptr[index];
}

int MarchingCubes::get_index(int x, int y, int z, int num_x, int num_y)
{
    return num_x * num_y * z + num_x * y + x;
}

glm::vec3 MarchingCubes::vertex_interpolation(float isoLevel, const GridCell &cell, int p1, int p2)
{
    // alpha = (isolevel - cell.p1.val) / (cell.p2.val - cell.p1.val) 
    // point = cell.p1 + alpha * (cell.p2 - cell.p1)
    const float EPSILON = 1e-6; // check against numerical errors
    float alpha; 
    if (glm::abs(cell.val[p2] - cell.val[p1]) < EPSILON)
    {
        std::cout << "[DEBUG] Too Close for comfort!" << std::endl;
        alpha = 0.5f;
    }
    else
    {
        alpha = (isoLevel - cell.val[p1]) / (cell.val[p2] - cell.val[p1]);
    }
    return cell.pnt[p1] + alpha * (cell.pnt[p2] - cell.pnt[p1]);
}


void MarchingCubes::threshold_iso_level(GLfloat &isoLevel)
{
    isoLevel = glm::max(0.005f, isoLevel);
    isoLevel = glm::min(0.999f, isoLevel);
}

std::vector<float> MarchingCubes::get_grid_points(const glm::vec3 &n_slices, const glm::vec3 &deltas)
{
    std::vector<float> points(n_slices.x * n_slices.y * n_slices.z);
    std::cout << "[DEBUG] Total Loops: " << (n_slices.z - 2) * (n_slices.y - 2) * (n_slices.z - 2)  << std::endl;
    for (int z = 1; z < n_slices.z - 1; z++)
    {
        for (int y = 1; y < n_slices.y - 1; y++)
        {
            for (int x = 1; x < n_slices.x - 1; x++)
            {
                points[get_index(x, y, z, n_slices.x, n_slices.y)] = trilinear_interpolation(deltas.x * (x - 1), deltas.y * (y - 1), deltas.z * (z - 1));
            }
        }
    }
    return points;
}

void MarchingCubes::fill_cells(const glm::vec3 &n_slices, const glm::vec3 &deltas, const std::vector<float> &points)
{
    int nslice_x = n_slices.x;
    int nslice_y = n_slices.y;
    int nslice_z = n_slices.z;

    int x_delta = deltas.x;
    int y_delta = deltas.y;
    int z_delta = deltas.z;

    gridcells.clear();
    gridcells.resize((nslice_x - 1) * (nslice_y - 1) * (nslice_z - 1));

    for (int z = 0; z < nslice_z - 1; z++)
    {
        for (int y = 0; y < nslice_y - 1; y++)
        {
            for (int x = 0; x < nslice_x - 1; x++)
            {
                float x0 = x_delta * x;
                float x1 = x0 + x_delta;
                float y0 = y_delta * y;
                float y1 = y0 + y_delta;
                float z0 = z_delta * z;
                float z1 = z0 + z_delta;

                gridcells[get_index(x, y, z, nslice_x - 1, nslice_y - 1)] = {
                    {glm::vec3(x0, y0, z0),
                     glm::vec3(x1, y0, z0),
                     glm::vec3(x1, y1, z0),
                     glm::vec3(x0, y1, z0),
                     glm::vec3(x0, y0, z1),
                     glm::vec3(x1, y0, z1),
                     glm::vec3(x1, y1, z1),
                     glm::vec3(x0, y1, z1)},
                    {
                        points[get_index(x, y, z, nslice_x, nslice_y)],
                        points[get_index(x + 1, y, z, nslice_x, nslice_y)],
                        points[get_index(x + 1, y + 1, z, nslice_x, nslice_y)],
                        points[get_index(x, y + 1, z, nslice_x, nslice_y)],
                        points[get_index(x, y, z + 1, nslice_x, nslice_y)],
                        points[get_index(x + 1, y, z + 1, nslice_x, nslice_y)],
                        points[get_index(x + 1, y + 1, z + 1, nslice_x, nslice_y)],
                        points[get_index(x, y + 1, z + 1, nslice_x, nslice_y)],
                    },
                    x,
                    y,
                    z};
            } // end x loop
        }     // end y loop
    }         // end z loop
}

int MarchingCubes::compute_cube_index(const GridCell &cell, float isoLevel)
{
    int cubeIndex = 0;
    if (cell.val[0] < isoLevel)
        cubeIndex |= 1;
    if (cell.val[1] < isoLevel)
        cubeIndex |= 2;
    if (cell.val[2] < isoLevel)
        cubeIndex |= 4;
    if (cell.val[3] < isoLevel)
        cubeIndex |= 8;
    if (cell.val[4] < isoLevel)
        cubeIndex |= 16;
    if (cell.val[5] < isoLevel)
        cubeIndex |= 32;
    if (cell.val[6] < isoLevel)
        cubeIndex |= 64;
    if (cell.val[7] < isoLevel)
        cubeIndex |= 128;

    return cubeIndex;
}

std::array<glm::vec3, 12> MarchingCubes::compute_vertex_list(const GridCell &cell, float isoLevel, int cubeIndex)
{
    std::array<glm::vec3, 12> vertex_list;
    if (McLookup::EdgeTable[cubeIndex] & 1)
        vertex_list[0] = vertex_interpolation(isoLevel, cell, 0, 1);
    if (McLookup::EdgeTable[cubeIndex] & 2)
        vertex_list[1] = vertex_interpolation(isoLevel, cell, 1, 2);
    if (McLookup::EdgeTable[cubeIndex] & 4)
        vertex_list[2] = vertex_interpolation(isoLevel, cell, 2, 3);
    if (McLookup::EdgeTable[cubeIndex] & 8)
        vertex_list[3] = vertex_interpolation(isoLevel, cell, 3, 0);
    if (McLookup::EdgeTable[cubeIndex] & 16)
        vertex_list[4] = vertex_interpolation(isoLevel, cell, 4, 5);
    if (McLookup::EdgeTable[cubeIndex] & 32)
        vertex_list[5] = vertex_interpolation(isoLevel, cell, 5, 6);
    if (McLookup::EdgeTable[cubeIndex] & 64)
        vertex_list[6] = vertex_interpolation(isoLevel, cell, 6, 7);
    if (McLookup::EdgeTable[cubeIndex] & 128)
        vertex_list[7] = vertex_interpolation(isoLevel, cell, 7, 4);
    if (McLookup::EdgeTable[cubeIndex] & 256)
        vertex_list[8] = vertex_interpolation(isoLevel, cell, 0, 4);
    if (McLookup::EdgeTable[cubeIndex] & 512)
        vertex_list[9] = vertex_interpolation(isoLevel, cell, 1, 5);
    if (McLookup::EdgeTable[cubeIndex] & 1024)
        vertex_list[10] = vertex_interpolation(isoLevel, cell, 2, 6);
    if (McLookup::EdgeTable[cubeIndex] & 2048)
        vertex_list[11] = vertex_interpolation(isoLevel, cell, 3, 7);

    return vertex_list;
}
