#version 330

uniform mat4 u_ModelViewProjectionMatrix;
uniform vec3 u_primitive_color;

layout (location = 0) in vec4 a_position;
out vec4 v_color;
out vec4 v_position;

void main(void) {
 	v_color = vec4(u_primitive_color, 1.0f);
	v_position = a_position;
    gl_Position =  u_ModelViewProjectionMatrix * a_position;
}
