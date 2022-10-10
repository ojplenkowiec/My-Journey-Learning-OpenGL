/* INCLUDES */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/* ERROR CHECKING */
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ") : " << function << " " << file << " : " << line << std::endl;
        return false;
    }
    return true;
}

/* SHADER/PROGRAM GENERATION AND COMPILATION */
// Struct for parsed GLSL code
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

// Parses GLSL code from file path to ShaderProgramSource struct
static ShaderProgramSource parseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType 
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) 
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos) 
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) 
            {
                type = ShaderType::FRAGMENT;
            }
        } 
        else 
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

// Creates and compiles a shader of a specific type with given GLSL code, returning ID
static unsigned int compileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Error handling for GLSL code
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) 
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

// Creates a program from 2 valid GLSL shader scripts (vertex and fragment), returning ID
static unsigned int createProgram(const std::string& vertexShader, const std::string& fragmentShader) 
{
    GLCall(unsigned int program = glCreateProgram()); // Creates a program
    GLCall(unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader)); // Creates a vertex shader
    GLCall(unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader)); // Creates a fragment shader

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

/* MAIN FUNCTION */
int main(void)
{
    GLFWwindow* window;

    if (!glfwInit()) // Initialize the library
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL); // Create a windowed mode window and its OpenGL context
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Make the window's context current

    if (glewInit() != GLEW_OK) // initialses glew AFTER glfw creates a current context
    {
        std::cout << "ERROR!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl; // Displays OpenGL version in console

    float positions[8] // Defining a vertex buffer
    {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f 
    };

    unsigned int indices[6] // Defining an index buffer
    {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); // Binds a buffer object ID to the specified buffer binding point (array)
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW)); // Pushes data to array buffer
                                                                                  
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0)); // Defines behavior of vertex attribute pointer 0

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); // Binds a buffer object ID to the specified buffer binding point (element array)
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = parseShader("res/shaders/Basic.shader"); // Parses shader file into ShaderProgramSource struct
    unsigned int program = createProgram(source.VertexSource, source.FragmentSource); // Creates a program from each part of the ShaderProgramSource struct
    GLCall(glUseProgram(program)); // Tells OpenGL to use program we created

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Render here
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        // Swap front and back buffers
        GLCall(glfwSwapBuffers(window));

        // Poll for and process events
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(program));

    glfwTerminate();
    return 0;
}