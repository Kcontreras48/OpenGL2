#version 460 core
layout (location = 0) in vec3 aPos;

// Pasar al fragment shader la posición en espacio global para el cálculo de luz/normales
out vec3 FragPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculamos la posición en espacio de mundo
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    gl_Position = projection * view * worldPos;
}
