#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

    window = glfwCreateWindow(512, 512, "Spinny Cube", NULL, NULL); // Create a windowed mode window and its OpenGL context
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Make the window's context current

    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        float cubePositions[24]
        {
             0.5,  0.5,  0.5, // top front right        [0]
             0.5,  0.5, -0.5, // top back right         [1]
            -0.5,  0.5,  0.5, // top front left         [2]
            -0.5,  0.5, -0.5, // top back left          [3]
             0.5, -0.5,  0.5, // bottom front right     [4]
             0.5, -0.5, -0.5, // bottom back right      [5]
            -0.5, -0.5,  0.5, // bottom front left      [6]
            -0.5, -0.5, -0.5, // bottom back left       [7]
        };

        unsigned int indices[36] // Defining an index buffer
        {
            0, 1, 2,
            1, 2, 3, // Top

            4, 5, 6,
            5, 6, 7, // Bottom

            2, 3, 7,
            2, 6, 7, // Left

            0, 1, 4,
            1, 5, 4, // Right

            0, 2, 4,
            2, 6, 4, // Front

            3, 1, 5,
            3, 7, 5  // Back
        };

        unsigned int vao;
        glGenVertexArrays(1, &vao);
        GLCall(glBindVertexArray(vao)); // Binds a vertex array object (vao)

        VertexArray va;
        VertexBuffer vb(cubePositions, sizeof(cubePositions));

        VertexBufferLayout layout;
        layout.Push<float>(3); // ?
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 36);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

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
            shader.SetUniform1f("theta", r);
            // shader.SetUniform4f("colorVector", 0.1f, 0.3f, 0.8f, 1.0f);

            va.Bind();
            ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));
            // shader.SetUniform4f("colorVector", 1.0f, 0.3f, 0.8f, 0.5f);
            GLCall(glDrawElements(GL_LINE_STRIP, 36, GL_UNSIGNED_INT, nullptr));

            // Logic for color incrementation
            r += increment;
            //if (r > 1.0f) {
            //    increment = -0.01f;
            //}
            //else if (r < 0.0f) {
            //    increment = 0.01f;
            //}

            // Swap front and back buffers
            GLCall(glfwSwapBuffers(window));

            // Poll for and process events
            GLCall(glfwPollEvents());
        }
    }

    glfwTerminate();
    return 0;
}