#version 460 core
out vec4 FragColor;

in vec3 localPos;

void main()
{
    // Convertimos de [-0.5, 0.5] a [0.0, 1.0] para que sean rangos de color válidos
    vec3 color = localPos + vec3(0.5);
    FragColor = vec4(color, 1.0f);
}
