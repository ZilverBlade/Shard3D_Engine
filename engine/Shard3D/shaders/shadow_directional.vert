#version 450
layout(location = 0) in vec3 position;

layout(push_constant) uniform Push {
	mat4 projectionMatrix; 
	mat4 modelMatrix;
} push;

void main() {
	gl_Position = push.projectionMatrix * push.modelMatrix * vec4(position, 1.f);
}