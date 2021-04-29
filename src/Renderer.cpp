#include "Renderer.h"

Utils::GuiControl *Renderer::m_gui = new Utils::GuiControl();
Camera *Renderer::m_camera = new Camera();
Lighting *Renderer::m_lightings = new Lighting();

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
    m_lightings->init();
    m_gui->init(m_window, m_camera, m_lightings);

    glewExperimental = GL_TRUE;
    glewInit();
}

void Renderer::display(GLFWwindow *window)
{
    Shader m_shader = Shader("../src/shader/light.vert", "../src/shader/light.frag");
    // Shader m_shader = Shader("../src/shader/basic.vert", "../src/shader/basic.frag");

    // Main frame while loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call
        // corresponding response functions
        glfwPollEvents();

        if (m_gui->reloadModel || m_gui->reset)
        {
            scene_reset();
            std::cout << "\n[DebugLog] Scene has been reset!\n";
            m_gui->reloadModel = false;
            m_gui->reset = false;
        }

        m_gui->camera_movement_controls();

        m_gui->update_lighting();

        m_shader.use();

        setup_uniform_values(m_shader);

        draw_scene(m_shader);

        m_gui->screen_draw_widgets();

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
    auto curr_model = m_gui->modelType;

    m_cubes.set_3d_model(Utils::load_3Dfrom_type(curr_model), Utils::get_dims(curr_model));
    m_cubes.compute_grid(m_gui->num_cuts);
    m_object.loadFaces(m_cubes.compute_faces(m_gui->view_depth));
    // m_object.loadFaces(m_cubes.compute_verts(m_gui->view_depth));
    m_cubes.clear_raw_data();

    std::cout << "[DEBUG] Object vao verts size:" << m_object.vao_vertices.size() << std::endl;
    std::cout << "[DEBUG] Cleaned Up!" << std::endl;

    glGenVertexArrays(1, &(m_object.vao)); // public member
    glGenBuffers(1, &(m_object.vbo));
    std::cout << "[DEBUG] Generated Buffers" << std::endl;
    glBindVertexArray(m_object.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_object.vbo);
    std::cout << "[DEBUG] Bound Buffers" << std::endl;
    glBufferData(GL_ARRAY_BUFFER, sizeof(Object::Vertex) * m_object.vao_vertices.size(),
                 &(m_object.vao_vertices[0]), GL_STATIC_DRAW);
    std::cout << "[DEBUG] Buffered Data" << std::endl;

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    std::cout << "[DEBUG] Bindings done" << std::endl;
}

void Renderer::draw_scene(Shader &shader)
{
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
    draw_object(shader, m_object);
}

void Renderer::draw_object(Shader &shader, Object &object)
{
    glBindVertexArray(object.vao);
    GLenum our_mode;
    if (m_gui->renderType == Utils::LINE)
    {
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
    // Reset back Polygon Mode to GL_FILL so as to not mess up the Nano GUI in Lineframe mode.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0); // unbind vao.
}

void Renderer::setup_uniform_values(Shader &shader)
{
    glm::mat4 model_mat = glm::mat4(1.0f); // Model matrix
    glm::mat4 view = glm::mat4(1.0f);      // View Matrix
    view = m_camera->GetViewMatrix();
    float aspect_ratio = (float)m_camera->width / (float)m_camera->height;
    glm::mat4 projection = glm::mat4(1.0f); // Projection Matrix
    projection = glm::perspective(glm::radians(m_camera->zoom), aspect_ratio, m_gui->z_near, m_gui->z_far);

    glm::vec3 custom_color = glm::vec3(m_gui->color[0], m_gui->color[1], m_gui->color[2]);

    shader.setMat4("model_mat", model_mat);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("object_color", custom_color);
    shader.setVec3("view_position", m_camera->position);

    // Directional Light Uniforms
    shader.setInt("on_Dlight", m_gui->on_dirL);
    shader.setVec3("dlight_dir", m_lightings->direction_light.direction);
    shader.setVec3("dlight_amb", m_lightings->direction_light.ambient);
    shader.setVec3("dlight_dif", m_lightings->direction_light.direction);
    shader.setVec3("dlight_spc", m_lightings->direction_light.specular);

    // Positional Light Uniforms
    shader.setInt("on_Plight", m_gui->on_posL);
    shader.setVec3("plight_pos", m_lightings->point_light.position);
    // shader.setVec3("plight_pos", glm::vec3(0, -1.5, 0));
    shader.setVec3("plight_amb", m_lightings->point_light.ambient);
    shader.setVec3("plight_dif", m_lightings->point_light.diffuse);
    shader.setVec3("plight_spc", m_lightings->point_light.specular);

}
