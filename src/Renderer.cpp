#include "Renderer.h"

Lighting *Renderer::m_lightings = new Lighting();

glm::vec3 CCS_lightDir = glm::vec3(0.0f, 1.0f, 1.0f);

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::run()
{
    init();
    display(this->m_window);
}

void Renderer::init()
{
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    #if defined(__APPLE__)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    this->m_window =
        glfwCreateWindow(m_camera->width, m_camera->height, "Marching Cubes", nullptr, nullptr);
    glfwMakeContextCurrent(this->m_window);

    // Create Camera, GUI and Lighting
    m_camera->init();
    m_gui->init(m_window, m_camera);
    m_lightings->init();

    glewExperimental = GL_TRUE;
    glewInit();

}

void Renderer::display(GLFWwindow *window)
{
    Shader m_shader = Shader("../src/shader/light.vert", "../src/shader/light.frag");

    // Main frame while loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call
        // corresponding response functions
        glfwPollEvents();

        /*
         * TODO: Able to reset scene and model as needed
         */
        // note, nano_reload_model is init with true for 1st run
        is_scene_reset = (m_gui->reloadModel || m_gui->reset);
        if (is_scene_reset)
        {
            scene_reset();
            std::cout << "\n[DebugLog] Scene has been reset!\n";
            is_scene_reset = false; // just a safety check!
            m_gui->reloadModel = false;
            m_gui->reset = false;
        }

        m_gui->camera_movement_controls();
        // camera_move();

        m_shader.use();

        setup_uniform_values(m_shader);

        draw_scene(m_shader);

        m_nanogui_screen->drawWidgets();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return;
}

void Renderer::scene_reset()
{
    load_models();
    m_camera->reset();
}

void Renderer::load_models()
{
    /*
     * TODO: Create Object class and bind VAO & VBO.
    */

    // if (cur_obj_ptr)
    // {
    //     delete cur_obj_ptr;
    // }
    // std::string obj_path = "../src/objs/";
    // if (nano_reload_model)
    // {
    //     model_name = obj_path + nano_model_name;
    //     std::cout << "\n[DebugLog] NanoObjPath: " << obj_path << nano_model_name;
    //     std::cout << "\n[DebugLog] Final Model Name: \n"
    //               << model_name;
    // }
    // cur_obj_ptr = new Object(model_name);

    // Object model(model_name);
    glGenVertexArrays(1, &(cur_obj_ptr->vao)); // public member
    glGenBuffers(1, &(cur_obj_ptr->vbo));
    glGenBuffers(1, &(cur_obj_ptr->ebo));

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute
    // pointer(s).
    glBindVertexArray(cur_obj_ptr->vao); // model.vao);

    glBindBuffer(GL_ARRAY_BUFFER, cur_obj_ptr->vbo); // model.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Object::Vertex) * cur_obj_ptr->vao_vertices.size(),
                 &(cur_obj_ptr->vao_vertices[0]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cur_obj_ptr->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * cur_obj_ptr->veo_indices.size(),
                 &(cur_obj_ptr->veo_indices[0]), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid *)(8 * sizeof(float)));

    // Bitangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid *)(11 * sizeof(float)));

    glBindVertexArray(0); // Unbind VAO

    // Texture Loading

    std::string texture_path = "../src/textures/";
    std::string texture_file;
    if (nano_model_name == "cyborg.obj")
    {
        texture_file = texture_path + "cyborg";
    }
    else if (nano_model_name == "cube.obj" || nano_model_name == "two_cubes.obj")
    {
        texture_file = texture_path + "cube";
    }
    std::string _diffuse_file = texture_file + "_diffuse.png";
    std::string _normal_file = texture_file + "_normal.png";
    cur_obj_ptr->diffuse_textureID = cur_obj_ptr->loadTexture(_diffuse_file.c_str());
    cur_obj_ptr->normal_textureID = cur_obj_ptr->loadTexture(_normal_file.c_str());

    /*
     * TODO: Set Camera parameters here
     */
    m_camera->model_center_position = cur_obj_ptr->center_cam_pos;
    // Sane default for camera position options in nano gui
    nano_campos_x = cur_obj_ptr->center_cam_pos[0];
    nano_campos_y = cur_obj_ptr->center_cam_pos[1];
    nano_campos_z = cur_obj_ptr->center_cam_pos[2];

    // Also Set the Initial Location for the Point Light
    m_lightings->point_light.position = cur_obj_ptr->center_cam_pos;
    m_lightings->set_reset_pos(cur_obj_ptr->center_cam_pos); // Save the reset position for point light

    nanogui_init(this->m_window);
}

void Renderer::draw_scene(Shader &shader)
{
    /*
     * TODO: Remember to enable GL_DEPTH_TEST and GL_CULL_FACE
     */
    glEnable(GL_DEPTH_TEST);
    glPointSize(4.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    if (m_gui->cullType == Utils::CW)
    {
        glFrontFace(GL_CW);
    }
    else if (m_gui->cullType == Utils::CCW)
    {
        glFrontFace(GL_CCW);
    }

    // Set up some basic parameters
    glClearColor(background_color[0], background_color[1], background_color[2],
                 background_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*
     * TODO: Draw object
     */
    draw_object(shader, *cur_obj_ptr);
}

void Renderer::draw_object(Shader &shader, Object &object)
{

    glBindVertexArray(object.vao);

    GLenum our_mode;
    if (m_gui->renderType == Utils::LINE)
    {
        // our_mode = GL_LINES;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        our_mode = GL_TRIANGLES;
    }
    else if (m_gui->renderType == Utils::TRIANGLE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        our_mode = GL_TRIANGLES;
    }
    else if (m_gui->renderType == Utils::POINT)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        our_mode = GL_POINTS;
    }
    glDrawArrays(our_mode, 0, object.vao_vertices.size());
    // glDrawElements(our_mode, object.vao_vertices.size(), GL_UNSIGNED_INT, 0);

    // Reset back Polygon Mode to GL_FILL so as to not mess up the Nano GUI in Lineframe mode.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0); // unbind vao.
}

void Renderer::setup_uniform_values(Shader &shader)
{
    // Model matrix.
    glm::mat4 model_mat = glm::mat4(1.0f);
    // View Matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = m_camera->GetViewMatrix();
    // Projection Matrix
    float aspect_ratio = (float)m_camera->width / (float)m_camera->height;
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(m_camera->zoom), aspect_ratio, m_gui->z_near, m_gui->z_far);

    glm::vec3 custom_color = glm::vec3(m_gui->color[0], m_gui->color[1], m_gui->color[2]);

    shader.setMat4("model_mat", model_mat);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    shader.setVec3("object_color", custom_color);
    shader.setVec3("view_position", m_camera->position);

    // // ****************************************************************
    // // Set the values of Lightings object from Nanogui Controls
    // m_lightings->direction_light.direction = glm::vec3(glm::inverse(view) * glm::vec4(CCS_lightDir, 0.0f));
    // // m_lightings->direction_light.direction = glm::vec3(0.0f, 0.0f, 10.0f);

    // m_lightings->direction_light.ambient = Utils::ngolor_to_glm(n_dirL_amb);
    // m_lightings->direction_light.diffuse = Utils::ngolor_to_glm(n_dirL_dif);
    // m_lightings->direction_light.specular = Utils::ngolor_to_glm(n_dirL_spc);

    // m_lightings->point_light.ambient = Utils::ngolor_to_glm(n_posL_amb);
    // m_lightings->point_light.diffuse = Utils::ngolor_to_glm(n_posL_dif);
    // m_lightings->point_light.specular = Utils::ngolor_to_glm(n_posL_spc);

    // // Rotation of Point Light
    // glm::vec3 pointL_pos = m_lightings->point_light.position;
    // float theta = (float)glfwGetTime();
    // float sin_theta = glm::sin(theta);
    // float cos_theta = glm::cos(theta);
    // if (n_pLight_rotX)
    // {
    //     float y = pointL_pos[1];
    //     float z = pointL_pos[2];
    //     float mag = glm::sqrt(y * y + z * z);
    //     pointL_pos[1] = mag * cos_theta;
    //     pointL_pos[2] = mag * sin_theta;
    //     m_lightings->point_light.position = pointL_pos;
    // }
    // else if (n_pLight_rotY)
    // {
    //     float x = pointL_pos[1];
    //     float z = pointL_pos[2];
    //     float mag = glm::sqrt(x * x + z * z);
    //     pointL_pos[0] = mag * cos_theta;
    //     pointL_pos[2] = mag * sin_theta;
    //     m_lightings->point_light.position = pointL_pos;
    // }
    // else if (n_pLight_rotZ)
    // {
    //     float x = pointL_pos[0];
    //     float y = pointL_pos[1];
    //     float mag = glm::sqrt(y * y + x * x);
    //     pointL_pos[0] = mag * cos_theta;
    //     pointL_pos[1] = mag * sin_theta;
    //     m_lightings->point_light.position = pointL_pos;
    // }
    // else if (n_pLight_reset)
    // {
    //     m_lightings->point_light.position = m_lightings->get_reset_pos();
    //     n_pLight_reset = false; // reset the value back to sane default.
    // }

    // // Directional Light Uniforms
    // unsigned int onDLightLoc = glGetUniformLocation(shader.program, "on_Dlight");
    // unsigned int dLightDirLoc = glGetUniformLocation(shader.program, "dlight_dir");
    // unsigned int dLightAmbLoc = glGetUniformLocation(shader.program, "dlight_amb");
    // unsigned int dLightDifLoc = glGetUniformLocation(shader.program, "dlight_dif");
    // unsigned int dLightSpcLoc = glGetUniformLocation(shader.program, "dlight_spc");

    // glUniform1i(onDLightLoc, n_on_dirL);
    // glUniform3fv(dLightDirLoc, 1, glm::value_ptr(m_lightings->direction_light.direction));
    // glUniform3fv(dLightAmbLoc, 1, glm::value_ptr(m_lightings->direction_light.ambient));
    // glUniform3fv(dLightDifLoc, 1, glm::value_ptr(m_lightings->direction_light.diffuse));
    // glUniform3fv(dLightSpcLoc, 1, glm::value_ptr(m_lightings->direction_light.specular));

    // // Positional Light Uniforms
    // unsigned int onPLightLoc = glGetUniformLocation(shader.program, "on_Plight");
    // unsigned int pLightPosLoc = glGetUniformLocation(shader.program, "plight_pos");
    // unsigned int pLightAmbLoc = glGetUniformLocation(shader.program, "plight_amb");
    // unsigned int pLightDifLoc = glGetUniformLocation(shader.program, "plight_dif");
    // unsigned int pLightSpcLoc = glGetUniformLocation(shader.program, "plight_spc");

    // glUniform1i(onPLightLoc, n_on_posL);
    // glUniform3fv(pLightPosLoc, 1, glm::value_ptr(m_lightings->point_light.position));
    // glUniform3fv(pLightAmbLoc, 1, glm::value_ptr(m_lightings->point_light.ambient));
    // glUniform3fv(pLightDifLoc, 1, glm::value_ptr(m_lightings->point_light.diffuse));
    // glUniform3fv(pLightSpcLoc, 1, glm::value_ptr(m_lightings->point_light.specular));

}
