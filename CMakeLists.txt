cmake_minimum_required(VERSION 3.15)
project(MotorOpenGL C CXX)

set(CMAKE_CXX_STANDARD 17)

# 1. Descargar GLFW y GLM automáticamente
include(FetchContent)

FetchContent_Declare(glfw 
    GIT_REPOSITORY https://github.com/glfw/glfw.git 
    GIT_TAG 3.3.8)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm 
    GIT_REPOSITORY https://github.com/g-truc/glm.git 
    GIT_TAG 0.9.9.8)
FetchContent_MakeAvailable(glm)

# 2. Configurar el ejecutable
add_executable(App 
    src/main.cpp 
    src/glad.c
)

# 3. Vincular las librerías y las cabeceras
target_link_libraries(App PRIVATE glfw glm)
target_include_directories(App PRIVATE include)

# 4. Copiar la carpeta de shaders al directorio de compilación
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})