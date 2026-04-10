#version 330 core
in vec3 vTexDir;

uniform vec3 uTopColor;
uniform vec3 uBottomColor;

out vec4 FragColor;

void main()
{
	vec3 direction = normalize(vTexDir);
	float gradientFactor = clamp(direction.y * 0.5 + 0.5, 0.0, 1.0);
	vec3 finalColor = mix(uBottomColor, uTopColor, gradientFactor);
	FragColor = vec4(finalColor, 1.0);
}