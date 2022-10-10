/* INCLUDES */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"

#include "IndexBuffer.h"

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL); // Create a windowed mode window and its OpenGL context
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Make the window's context current

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) // initialses glew AFTER glfw creates a current context
    {
        std::cout << "ERROR!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl; // Displays OpenGL version in console
    {
        float squarePositions[16] // Defining a vertex buffer
        {
            -0.5f, -0.5f,         -0.2f, -0.2f,
             0.5f, -0.5f,          0.2f, -0.2f,
             0.5f,  0.5f,          0.2f,  0.2f,
            -0.5f,  0.5f,         -0.2f,  0.2f
        };    /* big                  small */

        unsigned int indices[6] // Defining an index buffer
        {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int vao;
        glGenVertexArrays(1, &vao);
        GLCall(glBindVertexArray(vao)); // Binds a vertex array object (vao)

        VertexBuffer vb(squarePositions, sizeof(squarePositions));

        GLCall(glEnableVertexAttribArray(0)); //enable attrib pointers 0 and 1 of current vao
        GLCall(glEnableVertexAttribArray(1));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(0 * sizeof(float))));
        GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float))));

        IndexBuffer ib(indices, 6);

        ShaderProgramSource source = parseShader("res/shaders/Basic.shader"); // Parses shader file into ShaderProgramSource struct
        unsigned int program = createProgram(source.VertexSource, source.FragmentSource); // Creates a program from each part of the ShaderProgramSource struct
        GLCall(glUseProgram(program)); // Tells OpenGL to use program we created

        GLCall(int location = glGetUniformLocation(program, "u_Color"));
        ASSERT(location != -1);

        float r = 0.0f;
        float increment = 0.01f;
        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            // Render here
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            GLCall(glUseProgram(program));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

            GLCall(glBindVertexArray(vao));
            ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            // Logic for color incrementation
            r += increment;
            if (r > 1.0f) {
                increment = -0.01f;
            }
            else if (r < 0.0f) {
                increment = 0.01f;
            }

            // Swap front and back buffers
            GLCall(glfwSwapBuffers(window));

            // Poll for and process events
            GLCall(glfwPollEvents());
        }

        GLCall(glDeleteProgram(program));
    }

    glfwTerminate();
    return 0;
}