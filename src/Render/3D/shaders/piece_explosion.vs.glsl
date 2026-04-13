#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform float uExplosionProgress;

out vec3 vNormal;
out vec3 vLocalPos;
out float vExplosionProgress;

out vec3 vBurstDir;

void main()
{
    vec3 safeNormal = normalize(aNormal + vec3(0.0001, 0.0001, 0.0001));

    float seedA = fract(sin(dot(aPosition, vec3(12.9898, 78.233, 37.719))) * 43758.5453);
    float seedB = fract(sin(dot(aPosition, vec3(93.9898, 67.345, 11.135))) * 24634.6345);
    vec3 randomDir = normalize(vec3(seedA * 2.0 - 1.0, 0.45 + seedB * 0.8, seedB * 2.0 - 1.0));
    vec3 burstDir = normalize(safeNormal * 0.6 + randomDir * 0.9 + vec3(0.0, 0.2, 0.0));

    float radialPush = pow(uExplosionProgress, 0.7) * 10.0;
    float upwardPush = pow(uExplosionProgress, 0.85) * 0.95;

    vec3 explodedPosition = aPosition + burstDir * radialPush + vec3(0.0, upwardPush, 0.0);

    gl_Position = uMVP * vec4(explodedPosition, 1.0);
    vNormal = normalize(mat3(uModel) * aNormal);    
    vLocalPos = aPosition;
    vExplosionProgress = uExplosionProgress;

    vBurstDir = mat3(uModel) * burstDir;
}
