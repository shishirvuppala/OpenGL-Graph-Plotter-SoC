#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "rubik.h"
#include <cmath>

struct Button {
    float x, y, w, h;
    std::string label;
};

Button resetBtn = {5, 5, 150, 40, "Scramble"};


glm::mat4 view_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelviewproject_matrix;
GLuint uModelViewProjectMatrix;

GLuint shaderProgram;
GLuint vbo;
GLuint vao;

glm::vec4 v_cube[36];
glm::vec4 v_cubecolors[36];

glm::vec4 positions[8] = {
  glm::vec4(0.5, 0.5, 0.5, 1.0),
  glm::vec4(0.5, -0.5, 0.5, 1.0),
  glm::vec4(-0.5, 0.5, 0.5, 1.0),
  glm::vec4(-0.5, -0.5, 0.5, 1.0),
  glm::vec4(0.5, 0.5, -0.5, 1.0),
  glm::vec4(0.5, -0.5, -0.5, 1.0),
  glm::vec4(-0.5, 0.5, -0.5, 1.0),
  glm::vec4(-0.5, -0.5, -0.5, 1.0)
};

glm::vec4 cube[36];
int triidx = 0;


void fillup(int idx1,int idx2,int idx3,int idx4,glm::vec4 color){
  v_cube[triidx] = positions[idx1] ; v_cubecolors[triidx] = color; triidx++;
  v_cube[triidx] = positions[idx2] ; v_cubecolors[triidx] = color; triidx++;
  v_cube[triidx] = positions[idx4] ; v_cubecolors[triidx] = color; triidx++;


  v_cube[triidx] = positions[idx3] ; v_cubecolors[triidx] = color; triidx++;
  v_cube[triidx] = positions[idx2] ; v_cubecolors[triidx] = color; triidx++;
  v_cube[triidx] = positions[idx4] ; v_cubecolors[triidx] = color; triidx++;
}


void setupcube(){
  fillup(0,1,5,4,glm::vec4(1.0, 0.0, 0.0, 1.0)); // RED

  fillup(0,2,3,1,glm::vec4(0.0, 0.0, 1.0, 1.0)); // BLUE

  fillup(1,3,7,5,glm::vec4(1.0, 1.0, 0.0, 1.0)); // YELLOW

  fillup(2,3,7,6,glm::vec4(1.0, 0.5, 0.0, 1.0)); // ORANGE

  fillup(4,6,7,5,glm::vec4(0.0, 1.0, 0.0, 1.0)); // GREEN

  fillup(0,2,6,4,glm::vec4(1.0, 1.0, 1.0, 1.0)); // WHITE
}


glm::mat4 model_matrices[27];
glm::mat4 temp_rotations[27];

std::vector<std::tuple<int,int,int>> cube_positions(27);
enum FACE{
  FRONT,
  BACK,
  UP,
  DOWN,
  RIGHT,
  LEFT,
};

std::vector<std::vector<bool>> face_indices(6,std::vector<bool>(27,false));


void setupcubemodelmatrices(){
  float spacing = 1.05f;
  for(int i= 0;i<27;i++){
    int ycoord = 1-(i%3);
    int zcoord = 1-((i/3)%3);
    int xcoord = 1-((i/9)%3);
    model_matrices[i] = glm::translate(glm::mat4(1.0f),glm::vec3(spacing*xcoord,spacing*ycoord,spacing*zcoord));
    temp_rotations[i] = glm::mat4(1.0f);

    cube_positions[i] = {xcoord+1,ycoord+1,zcoord+1};

    if(xcoord==1) face_indices[FRONT][i] = true;
    if(xcoord==-1) face_indices[BACK][i] = true;
    if(ycoord==1) face_indices[RIGHT][i] = true;
    if(ycoord==-1) face_indices[LEFT][i] = true;
    if(zcoord==1) face_indices[UP][i] = true;
    if(zcoord==-1) face_indices[DOWN][i] = true;

  }
}

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

void setup(){

  setupcube();

  //initialise GL
  csX75::initGL();

  //shader program creation
  std::string vertex_shader_file("vshader.glsl");
  std::string fragment_shader_file("fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram( shaderProgram );

  uModelViewProjectMatrix =
    glGetUniformLocation(
        shaderProgram,
        "ModelViewProjectMatrix"
    );

  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (1, &vbo);
  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  //Copy the points into the current buffer - 9 float values, start pointer and static data
  glBufferData (GL_ARRAY_BUFFER, sizeof (v_cube) + sizeof(v_cubecolors), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(v_cube), v_cube );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(v_cube), sizeof(v_cubecolors), v_cubecolors );

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (1, &vao);
  //Set it as the current array to be used by binding it
  glBindVertexArray (vao);
  //Enable the vertex attribute
  glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);
  //This the layout of our first vertex buffer
  //"0" means define the layout for attribute number 0. "3" means that the variables are vec3 made from every 3 floats 
  glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(0));
  glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(sizeof(v_cube))  );
}

float sensitivity = 0.01f;

bool left_pressed = false;
double click_xpos = 0;
double click_ypos = 0;

double updownrot = acos(1.0 / sqrt(3.0));
double sidewayrot = glm::radians(45.0);

double temp_updownrot = 0;
double temp_sidewayrot = 0;

double r = 5 * sqrt(3);

double min_value = 0.15;
double max_value = M_PI - 0.15;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    left_pressed = true;
    glfwGetCursorPos(window, &click_xpos, &click_ypos);
  }

  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
    left_pressed = false;

    updownrot = glm::clamp(updownrot + temp_updownrot,min_value,max_value);
    sidewayrot += temp_sidewayrot;

    temp_updownrot = 0;
    temp_sidewayrot = 0;
  }

}


double lastInputTime = 0.0;
double rotation_duration = 0.15; // 200 ms
bool processinginput = false;
bool reversed = false;
enum input{
  NoInput,
  F,
  B,
  U,
  D,
  R,
  L,
};

input toprocess = NoInput;

std::pair<int,input> input_pairs[6] = {{GLFW_KEY_F,F}, {GLFW_KEY_B,B}, {GLFW_KEY_U,U}, {GLFW_KEY_D,D}, {GLFW_KEY_L,L}, {GLFW_KEY_R,R}, } ;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);


    if(!processinginput){
      for(int i = 0;i<6;i++){
        if (key == input_pairs[i].first && action == GLFW_PRESS){
          processinginput = true;
          toprocess = input_pairs[i].second;
          lastInputTime = glfwGetTime();
          if(mods & GLFW_MOD_SHIFT) reversed = true;
        }
      }
    }
    
  }


std::vector<glm::vec3> key_to_axis = {glm::vec3(1.0,1.0,1.0),glm::vec3(1.0,0.0,0.0),glm::vec3(-1.0,0.0,0.0),glm::vec3(0.0,0.0,1.0),glm::vec3(0.0,0.0,-1.0),glm::vec3(0.0,1.0,0.0),glm::vec3(0.0,-1.0,0.0)}; // put the axeses in here


glm::ivec3 rotateVec(glm::ivec3 p, input move)
{
    switch(move)
    {
        case F: return glm::ivec3(p.x, -p.z, p.y);
        case B: return glm::ivec3(p.x, p.z, -p.y);
        case U: return glm::ivec3(-p.y, p.x, p.z);
        case D: return glm::ivec3(p.y, -p.x, p.z);
        case R: return glm::ivec3(p.z, p.y, -p.x);
        case L: return glm::ivec3(-p.z, p.y, p.x);
    }
    return p;
}


void updateAnimation(){
    if(processinginput){
      double currenttime = glfwGetTime();
      
      float fullturn = 90.0;
      if(reversed) fullturn = -90.0;
      if( (currenttime-lastInputTime) < rotation_duration ){
        // define the partial rotation matrices
        float radians = (currenttime-lastInputTime)/rotation_duration * fullturn;
        auto part_rot = glm::rotate(glm::mat4(1.0f), glm::radians(radians), key_to_axis[toprocess]);

        int face_index = toprocess - 1; // because i arrnaged the two enums this way

        for(int i =0;i<27;i++){
          if(face_indices[face_index][i]){
            temp_rotations[i] = part_rot;
          }
        }
      }
      else{
        // update the actuall model matrices and set temp rotations back to zero
        auto fullrot = glm::rotate(glm::mat4(1.0f), glm::radians(fullturn), key_to_axis[toprocess]);
        int face_index = toprocess - 1; // because i arrnaged the two enums this way
        for(int i = 0;i<27;i++){
          if(face_indices[face_index][i]){
            model_matrices[i] = fullrot * model_matrices[i];
            temp_rotations[i] = glm::mat4(1.0f);
          }
        }

        // update the face_indices here (NOT DONE YET WILL DO SOON)

        for(int i = 0;i<27;i++){
          if(face_indices[face_index][i]){
            //get coord of index
            auto& [x,y,z] = cube_positions[i];
            
            for(int j =0;j<6;j++) face_indices[j][i] = false;
            
            input touse = toprocess;
            if(reversed){
              if(toprocess%2==1) touse = (input)(toprocess+1);
              if(toprocess%2==0) touse = (input)(toprocess-1);
            }

            auto newposition = rotateVec(glm::ivec3(x-1,y-1,z-1),touse);

            x = newposition[0]+1;
            y = newposition[1]+1;
            z = newposition[2]+1;

            if(x==2) face_indices[FRONT][i] = true;
            if(x==0) face_indices[BACK][i] = true;
            if(y==2) face_indices[RIGHT][i] = true;
            if(y==0) face_indices[LEFT][i] = true;
            if(z==2) face_indices[UP][i] = true;
            if(z==0) face_indices[DOWN][i] = true;
            
            cube_positions[i] = {x,y,z};

          }
        }

        processinginput = false;
        toprocess = NoInput;
        reversed = false;
      }
    }
}




void renderGL(GLFWwindow* window)
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  
  if(left_pressed){
      double xurx,xury;
      glfwGetCursorPos(window, &xurx, &xury);
      temp_updownrot = -(xury-click_ypos)*sensitivity;
      temp_sidewayrot = -(xurx-click_xpos)*sensitivity;
  }

  double x,y,z;

  double totalupdown = glm::clamp(updownrot + temp_updownrot,min_value,max_value);

  x = r * sin(totalupdown) * cos(sidewayrot+temp_sidewayrot);
  y = r * sin(totalupdown) * sin(sidewayrot+temp_sidewayrot);
  z = r * cos(totalupdown);

  view_matrix = glm::lookAt(glm::vec3(x,y,z),glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,1.0));

  ortho_matrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -20.0f, 20.0f);

  glm::mat4 projection_matrix =
    glm::perspective(
        glm::radians(45.0f), // FOV
        640.0f / 480.0f,     // aspect ratio
        0.1f,                // near plane
        100.0f               // far plane
    );
  
  modelviewproject_matrix = ortho_matrix * view_matrix;

  for(int i = 0; i < 27; i++)
  {
    modelviewproject_matrix = projection_matrix * view_matrix * temp_rotations[i] * model_matrices[i];

    glUniformMatrix4fv(uModelViewProjectMatrix,1,GL_FALSE,glm::value_ptr(modelviewproject_matrix));

    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
  
}


int main(int argc, char** argv)
{
  // GLFW

  //! The pointer to the GLFW window
  GLFWwindow* window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(csX75::error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;

  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  //We don't want the old OpenGL 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(1200, 900, "Too broke for a Rubiks Cube so need to Simulate it", NULL, NULL);
  if (!window)
    {
      glfwTerminate();
      return -1;
    }
  
  //! Make the window's context current 
  glfwMakeContextCurrent(window);

  //Initialize GLEW
  //Turn this on to get Shader based OpenGL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      //Problem: glewInit failed, something is seriously wrong.
      std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }

  //Print and see what context got enabled
  std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
  std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
  std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
  std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

  std::cout<<std::endl;
  std::cout<<std::endl;

  std::cout<<"Rubiks Cube Simulation!!!!!"<<std::endl;

  std::cout << "Rubik's Cube Simulation Controls:\n\n";

  std::cout << "Click and Drag Left Mouse button to adjust Camera\n\n";

  std::cout << "Press Shift to reverse direction of rotation\n";
  std::cout << "F : Rotate Front Face (clockwise)\n";
  std::cout << "B : Rotate Back Face (clockwise)\n";
  std::cout << "U : Rotate Up Face (clockwise)\n";
  std::cout << "D : Rotate Down Face (clockwise)\n";
  std::cout << "R : Rotate Right Face (clockwise)\n";
  std::cout << "L : Rotate Left Face (clockwise)\n\n";

  std::cout << "ESC : Quit simulation\n\n";

  //Keyboard Callback
  glfwSetKeyCallback(window, key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);
  //MOuse
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  //Initialize GL state
  setup();

  setupcubemodelmatrices();

  // Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0)
    {
       
      // Render here
      updateAnimation();
      renderGL(window);

      // Swap front and back buffers
      glfwSwapBuffers(window);
      
      // Poll for and process events
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;
}
