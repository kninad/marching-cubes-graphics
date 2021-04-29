#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Lighting
{
private:
    struct Direction_Light
    {
        glm::vec3 direction;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct Point_Light
    {
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

public:
    Direction_Light direction_light;
    Point_Light point_light;
    Point_Light fixed_light; 
    const glm::vec3 CCS_lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 reset_position;

    Lighting() {}

    ~Lighting() {}

    void set_reset_pos(glm::vec3 newpos)
    {
        reset_position = newpos;
    }

    glm::vec3 get_reset_pos()
    {
        return reset_position;
    }

    void init()
    {   
        // Some placeholder init values
        direction_light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        direction_light.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        direction_light.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        direction_light.specular = glm::vec3(0.5f, 0.5f, 0.5f);
        
        point_light.position = glm::vec3(0.5f, 0.5f, 2.5f);
        point_light.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        point_light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        point_light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

        fixed_light.position = glm::vec3(0.5f, 2.5f, 3.0f);
        fixed_light.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
        fixed_light.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        fixed_light.specular = glm::vec3(0.5f, 0.5f, 0.5f);

        reset_position = glm::vec3(0.5f, 0.5f, 2.5f); // just for init

    };
};