#version 330 core

in vec3 vNormal;

uniform vec3 uColor;
uniform vec3 uLightDirection;
uniform float uAmbientStrength;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightDirection);

    float diffuse = max(dot(normal, lightDir), 0.0);
    float lighting = uAmbientStrength + diffuse * (1.0 - uAmbientStrength);

    FragColor = vec4(uColor * lighting, 1.0);
}