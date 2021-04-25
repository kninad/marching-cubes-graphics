#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <nanogui/nanogui.h>

#include "Camera.h"

namespace Utils
{

    enum render_type
    {
        POINT = 0,
        LINE = 1,
        TRIANGLE = 2
    };
    enum culling_type
    {
        CW = 0,
        CCW = 1
    };
    enum shading_type
    {
        SMOOTH = 0,
        FLAT = 1,
    };
    enum depth_type
    {
        LESS = 0,
        ALWAYS = 1,
    };

    enum model3d_t
    {
        BUCKY,
        TEAPOT,
        BONSAI,
        HEAD,
    };

    glm::vec3 get_dims(model3d_t model_type);

    GLubyte *load_3d_raw_data(std::string texture_path, glm::vec3 dimension);

    GLubyte *load_3Dfrom_type(model3d_t model_type);

    void print_glmvec3(const glm::vec3 vec);

    glm::vec3 ngolor_to_glm(nanogui::Color color);

    // GUI CONTROLS

    nanogui::Screen *screen = nullptr;

    class GuiControl
    {
    public:
        nanogui::Color color;
        // camera
        float z_far = 1000.0f;
        float z_near = 1.0f;
        float fov = 45.0f;
        // model
        render_type renderType = TRIANGLE;
        culling_type cullType = CW;
        shading_type shadingType = SMOOTH;
        model3d_t modelType = TEAPOT;
        float view_depth = 0.01f;
        int num_cuts = 50;
        bool reloadModel = false;
        bool reset = false;

        static bool keys[1024];

    private:
        // camera
        Camera *camera = nullptr;
        GLfloat delta_time, last_frame_time;
        float campos_x, campos_y, campos_z;
        float cam_rotate_value;
        bool rotate_xup = false;
        bool rotate_yup = false;
        bool rotate_zup = false;
        bool rotate_xdown = false;
        bool rotate_ydown = false;
        bool rotate_zdown = false;

    public:
        GuiControl(GLFWwindow *window, Camera *camera);

        void init(GLFWwindow *window, Camera *camera);

        void camera_movement_controls();
    };

};

// // Bonsai_512_512_154.raw
// // BostonTeapot_256_256_178.raw
// // Bucky_32_32_32.raw
// // Head_256_256_225.raw
