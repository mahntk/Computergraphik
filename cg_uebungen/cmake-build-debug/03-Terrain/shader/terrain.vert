#version 330 core

layout(location = 0) in vec3 in_pos;

/* TODO (Aufgabe 2) Hier soll eine Transformation eingefuegt werden */


void main(){
	/* TODO (Aufgabe 2) Wenden Sie die Transformation an */
	
	gl_Position = vec4(in_pos, 1.0);
}