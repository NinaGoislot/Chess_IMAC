#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUv;

uniform vec3 uColor;
uniform vec3 uTopLightDirection;
uniform vec3 uTopLightColor;
uniform float uTopLightStrength;
uniform vec3 uSideLightDirection;
uniform vec3 uSideLightColor;
uniform float uSideLightStrength;
uniform float uAmbientStrength;
uniform sampler2D uTexture;
uniform int uUseTexture;
uniform float uTextureScale;
uniform int uUseMeshUv;

out vec4 FragColor;

vec2 projectedTextureUv(vec3 worldPos, vec3 worldNormal)
{
    vec3 absNormal = abs(worldNormal);

    if (absNormal.y >= absNormal.x && absNormal.y >= absNormal.z)
        return worldPos.xz * uTextureScale;

    if (absNormal.x >= absNormal.z)
        return worldPos.zy * uTextureScale;

    return worldPos.xy * uTextureScale;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 topLightDir = normalize(uTopLightDirection);
    vec3 sideLightDir = normalize(uSideLightDirection);

    float topDiffuse = max(dot(normal, topLightDir), 0.0);
    float sideDiffuse = max(dot(normal, sideLightDir), 0.0);

    vec3 lighting = vec3(uAmbientStrength)
                  + topDiffuse * uTopLightColor * uTopLightStrength
                  + sideDiffuse * uSideLightColor * uSideLightStrength;

    vec3 albedo = uColor;
    if (uUseTexture == 1)
    {
        vec2 uv = (uUseMeshUv == 1) ? (vUv * uTextureScale) : projectedTextureUv(vWorldPos, normal);
        vec3 textureColor = texture(uTexture, uv).rgb;
        albedo *= textureColor;
    }

    vec3 shaded = min(albedo * lighting, vec3(1.0));
    FragColor = vec4(shaded, 1.0);
}