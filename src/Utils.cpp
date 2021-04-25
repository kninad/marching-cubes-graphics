#include "Utils.h"

const std::string base_path = "../src/data/";

glm::vec3 Utils::get_dims(Utils::model3d_t model_type)
{
    if (model_type == Utils::BUCKY)
    {
        return glm::vec3(32, 32, 32);
    }
    else if (model_type == Utils::BONSAI)
    {
        return glm::vec3(512, 512, 154);
    }
    else if (model_type == Utils::TEAPOT)
    {
        return glm::vec3(256, 256, 178);
    }
    else if (model_type == Utils::HEAD)
    {
        return glm::vec3(256, 256, 225);
    }
    else
    {
        std::cout << "invalid model type!";
        // return glm::vec3(0,0,0);
        exit(100);
    }
}

GLubyte *Utils::load_3d_raw_data(std::string texture_path, glm::vec3 dimension)
{
    size_t size = dimension[0] * dimension[1] * dimension[2];

    FILE *fp;
    GLubyte *data = new GLubyte[size]; // 8bit
    if (!(fp = fopen(texture_path.c_str(), "rb")))
    {
        std::cout << "Error: opening .raw file failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "OK: open .raw file successed" << std::endl;
    }
    if (fread(data, sizeof(char), size, fp) != size)
    {
        std::cout << "Error: read .raw file failed" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "OK: read .raw file successed" << std::endl;
    }
    fclose(fp);
    return data;
}

GLubyte *Utils::load_3Dfrom_type(Utils::model3d_t model_type)
{
    GLubyte *data = nullptr;
    if (model_type == Utils::BUCKY)
    {
        data = Utils::load_3d_raw_data(base_path + "Bucky_32_32_32.raw", glm::vec3(32, 32, 32));
    }
    else if (model_type == Utils::BONSAI)
    {
        data = Utils::load_3d_raw_data(base_path + "Bonsai_512_512_154.raw", glm::vec3(512, 512, 154));
    }
    else if (model_type == Utils::TEAPOT)
    {
        data = Utils::load_3d_raw_data(base_path + "BostonTeapot_256_256_178.raw", glm::vec3(256, 256, 178));
    }
    else if (model_type == Utils::HEAD)
    {
        data = Utils::load_3d_raw_data(base_path + "Head_256_256_225.raw", glm::vec3(256, 256, 225));
    }

    // check for errors
    if (!data)
    {
        std::cout << "Error in reading raw data! data pointer is currently set to null." << std::endl;
    }
    return data;
}

void Utils::print_glmvec3(const glm::vec3 vec)
{
    std::cout << "\nVec3: ";
    for (int i = 0; i < 3; i++)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}

glm::vec3 Utils::ngolor_to_glm(nanogui::Color color)
{
    glm::vec3 ret_color = glm::vec3(color[0], color[1], color[2]);
    return ret_color;
}


// ************* Gui Control Class *************

Utils::GuiControl::GuiControl(GLFWwindow *window, Camera *camera)
{
    init(window, camera);
}

void Utils::GuiControl::init(GLFWwindow *window, Camera *camera)
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

void Utils::GuiControl::camera_movement_controls()
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

