#version 330 core

in vec3 vNormal;
in vec3 vLocalPos;
in float vExplosionProgress;

in vec3 vBurstDir;

uniform vec3 uColor;
uniform vec3 uLightDirection;
uniform float uAmbientStrength;

out vec4 FragColor;

float hash3(vec3 p) {
    p = fract(p * vec3(0.131, 0.1030, 0.0973));
    p += dot(p, p.zyx + 31.32);
    return fract((p.x + p.y) * p.z);
}

void main()
{
    vec3 normal = normalize(mix(vNormal, vBurstDir, vExplosionProgress * 0.8));
    vec3 lightDir = normalize(uLightDirection);

    float diffuse = max(dot(normal, lightDir), 0.0);
    float lighting = uAmbientStrength + diffuse * (1.0 - uAmbientStrength);

// Create chunky noise blocks based on local position
vec3 gridPos = floor(vLocalPos * 15.0); 
float noiseVal = hash3(gridPos);

// Animate the noise so sparks flicker and die out
float sparks = smoothstep(0.4, 1.0, noiseVal * (1.0 - vExplosionProgress) + sin(vExplosionProgress * 20.0 + noiseVal * 10.0) * 0.2);    float shell = 1.0 - smoothstep(0.0, 1.0, vExplosionProgress);

    vec3 deepRed = vec3(0.96, 0.10, 0.03);
    vec3 hotOrange = vec3(1.00, 0.35, 0.05);
    vec3 hotYellow = vec3(1.00, 0.82, 0.18);
    float yellowBoost = smoothstep(0.58, 1.0, sparks) * (1.0 - vExplosionProgress * 0.7);

    vec3 fireBase = mix(deepRed, hotOrange, sparks);
    vec3 fireColor = mix(fireBase, hotYellow, yellowBoost * 0.7);
    vec3 smokeColor = vec3(0.11, 0.12, 0.14);

    vec3 shadedBase = min(uColor * lighting, vec3(1.0));
    vec3 exploded = mix(shadedBase, fireColor, 0.78);
    vec3 finalColor = mix(exploded, smokeColor, vExplosionProgress);

    float alpha = shell * (0.65 + 0.35 * sparks);
    if (alpha < 0.02)
        discard;

    FragColor = vec4(finalColor, alpha);
}
