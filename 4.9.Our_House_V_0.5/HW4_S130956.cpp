#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))


int flag_draw_screen, flag_screen_effect, flag_blind_effect, flag_cartoon_effect;
float screen_width, cartoon_levels;

int flag_cull_face = 0, depth_test_on = 0;
short int IS_DYNAMIC = 1;

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "My_Shading.h"
GLuint h_ShaderProgram_simple, h_ShaderProgram_PS, h_ShaderProgram_GS; // handles to shader programs
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables
enum _CameraType { CAMERA_WORLD_VIEWER, CAMERA_TIGER, CAMERA_CAR };
short int camera_type, walls, shader_type, mouse_right, glow_tiger;
int freq = 90, density = 2, time = 0, screen_x = 3, screen_y = 2;

GLint loc_ModelViewProjectionMatrix_simple;

// for Phone Shading shaders
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color_PS, loc_global_ambient_color_GS;
loc_light_Parameters loc_light_PS[NUMBER_OF_LIGHT_SUPPORTED], loc_light_GS[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material_PS, loc_material_GS;
GLint loc_screen_effect, loc_screen_width;
GLint loc_blind_effect, loc_cartoon_effect, loc_cartoon_levels;
GLint loc_ModelViewProjectionMatrix_PS, loc_ModelViewMatrix_PS, loc_ModelViewMatrixInvTrans_PS;
GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;
GLint loc_isTiger;
GLint loc_density;
GLint loc_isSide;
GLint loc_isFront;
GLint loc_isBuilding;
GLint loc_freq;
GLint loc_time;
GLint loc_screen_x;
GLint loc_screen_y;


typedef struct _CALLBACK_CONTEXT {
	int left_button_status, rotation_mode_cow, timestamp_cow;
	int prevx, prevy;
	int direction;
	float rotation_angle_cow;
} CALLBACK_CONTEXT;
CALLBACK_CONTEXT cc;

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
	{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_PS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_sc.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Phong_sc.frag" },
	{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_GS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
	{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_PS = LoadShaders(shader_info_PS);
	loc_ModelViewProjectionMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrixInvTrans");

	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light_PS[i].light_on = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light_PS[i].position = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light_PS[i].ambient_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light_PS[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light_PS[i].specular_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light_PS[i].spot_direction = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light_PS[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light_PS[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light_PS[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_PS, string);
	}


	loc_global_ambient_color_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light_GS[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light_GS[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light_GS[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light_GS[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light_GS[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light_GS[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light_GS[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light_GS[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light_GS[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}


	loc_material_PS.ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.ambient_color");
	loc_material_PS.diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.diffuse_color");
	loc_material_PS.specular_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_color");
	loc_material_PS.emissive_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.emissive_color");
	loc_material_PS.specular_exponent = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_exponent");

	loc_screen_effect = glGetUniformLocation(h_ShaderProgram_PS, "screen_effect");
	loc_screen_width = glGetUniformLocation(h_ShaderProgram_PS, "screen_width");
	loc_isTiger = glGetUniformLocation(h_ShaderProgram_PS, "isTiger");
	loc_isSide = glGetUniformLocation(h_ShaderProgram_PS, "isSide");
	loc_isFront = glGetUniformLocation(h_ShaderProgram_PS, "isFront");
	loc_isBuilding = glGetUniformLocation(h_ShaderProgram_PS, "isBuilding");
	loc_freq = glGetUniformLocation(h_ShaderProgram_PS, "freq");
	loc_density = glGetUniformLocation(h_ShaderProgram_PS, "density");
	loc_time = glGetUniformLocation(h_ShaderProgram_PS, "time");
	loc_screen_x = glGetUniformLocation(h_ShaderProgram_PS, "screen_x");
	loc_screen_y = glGetUniformLocation(h_ShaderProgram_PS, "screen_y");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_blind_effect");

	loc_cartoon_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_cartoon_effect");
	loc_cartoon_levels = glGetUniformLocation(h_ShaderProgram_PS, "u_cartoon_levels");

	loc_material_GS.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material_GS.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material_GS.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material_GS.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material_GS.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");

}


void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_PS);

	glUniform4f(loc_global_ambient_color_PS, 0.2f, 0.2f, 0.2f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light_PS[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light_PS[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light_PS[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light_PS[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light_PS[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light_PS[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light_PS[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light_PS[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light_PS[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light_PS[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light_PS[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material_PS.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material_PS.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material_PS.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material_PS.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material_PS.specular_exponent, 0.0f); // [0.0, 128.0]

	glUniform1i(loc_screen_effect, 0);
	glUniform1i(loc_isTiger, flag_blind_effect);
	glUniform1i(loc_isSide, 0);
	glUniform1i(loc_isFront, 0);
	glUniform1i(loc_isBuilding, 0);
	glUniform1i(loc_freq, freq);
	glUniform1i(loc_density, density);
	glUniform1f(loc_screen_width, 0.1f);
	glUniform1i(loc_time, time);
	glUniform1i(loc_screen_x, screen_x);
	glUniform1i(loc_screen_y, screen_y);

	glUniform1i(loc_blind_effect, 0);

	glUniform1i(loc_cartoon_effect, 0);
	glUniform1i(loc_cartoon_levels, 3.0f);


	glUseProgram(h_ShaderProgram_GS);

	glUniform4f(loc_global_ambient_color_GS, 0.2f, 0.2f, 0.2f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light_GS[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light_GS[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light_GS[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light_GS[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light_GS[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light_GS[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light_GS[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light_GS[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light_GS[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light_GS[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light_GS[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material_GS.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material_GS.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material_GS.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material_GS.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material_GS.specular_exponent, 0.0f); // [0.0, 128.0]


	glUseProgram(0);
}


// End of shader setup
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> //inverse, affineInverse, etc.
// Begin of geometry setup
glm::mat4 ModelViewProjectionMatrix;
// End of geometry setup
glm::mat4 ModelViewMatrix, ViewMatrix, ProjectionMatrix;
glm::mat3 ModelViewMatrixInvTrans;

#define NUMBER_OF_CAMERAS 8

#define LOC_POSITION 0
#define LOC_NORMAL 1

Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

// for tiger animation
int cur_frame_tiger = 0;
float rotation_angle_tiger = 0.0f;

 // floor object
 GLuint rectangle_VBO, rectangle_VAO;
 GLfloat rectangle_vertices[12][3] = {  // vertices enumerated counterclockwise
	 { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
	 { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
	 { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
 };


Material_Parameters material_floor;
Material_Parameters material_screen;
// prepare floor and screen
void prepare_rectangle(void) { // Draw coordinate axes.
							   // Initialize vertex buffer object.
	glGenBuffers(1, &rectangle_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &rectangle_VAO);
	glBindVertexArray(rectangle_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glVertexAttribPointer(LOC_POSITION, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_floor.ambient_color[0] = 0.05f; material_floor.ambient_color[1] = 0.05f;
	material_floor.ambient_color[2] = 0.05f; material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.4f; material_floor.diffuse_color[1] = 0.4f;
	material_floor.diffuse_color[2] = 0.4f; material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.3f; material_floor.specular_color[1] = 0.3f;
	material_floor.specular_color[2] = 0.3f; material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 2.5f;

	material_floor.emissive_color[0] = 0.0f; material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f; material_floor.emissive_color[3] = 1.0f;
	// prepare_screen
	material_screen.ambient_color[0] = 0.1745f; material_screen.ambient_color[1] = 0.0117f;
	material_screen.ambient_color[2] = 0.0117f; material_screen.ambient_color[3] = 1.0f;

	material_screen.diffuse_color[0] = 0.6142f; material_screen.diffuse_color[1] = 0.0413f;
	material_screen.diffuse_color[2] = 0.0413f; material_screen.diffuse_color[3] = 1.0f;

	material_screen.specular_color[0] = 0.7278f; material_screen.specular_color[1] = 0.6269f;
	material_screen.specular_color[2] = 0.6269f; material_screen.specular_color[3] = 1.0f;

	material_screen.specular_exponent = 2.5f;

	material_screen.emissive_color[0] = 0.0f; material_screen.emissive_color[1] = 0.0f;
	material_screen.emissive_color[2] = 0.0f; material_screen.emissive_color[3] = 1.0f;
}

void set_material_floor(void) {
	// assume ShaderProgram_PS is used
	glUniform4fv(((shader_type==0)?loc_material_PS: loc_material_GS).ambient_color, 1, material_floor.ambient_color);
	glUniform4fv(((shader_type == 0) ? loc_material_PS: loc_material_GS).diffuse_color, 1, material_floor.diffuse_color);
	glUniform4fv(((shader_type == 0) ? loc_material_PS: loc_material_GS).specular_color, 1, material_floor.specular_color);
	glUniform1f(((shader_type == 0) ? loc_material_PS: loc_material_GS).specular_exponent, material_floor.specular_exponent);
	glUniform4fv(((shader_type == 0) ? loc_material_PS: loc_material_GS).emissive_color, 1, material_floor.emissive_color);
}

void draw_floor(void) {
	glFrontFace(GL_CCW);

	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


void set_material_screen(void) {
	// assume ShaderProgram_PS is used
	glUniform4fv(((shader_type == 0) ? loc_material_PS: loc_material_GS).ambient_color, 1, material_screen.ambient_color);
	glUniform4fv(((shader_type == 0) ? loc_material_PS: loc_material_GS).diffuse_color, 1, material_screen.diffuse_color);
	glUniform4fv(((shader_type == 0) ? loc_material_PS: loc_material_GS).specular_color, 1, material_screen.specular_color);
	glUniform1f(((shader_type == 0) ? loc_material_PS: loc_material_GS).specular_exponent, material_screen.specular_exponent);
	glUniform4fv(((shader_type == 0) ? loc_material_PS: loc_material_GS).emissive_color, 1, material_screen.emissive_color);
}

void draw_screen(void) {
	glFrontFace(GL_CCW);

	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void initialize_screen(void) {
	flag_draw_screen = flag_screen_effect = 0;
	screen_width = 0.125f;
}

void initialize_blind(void) {
	flag_blind_effect = 0;
}

void initialize_cartoon(void) {
	flag_cartoon_effect = 0; cartoon_levels = 3.0f;
}


enum CAM_TYPE { SET_VIEW_MATRIX, LOOK_AT, CCTV };
typedef struct _CAMERA {
	bool type;
	float trans_speed, rot_speed, key_magnitude;

	// for set_view_matrix
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
	float fov_y, aspect_ratio, near_clip, far_clip;
	int move_status;

	// for lookAt
	glm::vec3 prp, vrp, vup;	// in this example code, make up always equal to the v direction
	float zoom_factor;
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
int camera_selected;

typedef struct _VIEWPORT {
	int x, y, w, h;
} VIEWPORT;
VIEWPORT viewport[NUMBER_OF_CAMERAS];

// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 Cam_ViewProjectionMatrix[NUMBER_OF_CAMERAS], Cam_ViewMatrix[NUMBER_OF_CAMERAS], Cam_ProjectionMatrix[NUMBER_OF_CAMERAS];
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
//glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when it is ready.


// Begin of Callback function definitions
#include "Object_Definitions.h"
// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include "4.5.5.GeometrySetup.h"

void print_mat4(const char *string, glm::mat4 M) {
	fprintf(stdout, "\n***** %s ******\n", string);
	for (int i = 0; i < 4; i++)
		fprintf(stdout, "*** COL[%d] (%f, %f, %f, %f)\n", i, M[i].x, M[i].y, M[i].z, M[i].w);
	fprintf(stdout, "**************\n\n");
}

void set_ViewMatrix_from_camera_frame2(int cam_index) {
	Cam_ViewMatrix[cam_index][0] = glm::vec4(camera[cam_index].uaxis.x, camera[cam_index].vaxis.x, camera[cam_index].naxis.x, 0.0f);
	Cam_ViewMatrix[cam_index][1] = glm::vec4(camera[cam_index].uaxis.y, camera[cam_index].vaxis.y, camera[cam_index].naxis.y, 0.0f);
	Cam_ViewMatrix[cam_index][2] = glm::vec4(camera[cam_index].uaxis.z, camera[cam_index].vaxis.z, camera[cam_index].naxis.z, 0.0f);
	Cam_ViewMatrix[cam_index][3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	Cam_ViewMatrix[cam_index] = glm::translate(Cam_ViewMatrix[cam_index], -camera[cam_index].pos);
}

void set_ViewMatrix_from_camera_frame(int cam_index) {
	Cam_ViewMatrix[cam_index] = glm::mat4(1.0f);
	Cam_ViewMatrix[cam_index][0].x = camera[cam_index].uaxis.x;
	Cam_ViewMatrix[cam_index][0].y = camera[cam_index].vaxis.x;
	Cam_ViewMatrix[cam_index][0].z = camera[cam_index].naxis.x;

	Cam_ViewMatrix[cam_index][1].x = camera[cam_index].uaxis.y;
	Cam_ViewMatrix[cam_index][1].y = camera[cam_index].vaxis.y;
	Cam_ViewMatrix[cam_index][1].z = camera[cam_index].naxis.y;

	Cam_ViewMatrix[cam_index][2].x = camera[cam_index].uaxis.z;
	Cam_ViewMatrix[cam_index][2].y = camera[cam_index].vaxis.z;
	Cam_ViewMatrix[cam_index][2].z = camera[cam_index].naxis.z;

	Cam_ViewMatrix[cam_index] = glm::translate(Cam_ViewMatrix[cam_index], -camera[cam_index].pos);
}

void renew_cam_position_along_axis(int cam_index, float del, glm::vec3 trans_axis) {
	camera[cam_index].pos += camera[cam_index].trans_speed * del*trans_axis;
}

void renew_cam_orientation_rotation_around_axis(int cam_index, float angle, glm::vec3 rot_axis) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), camera[cam_index].rot_speed *TO_RADIAN*angle, rot_axis));
	// 다음 3 axis에 대한 rot axis로 선택된 axis에 대한 처리는 불필요
	camera[cam_index].uaxis = RotationMatrix * camera[cam_index].uaxis;
	camera[cam_index].vaxis = RotationMatrix * camera[cam_index].vaxis;
	camera[cam_index].naxis = RotationMatrix * camera[cam_index].naxis;
}

// Start of callback function definitions

enum DIRECTION{UP = 1, DOWN, LEFT, RIGHT};


// Begin of callback supplement
#include "4.5.5.CallbackSupplement.h"
// End of callback supplement

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	display_camera(1);
	display_camera(2);
	display_camera(3);
	display_camera(4);
	switch (camera_type) {
	case CAMERA_WORLD_VIEWER:
		display_camera(6); break;
	case CAMERA_TIGER:
		display_camera(5); break;
	}
	glutSwapBuffers();
}

void timer_scene(int timestamp_scene) {
	cur_frame_tiger = timestamp_scene % N_TIGER_FRAMES;
	rotation_angle_tiger = (timestamp_scene % 360)*TO_RADIAN;
	tiger_data.cur_frame = timestamp_scene % 12;
	tiger_data.rotation_angle = (timestamp_scene % 360)*TO_RADIAN;
	cc.rotation_angle_cow = (float)(cc.timestamp_cow);//
	time = (time + 1) % 10;
	glUseProgram(h_ShaderProgram_PS);
	glUniform1i(loc_time, time);
	glUniform1i(loc_blind_effect, flag_blind_effect);
	glUseProgram(0);
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
	cc.timestamp_cow = (cc.timestamp_cow + 1) % 760;//

}

void mousepress(int button, int state, int x, int y) {
	int modifer_status = glutGetModifiers();

	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			cc.left_button_status = GLUT_DOWN;
			camera[camera_selected].move_status = 1;
			cc.prevx = x; cc.prevy = y;
		}
		else if (state == GLUT_UP) {
			cc.left_button_status = GLUT_UP;
			camera[camera_selected].move_status = 0;
		}
	}
	else if ((button == GLUT_RIGHT_BUTTON)) {
		if (state == GLUT_DOWN && modifer_status == GLUT_ACTIVE_SHIFT) {
			camera[camera_selected].move_status = 1;
			mouse_right = 1;
			shader_type = 1;
			glUseProgram(h_ShaderProgram_GS);
			glutPostRedisplay();
		}
		else if (state == GLUT_UP) {
			camera[camera_selected].move_status = 0;
			mouse_right = 0;
			shader_type = 0;
			glUseProgram(h_ShaderProgram_PS);
			glutPostRedisplay();
		}
	}
}

// cam 0, 1은 setVIew로 정의됨->renew적용 가능
// cam 2는 lookAt으로 정의됨->renew적용 불가
void motion(int x, int y) {
	float delx, dely;

	if (!camera[camera_selected].move_status) return;

	switch (camera_selected) {
	case 0:
		if (camera_type == CAMERA_TIGER)
			return;
		break;
	case 1: case 2: case 3: case 5:
		return;
	}

	//if (camera_type == CAMERA_CAR) {

		if (cc.direction == RIGHT || cc.direction == LEFT) {
			float sign = (cc.direction == RIGHT) ? 0.1f: -0.1f;
			if(0.0f <= carMove + sign && carMove + sign <=55.0f)
				carMove += sign;
		}
	//}

	delx = (float)(x - cc.prevx); dely = (float)(cc.prevy - y);
	cc.prevx = x; cc.prevy = y;

	int modifer_status = glutGetModifiers();

	printf("entered motion, camera selected: %d\n", camera_selected);
	switch (modifer_status) { // you may define the key combinations yourself.
	case GLUT_ACTIVE_CTRL:
		//renew_cam_position_along_axis(camera_selected, dely, camera[camera_selected].vaxis);
		//renew_cam_position_along_axis(camera_selected, delx, camera[camera_selected].uaxis);
		renew_cam_orientation_rotation_around_axis(camera_selected, camera[camera_selected].rot_speed*delx, camera[camera_selected].vaxis);
		renew_cam_orientation_rotation_around_axis(camera_selected, camera[camera_selected].rot_speed*dely, camera[camera_selected].uaxis);
		if (cc.direction == UP || cc.direction == DOWN) {
			float sign = (cc.direction == UP) ? 1 : -1;
			renew_cam_orientation_rotation_around_axis(camera_selected, sign*camera[camera_selected].rot_speed*camera[camera_selected].key_magnitude, -camera[camera_selected].naxis);
			cc.direction = 0;
			camera[camera_selected].move_status = 0;
		}
		break;
	case GLUT_ACTIVE_ALT:
		if (5.0f <= camera[camera_selected].fov_y + dely && camera[camera_selected].fov_y + dely <= 100.0f) {
			camera[camera_selected].fov_y += 0.1f * dely;
			printf("fov_y : %f\n", camera[camera_selected].fov_y);
		}
		break;
		/*
	case GLUT_ACTIVE_SHIFT:
		if (mouse_right == 1) {
			shader_type = 1;
			glUseProgram(h_ShaderProgram_GS);
			glutPostRedisplay();
		}
		break;
		*/
	default:
		renew_cam_position_along_axis(camera_selected, dely, camera[camera_selected].vaxis);
		renew_cam_position_along_axis(camera_selected, delx, camera[camera_selected].uaxis);
		printf("delx:%f\tdely:%f\n", delx, dely);
		printf("cc direction in motion: %d\n", cc.direction);
		if (cc.direction == UP || cc.direction == DOWN) {
			float sign = (cc.direction == UP) ? 1 : -1; 
			renew_cam_position_along_axis(camera_selected, sign * camera[camera_selected].key_magnitude, -camera[camera_selected].naxis);
			cc.direction = 0;
			camera[camera_selected].move_status = 0;
		}
		//renew_cam_orientation_rotation_around_axis(camera_selected, delx, -camera[camera_selected].vaxis);

		break;
	}

	set_ViewMatrix_from_camera_frame(camera_selected);
	Cam_ProjectionMatrix[camera_selected] = glm::perspective(camera[camera_selected].fov_y*TO_RADIAN, camera[camera_selected].aspect_ratio, camera[camera_selected].near_clip, camera[camera_selected].far_clip);
	Cam_ViewProjectionMatrix[camera_selected] = Cam_ProjectionMatrix[camera_selected] * Cam_ViewMatrix[camera_selected];

	glutPostRedisplay();
}

void special(int key, int x, int y) {
	camera[camera_selected].move_status = 1;
	switch (key) {
	case GLUT_KEY_DOWN:
		printf("special down: %d->", cc.direction);
		cc.direction = DOWN;
		printf("%d\n", cc.direction);
		motion(cc.prevx, cc.prevy);
		break;
	case GLUT_KEY_UP:
		printf("special up: %d->", cc.direction);
		cc.direction = UP;
		printf("%d\n", cc.direction);
		motion(cc.prevx, cc.prevy);
		break;
	case GLUT_KEY_LEFT:
		cc.direction = LEFT;
		motion(cc.prevx, cc.prevy);
		break;
	case GLUT_KEY_RIGHT:
		cc.direction = RIGHT;
		motion(cc.prevx, cc.prevy);
		break;
	}
	
}

void initialize_camera(void);

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0;
	static int flag_polygon_mode = 1;
	static int flag_view_mode = 1;

	if ((key >= '0') && (key <= '0' + NUMBER_OF_LIGHT_SUPPORTED - 1)) {
		int light_ID = (int)(key - '0');
		if (shader_type == 1) return;
		glUseProgram(h_ShaderProgram_PS);
		light[light_ID].light_on = 1 - light[light_ID].light_on;
		glUniform1i(loc_light_PS[light_ID].light_on, light[light_ID].light_on);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 'g':
		if (flag_screen_effect != 1) return;
		++screen_x;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_screen_x, screen_x);
		glUniform1i(loc_screen_effect, flag_screen_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'h':
		if (flag_screen_effect != 1) return;
		if(screen_x - 1 >= 1)
			--screen_x;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_screen_x, screen_x);
		glUniform1i(loc_screen_effect, flag_screen_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 't':
		if (flag_screen_effect != 1) return;
		++screen_y;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_screen_y, screen_y);
		glUniform1i(loc_screen_effect, flag_screen_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'y':
		if (flag_screen_effect != 1) return;
		if (screen_y - 1 >= 1)
			--screen_y;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_screen_y, screen_y);
		glUniform1i(loc_screen_effect, flag_screen_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'S':
		shader_type = 1 - shader_type;
		glUseProgram((shader_type==0)? h_ShaderProgram: h_ShaderProgram_GS);
		glutPostRedisplay();
		break;
	case 'u':
		if (shader_type == 1) return;
		freq += 5;
		printf("cra %d\n", freq);
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_freq, freq);
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUseProgram(0);
		
		glutPostRedisplay();
		break;
	case 'i':
		if (shader_type == 1) return;
		freq -= 5;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_freq, freq);
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUseProgram(0);
		
		glutPostRedisplay();
		break;
	case 'j':
		if (shader_type == 1) return;
		if (flag_draw_screen != 1)
			return;
		if(density + 1 > 0)
			density += 1;
		printf("density %d\n", density);
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_density, density);
		glUniform1i(loc_screen_effect, flag_screen_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'k':
		if (shader_type == 1) return;
		if (flag_draw_screen != 1)
			return;
		if (density - 1 > 1)
			density -= 1;
		printf("density %d\n", density);
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_density, density);
		glUniform1i(loc_screen_effect, flag_screen_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'w':
		walls = (walls + 1)%3; 
		break;
	case 'p':
		if (shader_type == 1) return;
		flag_polygon_mode = 1 - flag_polygon_mode;
		if (flag_polygon_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	case 's':
		if (shader_type == 1) return;
		flag_draw_screen = 1 - flag_draw_screen;
		glutPostRedisplay();
		break;
	case 'e':
		if (shader_type == 1) return;
		if (flag_draw_screen) {
			flag_screen_effect = (flag_screen_effect + 1) % 4;
			glutPostRedisplay();
		}
		break;
	case 'b':
		if (shader_type == 1) return;
		flag_blind_effect = (flag_blind_effect + 1) % 3;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_isTiger, flag_blind_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'x':
		if (shader_type == 1) return;
		flag_cartoon_effect = 1 - flag_cartoon_effect;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_cartoon_effect, flag_cartoon_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'z':
		if (shader_type == 1) return;
		if (flag_cartoon_effect) {
			if (cartoon_levels >= 3.0f) {
				cartoon_levels -= 1.0f;
				glUseProgram(h_ShaderProgram_PS);
				glUniform1f(loc_cartoon_levels, cartoon_levels);
				glUseProgram(0);
				glutPostRedisplay();
			}
		}
		break;
	case 'a':
		if (shader_type == 1) return;
		if (flag_cartoon_effect) {
			if (cartoon_levels <= 9.0f) {
				cartoon_levels += 1.0f;
				glUseProgram(h_ShaderProgram_PS);
				glUniform1f(loc_cartoon_levels, cartoon_levels);
				glUseProgram(0);
				glutPostRedisplay();
			}
		}
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
		/*
	case 'f':
		printf("polygon fill on: %d\n", polygon_fill_on);
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		//glutPostRedisplay();
		break;
		*/
	/*
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
		*/
	case '0':	case '1':	case '2':	case '3':	case '4':	case '5':	case '6':	case '7':
		if (cc.left_button_status == GLUT_UP) {
			camera[camera_selected].move_status = 0;
			camera_selected = key - '0';
			glutPostRedisplay();
		}
		break;
	case 'D':
		camera_type = (camera_type + 1)%2;
		glutPostRedisplay();
		printf((camera_type == CAMERA_WORLD_VIEWER) ? "WORLD SELECTED\n" : "TIGER SELECTED\n");
		break;
	/*
	case 's':
		IS_DYNAMIC = 1 - IS_DYNAMIC;
		break;
	*/
	
	}
	
	//glutPostRedisplay();
}

void reshape(int width, int height) {
	float aspect_ratio;
	glViewport(0, 0, width, height);
	
	//aspect_ratio = (float)width / height;
	//ProjectionMatrix = glm::perspective(15.0f*TO_RADIAN, aspect_ratio, 1.0f, 10000.0f);

	//for (short int i = 0; i < NUMBER_OF_CAMERAS; ++i)
	//	camera[i].aspect_ratio = (float)23/16;	// for the time being...
	/*
	camera[0].aspect_ratio = 0.941738f;
	viewport[0].x = (int)(0.36f*width); viewport[0].y = (int)(0.0225 * height);
	viewport[0].w = (int)(0.625f*width); viewport[0].h = (int)(0.955f * height);
	Cam_ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
	Cam_ViewProjectionMatrix[0] = Cam_ProjectionMatrix[0] * Cam_ViewMatrix[0];
	*/

	camera[1].aspect_ratio = (float)23 / 16;
	viewport[1].x = viewport[1].y = 0;
	viewport[1].w = (int)(0.345f*width); viewport[1].h = (int)(0.36*height);
	//Cam_ProjectionMatrix[1] = glm::perspective(camera[1].fov_y*TO_RADIAN, camera[1].aspect_ratio, camera[1].near_clip, camera[1].far_clip);
	Cam_ProjectionMatrix[1] = glm::ortho(-115.0f, 115.0f, -80.0f, 80.0f, 5.0f, 1000.0f);

	camera[2].aspect_ratio = (float)23 / 5;
	//viewport[2].x = (int)(0.36f*width); viewport[2].y = (int)(0.3825f*height);
	viewport[2].x = 0; viewport[2].y = (int)(0.3825f*height);
	viewport[2].w = (int)(0.345f*width); viewport[2].h = (int)(0.1125f*height);
	//Cam_ProjectionMatrix[2] = glm::perspective(camera[2].fov_y*TO_RADIAN, camera[2].aspect_ratio, camera[2].near_clip, camera[2].far_clip);
	Cam_ProjectionMatrix[2] = glm::ortho(-115.0f, 115.0f, -25.0f, 25.0f, 5.0f, 1000.0f);
	/*
	Cam_ProjectionMatrix[2] = glm::mat4(1.0f);
	Cam_ProjectionMatrix[2][2][2] = 0.0f;
	Cam_ProjectionMatrix[2][3][2] = 10.0f;
	Cam_ViewProjectionMatrix[2] = Cam_ProjectionMatrix[2] * Cam_ViewMatrix[2];
	*/

	camera[3].aspect_ratio = (float)16 / 5;
	viewport[3].x = 0.015*width; viewport[3].y = (int)(0.5175f*height);
	//viewport[3].x = 0; viewport[3].y = (int)(0.495f*height);
	viewport[3].w = (int)(0.24f*width); viewport[3].h = (int)(0.1125f*height);
	Cam_ProjectionMatrix[3] = glm::ortho(-80.0f, 80.0f, -25.0f, 25.0f, 5.0f, 1000.0f);

	camera[4].aspect_ratio = 1.489209;
	//viewport[4].x = 0; viewport[4].y = (int)(0.64125f*height);
	//viewport[4].x = 0; viewport[4].y = (int)(0.63f*height);
	viewport[4].x = 0; viewport[4].y = (int)(0.6525f*height);
	viewport[4].w = (int)(0.345f*width); viewport[4].h = (int)(0.325f*height);
	Cam_ProjectionMatrix[4] = glm::perspective(camera[4].fov_y*TO_RADIAN, camera[4].aspect_ratio, camera[4].near_clip, camera[4].far_clip);
	Cam_ViewProjectionMatrix[4] = Cam_ProjectionMatrix[4] * Cam_ViewMatrix[4];

	camera[5].aspect_ratio = 0.941738f;
	viewport[5].x = (int)(0.36f*width); viewport[5].y = (int)(0.0225 * height);
	viewport[5].w = (int)(0.625f*width); viewport[5].h = (int)(0.955f * height);
	Cam_ProjectionMatrix[5] = glm::perspective(camera[5].fov_y*TO_RADIAN, camera[5].aspect_ratio, camera[5].near_clip, camera[5].far_clip);
	Cam_ViewProjectionMatrix[5] = Cam_ProjectionMatrix[5] * Cam_ViewMatrix[5];

	camera[6].aspect_ratio = 0.941738f;
	viewport[6].x = (int)(0.36f*width); viewport[6].y = (int)(0.0225 * height);
	viewport[6].w = (int)(0.625f*width); viewport[6].h = (int)(0.955f * height);
	Cam_ProjectionMatrix[6] = glm::perspective(camera[6].fov_y*TO_RADIAN, camera[6].aspect_ratio, camera[6].near_clip, camera[6].far_clip);
	Cam_ViewProjectionMatrix[6] = Cam_ProjectionMatrix[6] * Cam_ViewMatrix[6];
	/*
	camera[6].aspect_ratio = 1.689209;
	viewport[6].x = 0; viewport[6].y = (int)(0.6525f*height);
	viewport[6].w = (int)(0.345f*width); viewport[6].h = (int)(0.325f*height);
	Cam_ProjectionMatrix[6] = glm::perspective(camera[6].fov_y*TO_RADIAN, camera[6].aspect_ratio, camera[6].near_clip, camera[6].far_clip);
	Cam_ViewProjectionMatrix[6] = Cam_ProjectionMatrix[6] * Cam_ViewMatrix[6];
	*/
	camera[7].aspect_ratio = 0.941738f;
	viewport[7].x = (int)(0.36f*width); viewport[7].y = (int)(0.0225 * height);
	viewport[7].w = (int)(0.625f*width); viewport[7].h = (int)(0.955f * height);
	Cam_ProjectionMatrix[7] = glm::perspective(camera[7].fov_y*TO_RADIAN, camera[7].aspect_ratio, camera[7].near_clip, camera[7].far_clip);
	Cam_ViewProjectionMatrix[7] = Cam_ProjectionMatrix[7] * Cam_ViewMatrix[7];


	for (short int i = 1; i < 3; ++i)
		for (short int j = 0; j < 4; ++j) {
			for (short int k = 0; k < 4; ++k)
				printf("%.2f\t", Cam_ProjectionMatrix[i][k][j]);
			putchar('\n');
		}

	glutPostRedisplay();
}

void register_callbacks(void) {
	cc.left_button_status = GLUT_UP;
	cc.rotation_mode_cow = 1;
	cc.timestamp_cow = 0;
	cc.rotation_angle_cow = 0.0f;

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup_OpenGL_stuffs);
}

void initialize_camera(void) {
	// called only once when the OpenGL system is initialized!!!
	// only ViewMatrix[*] are set up in this function.
	// ProjectionMatrix will be initialized in the reshape callback function when the window pops up.

	//Camera 0
	camera[0].type = LOOK_AT;
	camera[0].prp = glm::vec3(-700.0f, 250.0f, 600.0f);
	camera[0].vrp = glm::vec3(125.0f, 80.0f, 25.0f);
	camera[0].vup = glm::vec3(0.0f, 0.0f, 1.0f);

	camera[0].move_status = 0;
	camera[0].fov_y = 15.0f;
	camera[0].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[0].near_clip = 0.001f;
	camera[0].far_clip = 10000.0f;

	camera[0].trans_speed = 1.0f;	// speed
	camera[0].rot_speed = 0.3f;
	camera[0].key_magnitude = 10.0f;

	//Camera 1
	camera[1].type = CCTV;
	camera[1].prp = glm::vec3(115.0f, 80.0f, 650.0f);
	camera[1].vrp = glm::vec3(115.0f, 80.0f, 0.0f);
	camera[1].vup = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[1].move_status = 0;
	camera[1].fov_y = 15.0f;
	camera[1].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[1].near_clip = 0.01f;
	camera[1].far_clip = 10000.0f;

	camera[1].trans_speed = 0.025f;	// speed
	camera[1].rot_speed = 0.3f;
	camera[1].key_magnitude = 10.0f;

	// Camera 2
	camera[2].type = LOOK_AT;
	camera[2].prp = glm::vec3(115.0f, -650.0f, 25.0f);
	camera[2].vrp = glm::vec3(115.0f, 0.0f, 25.0f);
	camera[2].vup = glm::vec3(0.0f, 0.0f, 1.0f);

	camera[2].move_status = 0;
	camera[2].fov_y = 15.0f;
	camera[2].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[2].near_clip = 0.01f;
	camera[2].far_clip = 10000.0f;

	camera[2].trans_speed = 0.025f;	// speed
	camera[2].rot_speed = 0.3f;
	camera[2].key_magnitude = 10.0f;

	// Camera 3
	camera[3].type = LOOK_AT;
	camera[3].prp = glm::vec3(-650.0f, 80.0f, 25.0f);
	camera[3].vrp = glm::vec3(0.0f, 80.0f, 25.0f);
	camera[3].vup = glm::vec3(0.0f, 0.0f, 1.0f);

	camera[3].move_status = 0;
	camera[3].fov_y = 15.0f;
	camera[3].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[3].near_clip = 0.01f;
	camera[3].far_clip = 10000.0f;

	camera[3].trans_speed = 0.025f;	// speed
	camera[3].rot_speed = 0.3f;
	camera[3].key_magnitude = 10.0f;

	//Camera 4
	camera[4].type = LOOK_AT;
	camera[4].prp = glm::vec3(115.0f, 84.0f, 650.0f);
	camera[4].vrp = glm::vec3(115.0f, 84.0f, 0.0f);
	camera[4].vup = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[4].move_status = 0;
	camera[4].fov_y = 15.0f;
	camera[4].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[4].near_clip = 0.01f;
	camera[4].far_clip = 10000.0f;

	camera[4].trans_speed = 1.0f;	// speed
	camera[4].rot_speed = 0.3f;
	camera[4].key_magnitude = 10.0f;

	//Camera 5
	camera[5].type = LOOK_AT;
	camera[5].prp = glm::vec3(-700.0f, 250.0f, 600.0f);
	camera[5].vrp = glm::vec3(125.0f, 80.0f, 25.0f);
	camera[5].vup = glm::vec3(0.0f, 0.0f, 1.0f);

	camera[5].move_status = 0;
	camera[5].fov_y = 15.0f;
	camera[5].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[5].near_clip = 3.0f;
	camera[5].far_clip = 95.0f;

	camera[5].trans_speed = 1.0f;	// speed
	camera[5].rot_speed = 0.3f;
	camera[5].key_magnitude = 10.0f;

	//Camera 6
	camera[6].type = LOOK_AT;
	camera[6].prp = glm::vec3(-345.0f, -240.0f, 25.0f);
	camera[6].vrp = glm::vec3(230.0f, 160.0f, 25.0f);
	camera[6].vup = glm::vec3(0.0f, 0.0f, 1.0f);

	camera[6].move_status = 0;
	camera[6].fov_y = 15.0f;
	camera[6].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[6].near_clip = 0.01f;
	camera[6].far_clip = 10000.0f;

	camera[6].trans_speed = 1.0f;	// speed
	camera[6].rot_speed = 0.3f;
	camera[6].key_magnitude = 10.0f;


	//Camera 7
	camera[7].type = LOOK_AT;
	camera[7].prp = glm::vec3(200.0f, 200.0f, 200.0f);
	camera[7].vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera[7].vup = glm::vec3(0.0f, 0.0f, 1.0f);

	camera[7].move_status = 0;
	camera[7].fov_y = 15.0f;
	camera[7].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[7].near_clip = 0.001f;
	camera[7].far_clip = 10000.0f;

	camera[7].trans_speed = 1.0f;	// speed
	camera[7].rot_speed = 0.3f;
	camera[7].key_magnitude = 10.0f;

	for (short int i = 0; i < NUMBER_OF_CAMERAS; ++i) {
		if (camera[i].type == LOOK_AT) {
			glm::mat4 tempViewMatrix;

			tempViewMatrix = lookAt(camera[i].prp, camera[i].vrp, camera[i].vup);

			camera[i].uaxis.x = tempViewMatrix[0].x;
			camera[i].vaxis.x = tempViewMatrix[0].y;
			camera[i].naxis.x = tempViewMatrix[0].z;

			camera[i].uaxis.y = tempViewMatrix[1].x;
			camera[i].vaxis.y = tempViewMatrix[1].y;
			camera[i].naxis.y = tempViewMatrix[1].z;

			camera[i].uaxis.z = tempViewMatrix[2].x;
			camera[i].vaxis.z = tempViewMatrix[2].y;
			camera[i].naxis.z = tempViewMatrix[2].z;

			camera[i].pos.x = camera[i].prp.x;
			camera[i].pos.y = camera[i].prp.y;
			camera[i].pos.z = camera[i].prp.z;
		}
		set_ViewMatrix_from_camera_frame(i);
	}
	
	camera_selected = 6;
	camera_type = CAMERA_WORLD_VIEWER;
	shader_type = 0;
	mouse_right = 0;
	glow_tiger = 0;
	walls = 1;
}

void initialize_OpenGL(void) {

	glEnable(GL_DEPTH_TEST); // Default state
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glClearColor(0.15f, 0.2f, 0.22f, 0.0f);

	initialize_camera();

	initialize_lights_and_material();
}


void set_up_scene_lights(void) {

	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 10.0f; 	// point light position in EC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
	light[0].ambient_color[2] = 0.3f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.7f; light[0].diffuse_color[1] = 0.7f;
	light[0].diffuse_color[2] = 0.7f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 0.9f; light[0].specular_color[1] = 0.9f;
	light[0].specular_color[2] = 0.9f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = 230.0f; light[1].position[1] = 160.0f; // spot light position in WC
	light[1].position[2] = 50.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.2f; light[1].ambient_color[1] = 0.2f;
	light[1].ambient_color[2] = 0.82f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.2f; light[1].diffuse_color[1] = 0.2f;
	light[1].diffuse_color[2] = 0.82f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.2f; light[1].specular_color[1] = 0.2f;
	light[1].specular_color[2] = 0.82f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = 0.0f; // spot light direction in WC
	light[1].spot_direction[2] = -1.0f;
	light[1].spot_cutoff_angle = 90.0f;
	light[1].spot_exponent = 27.0f;

	// spot_light_MC: use light 2
	light[2].light_on = 1;
	light[2].position[0] = 0.0f; light[2].position[1] = 0.0f; // spot light position in WC
	light[2].position[2] = 50.0f; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 0.2f; light[2].ambient_color[1] = 0.02f;
	light[2].ambient_color[2] = 0.02f; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 0.82f; light[2].diffuse_color[1] = 0.02f;	// red 조명 2
	light[2].diffuse_color[2] = 0.082f; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 0.82f; light[2].specular_color[1] = 0.082f;	// red 조명 2
	light[2].specular_color[2] = 0.082f; light[2].specular_color[3] = 1.0f;

	light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = 0.0f; // spot light direction in WC
	light[2].spot_direction[2] = -1.0f;

	light[2].spot_cutoff_angle = 90.0f;
	light[2].spot_exponent = 1.0f;

	glUseProgram(h_ShaderProgram_PS);
	glUniform1i(loc_freq, freq);
	glUniform1i(loc_light_PS[0].light_on, light[0].light_on);
	glUniform4fv(loc_light_PS[0].position, 1, light[0].position);
	glUniform4fv(loc_light_PS[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light_PS[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light_PS[0].specular_color, 1, light[0].specular_color);

	glUniform1i(loc_light_PS[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light_PS[1].position, 1, &position_EC[0]);
	glUniform4fv(loc_light_PS[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light_PS[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light_PS[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);
	glUniform3fv(loc_light_PS[1].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light_PS[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light_PS[1].spot_exponent, light[1].spot_exponent);


	glUniform1i(loc_light_PS[2].light_on, light[2].light_on);
	//	glUniform4fv(loc_light_PS[2].position, 1, &position_EC[0]);	// 안돼 지금! cpu->gpu
	glUniform4fv(loc_light_PS[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light_PS[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light_PS[2].specular_color, 1, light[2].specular_color);

	//	glUniform3fv(loc_light_PS[2].spot_direction, 1, &direction_EC[0]);	// 안돼 지금! cpu->gpu
	glUniform1f(loc_light_PS[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light_PS[2].spot_exponent, light[2].spot_exponent);



	glUseProgram(h_ShaderProgram_GS);
	glUniform1i(loc_light_GS[0].light_on, light[0].light_on);
	glUniform4fv(loc_light_GS[0].position, 1, light[0].position);
	glUniform4fv(loc_light_GS[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light_GS[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light_GS[0].specular_color, 1, light[0].specular_color);

	glUniform1i(loc_light_GS[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light_GS[1].position, 1, &position_EC[0]);
	glUniform4fv(loc_light_GS[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light_GS[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light_GS[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);
	glUniform3fv(loc_light_GS[1].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light_GS[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light_GS[1].spot_exponent, light[1].spot_exponent);

	glUniform1i(loc_light_GS[2].light_on, light[2].light_on);
	//	glUniform4fv(loc_light_GS[2].position, 1, &position_EC[0]);	// 안돼 지금! cpu->gpu
	glUniform4fv(loc_light_GS[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light_GS[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light_GS[2].specular_color, 1, light[2].specular_color);

	//	glUniform3fv(loc_light_GS[2].spot_direction, 1, &direction_EC[0]);	// 안돼 지금! cpu->gpu
	glUniform1f(loc_light_GS[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light_GS[2].spot_exponent, light[2].spot_exponent);

	glUseProgram(0);
}


void prepare_scene(void) {
	define_axes();
	define_static_objects();
	define_animated_tiger();
	prepare_tiger();

	char bodyTri[50] = "Data/car_body_triangles_v.txt";
	char carWheelTri[50] = "Data/car_wheel_triangles_v.txt";
	char carNutTri[50] = "Data/car_nut_triangles_v.txt";

	prepare_geom_obj(GEOM_OBJ_ID_CAR_BODY, bodyTri, GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_WHEEL, carWheelTri, GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_NUT, carNutTri, GEOM_OBJ_TYPE_V);
	
	prepare_points();
	prepare_square();
	prepare_tiger();
	prepare_cow();

	prepare_rectangle();
	set_up_scene_lights();
	initialize_screen();
	initialize_blind();
	initialize_cartoon();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[128] = "Sogang CSE4170 (4.5.5) Simple Camera Transformation - TRANSLATION/ROTATION";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: '0', '1', 'c', 'ESC'",
		"    - Mouse used: Left Butten Click and Move"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
