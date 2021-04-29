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
#include "Lighting.h"

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
        LOBSTER,
        ENGINE,
        TOOTH,
    };

    glm::ivec3 get_dims(model3d_t model_type);

    GLubyte *load_3d_raw_data(std::string texture_path, glm::vec3 dimension);

    GLubyte *load_3Dfrom_type(model3d_t model_type);

    void print_glmvec3(const glm::vec3 vec);

    glm::vec3 ngolor_to_glm(nanogui::Color color);

    // GUI CONTROLS

    class GuiControl
    {
    public:
        nanogui::Color color = nanogui::Color(0.5f, 0.5f, 0.5f, 1.0f);
        // Camera Parameters
        float z_far = 1000.0f;
        float z_near = 1.0f;
        float fov = 45.0f;
        GLfloat delta_time, last_frame_time;
        // Model Rendering
        render_type renderType = TRIANGLE;
        culling_type cullType = CW;
        shading_type shadingType = SMOOTH;
        model3d_t modelType = TEAPOT;
        bool reloadModel = true;
        bool reset = false;
        // Marching Cubes Paramters
        float view_depth = 0.1f;
        int num_cuts = 100;

        // static bool keys[1024];
        nanogui::Screen *gui_screen = nullptr;

        // Lighting Controls
        bool pLight_reset = false; // Reset dynamic point light?

        bool pLight_rotX = false;
        bool pLight_rotY = false;
        bool pLight_rotZ = false;

        bool on_dirL = false;
        nanogui::Color dirL_amb = nanogui::Color(0.05f, 0.05f, 0.05f, 1.0f);
        nanogui::Color dirL_dif = nanogui::Color(0.4f, 0.4f, 0.4f, 1.0f);
        nanogui::Color dirL_spc = nanogui::Color(0.5f, 0.5f, 0.5f, 1.0f);

        bool on_posL = false;
        nanogui::Color posL_amb = nanogui::Color(0.05f, 0.05f, 0.05f, 1.0f);
        nanogui::Color posL_dif = nanogui::Color(0.8f, 0.8f, 0.8f, 1.0f);
        nanogui::Color posL_spc = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);


    private:
        // camera
        Camera *camera = nullptr;
        // lighting
        Lighting *lighting = nullptr;
        // camera specific params
        float campos_x = 0.5f, campos_y = 0.5f, campos_z = 2.5f;
        float cam_rotate_value = 10;
        bool rotate_xup = false;
        bool rotate_yup = false;
        bool rotate_zup = false;
        bool rotate_xdown = false;
        bool rotate_ydown = false;
        bool rotate_zdown = false;

    public:

        GuiControl() {};

        GuiControl(GLFWwindow *window, Camera *camera, Lighting *lighting);

        void init(GLFWwindow *window, Camera *camera, Lighting *lighting);

        void camera_movement_controls();

        void screen_draw_widgets();

        void update_lighting();
    };

};

// // Bonsai_512_512_154.raw
// // BostonTeapot_256_256_178.raw
// // Bucky_32_32_32.raw
// // Head_256_256_225.raw
// file:///home/ninad/Documents/devel-github/marching-cubes-graphics/src/data/Tooth_103_94_161.raw
// file:///home/ninad/Documents/devel-github/marching-cubes-graphics/src/data/Engine_256_256_128.raw
// file:///home/ninad/Documents/devel-github/marching-cubes-graphics/src/data/Lobster_301_324_56.raw

