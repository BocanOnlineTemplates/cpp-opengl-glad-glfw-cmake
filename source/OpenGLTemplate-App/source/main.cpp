////////////////////////////////////////////////////////////////////////////////
// organization: Bocan Online Templates
// author: Matthew Buchanan
// 
// license: The Unlicense
// project: cpp-opengl-glfw-glad-cmake
// file: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "glm/ext/scalar_constants.hpp"
#include <cmath>

#include <iostream>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SCREEN_HEIGHT       1080
#define SCREEN_WIDTH        1920
#define WINDOW_TITLE        "Bocan Online C++ OpenGL 2D Demo"

#define MODEL_LENGTH        100   // pixels
#define CIRCLE_SEGMENTS     150   // number of points (must be divisible by 3)
#define ROTATION_SPEED       90   // degrees per second
#define TRANSLATION_SPEED   300   // pixels per second
#define SCALE_SPEED           1   // percent of scale per second

#define INFOLOG_SIZE        512

struct Settings { };

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
    KeyO,           // Translate Camera to Origin
    KeyH,           // Translate Model to Origin
    KeyQ,           // Rotate Camera CounterClockwise (+z)
    KeyE,           // Rotate Camera Clockwise (-z)
    KeyZ,           // Zoom Camera In (Scale Up)
    KeyX,           // Zoom Camera Out (Scale Down
    KeyR,           // Color Model Red
    KeyG,           // Color Model Green
    KeyB,           // Color Model Blue
    KeySpace,       // Color Model White
    Key1,           // Swap to Square Model
    Key2,           // Swap to Triangle Model
    Key3,           // Swap to Hexagon Model
    Key4,           // Swap to Circle Model
};

enum class UserModel {

    None = 0,
    Square,
    Triangle,
    Hexagon,
    Circle,
};

////////////////////////////////////////////////////////////////////////////////
// Entity Initialization
// Scene Entities: 
// --Environment Object Coordinate Grid (Red X-Axis and Green Y-Axis)
// --Environment Object (Orange Square)
// --User/Player Object (Various Colors and Various Models)
////////////////////////////////////////////////////////////////////////////////
UserModel active_usr_model = UserModel::Square;

unsigned int shader_program{};

unsigned int vao{};             // vertex array object

unsigned int vbo_x_axis{};      // vertex buffer object x-axis
unsigned int vbo_y_axis{};      // vertex buffer object y-axis
unsigned int vbo_square{};      // vertex buffer object square model
unsigned int vbo_triangle{};    // vertex buffer object triangle model
unsigned int vbo_hexagon{};     // vertex buffer object hexagon model
unsigned int vbo_circle{};      // vertex buffer object circle model

glm::vec4 usr_color_vec    = glm::vec4(1.0f);
glm::vec4 x_axis_color_vec = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 y_axis_color_vec = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
glm::vec4 env_color_vec    = glm::vec4(1.0f, 0.65f, 0.0f, 1.0f);

glm::mat4 mvp_mat = glm::mat4(1.0f);

glm::mat4 xyz_model_mat = glm::mat4(1.0f);  // model matrix for grid objects
glm::mat4 env_model_mat = glm::mat4(1.0f);  // model matrix for env object
glm::mat4 usr_model_mat = glm::mat4(1.0f);  // model matrix for user object

glm::mat4 view_mat = glm::mat4(1.0f);       // view matrix for single camera
glm::mat4 proj_mat = glm::mat4(1.0f);       // orthographic projection matrix

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
void OnKeyboardInput(GLFWwindow* window, float delta_time);
void OnWindowResize(GLFWwindow* window, int width, int height);
void OnRender(GLFWwindow* window);

void GenerateCircleVertices();

void SetupShader();
void SetupVertexArray();
void SetupVertexBuffer();
void Draw(unsigned int buffer, glm::mat4 model_mat, glm::vec4 color, int vertices_size);

void ResetCamera();
void RotateCamera(KeyboardInputType, float, GLFWwindow*);
void TranslateCamera(KeyboardInputType, float);
void ZoomCamera(KeyboardInputType, float);

void ResetModel(GLFWwindow*);
void RotateModel(KeyboardInputType, float);
void TranslateModel(KeyboardInputType, float);
void ScaleModel(KeyboardInputType, float);
void ColorModel(KeyboardInputType);
void SwapModel(KeyboardInputType);

////////////////////////////////////////////////////////////////////////////////
// Models Source Code (Coordinate Axis, Square, Triangle, Hexagon)
////////////////////////////////////////////////////////////////////////////////

std::array<float, 6> x_axis_vertices {

     -MODEL_LENGTH*10.0f,  0.0f,  0.0f, // left point
      MODEL_LENGTH*10.0f,  0.0f,  0.0f, // right point
};

std::array<float, 6> y_axis_vertices {

     0.0f,  MODEL_LENGTH*7.0f,  0.0f, // top point
     0.0f, -MODEL_LENGTH*7.0f,  0.0f, // bottom point
};

std::array<float, 24> square_vertices {
    
    -MODEL_LENGTH/2.0f,  MODEL_LENGTH/2.0f,  0.0f, // top-left
     MODEL_LENGTH/2.0f,  MODEL_LENGTH/2.0f,  0.0f, // top-right
     MODEL_LENGTH/2.0f,  MODEL_LENGTH/2.0f,  0.0f, // top-right
     MODEL_LENGTH/2.0f, -MODEL_LENGTH/2.0f,  0.0f, // bottom-right
     MODEL_LENGTH/2.0f, -MODEL_LENGTH/2.0f,  0.0f, // bottom-right
    -MODEL_LENGTH/2.0f, -MODEL_LENGTH/2.0f,  0.0f, // bottom-left
    -MODEL_LENGTH/2.0f, -MODEL_LENGTH/2.0f,  0.0f, // bottom-left
    -MODEL_LENGTH/2.0f,  MODEL_LENGTH/2.0f,  0.0f, // top-left
};

std::array<float, 18> triangle_vertices {

    0.0f,               (-MODEL_LENGTH/2.0f * std::cosf(60)), 0.0f, // top
   -MODEL_LENGTH/2.0f,  ( MODEL_LENGTH/2.0f * std::cosf(60)), 0.0f, // right
   -MODEL_LENGTH/2.0f,  ( MODEL_LENGTH/2.0f * std::cosf(60)), 0.0f, // right
    MODEL_LENGTH/2.0f,  ( MODEL_LENGTH/2.0f * std::cosf(60)), 0.0f, // left
    MODEL_LENGTH/2.0f,  ( MODEL_LENGTH/2.0f * std::cosf(60)), 0.0f, // left
    0.0f,               (-MODEL_LENGTH/2.0f * std::cosf(60)), 0.0f, // top
};

std::array<float, 36> hexagon_vertices {
   
    -MODEL_LENGTH/4.0f, ( MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // top-left
     MODEL_LENGTH/4.0f, ( MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // top-right
     MODEL_LENGTH/4.0f, ( MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // top-right
     MODEL_LENGTH/2.0f, 0.0f,                                                         0.0f, // right
     MODEL_LENGTH/2.0f, 0.0f,                                                         0.0f, // right
     MODEL_LENGTH/4.0f, (-MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // bottom-right
     MODEL_LENGTH/4.0f, (-MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // bottom-right
    -MODEL_LENGTH/4.0f, (-MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // bottom-left
    -MODEL_LENGTH/4.0f, (-MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // bottom-left
    -MODEL_LENGTH/2.0f, 0.0f,                                                         0.0f, // left
    -MODEL_LENGTH/2.0f, 0.0f,                                                         0.0f, // left
    -MODEL_LENGTH/4.0f, ( MODEL_LENGTH/2.0f * static_cast<float>(std::sqrt(3))/2.0f), 0.0f, // top-left
};

std::array<float, CIRCLE_SEGMENTS * 3> circle_vertices {};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader Source Code
////////////////////////////////////////////////////////////////////////////////
constexpr auto vertex_shader_source = R"(
    
    #version 330 core

    layout (location = 0) in vec4 a_Position;
    uniform mat4 u_MVP_mat;

    void main() {

        gl_Position = u_MVP_mat * a_Position;
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
    
    float x_scale;
    float y_scale;
    
    glfwGetWindowContentScale(window, &x_scale, &y_scale);
    glfwSetWindowSize(window, SCREEN_WIDTH/x_scale, SCREEN_HEIGHT/y_scale);
    glfwSetWindowSizeLimits(window,  640/x_scale,  360/y_scale, 
                                    3024/x_scale, 1964/y_scale);

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
        std::cout << "Fragment Shader Compilation Failed: " << info_log << std::endl;
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

    glGenVertexArrays(1, &vao); 
    glBindVertexArray(vao);

    GenerateCircleVertices();
    
    glGenBuffers(1, &vbo_x_axis);
    glGenBuffers(1, &vbo_y_axis);
    glGenBuffers(1, &vbo_square);
    glGenBuffers(1, &vbo_triangle);
    glGenBuffers(1, &vbo_hexagon);
    glGenBuffers(1, &vbo_circle);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_x_axis);
    glBufferData(GL_ARRAY_BUFFER, x_axis_vertices.size() * sizeof(float), 
                 x_axis_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
  

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_y_axis);
    glBufferData(GL_ARRAY_BUFFER, y_axis_vertices.size() * sizeof(float), 
                 y_axis_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_square);
    glBufferData(GL_ARRAY_BUFFER, square_vertices.size() * sizeof(float),
                 square_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
    glBufferData(GL_ARRAY_BUFFER, triangle_vertices.size() * sizeof(float),
                 triangle_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_hexagon);
    glBufferData(GL_ARRAY_BUFFER, hexagon_vertices.size() * sizeof(float),
                 hexagon_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_circle);
    glBufferData(GL_ARRAY_BUFFER, circle_vertices.size() * sizeof(float),
                 circle_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

////////////////////////////////////////////////////////////////////////////////
// Set Scene Initial Conditions
////////////////////////////////////////////////////////////////////////////////
    glfwSetFramebufferSizeCallback(window, OnWindowResize);
  
    int fb_width{};                 // framebuffer width
    int fb_height{};                // framebuffer height
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    std::cout << "GLFW Window Ready:\t" << fb_width << "\t" << fb_height << std::endl;

    glViewport(0, 0, fb_width, fb_height);
   
    float last_frame_start_time = 0.0f;
 
    env_model_mat = glm::translate(env_model_mat, glm::vec3(200.0, 200.0, 0.0f));
    
    proj_mat = glm::ortho(-fb_width/2.0f,   fb_width/2.0f,
                          -fb_height/2.0f,  fb_height/2.0f, 
                          -1.0f,            1.0f);

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
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo_x_axis);
    glDeleteBuffers(1, &vbo_y_axis);
    glDeleteBuffers(1, &vbo_square);
    glDeleteBuffers(1, &vbo_triangle);
    glDeleteBuffers(1, &vbo_hexagon);
    glDeleteProgram(shader_program);

    glfwTerminate(); 
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Event Called by glfwSetFramebufferSizeCallback; Handles Window Resize
////////////////////////////////////////////////////////////////////////////////
void OnWindowResize(GLFWwindow* window, int width, int height) {

    std::cout << "GLFW Window Resize:\t" << width << "\t" << height << std::endl;
    
    proj_mat = glm::ortho(-width/2.0f,   width/2.0f,
                          -height/2.0f,  height/2.0f, 
                          -1.0f,         1.0f);

    glViewport(0, 0, width, height);

    OnRender(window);
}

////////////////////////////////////////////////////////////////////////////////
// Event Called by glfwPollEvents; Dispatches Key Press to Appropriate Function
////////////////////////////////////////////////////////////////////////////////
void OnKeyboardInput(GLFWwindow* window, float delta_time) {

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "UP" << std::endl;
        TranslateModel(KeyboardInputType::KeyUp, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "DOWN" << std::endl;
        TranslateModel(KeyboardInputType::KeyDown, delta_time); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "LEFT" << std::endl;
        RotateModel(KeyboardInputType::KeyLeft, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "RIGHT" << std::endl;
        RotateModel(KeyboardInputType::KeyRight, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "LESS THAN" << std::endl;
        ScaleModel(KeyboardInputType::KeyLessThan, delta_time); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "GREATER THAN" << std::endl;
        ScaleModel(KeyboardInputType::KeyGreaterThan, delta_time); 
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "W" << std::endl;
        TranslateCamera(KeyboardInputType::KeyW, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "S" << std::endl;
        TranslateCamera(KeyboardInputType::KeyS, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "A" << std::endl;
        TranslateCamera(KeyboardInputType::KeyA, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "D" << std::endl;
        TranslateCamera(KeyboardInputType::KeyD, delta_time);
    }
   
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "O" << std::endl;
        ResetCamera();
    }
   
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "H" << std::endl;
        ResetModel(window);
    }
   
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "Q" << std::endl;
        RotateCamera(KeyboardInputType::KeyQ, delta_time, window);
    }
    
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "Z" << std::endl;
        ZoomCamera(KeyboardInputType::KeyZ, delta_time);
    }
    
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "X" << std::endl;
        ZoomCamera(KeyboardInputType::KeyX, delta_time);
    }
    
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "E" << std::endl;
        RotateCamera(KeyboardInputType::KeyE, delta_time, window);
    }
    
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "R" << std::endl;
        ColorModel(KeyboardInputType::KeyR); 
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { 

        std::cout << "GLFW Keyboard Input:\t" << "G" << std::endl;
        ColorModel(KeyboardInputType::KeyG); 
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "B" << std::endl;
        ColorModel(KeyboardInputType::KeyB); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "SPACE" << std::endl;
        ColorModel(KeyboardInputType::KeySpace); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "1" << std::endl;
        SwapModel(KeyboardInputType::Key1); 
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "2" << std::endl;
        SwapModel(KeyboardInputType::Key2); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "3" << std::endl;
        SwapModel(KeyboardInputType::Key3); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { 
        
        std::cout << "GLFW Keyboard Input:\t" << "4" << std::endl;
        SwapModel(KeyboardInputType::Key4); 
    }
}

////////////////////////////////////////////////////////////////////////////////
// Event Called on Main Loop
////////////////////////////////////////////////////////////////////////////////
void OnRender(GLFWwindow* window) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader_program);
   
    Draw(vbo_x_axis, xyz_model_mat, x_axis_color_vec, x_axis_vertices.size());
    Draw(vbo_y_axis, xyz_model_mat, y_axis_color_vec, y_axis_vertices.size());
    Draw(vbo_square, env_model_mat, env_color_vec, square_vertices.size());
    
    switch(active_usr_model) {
        case UserModel::Square:
            Draw(vbo_square, usr_model_mat, usr_color_vec, square_vertices.size());
            break;
        case UserModel::Triangle:
            Draw(vbo_triangle, usr_model_mat, usr_color_vec, triangle_vertices.size());
            break;
        case UserModel::Hexagon:
            Draw(vbo_hexagon, usr_model_mat, usr_color_vec, hexagon_vertices.size());
            break;
        case UserModel::Circle:
            Draw(vbo_circle, usr_model_mat, usr_color_vec, circle_vertices.size());
            break;
        default:
            std::cerr << "Invalid model selection." << std::endl;
            return;
    }
    
    glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void GenerateCircleVertices() {

    circle_vertices[ 0 ] = MODEL_LENGTH/2.0f * std::cosf(2.0f * glm::pi<float>() * 0 / CIRCLE_SEGMENTS);
    circle_vertices[ 1 ] = MODEL_LENGTH/2.0f * std::sinf(2.0f * glm::pi<float>() * 0 / CIRCLE_SEGMENTS);
    circle_vertices[ 2 ] = 0.0f;
        

    for(int i = 3; i < CIRCLE_SEGMENTS - 3; i+=6) {
        
        circle_vertices[ i ] = MODEL_LENGTH/2.0f * std::cosf(2.0f * glm::pi<float>() * i / CIRCLE_SEGMENTS);

        if(circle_vertices[ i ] < 0.1f && circle_vertices[ i ] > -0.1f) {

            circle_vertices[ i ] = 0.0f;
        }

        circle_vertices[i+1] = MODEL_LENGTH/2.0f * std::sinf(2.0f * glm::pi<float>() * i / CIRCLE_SEGMENTS);
        
        if(circle_vertices[i+1] < 0.1f && circle_vertices[i+1] > -0.1f) {

            circle_vertices[i+1] = 0.0f;
        }

        circle_vertices[i+2] = 0.0f;
        circle_vertices[i+3] = circle_vertices[ i ];
        circle_vertices[i+4] = circle_vertices[i+1];
        circle_vertices[i+5] = circle_vertices[i+2];

    }
    circle_vertices[CIRCLE_SEGMENTS-3] = MODEL_LENGTH/2.0f * std::cosf(2.0f * glm::pi<float>() * 0 / CIRCLE_SEGMENTS);
    circle_vertices[CIRCLE_SEGMENTS-2] = MODEL_LENGTH/2.0f * std::sinf(2.0f * glm::pi<float>() * 0 / CIRCLE_SEGMENTS);
    circle_vertices[CIRCLE_SEGMENTS-1] = 0.0f;
    

    for(int i = 0; i < CIRCLE_SEGMENTS; i+=3) {

        std::cout << i << "\t" << circle_vertices[i] << "\t\t" << circle_vertices[i+1] << "\t\t" << circle_vertices[i+2] << std::endl;
    }
}

void Draw(unsigned int buffer, glm::mat4 model_mat, glm::vec4 color, int vertices_size) {

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    
    mvp_mat = proj_mat * view_mat * model_mat;

    auto position_loc = glGetUniformLocation(shader_program, "u_MVP_mat");
    glUniformMatrix4fv(position_loc, 1, GL_FALSE, glm::value_ptr(mvp_mat));

    auto color_loc = glGetUniformLocation(shader_program, "u_Color_vec");
    glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices_size / 3));
}

void ResetCamera() {

    view_mat = glm::mat4(1.0f);
}

void RotateCamera(KeyboardInputType key, float delta_time, GLFWwindow* window) {

    float rotation_angle_per_frame = ROTATION_SPEED * delta_time;

    switch(key) {
        case(KeyboardInputType::KeyQ):
            rotation_angle_per_frame *= -1;
            break;
        case(KeyboardInputType::KeyE):
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }
   
    // glm::rotate(M, a, axis) returns mat_A * R(a), correct order is R(a) * mat_A
    view_mat = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_angle_per_frame),
                           glm::vec3(0.0f, 0.0f, 1.0f)) * view_mat;


}

void TranslateCamera(KeyboardInputType key, float delta_time) {

    float translation_units_per_frame = TRANSLATION_SPEED * delta_time;

    glm::vec3 translation_vec = glm::vec3(0.0f);

    switch(key) {
        case(KeyboardInputType::KeyW):
            translation_vec.y = -translation_units_per_frame;
            break;
        case(KeyboardInputType::KeyS):
            translation_vec.y = translation_units_per_frame;
            break;
        case(KeyboardInputType::KeyA):
            translation_vec.x = translation_units_per_frame;
            break;
        case(KeyboardInputType::KeyD):
            translation_vec.x = -translation_units_per_frame;
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }
    
    view_mat = glm::translate(glm::mat4(1.0f), translation_vec) * view_mat;

}
void ZoomCamera(KeyboardInputType key, float delta_time) {

    float scale_units_per_frame = SCALE_SPEED * delta_time;

    glm::vec3 scale_vec = glm::vec3(0.0f);

    switch(key) {
        case(KeyboardInputType::KeyZ):
            scale_vec = glm::vec3(1.0f + scale_units_per_frame); 
            break;
        case(KeyboardInputType::KeyX):
            scale_vec = glm::vec3(1.0f - scale_units_per_frame);
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }

    view_mat = glm::scale(glm::mat4(1.0f), scale_vec) * view_mat;
}

void ResetModel(GLFWwindow* window) {

    int fb_width{};                 // framebuffer width
    int fb_height{};                // framebuffer height
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    usr_model_mat = glm::mat4(1.0f);
    usr_color_vec = glm::vec4(1.0f);
}

void RotateModel(KeyboardInputType key, float delta_time) {

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

    usr_model_mat = glm::rotate(usr_model_mat,
                               glm::radians(rotation_angle_per_frame),
                               glm::vec3(0.0f, 0.0f, 1.0f));
}

void TranslateModel(KeyboardInputType key, float delta_time) {
   
    float translation_units_per_frame = TRANSLATION_SPEED * delta_time;

    glm::vec3 translation_matrix = glm::vec3(0.0f);

    switch(key) {
        case(KeyboardInputType::KeyUp):
            translation_matrix.y = translation_units_per_frame;
            break;
        case(KeyboardInputType::KeyDown):
            translation_matrix.y = -translation_units_per_frame;
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }
    
    usr_model_mat = glm::translate(usr_model_mat, translation_matrix);
}

void ScaleModel(KeyboardInputType key, float delta_time) {
    
    float scale_units_per_frame = SCALE_SPEED * delta_time;

    glm::vec3 scale_matrix = glm::vec3(0.0f);

    switch(key) {
        case(KeyboardInputType::KeyGreaterThan):
            scale_matrix = glm::vec3(1.0f + scale_units_per_frame); 
            break;
        case(KeyboardInputType::KeyLessThan):
            scale_matrix = glm::vec3(1.0f - scale_units_per_frame);
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }

    usr_model_mat = glm::scale(usr_model_mat, scale_matrix);
}

void ColorModel(KeyboardInputType key) {

    switch(key) {
        case(KeyboardInputType::KeyR):
            usr_color_vec = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        case(KeyboardInputType::KeyG):
            usr_color_vec = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            break;
        case(KeyboardInputType::KeyB):
            usr_color_vec = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
            break;
        case(KeyboardInputType::KeySpace):
            usr_color_vec = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }
}

void SwapModel(KeyboardInputType key) {

    switch(key) {
        case(KeyboardInputType::Key1):
            active_usr_model = UserModel::Square;
            break;
        case(KeyboardInputType::Key2):
            active_usr_model = UserModel::Triangle;
            break;
        case(KeyboardInputType::Key3):
            active_usr_model = UserModel::Hexagon;
            break;
        case(KeyboardInputType::Key4):
            active_usr_model = UserModel::Circle;
            break;
        default:
            std::cerr << "Invalid keyboard input." << std::endl;
            return;
    }
}
