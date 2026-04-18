#version 460 core
out vec4 FragColor;

in vec3 FragPos;

// Simular una posición de luz
const vec3 lightPos = vec3(5.0, 5.0, 5.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 objectColor = vec3(1.0, 0.5, 0.2); // Color naranja bonito

void main()
{
    // CÁLCULO DE NORMALES POR CARA:
    // usamos las derivadas parciales de la posición del fragmento para obtener
    // dos vectores tangentes a la cara, y su cross product da la normal plana de la cara.
    vec3 dx = dFdx(FragPos);
    vec3 dy = dFdy(FragPos);
    vec3 norm = normalize(cross(dx, dy));

    // Luz ambiental
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // Luz difusa
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
