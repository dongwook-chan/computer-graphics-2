glm::mat4 carMoveModelMatrix;
float carMove;

#define HOUSE_MAX 3

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

#define N_GEOMETRY_OBJECTS 6
#define GEOM_OBJ_ID_CAR_BODY 0
#define GEOM_OBJ_ID_CAR_WHEEL 1
#define GEOM_OBJ_ID_CAR_NUT 2
#define GEOM_OBJ_ID_COW 3
#define GEOM_OBJ_ID_TEAPOT 4
#define GEOM_OBJ_ID_BOX 5

GLuint geom_obj_VBO[N_GEOMETRY_OBJECTS];
GLuint geom_obj_VAO[N_GEOMETRY_OBJECTS];

int geom_obj_n_triangles[N_GEOMETRY_OBJECTS];
GLfloat *geom_obj_vertices[N_GEOMETRY_OBJECTS];

glm::mat4 ModelMatrix_CAR_BODY, ModelMatrix_CAR_WHEEL, ModelMatrix_CAR_NUT, ModelMatrix_CAR_DRIVER;
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER; // computed only once in initialize_camera()

										  // codes for the 'general' triangular-mesh object
typedef enum _GEOM_OBJ_TYPE { GEOM_OBJ_TYPE_V = 0, GEOM_OBJ_TYPE_VN, GEOM_OBJ_TYPE_VNT } GEOM_OBJ_TYPE;
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
int GEOM_OBJ_ELEMENTS_PER_VERTEX[3] = { 3, 6, 8 };

int read_geometry_file(GLfloat **object, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int i, n_triangles;
	float *flt_ptr;
	FILE *fp;

	fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the geometry file %s ...", filename);
		return -1;
	}

	fscanf(fp, "%d", &n_triangles);
	*object = (float *)malloc(3 * n_triangles*GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float));
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	flt_ptr = *object;
	for (i = 0; i < 3 * n_triangles * GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type]; i++)
		fscanf(fp, "%f", flt_ptr++);
	fclose(fp);

	fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);

	return n_triangles;
}

void prepare_geom_obj(int geom_obj_ID, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int n_bytes_per_vertex;

	n_bytes_per_vertex = GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float);
	geom_obj_n_triangles[geom_obj_ID] = read_geometry_file(&geom_obj_vertices[geom_obj_ID], filename, geom_obj_type);

	// Initialize vertex array object.
	glGenVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glGenBuffers(1, &geom_obj_VBO[geom_obj_ID]);
	glBindBuffer(GL_ARRAY_BUFFER, geom_obj_VBO[geom_obj_ID]);
	glBufferData(GL_ARRAY_BUFFER, 3 * geom_obj_n_triangles[geom_obj_ID] * n_bytes_per_vertex,
		geom_obj_vertices[geom_obj_ID], GL_STATIC_DRAW);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	if (geom_obj_type >= GEOM_OBJ_TYPE_VN) {
		glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	if (geom_obj_type >= GEOM_OBJ_TYPE_VNT) {
		glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	free(geom_obj_vertices[geom_obj_ID]);
}

void draw_geom_obj(int geom_obj_ID) {
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * geom_obj_n_triangles[geom_obj_ID]);
	glBindVertexArray(0);
}

void free_geom_obj(int geom_obj_ID) {
	glDeleteVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glDeleteBuffers(1, &geom_obj_VBO[geom_obj_ID]);
}


#define rad 1.7f
#define ww 1.0f
void draw_wheel_and_nut(int cam_index) {
	// angle is used in Hierarchical_Car_Correct later
	int i;
	glm::mat4 ViewProjectionMatrix = Cam_ProjectionMatrix[cam_index] * Cam_ViewMatrix[cam_index];

	glUniform3f(loc_primitive_color, 0.000f, 0.808f, 0.820f); // color name: DarkTurquoise
	draw_geom_obj(GEOM_OBJ_ID_CAR_WHEEL); // draw wheel

	for (i = 0; i < 5; i++) {
		ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*72.0f*i, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_CAR_NUT = glm::translate(ModelMatrix_CAR_NUT, glm::vec3(rad - 0.5f, 0.0f, ww));
		//ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_NUT, TO_RADIAN*carMove / 1.7f*2.0f*3.14159265359f*360.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_NUT;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		glUniform3f(loc_primitive_color, 0.690f, 0.769f, 0.871f); // color name: LightSteelBlue
		draw_geom_obj(GEOM_OBJ_ID_CAR_NUT); // draw i-th nut
	}
}


void draw_car_dummy(int cam_index) {
	glm::mat4 ViewProjectionMatrix = Cam_ProjectionMatrix[cam_index] * Cam_ViewMatrix[cam_index];

	glUniform3f(loc_primitive_color, 0.498f, 1.000f, 0.831f); // color name: Aquamarine
	draw_geom_obj(GEOM_OBJ_ID_CAR_BODY); // draw body

	glLineWidth(2.0f);
	//draw_axes(7); // draw MC axes of body
	glLineWidth(1.0f);

	ModelMatrix_CAR_DRIVER = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_DRIVER = glm::rotate(ModelMatrix_CAR_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_DRIVER;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(5.0f);
	//draw_axes(7); // draw camera frame at driver seat
	glLineWidth(1.0f);

	
	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY,  glm::vec3(-3.9f, -3.5f, 4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*carMove / 1.7f*2.0f*3.14159265359f*360.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 0

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*carMove / 1.7f*2.0f*3.14159265359f*360.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 1

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*carMove / 1.7f*2.0f*3.14159265359f*360.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 2

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*carMove / 1.7f*2.0f*3.14159265359f*360.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 2
}


void display_camera(int cam_index) {

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// should optimize this dispaly function further to reduce the amount of floating-point operations.
	glm::mat4 Matrix_TIGER_tmp;

	glViewport(viewport[cam_index].x, viewport[cam_index].y, viewport[cam_index].w, viewport[cam_index].h);

	// At this point, the matrix ViewProjectionMatrix has been properly set up.

	ModelViewProjectionMatrix = glm::scale(Cam_ViewProjectionMatrix[cam_index], glm::vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//glLineWidth(5.0f);
	//draw_axes(cam_index); // draw the WC axes.
	//glLineWidth(1.0f);	

	switch (cam_index) {
	case 0: case 1: case 2: case 3: case 4: case 5: case 6:
	


		glUseProgram((shader_type==0)? h_ShaderProgram_PS : h_ShaderProgram_GS);
		draw_animated_tiger(cam_index);
		

		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram((shader_type==0)?h_ShaderProgram_PS:h_ShaderProgram_GS);
		set_material_floor();
		ModelViewMatrix = glm::translate(Cam_ViewMatrix[cam_index], glm::vec3(10.0f, 10.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(220.0f, 150.0f, 1.0f));
		//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		//ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		ModelViewProjectionMatrix = Cam_ProjectionMatrix[cam_index] * ModelViewMatrix;

		glUniformMatrix4fv((shader_type == 0) ? loc_ModelViewProjectionMatrix_PS: loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv((shader_type == 0) ? loc_ModelViewMatrix_PS: loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv((shader_type == 0) ? loc_ModelViewMatrixInvTrans_PS: loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_floor();


		ModelViewMatrix = glm::translate(Cam_ViewMatrix[cam_index], glm::vec3(10.0f, 160.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(220.0f, 50.0f, 50.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		//ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		ModelViewProjectionMatrix = Cam_ProjectionMatrix[cam_index] * ModelViewMatrix;

		glUniformMatrix4fv((shader_type == 0) ? loc_ModelViewProjectionMatrix_PS: loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv((shader_type == 0) ? loc_ModelViewMatrix_PS: loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv((shader_type == 0) ? loc_ModelViewMatrixInvTrans_PS: loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_floor();


		ModelViewMatrix = glm::translate(Cam_ViewMatrix[cam_index], glm::vec3(230.0f, 10.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, 150.0f, 50.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		//ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		ModelViewProjectionMatrix = Cam_ProjectionMatrix[cam_index] * ModelViewMatrix;

		glUniformMatrix4fv((shader_type == 0) ? loc_ModelViewProjectionMatrix_PS: loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv((shader_type == 0) ? loc_ModelViewMatrix_PS : loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv((shader_type == 0) ? loc_ModelViewMatrixInvTrans_PS: loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_floor();


		
		if (shader_type == 0 && flag_draw_screen == 1) {
			set_material_screen();
			ModelViewMatrix = glm::translate(Cam_ViewMatrix[cam_index], glm::vec3(10.0f, 10.0f, 49.9f));
			ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(220.0f, 150.0f, 1.0f));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, 0.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

			ModelViewProjectionMatrix = Cam_ProjectionMatrix[cam_index] * ModelViewMatrix;

			glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
			glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

			glUniform1i(loc_screen_effect, flag_screen_effect);
			draw_screen();
			glUniform1i(loc_screen_effect, 0);
		}




		glUseProgram(0);
		glLineWidth(1.0f);
		//draw_axes(cam_index);
		glLineWidth(1.0f);
		glUseProgram((shader_type==0)?h_ShaderProgram_PS:h_ShaderProgram_GS);


		glm::mat4 ModelMatrix_T, ModelMatrix_R;

		if (walls != 0) {
			if (walls == 2) {
				glUniform1i(loc_isBuilding, 1);
				switch (cam_index) {
				case 3:
					glUniform1i(loc_isFront, 1);
				case 2:
					glUniform1i(loc_isSide, 1);
				case 6:
					glUniform1i(loc_isFront, 2);
				}
			}
			
			set_material_static_house(0);
			

			draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_BUILDING]), 0);

			glUniform1i(loc_isFront, 0);
			glUniform1i(loc_isSide, 0);
			glUniform1i(loc_isBuilding, 0);
		}
		glUseProgram((shader_type==0)?h_ShaderProgram_PS:h_ShaderProgram_GS);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		//draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_TABLE]), 0);

		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_LIGHT]), 0);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_LIGHT]), 1);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_LIGHT]), 2);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_LIGHT]), 3);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_LIGHT]), 4);
		

		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(-115.0f, -5.0f, 0.0f));
		set_material_static_house(3);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_TEAPOT]), 0);

		//ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(-115.0f, -10.0f, 0.0f));
		//draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_NEW_CHAIR]), 0);

		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(-115.0f, -10.0f, 0.0f));
		set_material_static_house(1);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_TABLE]), 1);


		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(-35.0f, -75.0f, 0.0f));
		set_material_static_house(3);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_TEAPOT]), 0);

		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(-47.5f, -80.0f, 0.0f));
		set_material_static_house(4);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_NEW_CHAIR]), 0);

		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(-45.0f, -80.0f, 0.0f));
		set_material_static_house(1);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_TABLE]), 1);

		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 47.5f, 0.0f));
		ModelMatrix_T = glm::rotate(ModelMatrix_T, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_T = glm::translate(ModelMatrix_T, glm::vec3(-187.5f, -120.0f, 0.0f));
		set_material_static_house(6);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_NEW_PICTURE]), 0);
		set_material_static_house(5);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_FRAME]), 0);

		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(-82.0f, -68.0f, 0.0f));
		set_material_static_house(7);
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_COW]), 0);
		
		ModelMatrix_T = glm::translate(glm::mat4(1.0f), glm::vec3(23.0f, 40.0f, 0.0f));
		ModelMatrix_T = glm::rotate(ModelMatrix_T, 90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_T = glm::translate(ModelMatrix_T, glm::vec3(-205.0f, -110.0f, 0.0f));
		draw_static_object(ModelMatrix_T, ModelMatrix_R, cam_index, &(static_objects[OBJ_COW]), 0);

		glUseProgram(h_ShaderProgram_simple);

		ModelMatrix_CAR_BODY = glm::translate(glm::mat4(1.0f), glm::vec3((float)carMove+80.0f, 132.0f, 5.0f));
		ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 180.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

		ModelViewProjectionMatrix = Cam_ProjectionMatrix[cam_index] * Cam_ViewMatrix[cam_index] * ModelMatrix_CAR_BODY;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car_dummy(cam_index);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		break;
		
	}
	//	break;
	//}
}