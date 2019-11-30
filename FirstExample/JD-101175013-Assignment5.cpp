//***************************************************************************
// JD-101175013-Assignment5.cpp by Jang Doosung (C) 2018 All Rights Reserved.
//
// Assignment 5 submission.
//
// Description:
//	Texture, Blending and Lighting
//  
//*****************************************************************************

using namespace std;
#include <iostream>
#include "stdlib.h"
#include "time.h"
#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "SoilLib/SOIL.h"

#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)

GLuint uniformModel = 0;
GLuint uniformView = 0;
GLuint uniformProj = 0;
GLuint uniformLightPos = 0;
GLuint uniformEyePos = 0;

GLuint vao, ibo, points_vbo, colours_vbo;
GLuint iNumOfCubeIndices = 0;
GLuint iNumOfVertices = 0;
GLuint iVertexLength = 0;
GLuint iLeather_tex = 0;
GLuint iFence_tex = 0;

float rotAngle = 0.0f;

// Horizontal and vertical ortho offsets.
float osH = 0.0f, osV = 0.0f, scrollSpd = 0.25f;

int deltaTime, currentTime, lastTime = 0;
glm::mat4 view, projection;

int WindowWidth = 800;
int WindowHeight = 600;

glm::vec3 CameraPosition = glm::vec3(0.f, 0.f, 10);
float fCameraSpeed = 0.5f;

GLfloat cube_vertices[] =
{
	//Front 
	//x, y, z				u, v		nx, ny, nz
	-1.0f, -1.0f, 1.0f,		0, 1,		0, 0, 0,	// 0.
	1.0f, -1.0f, 1.0f,		1, 1,		0, 0, 0,	// 1
	1.0f, 1.0f, 1.0f,		1, 0,		0, 0, 0,	// 2.
	-1.0f, 1.0f, 1.0f,		0, 0,		0, 0, 0,	// 3.

	//Right
	1.0f, -1.0f, 1.0f,		0, 1,		0, 0, 0,	// 4
	1.0f, -1.0f, -1.0f,		1, 1,		0, 0, 0,	// 5
	1.0f, 1.0f, -1.0f,		1, 0,		0, 0, 0,	// 6
	1.0f, 1.0f, 1.0f,		0, 0,		0, 0, 0,	// 7.

	//Left
	-1.0f, -1.0f, -1.0f,	0, 1,		0, 0, 0,	// 8
	-1.0f, -1.0f, 1.0f,		1, 1,		0, 0, 0,	// 9
	-1.0f, 1.0f, 1.0f,		1, 0,		0, 0, 0,	// 10
	-1.0f, 1.0f, -1.0f,		0, 0,		0, 0, 0,	// 11

	//Back
	1.0f, -1.0f, -1.0f,		0, 1,		0, 0, 0,	// 12
	-1.0f, -1.0f, -1.0f,	1, 1,		0, 0, 0,	// 13
	-1.0f, 1.0f, -1.0f,		1, 0,		0, 0, 0,	// 14
	1.0f, 1.0f, -1.0f,		0, 0,		0, 0, 0,	// 15

	//Up
	-1.0f, 1.0f, 1.0f,		0, 1,		0, 0, 0,	//16
	1.0f, 1.0f, 1.0f,		1, 1,		0, 0, 0,
	1.0f, 1.0f, -1.0f,		1, 0,		0, 0, 0,
	-1.0f, 1.0f, -1.0f,		0, 0,		0, 0, 0,

	//Down
	-1.0f, -1.0f, -1.0f,	0, 1,		0, 0, 0,	//20
	1.0f, -1.0f, -1.0f,		1, 1,		0, 0, 0,
	1.0f, -1.0f, 1.0f,		1, 0,		0, 0, 0,
	-1.0f, -1.0f, 1.0f,		0, 0,		0, 0, 0
};

GLshort cube_indices[] =
{
	//Front
	0, 1, 3,
	1, 2, 3,

	//Right
	4, 5, 7,
	5, 6, 7,

	//Left
	8, 9, 11,
	9, 10, 11,

	//Back
	12, 13, 15,
	13, 14, 15,

	//Up
	16, 17, 19,
	17, 18, 19,

	//DOwn
	20, 21, 23,
	21, 22, 23
};


bool bWireFrameMode = false;


struct Light
{
	GLfloat ambientStrength;

	glm::vec3 diffuseColor;
	GLfloat diffuseStrength;

	GLfloat specularStrength;
	GLfloat shininess;

	Light(GLfloat aStr, glm::vec3 dCol, GLfloat dStr, GLfloat sStr, GLfloat shin)
	{
		ambientStrength = aStr;

		diffuseColor = dCol;
		diffuseStrength = dStr;

		specularStrength = sStr;
		shininess = shin;
	}

};

struct PointLight : public Light
{
	glm::vec3 position;
	GLfloat constant, linear, exponent;	//Quadratic equation for attenuation

	PointLight(glm::vec3 pos, GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat aStr, glm::vec3 dCol, GLfloat dStr, GLfloat sStr, GLfloat shin)
		:Light(aStr, dCol, dStr, sStr, shin)
	{
		position = pos;

		constant = con;
		linear = lin;
		exponent = exp;
	}
};


PointLight pLight(glm::vec3(0.f, 3.f, 0.f), 1.f, 0.35f / 13.f, 0.44f / (13.f * 13.f),
				  0.2f, glm::vec3(1.f, 1.f, 1.f), 1.f, 1.f, 32.f);

//PointLight pLight2(glm::vec3(-1.f, 1.f, 1.f), 1.f, 0.35f / 13.f, 0.44f / (13.f * 13.f),
//	glm::vec3(1.f, 1.f, 1.f), 0.2f, glm::vec3(1.f, 1.f, 1.f), 1.f);


void calcAverageNormals(GLshort* indices, unsigned int indiceCount,
	GLfloat* vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
	for (int i = 0; i < indiceCount; i += 3)
	{
		//Get the location of vertex in vertex array(vertices) using index calculation
		unsigned int in0 = indices[i] * vLength;		//First vertex's index of vertex array
		unsigned int in1 = indices[i + 1] * vLength;	//Second vertex's index of vertex array
		unsigned int in2 = indices[i + 2] * vLength;	//Third vertex's index of vertex array

		//Get the line vector between two vectexs
		//Calculate vector from First vertex to Second vertex
		glm::vec3 v1(vertices[in1] - vertices[in0], 
					 vertices[in1 + 1] - vertices[in0 + 1], 
					 vertices[in1 + 2] - vertices[in0 + 2]);
		//Calculate vector from First vertex to Third vertex
		glm::vec3 v2(vertices[in2] - vertices[in0], 
					 vertices[in2 + 1] - vertices[in0 + 1], 
					 vertices[in2 + 2] - vertices[in0 + 2]);

		//Calculate perpendicular vector of two vectors(v1, v2) which is face normal
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		//Find the index of normal in vertex array
		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;
	
		//Add calculated normal to each vertex's normal
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	//Normalize each vertex's normal
	for (int i = 0; i < verticeCount; ++i)
	{
		//Find index of normal in vertex array
		unsigned int nOffset = i * vLength + normalOffset;

		//Normalize the vertex's normal
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);

		//Set normalized normal
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void init(void)
{
	//Specifying the name of vertex and fragment shaders.
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};
	
	//Loading and compiling shaders
	GLuint program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up

	uniformModel = glGetUniformLocation(program, "model");
	uniformView = glGetUniformLocation(program, "view");
	uniformProj = glGetUniformLocation(program, "projection");

	// Perspective arameters : Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	// projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Ortho parameters: left, right, bottom, top, nearVal, farVal
	//projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates
	projection = glm::perspective(glm::radians(45.f), (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.f);
	
	// Camera matrix
	view = glm::lookAt
	(
		CameraPosition,		// Camera pos in World Space
		glm::vec3(0, 0, 0),		// and looks at the origin
		glm::vec3(0, 1, 0)		// Head is up (set to 0,-1,0 to look upside-down)
	);



	//Set light
	glUniform1f(glGetUniformLocation(program, "pLight.base.ambientStrength"), pLight.ambientStrength);
	glUniform3f(glGetUniformLocation(program, "pLight.base.diffuseColor"), pLight.diffuseColor.x, pLight.diffuseColor.y, pLight.diffuseColor.z);
	glUniform1f(glGetUniformLocation(program, "pLight.base.diffuseStrength"), pLight.diffuseStrength);
	glUniform1f(glGetUniformLocation(program, "pLight.base.specularStrength"), pLight.specularStrength);
	glUniform1f(glGetUniformLocation(program, "pLight.base.shininess"), pLight.shininess);

	glUniform3f(glGetUniformLocation(program, "pLight.position"), pLight.position.x, pLight.position.y, pLight.position.z);
	glUniform1f(glGetUniformLocation(program, "pLight.constant"), pLight.constant);
	glUniform1f(glGetUniformLocation(program, "pLight.linear"), pLight.linear);
	glUniform1f(glGetUniformLocation(program, "pLight.exponent"), pLight.exponent);

	uniformLightPos = glGetUniformLocation(program, "pLight.position");
	uniformEyePos = glGetUniformLocation(program, "eyePos");



	iNumOfCubeIndices = sizeof(cube_indices) / sizeof(GLshort);
	iVertexLength = 8;
	iNumOfVertices = sizeof(cube_vertices) / (sizeof(GLfloat) * iVertexLength);
	calcAverageNormals(cube_indices, iNumOfCubeIndices, cube_vertices, iNumOfVertices, iVertexLength, 5);


	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

		points_vbo = 0;
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cube_vertices[0]) * iVertexLength, 0);
		glEnableVertexAttribArray(0);

		//tell location1 is for tex coordinate
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(cube_vertices[0]) * iVertexLength, (void*)(sizeof(cube_vertices[0]) * 3));
		glEnableVertexAttribArray(1);

		//tell location2 is for normal
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(cube_vertices[0]) * iVertexLength, (void*)(sizeof(cube_vertices[0]) * 5));
		glEnableVertexAttribArray(2);



	GLint width, height;
	unsigned char* image = SOIL_load_image("Leather.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr)
	{
		printf("Error: image not found\n");
	}
	//glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &iLeather_tex);
	glBindTexture(GL_TEXTURE_2D, iLeather_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);





	image = SOIL_load_image("Fence.png", &width, &height, 0, SOIL_LOAD_RGBA);
	if (image == nullptr)
	{
		printf("Error: image not found\n");
	}
	glGenTextures(1, &iFence_tex);
	glBindTexture(GL_TEXTURE_2D, iFence_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);


	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	// Enable depth test.
	glEnable(GL_DEPTH_TEST);

	//Set blend option
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Antialiasing
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
}

//---------------------------------------------------------------------
//
// transformModel
//

void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) 
{
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);
	
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, &Model[0][0]);
}

//---------------------------------------------------------------------
//
// display
//

void display(void)
{
	// Delta time stuff.
	currentTime = glutGet(GLUT_ELAPSED_TIME); // Gets elapsed time in milliseconds.
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	//Camera Update
	view = glm::lookAt
	(
		CameraPosition,		// Camera pos in World Space
		glm::vec3(0, 0, 0),		// and looks at the origin
		glm::vec3(0, 1, 0)		// Head is up (set to 0,-1,0 to look upside-down)
	);
	
	//Set projection and view matrix in shader
	glUniformMatrix4fv(uniformProj, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, &view[0][0]);
	

	//Set light position
	glUniform3f(uniformLightPos, pLight.position.x, pLight.position.y, pLight.position.z);

	//Set eye position
	glUniform3f(uniformEyePos, CameraPosition.x, CameraPosition.y, CameraPosition.z);




	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	



	//Draw Plane
	glBindTexture(GL_TEXTURE_2D, iLeather_tex);
	glBindVertexArray(vao);
	static float fAngle = 0.f;
	//fAngle += 0.1f;
	if (fAngle >= 360.f)
		fAngle -= 360.f;
	transformObject(glm::vec3(1.f, 1.f, 1.f), Y_AXIS, fAngle, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, iNumOfCubeIndices, GL_UNSIGNED_SHORT, 0);


	/*glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, iFence_tex);
	glBindVertexArray(vao);
	transformObject(glm::vec3(1.f, 1.f, 1.f), Y_AXIS, 0.f, glm::vec3(-1.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, iNumOfCubeIndices, GL_UNSIGNED_SHORT, 0);
	glDisable(GL_BLEND);*/





	glutSwapBuffers(); // Instead of double buffering.
}

void idle()
{
	//glutPostRedisplay();
}

void timer(int id)
{ 
	glutPostRedisplay();
	glutTimerFunc(33, timer, 0); 
}

void keyDown(unsigned char key, int x, int y)
{
	// Orthographic.
	switch(key)
	{
		case 'w':
			CameraPosition.z -= fCameraSpeed;
			//osV -= scrollSpd;
			break;
		case 's':
			CameraPosition.z += fCameraSpeed;
			//osV += scrollSpd;
			break;
		case 'a':
			CameraPosition.x -= fCameraSpeed;
			//osH += scrollSpd;
			break;
		case 'd':
			CameraPosition.x += fCameraSpeed;
			//osH -= scrollSpd;
			break;
		case 'r':
			CameraPosition.y += fCameraSpeed;
			break;
		case 'f':
			CameraPosition.y -= fCameraSpeed;
			break;

		case 'i':
			pLight.position.z -= fCameraSpeed;
			break;
		case 'k':
			pLight.position.z += fCameraSpeed;
			break;
		case 'j':
			pLight.position.x -= fCameraSpeed;
			break;
		case 'l':
			pLight.position.x += fCameraSpeed;
			break;
		case 'p':
			pLight.position.y += fCameraSpeed;
			break;
		case ';':
			pLight.position.y -= fCameraSpeed;
			break;




		case 'q':
			bWireFrameMode ? bWireFrameMode = false : bWireFrameMode = true;
			if (bWireFrameMode)
			{
				//Wireframe mode
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			break;
	}
}

void keyUp(unsigned char key, int x, int y)
{
	// Empty for now.
}

void mouseMove(int x, int y)
{
	//cout << "Mouse pos: " << x << "," << y << endl;
}

void mouseDown(int btn, int state, int x, int y)
{
	cout << "Clicked: " << (btn == 0 ? "left " : "right ") << (state == 0 ? "down " : "up ") <<
		"at " << x << "," << y << endl;
}

void clean()
{
	glDeleteTextures(1, &iLeather_tex);
	glDeleteTextures(1, &iFence_tex);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow("Jang, Doosung, 101175013");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.
	init();

	// Set all our glut functions.
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutTimerFunc(33, timer, 0);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseDown);
	glutPassiveMotionFunc(mouseMove); // or...
	//glutMotionFunc(mouseMove); // Requires click to register.

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);



	glutMainLoop();


	clean();

}
