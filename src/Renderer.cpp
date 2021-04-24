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

        // Textures
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, cur_obj_ptr->diffuse_textureID);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, cur_obj_ptr->normal_textureID);

        m_shader.use();

        // std::cout << "\n[DebugLog] Setting up uniform values\n";
        setup_uniform_values(m_shader);

        // std::cout << "\n[DebugLog] Drawing the scene!\n";
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
    if (nano_enum_cull == CW)
    {
        glFrontFace(GL_CW);
    }
    else if (nano_enum_cull == CCW)
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
    /*
     * TODO: Draw object
     */

    glBindVertexArray(object.vao);

    GLenum our_mode;
    if (nano_enum_render == LINE)
    {
        // our_mode = GL_LINES;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        our_mode = GL_TRIANGLES;
    }
    else if (nano_enum_render == TRIANGLE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        our_mode = GL_TRIANGLES;
    }
    else if (nano_enum_render == POINT)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        our_mode = GL_POINTS;
    }
    glDrawArrays(our_mode, 0, object.vao_vertices.size());
    // glDrawElements(our_mode, object.vao_vertices.size(), GL_UNSIGNED_INT, 0);

    // Reset back Polygon Mode to GL_FILL so as to not mess up the Nano GUI.
    // Otherwise it just make everything in the GUI to be in lineframe mode.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0); // unbind vao.
}

void Renderer::setup_uniform_values(Shader &shader)
{
    /*
     * TODO: Define uniforms for your shader
    */

    // Model matrix.
    glm::mat4 model_mat = glm::mat4(1.0f);

    // View Matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = m_camera->GetViewMatrix();

    // Projection Matrix
    float aspect_ratio = (float)m_camera->width / (float)m_camera->height;
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(m_camera->zoom), aspect_ratio, nano_znear, nano_zfar);

    // glm::vec3 custom_color = glm::vec3(nano_col_val[0], nano_col_val[1], nano_col_val[2]);
    glm::vec3 custom_color = glm::vec3(1.0f, 1.0f, 1.0f);

    // Set the values of Lightings object from Nanogui Controls
    m_lightings->direction_light.direction = glm::vec3(glm::inverse(view) * glm::vec4(CCS_lightDir, 0.0f));
    // m_lightings->direction_light.direction = glm::vec3(0.0f, 0.0f, 10.0f);

    m_lightings->direction_light.ambient = Renderer::ngolor_to_glm(n_dirL_amb);
    m_lightings->direction_light.diffuse = Renderer::ngolor_to_glm(n_dirL_dif);
    m_lightings->direction_light.specular = Renderer::ngolor_to_glm(n_dirL_spc);

    m_lightings->point_light.ambient = Renderer::ngolor_to_glm(n_posL_amb);
    m_lightings->point_light.diffuse = Renderer::ngolor_to_glm(n_posL_dif);
    m_lightings->point_light.specular = Renderer::ngolor_to_glm(n_posL_spc);

    // Rotation of Point Light
    glm::vec3 pointL_pos = m_lightings->point_light.position;
    float theta = (float)glfwGetTime();
    float sin_theta = glm::sin(theta);
    float cos_theta = glm::cos(theta);
    if (n_pLight_rotX)
    {
        float y = pointL_pos[1];
        float z = pointL_pos[2];
        float mag = glm::sqrt(y * y + z * z);
        pointL_pos[1] = mag * cos_theta;
        pointL_pos[2] = mag * sin_theta;
        m_lightings->point_light.position = pointL_pos;
    }
    else if (n_pLight_rotY)
    {
        float x = pointL_pos[1];
        float z = pointL_pos[2];
        float mag = glm::sqrt(x * x + z * z);
        pointL_pos[0] = mag * cos_theta;
        pointL_pos[2] = mag * sin_theta;
        m_lightings->point_light.position = pointL_pos;
    }
    else if (n_pLight_rotZ)
    {
        float x = pointL_pos[0];
        float y = pointL_pos[1];
        float mag = glm::sqrt(y * y + x * x);
        pointL_pos[0] = mag * cos_theta;
        pointL_pos[1] = mag * sin_theta;
        m_lightings->point_light.position = pointL_pos;
    }
    else if (n_pLight_reset)
    {
        m_lightings->point_light.position = m_lightings->get_reset_pos();
        n_pLight_reset = false; // reset the value back to sane default.
    }

    // Set in shader program
    // Uniforms for GLSL: halfway -- ab
    unsigned int modelLoc = glGetUniformLocation(shader.program, "model_mat");
    unsigned int viewLoc = glGetUniformLocation(shader.program, "view");
    unsigned int projLoc = glGetUniformLocation(shader.program, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_mat));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int objcolLoc = glGetUniformLocation(shader.program, "object_color");
    unsigned int camposLoc = glGetUniformLocation(shader.program, "view_position");
    glUniform3fv(objcolLoc, 1, glm::value_ptr(custom_color));
    glUniform3fv(camposLoc, 1, glm::value_ptr(m_camera->position));

    // Directional Light Uniforms
    unsigned int onDLightLoc = glGetUniformLocation(shader.program, "on_Dlight");
    unsigned int dLightDirLoc = glGetUniformLocation(shader.program, "dlight_dir");
    unsigned int dLightAmbLoc = glGetUniformLocation(shader.program, "dlight_amb");
    unsigned int dLightDifLoc = glGetUniformLocation(shader.program, "dlight_dif");
    unsigned int dLightSpcLoc = glGetUniformLocation(shader.program, "dlight_spc");

    glUniform1i(onDLightLoc, n_on_dirL);
    glUniform3fv(dLightDirLoc, 1, glm::value_ptr(m_lightings->direction_light.direction));
    glUniform3fv(dLightAmbLoc, 1, glm::value_ptr(m_lightings->direction_light.ambient));
    glUniform3fv(dLightDifLoc, 1, glm::value_ptr(m_lightings->direction_light.diffuse));
    glUniform3fv(dLightSpcLoc, 1, glm::value_ptr(m_lightings->direction_light.specular));

    // Positional Light Uniforms
    unsigned int onPLightLoc = glGetUniformLocation(shader.program, "on_Plight");
    unsigned int pLightPosLoc = glGetUniformLocation(shader.program, "plight_pos");
    unsigned int pLightAmbLoc = glGetUniformLocation(shader.program, "plight_amb");
    unsigned int pLightDifLoc = glGetUniformLocation(shader.program, "plight_dif");
    unsigned int pLightSpcLoc = glGetUniformLocation(shader.program, "plight_spc");

    glUniform1i(onPLightLoc, n_on_posL);
    glUniform3fv(pLightPosLoc, 1, glm::value_ptr(m_lightings->point_light.position));
    glUniform3fv(pLightAmbLoc, 1, glm::value_ptr(m_lightings->point_light.ambient));
    glUniform3fv(pLightDifLoc, 1, glm::value_ptr(m_lightings->point_light.diffuse));
    glUniform3fv(pLightSpcLoc, 1, glm::value_ptr(m_lightings->point_light.specular));

    // Textures: ourTexture corresponds to the diffuse texture.

    unsigned int ourTextureLoc = glGetUniformLocation(shader.program, "ourTexture");
    unsigned int normalMapLoc = glGetUniformLocation(shader.program, "normalMap");
    glUniform1i(ourTextureLoc, 0);
    glUniform1i(normalMapLoc, 1);

    // Textures On / Off nano gui uniforms
    unsigned int on_diffuseMapLoc = glGetUniformLocation(shader.program, "on_diffuseMap");
    unsigned int on_normalMapLoc = glGetUniformLocation(shader.program, "on_normalMap");
    glUniform1i(on_diffuseMapLoc, n_on_diffuseTex);
    glUniform1i(on_normalMapLoc, n_on_normalTex);
}

void Renderer::camera_move()
{
    GLfloat current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    // Change of camera position (x, y, z) according to NanoGui values
    glm::vec3 current_cam_pos = glm::vec3(nano_campos_x, nano_campos_y, nano_campos_z);
    m_camera->position = current_cam_pos;

    // Change camera FOV or Zoom
    // Extra part!
    if (nano_fov < 1.0f)
        nano_fov = 1.0f;
    if (nano_fov > 75.0f)
        nano_fov = 75.0f;
    m_camera->zoom = nano_fov;

    // Camera controls
    if (keys[GLFW_KEY_W])
    {
        m_camera->process_keyboard(FORWARD, delta_time);
    }
    if (keys[GLFW_KEY_S])
    {
        m_camera->process_keyboard(BACKWARD, delta_time);
    }
    if (keys[GLFW_KEY_A])
    {
        m_camera->process_keyboard(LEFT, delta_time);
    }
    if (keys[GLFW_KEY_D])
    {
        m_camera->process_keyboard(RIGHT, delta_time);
    }
    if (keys[GLFW_KEY_Q])
    {
        m_camera->process_keyboard(UP, delta_time);
    }
    if (keys[GLFW_KEY_E])
    {
        m_camera->process_keyboard(DOWN, delta_time);
    }

    // For Rotation, use the "velocity" as the nanogui rotation value
    if (keys[GLFW_KEY_I] || n_rotate_xup)
    {
        m_camera->process_keyboard(ROTATE_X_UP, nano_rotate_val);
        n_rotate_xup = false;
    }
    if (keys[GLFW_KEY_K] || n_rotate_xdown)
    {
        m_camera->process_keyboard(ROTATE_X_DOWN, nano_rotate_val);
        n_rotate_xdown = false;
    }
    if (keys[GLFW_KEY_J] || n_rotate_yup)
    {
        m_camera->process_keyboard(ROTATE_Y_UP, nano_rotate_val);
        n_rotate_yup = false;
    }
    if (keys[GLFW_KEY_L] || n_rotate_ydown)
    {
        m_camera->process_keyboard(ROTATE_Y_DOWN, nano_rotate_val);
        n_rotate_ydown = false;
    }
    if (keys[GLFW_KEY_U] || n_rotate_zup)
    {
        m_camera->process_keyboard(ROTATE_Z_UP, nano_rotate_val);
        n_rotate_zup = false;
    }
    if (keys[GLFW_KEY_O] || n_rotate_zdown)
    {
        m_camera->process_keyboard(ROTATE_Z_DOWN, nano_rotate_val);
        n_rotate_zdown = false;
    }

    // Update nanogui pos x,y,z to reflect updated postion.
    nano_campos_x = m_camera->position[0];
    nano_campos_y = m_camera->position[1];
    nano_campos_z = m_camera->position[2];
}
