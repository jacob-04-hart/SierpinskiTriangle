//#define GLAD
//#define GLFW_INCLUDE_NONE
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <map>
#include <tuple>

const int maxDepth = 8;

// Shader source code
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

std::vector<float> midpoint(std::vector<float> c1,std::vector<float> c2) {
    float x = (c1[0]+c2[0])/2;
    float y = (c1[1]+c2[1])/2;
    float z = (c1[2]+c2[2])/2;
    std::vector<float> midpoint;
    midpoint.insert(midpoint.end(),x);
    midpoint.insert(midpoint.end(),y);
    midpoint.insert(midpoint.end(),z);
    return midpoint;
}

// Helper function to add a vertex and get its index
unsigned int addVertex(std::vector<float>& vertices, 
                       std::map<std::tuple<float, float, float>, unsigned int>& vertexMap, 
                       std::vector<float> vertex) {
    std::tuple<float, float, float> key = {vertex[0], vertex[1], vertex[2]};
    if (vertexMap.find(key) == vertexMap.end()) {
        // Vertex is not in the map, add it
        vertexMap[key] = vertices.size() / 3; // Index of the new vertex
        vertices.insert(vertices.end(), vertex.begin(), vertex.end());
    }
    return vertexMap[key];
}

// Recursive function to generate vertices and indices
void drawST(std::vector<float> c1, std::vector<float> c2, std::vector<float> c3, int depth, 
            std::vector<float>& vertices, std::vector<unsigned int>& indices, 
            std::map<std::tuple<float, float, float>, unsigned int>& vertexMap) {
    if (depth < maxDepth) {
        // Calculate midpoints
        std::vector<float> newC1 = midpoint(c1, c2);
        std::vector<float> newC2 = midpoint(c1, c3);
        std::vector<float> newC3 = midpoint(c2, c3);

        // Recursively generate smaller triangles
        drawST(c1, newC1, newC2, depth + 1, vertices, indices, vertexMap);
        drawST(newC1, c2, newC3, depth + 1, vertices, indices, vertexMap);
        drawST(newC2, newC3, c3, depth + 1, vertices, indices, vertexMap);
    } else {
        // Base case: Add the triangle's vertices and indices
        unsigned int i1 = addVertex(vertices, vertexMap, c1);
        unsigned int i2 = addVertex(vertices, vertexMap, c2);
        unsigned int i3 = addVertex(vertices, vertexMap, c3);

        // Add the indices for the triangle
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(800, 600, "Sierpinski's Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);   
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    // Triangle vertices
    std::vector<float> vertex1 = {
     0.0f,  0.5f, 0.0f
    };

    std::vector<float> vertex2 = {
    -0.5f, -0.5f, 0.0f
    };

    std::vector<float> vertex3 = {
     0.5f, -0.5f, 0.0f
    };

    // Vetex Shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    /* Check for vertex shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    } 
    */

    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    /* Check for fragment shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    */

    // Shader Program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    /* Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    */

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate vertices and indices for the Sierpinski Triangle
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::map<std::tuple<float, float, float>, unsigned int> vertexMap;

    drawST(vertex1, vertex2, vertex3, 0, vertices, indices, vertexMap);

    // Vertex Array Object (VAO)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex Buffer Object (VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Element Buffer Object (EBO)
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Set the polygon mode to line (wireframe mode)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Set the polygon mode to fill (default mode)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // Set the polygon mode to point

    std::cout << "Vertices:" << std::endl;
    std::cout << vertices.size() << std::endl;

    std::cout << "Indices:" << std::endl;
    std::cout << indices.size() << std::endl;

    // Main loop
    while(!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the triangles using EBO
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Swap the buffers
        glfwSwapBuffers(window);

        // Check and call events
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

