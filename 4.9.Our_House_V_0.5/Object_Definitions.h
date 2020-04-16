
// The object modelling tasks performed by this file are usually done 
// by reading a scene configuration file or through a help of graphics user interface!!!

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

typedef struct _material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
} Material;

#define N_MAX_GEOM_COPIES 5
typedef struct _Object {
	char filename[512];

	GLenum front_face_mode; // clockwise or counter-clockwise
	int n_triangles;

	int n_fields; // 3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	GLfloat *vertices; // pointer to vertex array data
	GLfloat xmin, xmax, ymin, ymax, zmin, zmax; // bounding box <- compute this yourself

	GLuint VBO, VAO; // Handles to vertex buffer object and vertex array object

	int n_geom_instances;
	glm::mat4 ModelMatrix[N_MAX_GEOM_COPIES];
	Material material[N_MAX_GEOM_COPIES];
} Object;

#define N_MAX_STATIC_OBJECTS		10
Object static_objects[N_MAX_STATIC_OBJECTS]; // allocage memory dynamically every time it is needed rather than using a static array
int n_static_objects = 0;

#define OBJ_BUILDING		0
#define OBJ_TABLE			1
#define OBJ_LIGHT			2
#define OBJ_TEAPOT			3
#define OBJ_NEW_CHAIR		4
#define OBJ_FRAME			5
#define OBJ_NEW_PICTURE		6
#define OBJ_COW				7

int read_geometry(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(*object, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void compute_AABB(Object *obj_ptr) {
	// Do it yourself.
}
	 
void prepare_geom_of_static_object(Object *obj_ptr) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = obj_ptr->n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	obj_ptr->n_triangles = read_geometry(&(obj_ptr->vertices), n_bytes_per_triangle, obj_ptr->filename);

	// Initialize vertex buffer object.
	glGenBuffers(1, &(obj_ptr->VBO));

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glBufferData(GL_ARRAY_BUFFER, obj_ptr->n_triangles*n_bytes_per_triangle, obj_ptr->vertices, GL_STATIC_DRAW);

	compute_AABB(obj_ptr);
	free(obj_ptr->vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &(obj_ptr->VAO));
	glBindVertexArray(obj_ptr->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void define_static_objects(void) {
	// building
	strcpy(static_objects[OBJ_BUILDING].filename, "Data/Building1_vnt.geom");
	static_objects[OBJ_BUILDING].n_fields = 8;

	static_objects[OBJ_BUILDING].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUILDING]));

	static_objects[OBJ_BUILDING].n_geom_instances = 1;

    static_objects[OBJ_BUILDING].ModelMatrix[0] = glm::mat4(1.0f);
	
	static_objects[OBJ_BUILDING].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].exponent = 128.0f*0.1f;

	// table
	strcpy(static_objects[OBJ_TABLE].filename, "Data/Table_vn.geom");
	static_objects[OBJ_TABLE].n_fields = 6;

	static_objects[OBJ_TABLE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TABLE]));

	static_objects[OBJ_TABLE].n_geom_instances = 2;

	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[0], 
		glm::vec3(0.5f, 0.5f, 0.5f));

	static_objects[OBJ_TABLE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[0].ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	static_objects[OBJ_TABLE].material[0].diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	static_objects[OBJ_TABLE].material[0].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[0].exponent = 15.0f;

	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[1],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[1].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TABLE].material[1].diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	static_objects[OBJ_TABLE].material[1].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[1].exponent = 128.0f*0.078125f;

	// Light
	strcpy(static_objects[OBJ_LIGHT].filename, "Data/Light_vn.geom");
	static_objects[OBJ_LIGHT].n_fields = 6;

	static_objects[OBJ_LIGHT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(static_objects + OBJ_LIGHT);

	static_objects[OBJ_LIGHT].n_geom_instances = 5;

	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[3],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[3].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[4],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[4].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].exponent = 128.0f*0.4f;

	// teapot
	strcpy(static_objects[OBJ_TEAPOT].filename, "Data/Teapotn_vn.geom");
	static_objects[OBJ_TEAPOT].n_fields = 6;

	static_objects[OBJ_TEAPOT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TEAPOT]));

	static_objects[OBJ_TEAPOT].n_geom_instances = 1;

	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[0],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].exponent = 128.0f*0.6;

	// new_chair
	strcpy(static_objects[OBJ_NEW_CHAIR].filename, "Data/new_chair_vnt.geom");
	static_objects[OBJ_NEW_CHAIR].n_fields = 8;

	static_objects[OBJ_NEW_CHAIR].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_CHAIR]));

	static_objects[OBJ_NEW_CHAIR].n_geom_instances = 1;

	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].exponent = 128.0f*0.078125f;

	// frame
	strcpy(static_objects[OBJ_FRAME].filename, "Data/Frame_vn.geom");
	static_objects[OBJ_FRAME].n_fields = 6;

	static_objects[OBJ_FRAME].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_FRAME]));

	static_objects[OBJ_FRAME].n_geom_instances = 1;

	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[0],
		glm::vec3(0.6f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_FRAME].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[0].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[0].diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[0].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[0].exponent = 128.0f*0.1f;


	// new_picture
	strcpy(static_objects[OBJ_NEW_PICTURE].filename, "Data/new_picture_vnt.geom");
	static_objects[OBJ_NEW_PICTURE].n_fields = 8;

	static_objects[OBJ_NEW_PICTURE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_PICTURE]));

	static_objects[OBJ_NEW_PICTURE].n_geom_instances = 1;

	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		glm::vec3(13.5f*0.6f, 13.5f*0.6f, 13.5f*0.6f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_NEW_PICTURE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].exponent = 128.0f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_COW].filename, "Data/cow_vn.geom");
	static_objects[OBJ_COW].n_fields = 6;

	static_objects[OBJ_COW].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_COW]));

	static_objects[OBJ_COW].n_geom_instances = 1;

	static_objects[OBJ_COW].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 100.0f, 9.5f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::scale(static_objects[OBJ_COW].ModelMatrix[0],
		glm::vec3(30.0f, 30.0f, 30.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
 
	static_objects[OBJ_COW].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_COW].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_COW].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_COW].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_COW].material[0].exponent = 0.21794872f*0.6f;

	n_static_objects = 8;
}

void draw_static_object(glm::mat4 ModelMatrix_T, glm::mat4 ModelMatrix_R, int camera_selected, Object *obj_ptr, int instance_ID) {
	glFrontFace(obj_ptr->front_face_mode);

	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix:Cam_ViewMatrix[camera_selected]) * ModelMatrix_T *obj_ptr->ModelMatrix[instance_ID] * ModelMatrix_R;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix:Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::mat3(glm::affineInverse(ModelViewMatrix));

	glUniformMatrix4fv((shader_type==0)?loc_ModelViewProjectionMatrix_PS: loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv((shader_type==0)?loc_ModelViewMatrix_PS: loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv((shader_type == 0) ? loc_ModelViewMatrixInvTrans_PS: loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);
}

GLuint VBO_axes, VAO_axes;
GLfloat vertices_axes[6][3] = {
	{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

void define_axes(void) {  
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define WC_AXIS_LENGTH		60.0f
void draw_axes(int camera_selected) {
	ModelViewMatrix = glm::scale(((camera_selected == 5)?ViewMatrix: Cam_ViewMatrix[camera_selected]), glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	ModelViewProjectionMatrix = ((camera_selected == 5)? ProjectionMatrix:Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}


// tiger object
#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat *tiger_vertices[N_TIGER_FRAMES];

Object tiger[N_TIGER_FRAMES];
struct {
	int cur_frame = 0;
	float rotation_angle = 0.0f;
} tiger_data;

glm::mat4 Tiger_ModelMatrix;
glm::mat4 Tiger_Movement_ModelMatrix;

Material_Parameters material_tiger;

 
void define_animated_tiger(void) {
	int time_interval = 20;
	int time = cc.timestamp_cow % time_interval;
	int add = 0;


	
	for (int i = 0; i < N_TIGER_FRAMES; i++) {

		if (glow_tiger == 1) {
			if (0 <= time && time < time_interval / 2)
				add = time;
			else
				add = time_interval - time;
			add = (add * 2.0f) / (time_interval * 3.0f);
		}

		sprintf(tiger[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);

		tiger[i].n_fields = 8;
		tiger[i].front_face_mode = GL_CW;
		prepare_geom_of_static_object(&(tiger[i]));

		tiger[i].n_geom_instances = 1;

		tiger[i].ModelMatrix[0] = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

		tiger[i].material[0].emission = glm::vec4(0.1f, 0.1f, 0.0f, 1.0f);
		tiger[i].material[0].ambient = glm::vec4(0.24725f, 0.1995f + add, 0.0745f, 1.0f);
		tiger[i].material[0].diffuse = glm::vec4(0.75164f, 0.60648f + add, 0.60648f, 1.0f);
		tiger[i].material[0].specular = glm::vec4(0.628281f, 0.555802f + add, 0.366065f, 1.0f);
		tiger[i].material[0].exponent = 51.2f;
	}
}


void prepare_tiger(void) { // vertices enumerated clockwise
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_geometry(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &tiger_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles*n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(LOC_POSITION, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_tiger.ambient_color[0] = 0.24725f; material_tiger.ambient_color[1] = 0.1995f;
	material_tiger.ambient_color[2] = 0.0745f; material_tiger.ambient_color[3] = 1.0f;

	material_tiger.diffuse_color[0] = 0.75164f; material_tiger.diffuse_color[1] = 0.60648f;
	material_tiger.diffuse_color[2] = 0.22648f; material_tiger.diffuse_color[3] = 1.0f;

	material_tiger.specular_color[0] = 0.628281f; material_tiger.specular_color[1] = 0.555802f;
	material_tiger.specular_color[2] = 0.366065f; material_tiger.specular_color[3] = 1.0f;

	material_tiger.specular_exponent = 51.2f;

	material_tiger.emissive_color[0] = 0.1f; material_tiger.emissive_color[1] = 0.1f;
	material_tiger.emissive_color[2] = 0.0f; material_tiger.emissive_color[3] = 1.0f;
}

void set_material_tiger(void) {
	// assume ShaderProgram_PS is used
	glUniform4fv(((shader_type==0)?loc_material_PS:loc_material_GS).ambient_color, 1, material_tiger.ambient_color);
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).diffuse_color, 1, material_tiger.diffuse_color);
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).specular_color, 1, material_tiger.specular_color);
	glUniform1f(((shader_type == 0) ? loc_material_PS : loc_material_GS).specular_exponent, material_tiger.specular_exponent);
	glUniform4fv(((shader_type==0)?loc_material_PS:loc_material_GS).emissive_color, 1, material_tiger.emissive_color);
}

void set_material_tiger_house(void) {
	// assume ShaderProgram_PS is used
	int i;
	for (i = 0; i < 4; ++i)
		material_tiger.ambient_color[i] = tiger[tiger_data.cur_frame].material[0].ambient[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).ambient_color, 1, material_tiger.ambient_color);
	
	for (i = 0; i < 4; ++i)
		material_tiger.diffuse_color[i] = tiger[tiger_data.cur_frame].material[0].diffuse[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).diffuse_color, 1, material_tiger.diffuse_color);

	for (i = 0; i < 4; ++i)
		material_tiger.specular_color[i] = tiger[tiger_data.cur_frame].material[0].specular[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).specular_color, 1, material_tiger.specular_color);
	
	material_tiger.specular_exponent = tiger[tiger_data.cur_frame].material[0].exponent;
	glUniform1f(((shader_type == 0) ? loc_material_PS : loc_material_GS).specular_exponent, material_tiger.specular_exponent);

	for (i = 0; i < 4; ++i)
		material_tiger.emissive_color[i] = tiger[tiger_data.cur_frame].material[0].emission[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).emissive_color, 1, material_tiger.emissive_color);
}


void set_material_static_house(int key) {
	// assume ShaderProgram_PS is used
	int i;
	for (i = 0; i < 4; ++i)
		material_tiger.ambient_color[i] = static_objects[key].material[0].ambient[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).ambient_color, 1, material_tiger.ambient_color);

	for (i = 0; i < 4; ++i)
		material_tiger.diffuse_color[i] = static_objects[key].material[0].diffuse[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).diffuse_color, 1, material_tiger.diffuse_color);

	for (i = 0; i < 4; ++i)
		material_tiger.specular_color[i] = static_objects[key].material[0].specular[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).specular_color, 1, material_tiger.specular_color);

	material_tiger.specular_exponent = tiger[tiger_data.cur_frame].material[0].exponent;
	glUniform1f(((shader_type == 0) ? loc_material_PS : loc_material_GS).specular_exponent, material_tiger.specular_exponent);

	for (i = 0; i < 4; ++i)
		material_tiger.emissive_color[i] = static_objects[key].material[0].emission[i];
	glUniform4fv(((shader_type == 0) ? loc_material_PS : loc_material_GS).emissive_color, 1, material_tiger.emissive_color);
}



void draw_tiger(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);
}


#define MC_EC_LEN 15.0f
//#define EYEBALL_Y -86.0f
//#define EYEBALL_Z 60.0f
#define EYEBALL_Y -18.4f
#define EYEBALL_Z 11.36f


void draw_tiger_MC(int camera_selected) {
	glm::mat4 ModelMatrix;

	glLineWidth(1.0f);

	ModelMatrix = glm::scale(Tiger_Movement_ModelMatrix, glm::vec3(MC_EC_LEN, MC_EC_LEN, MC_EC_LEN));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);

	glLineWidth(1.0f);
}

void draw_tiger_EC(int camera_selected) {
	glm::mat4 ModelMatrix;

	glLineWidth(1.0f);

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, EYEBALL_Y, EYEBALL_Z));
	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -18.4f, 11.36f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(MC_EC_LEN, MC_EC_LEN, MC_EC_LEN));
	ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN*180.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN*90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);	// 빨간색 v, x
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);	// 초록색 u, y
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);	// 파란색 n, z
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);

	glLineWidth(1.0f);
}

void draw_tiger_ViewVolume(int camera_selected) {
	switch (camera_selected) {
	case 0: case 5:
		return;
	}

	if (camera_type != CAMERA_TIGER)
		return;

	glm::mat4 ModelMatrix;
	glm::mat4 ModelViewProjectionMatrix_temp;
	float len = ((camera_type == CAMERA_WORLD_VIEWER) ? camera[0].far_clip/cosh(TO_RADIAN*camera[0].fov_y/2.0f) : camera[5].far_clip/ cosh(TO_RADIAN*camera[5].fov_y/2.0f));// * 5.0f; //@

	switch (camera_type) {
	case CAMERA_WORLD_VIEWER:

		break;
	case CAMERA_TIGER:
		ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, EYEBALL_Y, EYEBALL_Z));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(len, len, len));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * 180.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	}
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix_temp = ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;


	glLineWidth(1.0f);

	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix_temp, TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 0.0f, 1.0f));	// 위로
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 1.0f, 0.0f));		// 오른쪽으로

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix_temp, -TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 0.0f, 1.0f));	// 아래로
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 1.0f, 0.0f));		// 오른쪽으로

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix_temp, -TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 0.0f, 1.0f));	// 아래로
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 1.0f, 0.0f));		// 왼쪽으로

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix_temp, TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 0.0f, 1.0f));	// 위로
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, TO_RADIAN * camera[camera_selected].fov_y / 2, glm::vec3(0.0f, 1.0f, 0.0f));		// 왼쪽으로

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);



	glLineWidth(1.0f);

	glm::mat4 ModelMatrix_temp;
	float semi_diameter = ((camera_type == CAMERA_WORLD_VIEWER) ? camera[0].far_clip * tanh(TO_RADIAN*camera[0].fov_y / 2.0f) : camera[5].far_clip * tanh(TO_RADIAN*camera[5].fov_y / 2.0f));
	len = semi_diameter * 2.0f;
	float far_clip = ((camera_type == CAMERA_WORLD_VIEWER) ? camera[0].far_clip: camera[5].far_clip);// * 5.0f; //@

	switch (camera_type) {
	case CAMERA_WORLD_VIEWER:
	case CAMERA_TIGER:
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, EYEBALL_Y, EYEBALL_Z));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(len, len, len));
		ModelMatrix_temp = ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * 180.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -far_clip, semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, TO_RADIAN * 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -far_clip, -semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, TO_RADIAN * 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-len, -far_clip, -semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, -TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -far_clip, -semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, -TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);



	glLineWidth(5.0f);

	semi_diameter = ((camera_type == CAMERA_WORLD_VIEWER) ? camera[0].near_clip * tanh(TO_RADIAN*camera[0].fov_y / 2.0f) : camera[5].near_clip * tanh(TO_RADIAN*camera[5].fov_y / 2.0f));
	len = semi_diameter * 2.0f;
	float near_clip = ((camera_type == CAMERA_WORLD_VIEWER) ? camera[0].near_clip : camera[5].near_clip);// * 5.0f; //@

	switch (camera_type) {
	case CAMERA_WORLD_VIEWER:
	case CAMERA_TIGER:
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, EYEBALL_Y, EYEBALL_Z));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(len, len, len));
		ModelMatrix_temp = ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * 180.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -near_clip, semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, TO_RADIAN * 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -near_clip, -semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, TO_RADIAN * 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-len, -near_clip, -semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, -TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);


	ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(semi_diameter, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -near_clip, -semi_diameter));
	ModelMatrix *= glm::rotate(ModelMatrix_temp, -TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = ((camera_selected == 5) ? ViewMatrix : Cam_ViewMatrix[camera_selected])*ModelMatrix;
	ModelViewProjectionMatrix = ((camera_selected == 5) ? ProjectionMatrix : Cam_ProjectionMatrix[camera_selected]) * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);



	glLineWidth(1.0f);
}

// 기준 속도: 1.74532925199

// 0초 ~ 12초
// 13초에 도착점, course2의 시작점 -> course2의 시작점으로 표시
void course1(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 37.5, 22.5
	// 도착점: 60, 22.5
	// 궤적의 길이: 22.5(약 12.8915503905초 소요->13초 걸리도록 속도 변경)
	// 수정된 속도: 1.73076923077

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(37.5f + adjusted_time * 1.73076923077f, 22.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));		// 진행 방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

void course2(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 60, 22.5
	// 도착점: 77.5, 40
	// 반지름의 길이: 17.5
	// 궤적의 길이	: 27.4889357189(약 15.75 -> 16초 걸리도록 속도 변경)
	// 수정된 속도	: 1.71805848243
	// 1초당 각도	: 5.6245

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 22.5f, 0.0f));		// 시작 위치로
	//Tiger_Movement_ModelMatrix = glm::scale(Tiger_Movement_ModelMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));			// 반대방향으로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 17.5f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 5.6245f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, - 17.5f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course3(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 77.5, 40
	// 도착점: 77.5, 55
	// 궤적의 길이: 15(약 8.59436692698초 소요->9초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667
	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(77.5f, 40.0f + adjusted_time * 1.66666666667f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course4(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 77.5, 55
	// 도착점: 45, 87.5
	// 반지름의 길이: 32.5
	// 궤적의 길이	: 51.0508806208(약 29.25 -> 29초 걸리도록 속도 변경)
	// 2*pi*r/4		:              (/기준속도 ...
	// 수정된 속도	: 1.76037519382
	// 1초당 각도	: 3.10344827586 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(77.5f, 55.0f, 0.0f));		// 시작 위치로
	//Tiger_Movement_ModelMatrix = glm::scale(Tiger_Movement_ModelMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));			// 반대방향으로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-32.5f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 3.10344827586f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(32.5f, 0.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course5(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 45, 87.5
	// 도착점: 22.5, 110
	// 반지름의 길이: 22.5
	// 궤적의 길이	: 35.3429173529(약 20.25 -> 20초 걸리도록 속도 변경)
	// 수정된 속도	: 1.76714586765
	// 1초당 각도	: 4.5 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(45.0f, 87.5f, 0.0f));		// 시작 위치로
	//Tiger_Movement_ModelMatrix = glm::scale(Tiger_Movement_ModelMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));			// 반대방향으로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 22.5f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 4.5f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -22.5f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course6(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 22.5, 110
	// 도착점: 22.5, 132.5
	// 궤적의 길이: 22.5(약 12.8915503905초 소요-> 13초 걸리도록 속도 변경)
	// 수정된 속도: 1.73076923077

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(22.5f, 110.0f + adjusted_time * 1.73076923077f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course7(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 22.5, 132.5
	// 도착점: 37.5, 147.5
	// 반지름의 길이: 15
	// 궤적의 길이	: 23.5619449019(약 13.5 -> 14초 걸리도록 속도 변경)
	// 2*pi*r/4		:              (/기준속도 ...
	// 수정된 속도	: 1.68299606442
	// 1초당 각도	: 6.42857142856 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(22.5f, 132.5f, 0.0f));		// 시작 위치로
	//Tiger_Movement_ModelMatrix = glm::scale(Tiger_Movement_ModelMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));			// 반대방향으로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(15.0f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 6.42857142856f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-15.0f, 0.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course8(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 37.5, 147.5
	// 시작점: 22.5, 132.5
	// 반지름의 길이: 15
	// 궤적의 길이	: 23.5619449019(약 13.5 -> 14초 걸리도록 속도 변경)
	// 2*pi*r/4		:              (/기준속도 ...
	// 수정된 속도	: 1.68299606442
	// 1초당 각도	: 6.42857142856 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(37.5f, 147.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -15.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 6.42857142856f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 15.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course9(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 22.5, 132.5
	// 시작점: 22.5, 110
	// 궤적의 길이: 22.5(약 12.8915503905초 소요-> 13초 걸리도록 속도 변경)
	// 수정된 속도: 1.73076923077

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(22.5f, 132.5f - adjusted_time * 1.73076923077f, 0.0f));	// 시작점 + 진행
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course10(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 22.5, 110
	// 시작점: 45, 87.5
	// 반지름의 길이: 22.5
	// 궤적의 길이	: 35.3429173529(약 20.25 -> 20초 걸리도록 속도 변경)
	// 수정된 속도	: 1.76714586765
	// 1초당 각도	: 4.5 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(22.5f, 110.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(22.5f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 4.5f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-22.5f, 0.0f, 0.0f));		// 반지름만큼 이동
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course11(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 45, 87.5
	// 도착점: 115, 87.5
	// 궤적의 길이: 70(약 40.1070456593초 소요-> 40초 걸리도록 속도 변경)
	// 수정된 속도: 1.75

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(45.0f + adjusted_time * 1.75f, 87.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course12(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 115, 87.5
	// 도착점: 125, 77.5
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(115.0f, 87.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 10.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course13(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 125, 77.5
	// 도착점: 135, 67.5
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(125.0f, 77.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(10.0f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-10.0f, 0.0f, 0.0f));		// 반지름만큼 이동
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course14(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 135, 67.5
	// 도착점: 145, 67.5
	// 궤적의 길이: 10(약 5.72957795132초 소요-> 6초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(135.0f + adjusted_time * 1.66666666667f, 67.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course15(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 145, 67.5
	// 도착점: 122.5, 67.5
	// 궤적의 길이: 22.5(약 12.8915503905초 소요-> 13초 걸리도록 속도 변경)
	// 수정된 속도: 1.73076923077

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(145.0f - adjusted_time * 1.73076923077f, 67.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course16(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 122.5, 67.5
	// 시작점: 110, 55
	// 반지름의 길이: 12.5
	// 궤적의 길이	: 19.6349540849362(약 11.25 ->11초 걸리도록 속도 변경)
	// 수정된 속도	: 1.78499582590329
	// 1초당 각도	: 8.18181818181818 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(122.5f, 67.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -12.5f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 8.18181818181818f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 12.5f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course17(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 110, 55
	// 시작점: 110, 37.5
	// 궤적의 길이: 17.5(약 10.0267614148초 소요-> 10초 걸리도록 속도 변경)
	// 수정된 속도: 1.75

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, 55.0f - adjusted_time * 1.75f, 0.0f));	// 시작점 + 진행
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course18(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 110, 37.5
	// 도착점: 110, 55
	// 궤적의 길이: 17.5(약 10.0267614148초 소요-> 10초 걸리도록 속도 변경)
	// 수정된 속도: 1.75

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, 37.5f + adjusted_time * 1.75f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course19(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 110, 55
	// 도착점: 122.5, 67.5
	// 반지름의 길이: 12.5
	// 궤적의 길이	: 19.6349540849362(약 11.25 ->11초 걸리도록 속도 변경)
	// 수정된 속도	: 1.78499582590329
	// 1초당 각도	: 8.18181818181818 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, 55.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(12.5f, 0.0f,  0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 8.18181818181818f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-12.5f, 0.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course20(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 122.5, 67.5
	// 시작점: 145, 67.5
	// 궤적의 길이: 22.5(약 12.8915503905초 소요-> 13초 걸리도록 속도 변경)
	// 수정된 속도: 1.73076923077

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(122.5f + adjusted_time * 1.73076923077f, 67.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course21(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 145, 67.5
	// 시작점: 135, 67.5
	// 궤적의 길이: 10(약 5.72957795132초 소요-> 6초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(145.0f - adjusted_time * 1.66666666667f, 67.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course22(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 135, 67.5
	// 시작점: 125, 77.5
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(135.0f, 67.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 10.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course23(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 125, 77.5
	// 도착점: 125, 87.5
	// 궤적의 길이: 10(약 5.72957795132초 소요-> 6초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(125.0f, 77.5f + adjusted_time * 1.66666666667f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course24(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 125, 87.5
	// 시작점: 135, 97.5
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(125.0f, 87.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(10.0f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-10.0f, 0.0f,  0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course25(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 135, 97.5
	// 도착점: 142.5, 97.5
	// 궤적의 길이: 7.5(약 4.29718346349초 소요-> 4초 걸리도록 속도 변경)
	// 수정된 속도: 1.875

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(135.0f + adjusted_time * 1.875f, 97.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course26(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 142.5, 97.5
	// 도착점: 165, 120
	// 반지름의 길이: 22.5
	// 궤적의 길이	: 35.3429173529(약 20.25 -> 20초 걸리도록 속도 변경)
	// 수정된 속도	: 1.76714586765
	// 1초당 각도	: 4.5 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(142.5f, 97.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 22.5f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 4.5f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -22.5f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course27(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 165, 120
	// 도착점: 165, 132.5
	// 궤적의 길이: 12.5(약 7.16197243915초 소요-> 7초 걸리도록 속도 변경)
	// 수정된 속도: 1.78571428571

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(165.0f, 120.0f + adjusted_time * 1.78571428571f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course28(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 165, 132.5
	// 시작점: 150, 147.5
	// 반지름의 길이: 15
	// 궤적의 길이	: 23.5619449019(약 13.5 -> 14초 걸리도록 속도 변경)
	// 2*pi*r/4		:              (/기준속도 ...
	// 수정된 속도	: 1.68299606442
	// 1초당 각도	: 6.42857142856 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(165.0f, 132.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-15.0f, 0.0f,  0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 6.42857142856f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(15.0f, 0.0f,  0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course29(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 150, 147.5
	// 시작점: 92.5, 147.5
	// 궤적의 길이: 57.5(약 32.9450732201초 소요-> 33초 걸리도록 속도 변경)
	// 수정된 속도: 1.74242424242

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f - adjusted_time * 1.74242424242f, 147.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course30(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 92.5, 147.5
	// 시작점: 150, 147.5
	// 궤적의 길이: 72.5(약 41.5394401471초 소요-> 33초 걸리도록 속도 변경)
	// 수정된 속도: 1.72619047619

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(92.5f + adjusted_time * 1.74242424242f, 147.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course31(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 150, 147.5
	// 도착점: 165, 132.5
	// 반지름의 길이: 15
	// 궤적의 길이	: 23.5619449019(약 13.5 -> 14초 걸리도록 속도 변경)
	// 2*pi*r/4		:              (/기준속도 ...
	// 수정된 속도	: 1.68299606442
	// 1초당 각도	: 6.42857142856 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 147.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3( 0.0f, -15.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 6.42857142856f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3( 0.0f, 15.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course32(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 145, 132.5
	// 시작점: 165, 112.5
	// 궤적의 길이: 20(약 11.4591559026초 소요-> 11초 걸리도록 속도 변경)
	// 수정된 속도: 1.81818181818

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(165.0f, 132.5f - adjusted_time * 1.81818181818f, 0.0f));	// 시작점 + 진행
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course33(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 165, 112.5
	// 도착점: 180, 97.5
	// 반지름의 길이: 15
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(165.0f, 112.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(15.0f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-15.0f, 0.0f, 0.0f));		// 반지름만큼 이동
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course34(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 180, 97.5
	// 도착점: 195, 97.5
	// 궤적의 길이: 15(약 8.59436692698초 소요-> 9초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f + adjusted_time * 1.66666666667f, 97.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course35(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 195, 97.5
	// 시작점: 217.5, 75
	// 반지름의 길이: 22.5
	// 궤적의 길이	: 35.3429173529(약 20.25 -> 20초 걸리도록 속도 변경)
	// 수정된 속도	: 1.76714586765
	// 1초당 각도	: 4.5 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(195.0f, 97.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3( 0.0f, -22.5f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 4.5f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 22.5f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course36(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 217.5, 75
	// 시작점: 217.5, 65
	// 궤적의 길이: 10(약 5.72957795132초 소요-> 6초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(217.5f, 75.0f - adjusted_time * 1.66666666667f, 0.0f));	// 시작점 + 진행
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course37(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 217.5, 65
	// 도착점: 202.5, 50
	// 반지름의 길이: 15
	// 궤적의 길이	: 23.5619449019(약 13.5 -> 14초 걸리도록 속도 변경)
	// 2*pi*r/4		:              (/기준속도 ...
	// 수정된 속도	: 1.68299606442
	// 1초당 각도	: 6.42857142856 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(217.5f, 65.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-15.0f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 6.42857142856f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(15.0f, 0.0f, 0.0f));		// 반지름만큼 이동
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course38(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 202.5, 50
	// 시작점: 192.5, 40
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(202.5f, 50.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 10.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course39(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 192.5, 40
	// 시작점: 192.5, 35
	// 궤적의 길이: 5(약 2.86478897566초 소요-> 3초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(192.5f, 40.0f - adjusted_time * 1.66666666667f, 0.0f));	// 시작점 + 진행
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course40(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 192.5, 35
	// 시작점: 202.5, 25
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(192.5f, 35.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(10.0f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-10.0f, 0.0f, 0.0f));		// 반지름만큼 이동
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course41(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 202.5, 25
	// 도착점: 192.5, 35
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(202.5f, 25.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3( 0.0f, 10.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course42(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 192.5, 35
	// 시작점: 192.5, 40
	// 궤적의 길이: 5(약 2.86478897566초 소요-> 3초 걸리도록 속도 변경)
	// 수정된 속도: 1.66666666667

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(192.5f, 35.0f + adjusted_time * 1.66666666667f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course43(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 192.5, 40
	// 도착점: 202.5, 50
	// 반지름의 길이: 10
	// 궤적의 길이	: 15.707963267949(약 9 -> 9초 걸리도록 속도 변경)
	// 수정된 속도	: 1.74532925199433
	// 1초당 각도	: 10 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(192.5f, 40.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(10.0f, 0.0f,  0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-10.0f, 0.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course44(float angle, int camera_selected) {
	//			집 MC
	// 도착점: 202.5, 50
	// 시작점: 217.5, 65
	// 반지름의 길이: 15
	// 궤적의 길이	: 23.5619449019(약 13.5 -> 14초 걸리도록 속도 변경)
	// 2*pi*r/4		:              (/기준속도 ...
	// 수정된 속도	: 1.68299606442
	// 1초당 각도	: 6.42857142856 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(202.5f, 50.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 15.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 6.42857142856f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3( 0.0f, -15.0f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course45(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 217.5, 65
	// 도착점: 217.5, 132.5
	// 궤적의 길이: 67.5(약 38.6746511714초 소요-> 39초 걸리도록 속도 변경)
	// 수정된 속도: 1.73076923077

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(217.5f, 65.0f + adjusted_time * 1.73076923077f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course46(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 217.5, 132.5
	// 도착점: 217.5, 120
	// 궤적의 길이: 12.5(약 7.16197243915초 소요-> 7초 걸리도록 속도 변경)
	// 수정된 속도: 1.78571428571

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(217.5f, 132.5f - adjusted_time * 1.78571428571f, 0.0f));	// 시작점 + 진행
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course47(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 217.5, 120
	// 도착점: 195, 97.5
	// 반지름의 길이: 22.5
	// 궤적의 길이	: 35.3429173529(약 20.25 -> 35초 걸리도록 속도 변경)
	// 수정된 속도	: 1.76714586765
	// 1초당 각도	: 4.5 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(217.5f, 120.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-22.5f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 4.5f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(22.5f, 0.0f,  0.0f));		// 반지름만큼 이동
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course48(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 195, 97.5
	// 시작점: 100, 67.5
	// 궤적의 길이: 95(약 54.43099053765초 소요-> 54초 걸리도록 속도 변경)
	// 수정된 속도: 1.75925925926

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(195.0f - adjusted_time * 1.75925925926f, 97.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course49(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 100, 67.5
	// 도착점: 195, 97.5
	// 반지름의 길이: 22.5
	// 궤적의 길이	: 35.3429173529(약 20.25 -> 35초 걸리도록 속도 변경)
	// 수정된 속도	: 1.76714586765
	// 1초당 각도	: 4.5 (수정된속도 * 360 / (2*pi*r))

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 97.5f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, -22.5f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, angle * 4.5f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(0.0f, 22.5f, 0.0f));		// 반지름만큼 이동
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

// 기준 속도: 1.74532925199
void course50(int adjusted_time, int camera_selected) {
	//			집 MC
	// 시작점: 77.5, 75
	// 도착점: 77.5, 40
	// 궤적의 길이: 35(약 20.0535228296초 소요-> 20초 걸리도록 속도 변경)
	// 수정된 속도: 1.75

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(77.5f, 75.0f - adjusted_time * 1.75f, 0.0f));	// 시작점 + 진행
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

void course51(float angle, int camera_selected) {
	//			집 MC
	// 시작점: 77.5, 40
	// 도착점: 77.5, 40
	// 반지름의 길이: 17.5
	// 궤적의 길이	: 27.4889357189(약 15.75 -> 16초 걸리도록 속도 변경)
	// 수정된 속도	: 1.71805848243
	// 1초당 각도	: 5.6245

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(77.5f, 40.0f, 0.0f));		// 시작 위치로
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(-17.5f, 0.0f, 0.0f));		// 다시 원점으로
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -angle * 5.6245f, glm::vec3(0.0f, 0.0f, 1.0f));	// 반지름 따라 회전
	Tiger_Movement_ModelMatrix = glm::translate(Tiger_Movement_ModelMatrix, glm::vec3(17.5f, 0.0f,  0.0f));		// 반지름만큼 이동
	//Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	// 진행 방향 맞게 호랑이 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

void course52(int adjusted_time, int camera_selected) {
	//			집 MC
	// 도착점: 60, 22.5
	// 시작점: 37.5, 22.5
	// 도착점: 60, 22.5
	// 궤적의 길이: 22.5(약 12.8915503905초 소요->13초 걸리도록 속도 변경)
	// 수정된 속도: 1.73076923077

	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f - adjusted_time * 1.73076923077f, 22.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));		// 진행 방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
}

void draw_animated_tiger(int camera_selected) {
	glm::mat4 Matrix_CAMERA_tiger_inverse;
	int time = cc.timestamp_cow;
	int adjusted_time;

	//printf("%d\n", time);

	// 전 범위 상한 <= time <= 전 범위 상한 + 소요시간 - 1
	//		1번째 인자는 time - 전 범위 상한 [ * TO_RADIAN ](회전일 경우)
	
	if (0 <= time && time <= 12)
		course1(time, camera_selected);
	else if (13 <= time && time <= 28)
		course2((time - 13)*TO_RADIAN, camera_selected);
	else if (29 <= time && time <= 37)
		course3(time - 29, camera_selected);
	else if (38 <= time && time <= 66)
		course4((time - 38)*TO_RADIAN, camera_selected);
	else if (67 <= time && time <= 86)
		course5((time - 67)*TO_RADIAN, camera_selected);
	else if (87 <= time && time <= 99)
		course6(time - 87, camera_selected);
	else if (100 <= time && time <= 113)
		course7((time - 100)*TO_RADIAN, camera_selected);
	else if (114 <= time && time <= 127)
		course8((time - 114)*TO_RADIAN, camera_selected);
	else if (128 <= time && time <= 140)
		course9(time - 128, camera_selected);
	else if (141 <= time && time <= 160)
		course10((time - 141)*TO_RADIAN, camera_selected);
	else if (161 <= time && time <= 200)
		course11(time - 161, camera_selected);
	else if (201 <= time && time <= 209)
		course12((time - 201)*TO_RADIAN, camera_selected);
	else if (210 <= time && time <= 218)
		course13((time - 210)*TO_RADIAN, camera_selected);
	else if (219 <= time && time <= 224)
		course14(time - 219, camera_selected);
	else if (225 <= time && time <= 237)
		course15(time - 225, camera_selected);
	else if (238 <= time && time <= 248)
		course16((time - 238)*TO_RADIAN, camera_selected);
	else if (249 <= time && time <= 258)
		course17(time - 249, camera_selected);
	else if (259 <= time && time <= 268)
		course18(time - 259, camera_selected);
	else if (269 <= time && time <= 279)
		course19((time - 269)*TO_RADIAN, camera_selected);
	else if (280 <= time && time <= 292)
		course20(time - 280, camera_selected);
	else if (293 <= time && time <= 298)
		course21(time - 293, camera_selected);
	else if (299 <= time && time <= 307)
		course22((time - 299)*TO_RADIAN, camera_selected);
	else if (308 <= time && time <= 313)
		course23(time - 308, camera_selected);
	else if (314 <= time && time <= 322)
		course24((time - 314)*TO_RADIAN, camera_selected);
	else if (323 <= time && time <= 326)
		course25(time - 323, camera_selected);
	else if (327 <= time && time <= 346)
		course26((time - 327)*TO_RADIAN, camera_selected);
	else if (347 <= time && time <= 353)
		course27(time - 347, camera_selected);
	else if (354 <= time && time <= 367)
		course28((time - 354)*TO_RADIAN, camera_selected);
	else if (368 <= time && time <= 400)
		course29(time - 368, camera_selected);
	else if (401 <= time && time <= 432)
		course30(time - 401, camera_selected);
	else if (433 <= time && time <= 446)
		course31((time - 433)*TO_RADIAN, camera_selected);
	else if (447 <= time && time <= 457)
		course32(time - 447, camera_selected);
	else if (458 <= time && time <= 466)
		course33((time - 458)*TO_RADIAN, camera_selected);
	else if (467 <= time && time <= 475)
		course34(time - 467, camera_selected);
	else if (476 <= time && time <= 495)
		course35((time - 476)*TO_RADIAN, camera_selected);
	else if (496 <= time && time <= 501)
		course36(time - 496, camera_selected);
	else if (502 <= time && time <= 515)
		course37((time - 502)*TO_RADIAN, camera_selected);
	else if (516 <= time && time <= 524)
		course38((time - 516)*TO_RADIAN, camera_selected);
	else if (525 <= time && time <= 527)
		course39(time - 525, camera_selected);
	else if (528 <= time && time <= 536)
		course40((time - 528)*TO_RADIAN, camera_selected);
	else if (537 <= time && time <= 545)
		course41((time - 537)*TO_RADIAN, camera_selected);
	else if (546 <= time && time <= 548)
		course42(time - 546, camera_selected);
	else if (549 <= time && time <= 557)
		course43((time - 549)*TO_RADIAN, camera_selected);
	else if (558 <= time && time <= 571)
		course44((time - 558)*TO_RADIAN, camera_selected);
	else if (572 <= time && time <= 610)
		course45(time - 572, camera_selected);
	else if (611 <= time && time <= 616)
		course46(time - 611, camera_selected);
	else if (617 <= time && time <= 636)
		course47((time - 617)*TO_RADIAN, camera_selected);
	else if (637 <= time && time <= 690)
		course48(time - 637, camera_selected);
	else if (691 <= time && time <= 710)
		course49((time - 691)*TO_RADIAN, camera_selected);
	else if (711 <= time && time <= 730)
		course50(time - 711, camera_selected);
	else if (731 <= time && time <= 746)
		course51((time - 731)*TO_RADIAN, camera_selected);
	else if (747 <= time && time <= 759)
		course52(time - 747, camera_selected);

	/*
	Tiger_Movement_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(145.f, 67.5f, 0.0f));	// 시작점 + 진행
	Tiger_Movement_ModelMatrix = glm::rotate(Tiger_Movement_ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));			// 진행방향으로 고개 돌리기
	Tiger_ModelMatrix = Tiger_Movement_ModelMatrix * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewMatrix = Cam_ViewMatrix[camera_selected] * Tiger_ModelMatrix;
	*/

	if (camera_selected == 5) {
		glm::mat4 ModelMatrix_TIGER_BODY_to_DRIVER;
		//ModelMatrix_TIGER_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, EYEBALL_Y, EYEBALL_Z));
		ModelMatrix_TIGER_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, EYEBALL_Y, EYEBALL_Z));
		ModelMatrix_TIGER_BODY_to_DRIVER = glm::rotate(ModelMatrix_TIGER_BODY_to_DRIVER, TO_RADIAN*180.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		ModelMatrix_TIGER_BODY_to_DRIVER = glm::rotate(ModelMatrix_TIGER_BODY_to_DRIVER, TO_RADIAN*90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		Matrix_CAMERA_tiger_inverse = Tiger_Movement_ModelMatrix * ModelMatrix_TIGER_BODY_to_DRIVER;
		ViewMatrix = glm::affineInverse(Matrix_CAMERA_tiger_inverse);
		ProjectionMatrix = Cam_ProjectionMatrix[camera_selected];
		ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * Tiger_ModelMatrix;

		//return;
	}
	else
		ModelViewProjectionMatrix = Cam_ProjectionMatrix[camera_selected] * ModelViewMatrix;
	
	// 2번 광원 위한 계산
	ModelViewMatrixInvTrans = glm::mat3(glm::transpose(glm::affineInverse(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	// 추가
	glUniformMatrix4fv((shader_type==0)?loc_ModelViewProjectionMatrix_PS:loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv((shader_type == 0) ? loc_ModelViewMatrix_PS: loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv((shader_type == 0) ? loc_ModelViewMatrixInvTrans_PS: loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	glm::vec4 light_pos_EC = ModelViewMatrix * glm::vec4(light[2].position[0], light[2].position[1], light[2].position[2], light[2].position[3]);

	glUniform4fv(((shader_type == 0) ? loc_light_PS: loc_light_GS)[2].position, 1, &light_pos_EC[0]);

	glm::vec3 light_dir_EC = normalize(ModelViewMatrixInvTrans * glm::vec3(light[2].spot_direction[0], light[2].spot_direction[1], light[2].spot_direction[2]));

	glUniform3fv(((shader_type == 0) ? loc_light_PS: loc_light_GS)[2].spot_direction, 1, &light_dir_EC[0]);

	set_material_tiger_house();
	glFrontFace(GL_CW);

	

	// draw_tiger()
	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);




	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUseProgram(h_ShaderProgram_simple);

	draw_tiger_MC(camera_selected);
	draw_tiger_EC(camera_selected);
	draw_tiger_ViewVolume(camera_selected);
}

void cleanup_OpenGL_stuffs(void) {
	for (int i = 0; i < n_static_objects; i++) {
		glDeleteVertexArrays(1, &(static_objects[i].VAO));
		glDeleteBuffers(1, &(static_objects[i].VBO));
	}

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		glDeleteVertexArrays(1, &(tiger[i].VAO));
		glDeleteBuffers(1, &(tiger[i].VBO));
	}

	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);
}