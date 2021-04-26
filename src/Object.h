#pragma once

#include <vector>
#include "MarchingCubes.h"

class Object
{
public:
    struct Vertex
    {
        // Position
        glm::vec3 Position;
        // Normal
        glm::vec3 Normal;
    };

    std::vector<Vertex> vao_vertices;
    GLuint vao, vbo;

public:
    Object(){};

    ~Object(){};

    // void loadObjMesh(const std::vector<Face *> &faces)
    // {
    //     // Clear out previous contents of vao_vertices
    //     vao_vertices.clear();
    //     for (int i = 0; i < faces.size(); i++)
    //     {
    //         for (int j = 0; j < 3; j++)
    //         {
    //             Vertex tmp_vert;
    //             tmp_vert.Position = faces[i]->iList[j]->position;
    //             tmp_vert.Normal = faces[i]->iList[j]->normal;
    //             vao_vertices.push_back(tmp_vert);
    //         }
    //     }
    //     std::cout << "[DEBUG] Number of Triangles in Mesh: " << vao_vertices.size() / 3 << std::endl;
    // }

    void loadFaces(const std::vector<TriFace> &faces)
    {
        // Clear out previous contents of vao_vertices
        vao_vertices.clear();
        for(const auto& triangle : faces)
        {
            for(int i = 0; i < 3; i++)
            {
                Vertex tmp_vert;
                tmp_vert.Position = triangle.points[i];
                tmp_vert.Normal = triangle.normal;
                vao_vertices.push_back(tmp_vert);
            }
        }
        std::cout << "[DEBUG] Number of Triangles in Mesh: " << vao_vertices.size() / 3 << std::endl;
    }

};