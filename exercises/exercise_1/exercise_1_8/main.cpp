#include <glad/glad.h>
#include <GLFW/glfw3.h>\

#include <iostream>
#include <iomanip>

#include <iostream>
#include <vector>
#include <cmath>
#include <iostream>
#include <math.h>       /* sin cos */

#define PI 3.14159265


// function declarations
// ---------------------
void createArrayBuffer(const std::vector<float>& array, unsigned int& VBO);
void setupShape(unsigned int shaderProgram, unsigned int& VAO, unsigned int& EBO, unsigned int& vertexCount);
void draw(unsigned int shaderProgram, unsigned int VAO, unsigned int vertexCount);
std::vector<float> makeDisc(int sides);


// glfw functions
// --------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


// settings
// --------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const unsigned int sides = 15;


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



int main()
{

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
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
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
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
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
    unsigned int VAO, EBO, vertexCount;
    // generate geometry in a vertex array object (VAO), record the number of vertices in the mesh,
    // tells the shader how to read it
    setupShape(shaderProgram, VAO, EBO, vertexCount);


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
        glfwSwapBuffers(window); // we normally use 2 frame buffers, a back (to draw on) and a front (to show on the screen)
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// create a vertex buffer object (VBO) from an array of values, return VBO handle (set as reference)
// -------------------------------------------------------------------------------------------------
void createArrayBuffer(const std::vector<float>& array, unsigned int& VBO) {
    // create the VBO on OpenGL and get a handle to it
    glGenBuffers(1, &VBO);
    // bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the content of the VBO (type, size, pointer to start, and how it is used)
    glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(GLfloat), &array[0], GL_STATIC_DRAW);

}



// create the geometry, a vertex array object representing it, and set how a shader program should read it
// -------------------------------------------------------------------------------------------------------
void setupShape(const unsigned int shaderProgram, unsigned int& VAO, unsigned int& EBO, unsigned int& vertexCount) {

    unsigned int  genVBO;
    createArrayBuffer(makeDisc(sides), genVBO);

    vertexCount = sides * 6;

    // create a vertex array object (VAO) on OpenGL and save a handle to it
    glGenVertexArrays(1, &VAO);

    // bind vertex array object
    glBindVertexArray(VAO);

    // set vertex shader attribute "aPos"
    glBindBuffer(GL_ARRAY_BUFFER, genVBO);

    int posSize = 3;
    int colorSize = 3;
    int posAttributeLocation = glGetAttribLocation(shaderProgram, "aPos");
    int colorAttributeLocation = glGetAttribLocation(shaderProgram, "aColor");

    glEnableVertexAttribArray(posAttributeLocation);
    glEnableVertexAttribArray(colorAttributeLocation);

    glVertexAttribPointer(posAttributeLocation, posSize, GL_FLOAT, GL_FALSE, sizeof(float) * (posSize + colorSize), 0);
    glVertexAttribPointer(colorAttributeLocation, colorSize, GL_FLOAT, GL_FALSE, sizeof(float) * (posSize + colorSize), (const void*)(sizeof(float) * posSize));

    std::vector<unsigned int> indices;

    for (int i = 0; i < sides; i++)
    {
        //rows
        indices.push_back(0);
        indices.push_back(i + 1);
        if (!(i == sides - 1))
            indices.push_back(i + 2);
        else
        {
            indices.push_back(1);
        }
    }

    std::cout << "------------- indices -----------------" << std::endl;
    int newLine = 0;
    for (size_t i = 0; i < indices.size(); i++)
    {
        std::cout << " " << indices[i];
        if (i % (2 + newLine * 3) == 0 && i != 0) {
            std::cout << std::endl;
            newLine++;
        }
    }

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

}

std::vector<float> makeDisc(int sides) {
    std::vector<float> vertices;
    float radius = 0.5f;
    float alpha = 2 * PI / sides; //alpha of each circle
    std::cout << "Sides: " << sides << std::endl;
    int offset = 17;
    int index = 0;
    for (int i = 0; i < sides; i++) {
        //calculate vertices on the circle
        float angle = alpha * i;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        if (i == 0) {
            //centre
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);

            vertices.push_back(0.5f);
            vertices.push_back(0.5f);
            vertices.push_back(0.5f);

            //point A
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0.0f);

            vertices.push_back(cos(angle) / 2 + 0.5f);
            vertices.push_back(sin(angle) / 2 + 0.5f);
            vertices.push_back(0.5f);
        }

        if (i < sides - 1) {
            //point B
            angle = alpha * (i + 1);
            x = cos(angle) * radius;
            y = sin(angle) * radius;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0.0f);

            vertices.push_back(cos(angle) / 2 + 0.5f);
            vertices.push_back(sin(angle) / 2 + 0.5f);
            vertices.push_back(0.5f);
        }

    }
    int newLine = 0;
    for (size_t i = 0; i < vertices.size(); i++)
    {
        std::cout << std::setprecision(2) << std::fixed;
        std::cout << " " << vertices[i] << " - ";
        if ((i % (5 + newLine * 6)) == 0 && i != 0) {
            std::cout << std::endl;
            newLine++;
        }

    }
    return vertices;
}

// tell opengl to draw a vertex array object (VAO) using a give shaderProgram
// --------------------------------------------------------------------------
void draw(const unsigned int shaderProgram, const unsigned int VAO, const unsigned int vertexCount) {
    // set active shader program
    glUseProgram(shaderProgram);
    // bind vertex array object
    glBindVertexArray(VAO);
    // draw geometry
    glDrawElements(GL_TRIANGLES, sides * 3, GL_UNSIGNED_INT, 0);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

