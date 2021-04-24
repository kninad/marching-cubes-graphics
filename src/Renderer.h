#pragma once

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Camera.h"
#include "Lighting.h"
// #include "Object.h"
#include "Shader.h"
#include "Utils.h"

class Renderer
{
   public:
    GLFWwindow* m_window;

    static Utils::GuiControl *m_gui;
    static Camera* m_camera;
    static Lighting* m_lightings;

    static nanogui::Screen* m_nanogui_screen;

    glm::vec4 background_color = glm::vec4(0.0, 0.0, 0.0, 0.0);

    bool is_scene_reset = true;

    GLfloat delta_time = 0.0;
    GLfloat last_frame = 0.0;


   public:
    Renderer();

    ~Renderer();

    void run();

   private:

    void init();

    void nanogui_init(GLFWwindow* window);

    void display(GLFWwindow* window);

    void load_models();

    void draw_scene(Shader& shader);

    void draw_object(Shader& shader, Object& object);

    void setup_uniform_values(Shader& shader);

    void scene_reset();

};
