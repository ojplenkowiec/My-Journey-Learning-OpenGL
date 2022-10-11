#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
        float squarePositions[8] // Defining a vertex buffer
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

        unsigned int vao;
        glGenVertexArrays(1, &vao);
        GLCall(glBindVertexArray(vao)); // Binds a vertex array object (vao)

        VertexArray va;
        VertexBuffer vb(squarePositions, sizeof(squarePositions));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        float r = 0.0f;
        float increment = 0.01f;
        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            // Render here
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            va.Bind();
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
    }

    glfwTerminate();
    return 0;
}