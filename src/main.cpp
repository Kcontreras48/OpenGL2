#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

// Función para leer un archivo de texto (shaders)
std::string readShaderFile(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo (" << filePath << ")\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Función para ajustar el tamaño del renderizado si el usuario cambia el tamaño de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Función para manejar las entradas del teclado (ej: presionar ESC para cerrar)
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 1. Inicializar GLFW
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Crear la ventana
    GLFWwindow* window = glfwCreateWindow(800, 800, "Ventana Bonita", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error: Fallo al crear la ventana de GLFW. ¿Tu GPU soporta OpenGL 4.6?" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 3. Cargar los punteros de OpenGL usando GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error: Fallo al inicializar GLAD" << std::endl;
        return -1;
    }

    // --- Configuración de Shaders y Buffers ---
    
    // Cargar y compilar el Vertex Shader
    std::string vertCode = readShaderFile("shaders/default.vert");
    const char* vertexShaderSource = vertCode.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // Cargar y compilar el Fragment Shader
    std::string fragCode = readShaderFile("shaders/default.frag");
    const char* fragmentShaderSource = fragCode.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // Enlazar shaders a un Shader Program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Definir los vértices únicos de la 'L'
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // 0: Izquierda Abajo
         0.5f, -0.5f, 0.0f, // 1: Derecha Abajo
        -0.5f,  0.5f, 0.0f, // 2: Arriba Izquierda
         0.0f,  0.5f, 0.0f, // 3: Arriba Interior
         0.0f,  0.0f, 0.0f, // 4: Centro
         0.5f,  0.0f, 0.0f  // 5: Derecha Arriba (punta horizontal)
    };

    // Usar "Indices" (EBO) para re-utilizar los vértices y formar 4 triángulos
    unsigned int indices[] = {
        2, 0, 4, // Triángulo 1 (Parte del palo vertical)
        2, 4, 3, // Triángulo 2 (Resto del palo vertical)
        0, 1, 4, // Triángulo 3 (Parte baja del horizontal)
        4, 1, 5  // Triángulo 4 (Punta derecha horizontal)
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // Generar buffer de los indices
    
    // Vincular y configurar VAO/VBO/EBO
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Configurar EBO con los índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Desvincular VAO
    glBindVertexArray(0);

    // 4. Bucle principal de renderizado (Game Loop)
    while (!glfwWindowShouldClose(window)) {
        // Entradas
        processInput(window);

        // Renderizado: Limpiar la pantalla con un color sólido
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Dibujar la 'L' usando los índices
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0); // Dibujar los 12 índices

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 5. Limpiar y cerrar
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}