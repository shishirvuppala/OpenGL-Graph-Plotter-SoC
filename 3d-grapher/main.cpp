// Defining the ESCAPE Key Code
#define ESCAPE 27
// Defining the DELETE Key Code
#define DELETE 127

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "gl_framework.hpp"
#include "shader_util.hpp"

#include "common.h"
#include "graph.h"
#include <cmath>
#include <vector>

glm::mat4 view_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelviewproject_matrix;
GLuint uModelViewProjectMatrix;

float sensitivity = 0.01f;
bool left_pressed = false;

double click_xpos = 0;
double click_ypos = 0;

float updownrot = acos(1.0f / sqrt(3.0f));
float sidewayrot = glm::radians(45.0f);
float temp_updownrot = 0;
float temp_sidewayrot = 0;
float r = 15.0f; 
float min_value = 0.15f;
float max_value = M_PI - 0.15f;

GLuint shaderProgram;
GLuint vbo;
GLuint vao;
int vertexCount = 0; 

#define BUFFER_OFFSET(offset) ((GLvoid*) (offset))

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        left_pressed = true;
        glfwGetCursorPos(window, &click_xpos, &click_ypos);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        left_pressed = false;
        updownrot = glm::clamp(updownrot + temp_updownrot, min_value, max_value);
        sidewayrot += temp_sidewayrot;
        temp_updownrot = 0;
        temp_sidewayrot = 0;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


void setup(const std::vector<float>& graphpoints) {
    csX75::initGL();

    std::string vertex_shader_file("vshader.glsl");
    std::string fragment_shader_file("fshader.glsl");
    std::vector<GLuint> shaderList;
    shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
    shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

    shaderProgram = csX75::CreateProgramGL(shaderList);
    glUseProgram(shaderProgram);

    uModelViewProjectMatrix = glGetUniformLocation(shaderProgram, "ModelViewProjectMatrix");

    vertexCount = graphpoints.size() / 3; 

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glBufferData(GL_ARRAY_BUFFER, graphpoints.size() * sizeof(float), graphpoints.data(), GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), BUFFER_OFFSET(0));
}

void renderGL(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (left_pressed) {
        double xurx, xury;
        glfwGetCursorPos(window, &xurx, &xury);
        temp_updownrot = (float)-(xury - click_ypos) * sensitivity;
        temp_sidewayrot = (float)-(xurx - click_xpos) * sensitivity;
    }

    float totalupdown = glm::clamp(updownrot + temp_updownrot, min_value, max_value);
    float x = r * sin(totalupdown) * cos(sidewayrot + temp_sidewayrot);
    float y = r * sin(totalupdown) * sin(sidewayrot + temp_sidewayrot);
    float z = r * cos(totalupdown);

    view_matrix = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    glm::mat4 projection_matrix = glm::perspective(
        glm::radians(45.0f), 
        640.0f / 480.0f,     
        0.1f,                
        100.0f               
    );
  
    modelviewproject_matrix = projection_matrix * view_matrix;

    glUniformMatrix4fv(uModelViewProjectMatrix, 1, GL_FALSE, glm::value_ptr(modelviewproject_matrix));

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

int main(int argc, char** argv) {
    glfwSetErrorCallback(csX75::error_callback);
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    GLFWwindow* window = glfwCreateWindow(1200, 900, "Marching Cubes 3D Grapher", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
  
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    double startTime = glfwGetTime();
    Graph* myGraph = new Graph(startTime);
    std::vector<float> graphpoints;

    myGraph->draw(startTime, graphpoints);

    setup(graphpoints);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

    double targetFrameTime = 1.0 / 20.0;
    double lastTime = glfwGetTime();

    while (glfwWindowShouldClose(window) == 0) {
        double currentTime = glfwGetTime();

        if (currentTime - lastTime >= targetFrameTime) {
            lastTime = currentTime;

            graphpoints.clear();

            myGraph->draw(currentTime, graphpoints);

            vertexCount = graphpoints.size() / 3;

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, graphpoints.size() * sizeof(float), graphpoints.data(), GL_DYNAMIC_DRAW);

            renderGL(window);
            glfwSwapBuffers(window);
        }
        
        glfwPollEvents();
    }
  
    glfwTerminate();
    return 0;
}