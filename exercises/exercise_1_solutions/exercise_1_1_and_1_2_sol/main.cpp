#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>


#include <iostream>
#include <iomanip>

using namespace std;
using namespace glm;
#define PI 3.14159265


// function declarations
// ---------------------
void createArrayBufferExercise8(const std::vector<float>& array, unsigned int& VBO);

void setupShapeEx8(unsigned int shaderProgram, unsigned int& VAO, unsigned int& vertexCount);

void draw(unsigned int shaderProgram, unsigned int VAO, unsigned int vertexCount);


// glfw functions
// --------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);


// settings
// --------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
bool displayOrHideWireframes = false;


// shader programs
// ---------------
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vtxColor; // output a color to the fragment shader\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   vtxColor = aColor;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in  vec3 vtxColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(vtxColor, 1.0);\n"
"}\n\0";


int main() {

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // setup vertex array object (VAO)
    // -------------------------------
    unsigned int VAO, vertexCount;

    glGenVertexArrays(1, &VAO);

    // bind vertex array object
    glBindVertexArray(VAO);
    // generate geometry in a vertex array object (VAO), record the number of vertices in the mesh,
    // tells the shader how to read it
    setupShapeEx8(shaderProgram, VAO, vertexCount);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(.2f, .2f, .2f, 1.0f); // background
        glClear(GL_COLOR_BUFFER_BIT); // clear the framebuffer

        draw(shaderProgram, VAO, vertexCount);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(
            window); // we normally use 2 frame buffers, a back (to draw on) and a front (to show on the screen)
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// create a vertex buffer object (VBO) from an array of values, return VBO handle (set as reference)
// -------------------------------------------------------------------------------------------------
void createArrayBufferExercise8(const std::vector<float>& array, const std::vector<GLint>& indices, unsigned int& VBO,
    unsigned int& EBO) {
    // create the VBO on OpenGL and get a handle to it
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(float),
        &array[0], GL_STATIC_DRAW);


    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint),
        &indices[0], GL_STATIC_DRAW);
}

// create the geometry, a vertex array object representing it, and set how a shader program should read it
// -------------------------------------------------------------------------------------------------------
void setupShapeEx8(const unsigned int shaderProgram, unsigned int& VAO, unsigned int& vertexCount) {
    unsigned int vertexDataVBO, vertexIndicesEBO;
    vector<float> vertexData;
    vector<GLint> vertexIndices;
    int triangleCount = 4;
    float angleInterval = (2 * (float)PI) / (float)triangleCount;

    // vertex center of the circle
    vertexData.push_back(0.0f);
    vertexData.push_back(0.0f);
    vertexData.push_back(0.0f);
    // color
    vertexData.push_back(.5f);
    vertexData.push_back(.5f);
    vertexData.push_back(.5f);

    for (int i = 0; i < triangleCount; i++) {
        float angle = (float)i * angleInterval;
        // // vertex circle at angle i *angleInterval
        vertexData.push_back(cos(angle) / 2);
        vertexData.push_back(sin(angle) / 2);
        vertexData.push_back(0.0f);

        // vertex 3: next vertex of the circle
        angle += angleInterval;
        vertexData.push_back(cos(angle) / 2 + .5f);
        vertexData.push_back(sin(angle) / 2 + .5f);
        vertexData.push_back(.5f);
    }

    // connect the vertices into triangles!
    for (int i = 0; i < triangleCount; i++) {
        vertexIndices.push_back(0);
        vertexIndices.push_back(i + 1);
        vertexIndices.push_back(i + 2);
    }
    int newLine = 0;
    for (size_t i = 0; i < vertexData.size(); i++)
    {
        std::cout << std::setprecision(2) << std::fixed;
        std::cout << " " << vertexData[i] << " - ";
        if ((i % (5 + newLine * 6)) == 0 && i != 0) {
            std::cout << std::endl;
            newLine++;
        }

    }


    createArrayBufferExercise8(vertexData, vertexIndices, vertexDataVBO, vertexIndicesEBO);
    vertexCount = vertexIndices.size();

    // set vertex shader attribute "aPos"
    glBindBuffer(GL_ARRAY_BUFFER, vertexDataVBO);

    int posSize = 3, colorSize = 3;
    int posAttributeLocation = glGetAttribLocation(shaderProgram, "aPos");

    // position attribute
    glEnableVertexAttribArray(posAttributeLocation);
    glVertexAttribPointer(posAttributeLocation, posSize, GL_FLOAT, GL_FALSE,
        (posSize + colorSize) * (int)sizeof(float),
        (void*)0);

    // set vertex shader attribute "aColor"
    int colorAttributeLocation = glGetAttribLocation(shaderProgram, "aColor");

    // color attribute
    glEnableVertexAttribArray(colorAttributeLocation);
    glVertexAttribPointer(colorAttributeLocation, colorSize, GL_FLOAT, GL_FALSE,
        (posSize + colorSize) * (int)sizeof(float),
        (void*)(posSize * sizeof(float)));
}


// tell opengl to draw a vertex array object (VAO) using a give shaderProgram
// --------------------------------------------------------------------------
void draw(const unsigned int shaderProgram, const unsigned int VAO, const unsigned int vertexCount) {
    // set active shader program
    glUseProgram(shaderProgram);
    // bind vertex array object
    glBindVertexArray(VAO);
    // draw geometry
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    //todo find an actual solution
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!displayOrHideWireframes) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            displayOrHideWireframes = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        //find an actual solution
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        displayOrHideWireframes = false;
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

