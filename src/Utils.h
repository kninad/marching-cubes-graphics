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
        void init(GLFWwindow *window)
        {
            if (screen)
            {
                delete screen;
            }
            screen = new nanogui::Screen();
            screen->initialize(window, true);
            glViewport(0, 0, camera->width, camera->height);
            bool enabled = true;

            // ********************************************************************
            // 1st nanogui GUI
            nanogui::FormHelper *gui = new nanogui::FormHelper(screen);
            nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(0, 0), "Camera Controls");

            gui->addGroup("Color");
            gui->addVariable("Object Color", color);

            gui->addGroup("Camera Position");
            gui->addVariable("X", campos_x)->setSpinnable(true);
            gui->addVariable("Y", campos_y)->setSpinnable(true);
            gui->addVariable("Z", campos_z)->setSpinnable(true);

            gui->addGroup("Camera Rotation");
            gui->addVariable("Rotate Value", cam_rotate_value)->setSpinnable(true);
            gui->addButton("Rotate Right +", [this]() { rotate_xup = true; });
            gui->addButton("Rotate Right -", [this]() { rotate_xdown = true; });
            gui->addButton("Rotate Up +", [this]() { rotate_yup = true; });
            gui->addButton("Rotate Up -", [this]() { rotate_ydown = true; });
            gui->addButton("Rotate Front +", [this]() { rotate_zup = true; });
            gui->addButton("Rotate Front -", [this]() { rotate_zdown = true; });

            gui->addGroup("Camera Zoom");
            gui->addVariable("Z Near", z_near)->setSpinnable(true);
            gui->addVariable("Z Far", z_far)->setSpinnable(true);
            gui->addVariable("FOV", fov)->setSpinnable(true);

            gui->addGroup("Configuration");
            gui->addVariable("Model Name", modelType, enabled)
                ->setItems({"BUCKY", "TEAPOT", "BONSAI", "HEAD"});
            gui->addVariable("Render Type", renderType, enabled)
                ->setItems({"POINT", "LINE", "TRIANGLE"});
            // gui->addVariable("Cull Type", cullType, enabled)->setItems({"CW", "CCW"});
            gui->addVariable("Shading Type", shadingType, enabled)->setItems({"SMOOTH", "FLAT"});
            gui->addButton("Reload Model", [this]() { reloadModel = true; });
            gui->addButton("Reset Camera", [this]() { reset = true; });
            gui->addVariable("Viewing Depth", view_depth)->setSpinnable(true);
            gui->addVariable("Num Cuts", num_cuts);

            // ********************************************************************

            screen->setVisible(true);
            screen->performLayout();

            glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
                screen->cursorPosCallbackEvent(x, y);
            });

            glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int modifiers) {
                screen->mouseButtonCallbackEvent(button, action, modifiers);
            });

            glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                    glfwSetWindowShouldClose(window, GL_TRUE);
                if (key >= 0 && key < 1024)
                {
                    if (action == GLFW_PRESS)
                        keys[key] = true;
                    else if (action == GLFW_RELEASE)
                        keys[key] = false;
                }
            });

            glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
                screen->charCallbackEvent(codepoint);
            });

            glfwSetDropCallback(window, [](GLFWwindow *, int count, const char **filenames) {
                screen->dropCallbackEvent(count, filenames);
            });

            glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
                screen->scrollCallbackEvent(x, y);
                // camera->ProcessMouseScroll(y);
            });

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
                screen->resizeCallbackEvent(width, height);
            });
        }

        void camera_controls()
        {
            GLfloat current_frame = glfwGetTime();
            delta_time = current_frame - last_frame_time;
            last_frame_time = current_frame;

            camera->position = glm::vec3(campos_x, campos_y, campos_z);

            // Change camera FOV or Zoom
            // Extra part!
            if (fov < 1.0f)
            {
                fov = 1.0f;
            }
            else if (fov > 75.0f)
            {
                fov = 75.0f;
            }
            camera->zoom = fov;

            // Camera controls
            if (keys[GLFW_KEY_W])
            {
                camera->process_keyboard(FORWARD, delta_time);
            }
            if (keys[GLFW_KEY_S])
            {
                camera->process_keyboard(BACKWARD, delta_time);
            }
            if (keys[GLFW_KEY_A])
            {
                camera->process_keyboard(LEFT, delta_time);
            }
            if (keys[GLFW_KEY_D])
            {
                camera->process_keyboard(RIGHT, delta_time);
            }
            if (keys[GLFW_KEY_Q])
            {
                camera->process_keyboard(UP, delta_time);
            }
            if (keys[GLFW_KEY_E])
            {
                camera->process_keyboard(DOWN, delta_time);
            }

            // For Rotation, use the "velocity" as the nanogui rotation value
            if (keys[GLFW_KEY_I] || rotate_xup)
            {
                camera->process_keyboard(ROTATE_X_UP, cam_rotate_value);
                rotate_xup = false;
            }
            if (keys[GLFW_KEY_K] || rotate_xdown)
            {
                camera->process_keyboard(ROTATE_X_DOWN, cam_rotate_value);
                rotate_xdown = false;
            }
            if (keys[GLFW_KEY_J] || rotate_yup)
            {
                camera->process_keyboard(ROTATE_Y_UP, cam_rotate_value);
                rotate_yup = false;
            }
            if (keys[GLFW_KEY_L] || rotate_ydown)
            {
                camera->process_keyboard(ROTATE_Y_DOWN, cam_rotate_value);
                rotate_ydown = false;
            }
            if (keys[GLFW_KEY_U] || rotate_zup)
            {
                camera->process_keyboard(ROTATE_Z_UP, cam_rotate_value);
                rotate_zup = false;
            }
            if (keys[GLFW_KEY_O] || rotate_zdown)
            {
                camera->process_keyboard(ROTATE_Z_DOWN, cam_rotate_value);
                rotate_zdown = false;
            }
        }
    };

};

// // Bonsai_512_512_154.raw
// // BostonTeapot_256_256_178.raw
// // Bucky_32_32_32.raw
// // Head_256_256_225.raw
