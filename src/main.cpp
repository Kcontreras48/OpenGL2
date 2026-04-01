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

// Función para ajustar el tamaño del renderizado
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Manejo de las entradas del teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
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
    glfwSetKeyCallback(window, key_callback);

    // 3. Cargar los punteros de OpenGL usando GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error: Fallo al inicializar GLAD" << std::endl;
        return -1;
    }

    // Configurar OpenGL para 3D
    glEnable(GL_DEPTH_TEST);

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

    // Definir los 8 vértices únicos de un cubo 3D
    float vertices[] = {
        // Cara Frontal (Z positivo)
        -0.5f, -0.5f,  0.5f, // 0: Abajo-Izquierda
         0.5f, -0.5f,  0.5f, // 1: Abajo-Derecha
         0.5f,  0.5f,  0.5f, // 2: Arriba-Derecha
        -0.5f,  0.5f,  0.5f, // 3: Arriba-Izquierda
        
        // Cara Trasera (Z negativo)
        -0.5f, -0.5f, -0.5f, // 4: Abajo-Izquierda
         0.5f, -0.5f, -0.5f, // 5: Abajo-Derecha
         0.5f,  0.5f, -0.5f, // 6: Arriba-Derecha
        -0.5f,  0.5f, -0.5f  // 7: Arriba-Izquierda
    };

    // Usar "Indices" (EBO) para re-utilizar los vértices
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0, // Cara Frontal
        1, 5, 6,  6, 2, 1, // Cara Derecha
        5, 4, 7,  7, 6, 5, // Cara Trasera
        4, 0, 3,  3, 7, 4, // Cara Izquierda
        4, 5, 1,  1, 0, 4, // Cara Inferior
        3, 2, 6,  6, 7, 3  // Cara Superior
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
        // Renderizado: Limpiar pantalla y buffer de profundidad
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activar el shader antes de pasar los uniforms
        glUseProgram(shaderProgram);

        // --- Crear las matrices MVP (Model, View, Projection) ---
        // 1. Modelo: Rotar continuamente el cubo para verlo en 3D
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // 2. Vista: Mover la "cámara" un poco hacia atrás
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // 3. Proyección: Dar perspectiva (FOV, Aspect Ratio, Near, Far)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

        // Pasar las matrices al shader program
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Dibujar el cubo usando sus 36 índices
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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