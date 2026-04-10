#version 330 core
layout(location = 0) in vec3 aPosition;

out vec3 vTexDir;
uniform mat4 uVP;

void main()
{
	vTexDir = aPosition;
	vec4 pos = uVP * vec4(aPosition, 1.0);
	gl_Position = pos.xyww;
}