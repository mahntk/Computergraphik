#version 330 core

layout(location = 0) in vec3 in_pos;

// Uniform für die Transformationsmatrix
uniform mat4 u_transform;

void main(){
	// Vertausche y- und z-Komponente
	vec4 position = vec4(in_pos.x, in_pos.z, in_pos.y, 1.0);
	gl_Position = u_transform * position;
}
