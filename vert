#version 330 core

layout (location = 0) in vec2 point;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform float zoom, shx, shy;

void main() {
	gl_Position = vec4((point.x + shx) * zoom, (point.y + shy) * zoom, 0, 1);
	color = aColor;
}
