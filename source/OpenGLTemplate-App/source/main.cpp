////////////////////////////////////////////////////////////////////////////////
// Bocan Online Templates
// 
// cpp-opengl-glfw-glad-cmake
// main.cpp
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH  1280
#define WINDOW_TITLE  "Bocan Online C++ Template (OpenGL, GLFW, GLAD, GLM)"

#define SQUARE_LENGTH     100   // pixels
#define ROTATION_SPEED     90   // degrees per second
#define TRANSLATION_SPEED 200   // pixels per second
#define SCALE_SPEED         1   // percent of scale per second

#define INFOLOG_SIZE  512

enum class KeyboardInputType {

    None = 0,
    KeyLeft,        // Rotate Model CounterClockwise
    KeyRight,       // Rotate Model Clockwise
    KeyUp,          // Translate Model "Forward" (+y)
    KeyDown,        // Translate Model "Rearward" (-y)
    KeyLessThan,    // Scale Model Up
    KeyGreaterThan, // Scale Model Down
    KeyW,           // Translate Camera Up
    KeyS,           // Translate Camera Down
    KeyA,           // Translate Camera Left
    KeyD,           // Translate Camera Right
    KeyR,           // Color Model Red
    KeyG,           // Color Model Green
    KeyB,           // Color Model Blue
    KeySpace,       // Color Model White
    Key1,           // Change to Square Model
    Key2,           // Change to Circle Model
    Key3,           // Change to Triangle Model
};
unsigned int shader_program{};

glm::vec4 color_vec = glm::vec4(1.0f);

glm::mat4 mvp_matrix = glm::mat4(1.0f);
glm::mat4 model_matrix = glm::mat4(1.0f);
glm::mat4 view_matrix = glm::mat4(1.0f);
glm::mat4 projection_matrix = glm::mat4(1.0f);

void OnKeyboardInput(GLFWwindow* window, float delta_time);
void OnWindowResize(GLFWwindow* window, int width, int height);
void OnRender(GLFWwindow* window);

void RotateSquare(KeyboardInputType, float);
void TranslateSquare(KeyboardInputType, float);
void ScaleSquare(KeyboardInputType, float);
void ColorSquare(KeyboardInputType);

constexpr auto square_vertices = std::array {
    
    -SQUARE_LENGTH/2.0f,  SQUARE_LENGTH/2.0f,  0.0f, // top-left
     SQUARE_LENGTH/2.0f,  SQUARE_LENGTH/2.0f,  0.0f, // top-right
     SQUARE_LENGTH/2.0f,  SQUARE_LENGTH/2.0f,  0.0f, // top-right
     SQUARE_LENGTH/2.0f, -SQUARE_LENGTH/2.0f,  0.0f, // bottom-right
     SQUARE_LENGTH/2.0f, -SQUARE_LENGTH/2.0f,  0.0f, // bottom-right
    -SQUARE_LENGTH/2.0f, -SQUARE_LENGTH/2.0f,  0.0f, // bottom-left
    -SQUARE_LENGTH/2.0f, -SQUARE_LENGTH/2.0f,  0.0f, // bottom-left
    -SQUARE_LENGTH/2.0f,  SQUARE_LENGTH/2.0f,  0.0f, // top-left
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader Source Code
////////////////////////////////////////////////////////////////////////////////
constexpr auto vertex_shader_source = R"(
    
    #version 330 core

    layout (location = 0) in vec4 a_Position;
    uniform mat4 u_MVP_matrix;

    void main() {

        gl_Position = u_MVP_matrix * a_Position;
    }
)";

////////////////////////////////////////////////////////////////////////////////
// Fragment Shader Source Code
////////////////////////////////////////////////////////////////////////////////
constexpr auto fragment_shader_source = R"(

    #version 330 core

    out vec4 FragColor;

    uniform vec4 u_Color_vec;

    void main() {

        FragColor = u_Color_vec;
    }
)";

int main(int argc, char** argv) {

////////////////////////////////////////////////////////////////////////////////
// Initialize Graphical User Interface Window Using GLFW
////////////////////////////////////////////////////////////////////////////////
    if (!glfwInit()) {

        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                                          WINDOW_TITLE, NULL, NULL);

    if (!window) {
        
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
    
////////////////////////////////////////////////////////////////////////////////
// Initialize and Load OpenGL Functions with GLAD
////////////////////////////////////////////////////////////////////////////////
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
////////////////////////////////////////////////////////////////////////////////
// Compile and Load Vertex Shader and Fragment Shader with OpenGL
////////////////////////////////////////////////////////////////////////////////
    int success = 0;
    char info_log[INFOLOG_SIZE];
   
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(vertex_shader, INFOLOG_SIZE, nullptr, info_log);
        std::cout << "Vertex Shader Compilation Failed: " << info_log << std::endl;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(fragment_shader, INFOLOG_SIZE, nullptr, info_log);
        std::cout << "Fragment Shader Compliation Failed: " << info_log << std::endl;
    }

////////////////////////////////////////////////////////////////////////////////
// Create and Link Shader Program with OpenGL
////////////////////////////////////////////////////////////////////////////////
    shader_program = glCreateProgram();
    
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glValidateProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if (!success) {

        glGetProgramInfoLog(shader_program, INFOLOG_SIZE, nullptr, info_log);
        std::cout << "Shader Program Linking Failed: " << info_log << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

////////////////////////////////////////////////////////////////////////////////
// Initialize Vertex Buffer and Vertex Array with OpenGL
////////////////////////////////////////////////////////////////////////////////

    unsigned int VBO{}; // vertex buffer object
    unsigned int VAO{}; // vertex array object

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 square_vertices.size() * sizeof(float),
                 square_vertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

////////////////////////////////////////////////////////////////////////////////
// Setup for Main Loop
////////////////////////////////////////////////////////////////////////////////
    glfwSetFramebufferSizeCallback(window, OnWindowResize);
    
    int fb_width{};                 // framebuffer width
    int fb_height{};                // framebuffer height
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    std::cout << "GLFW Window Ready:\t" << fb_width << "\t" << fb_height << std::endl;

    glViewport(0, 0, fb_width, fb_height);
    
    float last_frame_start_time = 0.0f;

    model_matrix = glm::translate(model_matrix, glm::vec3(fb_width/2.0f, 
                                                          fb_height/2.0f, 
                                                          0.0f));

    projection_matrix = glm::ortho(0.0f, static_cast<float>(fb_width),
                                   0.0f, static_cast<float>(fb_height), 
                                  -1.0f, 1.0f);

////////////////////////////////////////////////////////////////////////////////
// Main Loop
////////////////////////////////////////////////////////////////////////////////
    while (!glfwWindowShouldClose(window)) {

        float current_frame_start_time = static_cast<float>(glfwGetTime());
        float delta_time = current_frame_start_time - last_frame_start_time;
        last_frame_start_time = current_frame_start_time;

        OnKeyboardInput(window, delta_time);
        OnRender(window);
        glfwPollEvents();
    }

////////////////////////////////////////////////////////////////////////////////
// Delete Objects and Programs, Close Window, Exit Program
////////////////////////////////////////////////////////////////////////////////
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    glfwTerminate(); 
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Event Called by glfwSetFramebufferSizeCallback; Handles Window Resize
////////////////////////////////////////////////////////////////////////////////
void OnWindowResize(GLFWwindow* window, int width, int height) {

    std::cout << "GLFW Window Resize:\t" << width << "\t" << height << std::endl;

    projection_matrix = glm::ortho(0.0f, static_cast<float>(width), 
                                   0.0f, static_cast<float>(height), 
                                  -1.0f, 1.0f);

    glViewport(0, 0, width, height);

    OnRender(window);
}

////////////////////////////////////////////////////////////////////////////////
// Event Called by glfwPollEvents; Dispatches Key Press to Appropriate Function
////////////////////////////////////////////////////////////////////////////////
void OnKeyboardInput(GLFWwindow* window, float delta_time) {

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "UP" << std::endl;
        ScaleSquare(KeyboardInputType::KeyUp, delta_time); 
    }

    if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "DOWN" << std::endl;
        ScaleSquare(KeyboardInputType::KeyDown, delta_time); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "LEFT" << std::endl;
        RotateSquare(KeyboardInputType::KeyLeft, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "RIGHT" << std::endl;
        RotateSquare(KeyboardInputType::KeyRight, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "W" << std::endl;
        TranslateSquare(KeyboardInputType::KeyW, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "S" << std::endl;
        TranslateSquare(KeyboardInputType::KeyS, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "A" << std::endl;
        TranslateSquare(KeyboardInputType::KeyA, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "D" << std::endl;
        TranslateSquare(KeyboardInputType::KeyD, delta_time); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "R" << std::endl;
        ColorSquare(KeyboardInputType::KeyR); 
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "G" << std::endl;
        ColorSquare(KeyboardInputType::KeyG); 
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "B" << std::endl;
        ColorSquare(KeyboardInputType::KeyB); 
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "SPACE" << std::endl;
        ColorSquare(KeyboardInputType::KeySpace); 
    }
}

////////////////////////////////////////////////////////////////////////////////
// Event Called on Main Loop
////////////////////////////////////////////////////////////////////////////////
void OnRender(GLFWwindow* window) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mvp_matrix = projection_matrix * view_matrix * model_matrix;

    glUseProgram(shader_program);

    auto position_loc = glGetUniformLocation(shader_program, "u_MVP_matrix");
    glUniformMatrix4fv(position_loc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

    auto color_loc = glGetUniformLocation(shader_program, "u_Color_vec");
    glUniform4f(color_loc, color_vec[0], color_vec[1], color_vec[2], color_vec[3]);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(square_vertices.size() / 3));

    glfwSwapBuffers(window);
}

void RotateSquare(KeyboardInputType key, float delta_time) {

    float rotation_angle_per_frame = ROTATION_SPEED * delta_time;

    switch(key) {
        case(KeyboardInputType::KeyLeft):
            break;
        case(KeyboardInputType::KeyRight):
            rotation_angle_per_frame *= -1;
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }

    model_matrix = glm::rotate(model_matrix,
                               glm::radians(rotation_angle_per_frame),
                               glm::vec3(0.0f, 0.0f, 1.0f));
}

void TranslateSquare(KeyboardInputType key, float delta_time) {
   
    float translation_units_per_frame = TRANSLATION_SPEED * delta_time;

    glm::vec3 translation_matrix = glm::vec3(0.0f);

    switch(key) {
        case(KeyboardInputType::KeyW):
            translation_matrix.y = translation_units_per_frame;
            break;
        case(KeyboardInputType::KeyS):
            translation_matrix.y = -translation_units_per_frame;
            break;
        case(KeyboardInputType::KeyA):
            translation_matrix.x = -translation_units_per_frame;
            break;
        case(KeyboardInputType::KeyD):
            translation_matrix.x = translation_units_per_frame;
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }
    
    model_matrix = glm::translate(model_matrix, translation_matrix);
}

void ScaleSquare(KeyboardInputType key, float delta_time) {
    
    float scale_units_per_frame = SCALE_SPEED * delta_time;

    glm::vec3 scale_matrix = glm::vec3(0.0f);

    switch(key) {
        case(KeyboardInputType::KeyUp):
            scale_matrix = glm::vec3(1.0f + scale_units_per_frame); 
            break;
        case(KeyboardInputType::KeyDown):
            scale_matrix = glm::vec3(1.0f - scale_units_per_frame);
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }

    model_matrix = glm::scale(model_matrix, scale_matrix);
}

void ColorSquare(KeyboardInputType key) {

    switch(key) {
        case(KeyboardInputType::KeyR):
            color_vec = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        case(KeyboardInputType::KeyG):
            color_vec = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            break;
        case(KeyboardInputType::KeyB):
            color_vec = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
            break;
        case(KeyboardInputType::KeySpace):
            color_vec = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }
}
