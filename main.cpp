#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const char* vertexShader =
  "#version 400\n"
  "in vec3 vp;"
  "void main(){"
  "   float y = vp.y;"
      "if(vp.y > 0) y *= -.5;"
  "   gl_Position = vec4(vp.x, y, vp.z, 1.0);"
  "}";

const char* fragmentShader =
    "#version 400\n"
    "out vec4 frag_color;"
    "void main(){"
    "   frag_color = vec4(.5, 1, .5, 1.0);"
    "}";

// Returns shaderId or 0 on error
GLuint loadShader(const char *shaderFilePath, GLuint shaderType){
  ifstream file;
  
  file.open(shaderFilePath, ifstream::in);

  if(!file.good()) return 0;

  // Calculate file size
  unsigned long fileSize;
  file.seekg(0, ios::end);
  fileSize = file.tellg();
  // Reset cursor
  file.seekg(ios::beg);

  GLchar *shaderText = new char[fileSize + 1];
  unsigned int fileCursor = 0;
  while(file.good()){
    shaderText[fileCursor] = (GLchar) file.get();
    if (!file.eof()) fileCursor++;
  }

  shaderText[fileCursor] = '\0';

  file.close();

  GLuint fragmentShaderId = glCreateShader(shaderType);
  glShaderSource(fragmentShaderId, 1, (const GLchar**)&shaderText, NULL);
  glCompileShader(fragmentShaderId);

  GLint shaderCompiled;
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &shaderCompiled);

  if(shaderCompiled == GL_FALSE){
    GLint maxLength = 0;
    glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &maxLength);

    GLchar *errorLog = new char[maxLength];
    glGetShaderInfoLog(fragmentShaderId, maxLength, &maxLength, errorLog);

    cerr << "Shader Error: " << endl << errorLog;

    glDeleteShader(fragmentShaderId);
    fragmentShaderId = 0;
  }
  return fragmentShaderId;
}


int main() {
  // INIT
  if(!glfwInit()){
    cerr << "ERROR: Could not start glfw" << endl;
    return 1;
  }

  GLFWwindow *window = glfwCreateWindow(640, 420, "Spac Invaders", NULL, NULL);

  if (!window) {
    cerr << "Failed to open window with GLFW3" << endl;
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);

  if (gl3wInit()) {
    cerr << "Failed to initialize OpenGL" << endl;
    return -1;
  }
  if (!gl3wIsSupported(3, 2)) {
    cerr << "OpenGL 3.2 not supported" << endl;
    return -1;
  }

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  cout << "Renderer: " << renderer << endl;
  cout << "OpenGL version: " << version << endl;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);


  float points[] = {
    0.f, 0.f, 0.f,
    1.f, 1.f, 0.f,
    1.f, 0.f, 0.f
  };

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 9* sizeof(float), points, GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  // SHADERS
  cout << "Compiling fragment shader" << endl;
  GLuint fragmentShaderId = loadShader("../shaders/mainF.glsl", GL_FRAGMENT_SHADER);
  cout << "Compiling vertex shader" << endl;
  GLuint vertexShaderId = loadShader("../shaders/mainV.glsl", GL_VERTEX_SHADER);

  if(!vertexShaderId || !fragmentShaderId){
    cerr << "Failed to compile a shader" << endl;
    return -1;
  }

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, fragmentShaderId);
  glAttachShader(shaderProgram, vertexShaderId);
  glLinkProgram(shaderProgram);

  // MAIN LOOP
  while(!glfwWindowShouldClose(window))
  {
    glClearColor(.5f, 0.0f, .0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}