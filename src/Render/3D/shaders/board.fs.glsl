#version 330 core

in vec3 vNormal;

uniform vec3 uColor;
uniform vec3 uLightDirection;
uniform float uAmbientStrength;
uniform vec3 uTurnTint;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightDirection);

    float diffuse = max(dot(normal, lightDir), 0.0);
    float lighting = uAmbientStrength + diffuse * (1.0 - uAmbientStrength);

    vec3 shaded = min(uColor * lighting * uTurnTint, vec3(1.0));
    FragColor = vec4(shaded, 1.0);
}