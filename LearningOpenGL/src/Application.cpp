#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLPrerequisites.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit()) // Initialize the library
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(960, 540, "GOJO", NULL, NULL); // Create a windowed mode window and its OpenGL context
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
        float vertexData[16] // Defining a vertex buffer
        {
             0.0f,   0.0f,      0.0f,  0.0f,    // 0
             256.0f, 0.0f,      1.0f,  0.0f,    // 1
             256.0f, 256.0f,    1.0f,  1.0f,    // 2
             0.0f,   256.0f,    0.0f,  1.0f     // 3
        };

        unsigned int indices[6] // Defining an index buffer
        {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray vao;

        VertexBuffer vbo(vertexData, sizeof(vertexData));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        vao.AddBuffer(vbo, layout);

        IndexBuffer ibo(indices, 6);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        // shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/GojoTexture256x256.png");
        texture.Bind(0);
        shader.SetUniform1i("u_Texture", 0);

        glm::mat4 projectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        shader.SetUniformMat4f("u_MVP", projectionMatrix);

        vao.Unbind();
        vbo.Unbind();
        ibo.Unbind();
        shader.Unbind();

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.01f;
        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            // Render here
            renderer.Clear();

            shader.Bind();
            // shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            renderer.Draw(vao, ibo, shader);

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