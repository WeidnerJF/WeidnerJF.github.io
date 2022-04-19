////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Source file for holding mesh data, render function, Window creation and interaction funtions, and main function                     //
//                                                                                                                                    //
//  Removed shader information and created basic shader file. Added Shader class file. Shader class parses infomration from           // 
// the basic shader file and creating the shader information for both vertex and fragment shaders.                                    //
// Added Texture class. Used to pull texture images and maps them to the texture coordinates on the objects based on a render id      //
//                                                                                                                                    //
//Cleaned up comments and adjusted cube mesh object to use indices to map out the shape of the object.                                //
//This removed Vertices that were unnecessary. Removed excess class files that are not performing functions at this time.             //
// Removed extra header files and adjusted includes so that all files function properly. Adjusted object position of the larger metal //
// cube so that it is positioned properly.                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//basic include
#include <iostream>
#include <cstdlib>

// required external headers
#include "shader.h"
#include "Texture.h"
// image header and implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions

// glew and glfw libraries
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GL/GLU.h>

//OpenGL Math Headers
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// camera header
#include "camera2.h"

// settign namespace to std
using namespace std;


// Namespace Data
namespace
{
	//Setting Window Title
	const char* const WINDOW_TITLE = "Milestone";

	//Window Size
	const int WINDOW_WIDTH = 1200;
	const int WINDOW_HEIGHT = 720;

	// Mesh Data Structure
	struct GLMesh 
	{
		GLuint vao;			//Vertex Array Object
		GLuint vbos[2];		//Vertex Buffer Object [2]
		unsigned int nIndices;	//Indices
	};
	//GLFW Window
	GLFWwindow* gWindow = nullptr;
	
	// mesh data
	GLMesh gMesh;
	GLMesh gCube;
	GLMesh gPlane;
	
	// shader program
	unsigned int gProgramId;
	
	// camera
	Camera gCamera(glm::vec3(0.0f, 4.0f, 3.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;
	
	// perspective variable
	char perspective;
	glm::mat4 projection;
	
	// timing for frames
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;
	
	// load Textures
	Texture Plane("res/textures/table.jpg");
	Texture Metal("res/textures/metal.png");
	Texture Gray("res/textures/Gray.jpg");
}
// define functions
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UPyramid(GLMesh &mesh);
void UCube(GLMesh &Cube);
void UPlane(GLMesh& Plane);
void UDestroyMesh(GLMesh& mesh, GLMesh &Cube, GLMesh &Plane);
void URender();

//main function
int main(int argc, char* argV[]) {
	// Initialization check
	if (!UInitialize(argc, argV, &gWindow))
		return EXIT_FAILURE;
	
	//sets background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspective = 'p';
	// create mesh data
	UPyramid(gMesh);
	UCube(gCube);
	UPlane(gPlane);
	// render loop
	while (!glfwWindowShouldClose(gWindow)) {
		// per-frame timing
		double currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;
		
		
		
		//input
		UProcessInput(gWindow);
		// projection Inilization for camera
		projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
		// Clears intial buffer to free up memory
		glClear(GL_COLOR_BUFFER_BIT);
		// Initilazie render function
		URender();
		glfwPollEvents();
	}
	
	exit(EXIT_SUCCESS); // Terminates Program
}

//Initialize GLFW,GLEW and create window
bool UInitialize(int argc, char* argv[], GLFWwindow** window) {
	// Initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW Window Creation
		* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL) {
		std::cout << "Faile to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	//GLEW intialize
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();
	// Results Check
	if (GLEW_OK != GlewInitResult) {
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}
	// displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
	return true;
}

// Process inputs
void UProcessInput(GLFWwindow* window) {

	static const float cameraSpeed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// offset value
	float cameraOffset = cameraSpeed * gDeltaTime;
	// keyboard camera movements
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
	
}

// glfw window size change
void UResizeWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// glfw mouse movements
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
	//mouse positioning on the x and why axis
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}
	// offsets for mouse movement
	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos;
	
	gLastX = xpos;
	gLastY = ypos;
	// mouse movement used to adjust camera direction
	gCamera.ProcessMouseMovement(xoffset, yoffset);
}

//glfw mouse scroll movement
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	gCamera.ProcessMouseScroll(yoffset);
}

// moust buttons
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	switch (button) {
		// Left button
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			std::cout << "Left mouse button pressd" << std::endl;
		else
			std::cout << "Left mouse button released" << std::endl;
	}
	// Middle button
	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			std::cout << "MIDDLE mouse button pressd" << std::endl;
		else
			std::cout << "MIDDLE mouse button released" << std::endl;
	}
	// Right button
	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			std::cout << "Right mouse button pressd" << std::endl;
		else
			std::cout << "Right mouse button released" << std::endl;
	}
	break;
	default:
		std::cout << "Unhandled mouse button event" << std::endl;
	}
}

// Render frame
void URender() {
	// Render Variables
	const int nrows = 10;
	const int ncols = 10;
	const int nlevels = 10;
	const float xsize = 10.0f;
	const float ysize = 10.0f;
	const float zsize = 10.0f;
	
	// endable z buffer
	glEnable(GL_DEPTH_TEST);
	// sets background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Pull shader sources
	Shader shader("res/shaders/basic.shader");
	shader.Bind();
	// load Textures
		// table top texture
	Texture Plane("res/textures/table.jpg");
	    // metal object texture
	Texture Metal("res/textures/metal.png");
		// gray object texture
	Texture Gray("res/textures/Gray.jpg");

   // creates a perspective  projection using shader program
	shader.SetUniformMat4f("Projection", projection);

	// transforms the camera and sets uniform
	glm::mat4 view = gCamera.GetViewMatrix();
	shader.SetUniformMat4f("View", view);

	// Pyramid
	{
		//Scales object by 2
		glm::mat4 scale = glm::scale(glm::vec3(2.0f, 1.5f, 2.0f));
		// rotates shape on z axis
		glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(3.0, 3.0f, 1.0f));
		// object at origin
		glm::mat4 translation = glm::translate(glm::vec3(-3.0f, -0.1f, -1.0f));
		// Transformations are applied
		glm::mat4 model = translation * rotation * scale;
		// retrieves and passes transform matrices to shader
		shader.SetUniformMat4f("Model", model);

		// Binds pyramid array
		glBindVertexArray(gMesh.vao);
		// Draws Triangles
		//binds texture to pyramid
		Gray.Bind();
		// set texture
		shader.SetUniform1i("uTexture", 0);
		glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);
		// Deavtivate VAO
		glBindVertexArray(0);
	}
	//Metal Block
	{
		glm::mat4 scale2 = glm::scale(glm::vec3(1.0f, 2.5f, 6.0f));
		// rotates shape on z axis
		glm::mat4 rotation2 = glm::rotate(0.0f, glm::vec3(0.0, 3.0f, 1.0f));
		//object at origin
		glm::mat4 translation2 = glm::translate(glm::vec3(0.0f, 1.0f, -1.5f));
		// Transformations are applied
		glm::mat4 model2 = translation2 * rotation2 * scale2;
		// retrieves and passes transform matrices to shader
		shader.SetUniformMat4f("Model", model2);

		// Activate VBOs
		glBindVertexArray(gCube.vao);
		// bind shader to metal block
		shader.Bind();
		// bind metal shader
		Metal.Bind(0);
		// set texture to metal box
		shader.SetUniform1i("u_Texture", 0);
		// Draws Triangles
		glDrawElements(GL_TRIANGLES, gCube.nIndices, GL_UNSIGNED_SHORT, NULL);

		//possible removal
		//glDrawArrays(GL_TRIANGLES, 0, gCube.nIndices);

		//Deactivate VAO
		glBindVertexArray(0);
	}
	//contact on metal block
	{
		glm::mat4 scale3 = glm::scale(glm::vec3(0.5f, 2.5f, 1.5f));
		
		// rotates shape on z axis
		glm::mat4 rotation3 = glm::rotate(0.0f, glm::vec3(0.0, 3.0f, 1.0f));
		
		//object at origin
		glm::mat4 translation3 = glm::translate(glm::vec3(0.5f, 1.0f, -3.5f));
		
		// Transformations are applied
		glm::mat4 model3 = translation3 * rotation3 * scale3;
		
		// retrieves and passes transform matrices to shader
		shader.SetUniformMat4f("Model", model3);

		// Activate VBOs
		glBindVertexArray(gCube.vao);
		
		// bind shader to metal contact
		shader.Bind();
		
		// bind metal shader
		Metal.Bind(0);
		
		// set texture for metal contact
		shader.SetUniform1i("u_Texture", 0);
	
		// Draws Triangles
		glDrawElements(GL_TRIANGLES, gCube.nIndices, GL_UNSIGNED_SHORT, NULL);
		
		//Deactivate VAO
		glBindVertexArray(0);
	}
	// Table Top Plane
	{
		//Scales object by 2
		glm::mat4 scale4 = glm::scale(glm::vec3(2.0f, 1.5f, 2.0f));
		// rotates shape by 70 degrees z axis
		glm::mat4 rotation4 = glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f));
		// object at origin
		glm::mat4 translation4 = glm::translate(glm::vec3(-3.0f, -0.1f, -1.0f));
		// Transformations are applied
		glm::mat4 model4 = translation4 * rotation4 * scale4;
		// retrieves and passes transform matrices to shader
		shader.SetUniformMat4f("Model", model4);

		//bind texture to table top plane
		Plane.Bind();
		shader.SetUniform1i("u_Texture", 0);
		//Bind Array for the Table top plane
		glBindVertexArray(gPlane.vao);
		// Draws Triangles
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Deavtivate VAO
		glBindVertexArray(0);
	}
	//Gray/blue Box
	{
		glm::mat4 scale5 = glm::scale(glm::vec3(6.0f, 3.25f, 2.15f));
		// rotates shape by 70 degrees z axis
		glm::mat4 rotation5 = glm::rotate(10.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		//object at origin
		glm::mat4 translation5 = glm::translate(glm::vec3(2.5f, 2.9f, -3.0f));
		// Transformations are applied
		glm::mat4 model5 = translation5 * rotation5 * scale5;
		// retrieves and passes transform matrices to shader
		shader.SetUniformMat4f("Model", model5);

		// Activate VBOs
		//Bind Array for blue/gray box
		glBindVertexArray(gCube.vao);
		//bind shader to blue/gray box array
		shader.Bind();
		//bind Texture to blue/gray box
		Gray.Bind();
		//set uniform for texture
		shader.SetUniform1i("u_Texture", 0);
		
		// Draws Triangles
		glDrawElements(GL_TRIANGLES, gCube.nIndices, GL_UNSIGNED_SHORT, NULL);
		
		//Deactivate VAO
		glBindVertexArray(0);
	}
	//glfw swap buffers
	glfwSwapBuffers(gWindow);

}

//Create Pyramid mesh
void UPyramid(GLMesh& mesh) {
	// Vertex Data
	GLfloat verts[] =
	{

		// Positions       //Texture Coordinates
		 -0.5f, 0.0f,  0.5f,  0.0f, 0.0f, // Vertex 0
		 -0.5f, 0.0f, -0.5f,  1.0f, 0.0f, // Vertex 1
		  0.0f, 1.0f,  0.0f,  1.0f, 1.0f, // Vertex 2
		 -0.5f, 0.0f,  0.5f,  0.0f, 0.0f, // Vertex 3
		  0.5f, 0.0f,  0.5f,  1.0f, 0.0f, // Vertex 4
		  0.0f, 1.0f,  0.0f,  1.0f, 1.0f, // Vertex 5
		  0.5f, 0.0f,  0.5f,  0.0f, 0.0f, // Vertex 6
		  0.5f, 0.0f, -0.5f,  1.0f, 0.0f, // Vertex 7
		  0.0f, 1.0f,  0.0f,  1.0f, 1.0f, // Vertex 8
		 -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, // Vertex 9
		  0.5f, 0.0f, -0.5f,  1.0f, 0.0f, // Vertex 10
		  0.0f, 1.0f,  0.0f,  1.0f, 1.0f, // Vertex 11
		 -0.5f, 0.0f,  0.5f,  0.0f, 0.0f, // Vertex 12
		 -0.5f, 0.0f, -0.5f,  1.0f, 0.0f, // Vertex 13
		  0.5f, 0.0f, -0.5f,  1.0f, 1.0f, // Vertex 14
		  0.5f, 0.0f, -0.5f,  1.0f, 1.0f, // Vertex 15
		  0.5f, 0.0f,  0.5f,  0.0f, 1.0f, // Vertex 16
		 -0.5f, 0.0f,  0.5f,  0.0f, 0.0f, // Vertex 17
	};
	// Indices data
	GLushort indices[] = { 0, 1, 2,     // Triangle 1
						   3, 4, 5,     // Triangle 2
						   6, 7, 8,     // Triangle 3
						   9, 10, 11,   // Triangle 4
						   12, 13, 14,  // Triangle 5
						   15, 16, 17,  // Triangle 6
	};

	//Pyaramid Consts
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerColor = 4;
	const GLuint floatsPerUV = 2;

	// set number of Indices
	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV);

	//Generates and Binds Vertex Array
	glGenVertexArrays(1, &mesh.vao); 
	glBindVertexArray(mesh.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
	

	//bind Indices buffer buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
	//set buffer data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	
	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * floatsPerVertex ));
	glEnableVertexAttribArray(1);
}

//create Cube Mesh
void UCube(GLMesh& cube) {
	
	// Vertex Data
	GLfloat verts[] = {
		//Positions       //Texture Coordinates
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // cube face 1 (0)
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // cube face 1 (1)
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // cube face 1 (2)
	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // cube face 1 (3)

	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // cube face 2 (4)
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // cube face 2 (5)
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // cube face 2 (6)
	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // cube face 2 (7)

	   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // cube face 3 (8)
	   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // cube face 3 (9)
	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // cube face 3 (10)
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // cube face 3 (11) 

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // cube face 4 (12)
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // cube face 4 (13)
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // cube face 4 (14)
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // cube face 4 (15)

	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // cube face 5 (16)
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // cube face 5 (17)
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // cube face 5 (18)		
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // cube face 5 (19)

	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // cube face 6 (20)
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // cube face 6 (21)
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // cube face 6 (22)	
	   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // cube face 6 (23)
	};

	// Index Data
	GLushort indices[] = {
		0, 1, 2,  //cube distant face 1
		2, 3, 0,  //cube distant  face 1
		4, 5, 6,  //cube close face 2
		6, 7, 4,  //cube close face 2
		8, 9, 10, //cube left face 3
		10, 11, 8, //cube left face 3
		12, 13, 14, //cube right face 4
		14, 15, 12, //cube right  face 4
		16, 17, 18, //cube top face face 5
		18, 19, 16, //cube top face face 5
		20, 21, 22, //cube bottom face face 6
		22, 23, 20, //cube bottom face face 6
	};

	//Cube consts
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerColor = 4;
	const GLuint floatsPerUV = 2;

	//determine number of Indices
	cube.nIndices = sizeof(indices) / sizeof(indices[0]);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV);

	// Generates and binds Vertex Array
	glGenVertexArrays(1, &cube.vao); 
	glBindVertexArray(cube.vao);

	// Create VBO
	//generates vertex buffer
	glGenBuffers(2, cube.vbos);

	// binds Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, cube.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex coordinate data to the GPU

	//bind Indices buffer buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.vbos[1]);// Activates the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// Sends index coordinate data to the GPU
	
	
	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0); 
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
}

//create Plane 
void UPlane(GLMesh& Plane) {
	float verts[]{
		// Vertex Positions    // Colors (r,g,b,a)
		 -5.0f, 0.0f, -5.0f, 0.0f, 0.0f, // front left corner of plane (0)
		  5.0f, 0.0f, -5.0f, 1.0f, 0.0f, // front right corner of plane (1)
		  5.0f, 0.0f,  5.0f, 1.0f, 1.0f, // back right corner of plane (2)
		   5.0f, 0.0f,  5.0f, 1.0f, 1.0f, // back right corner of plane (2)
		 -5.0f, 0.0f,  5.0f, 0.0f, 1.0f, // back left corner of plane (3)
		 -5.0f, 0.0f, -5.0f, 0.0f, 0.0f, // front left corner of plane (0)

	};


	//Plane  Consts
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerColor = 4;
	const GLuint floatsPerUV = 2;

	// Generates and binds Vertex Array
	glGenVertexArrays(1, &Plane.vao); 
	glBindVertexArray(Plane.vao);

	// create 2 buffers
	glGenBuffers(2, Plane.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, Plane.vbos[0]);// Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);// Sends vertex coordinate data to the GPU


	//Create VAP
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
}


void UDestroyMesh(GLMesh& mesh, GLMesh& Cube, GLMesh& Plane) {
	//removes mesh VAO and VBO
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
	//Removes Cube VAO and VBO
	glDeleteVertexArrays(1, &Cube.vao);
	glDeleteBuffers(2, Cube.vbos);
	//Removes Plane VAO and VBO
	glDeleteVertexArrays(1, &Plane.vao);
	glDeleteBuffers(2, Plane.vbos);
}

