#include <iostream>
#include <cstdlib>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions
// glew and glfw libraries


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>
// Math Headers
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// camera header
#include "camera2.h"
// Shader program
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif
using namespace std;


// Namespace


namespace
{
	const char* const WINDOW_TITLE = "Module 5 Assignment";

	//Variables
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// GL Data
	struct GLMesh
	{
		GLuint vao;			//Vertex Array Object
		GLuint vbos[2];		//Vertex Buffer Object [2]
		GLuint nIndices;	//Indices
	};
	//GLFW Window
	GLFWwindow* gWindow = nullptr;
	// mesh data
	GLMesh gMesh;
	GLMesh gCube;
	GLMesh gBox;
	GLMesh gPlane;
	// texture program
	GLuint gTextureId;
	GLuint gTableTexture;
	GLuint gMetalTexture;
	GLuint gGrayTexture;
	glm::vec2 gUVScale(5.0f, 5.0f);
	// shader program
	GLuint gProgramId;
	GLuint gLampProgramId;
	// camera
	Camera gCamera(glm::vec3(0.0f, 0.0f, 3.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;
	// perspective variable
	char perspective;
	glm::vec3 gCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;
	// Subject position and scale
	glm::vec3 gPyramidPosition(-3.0f, -0.1f, -1.0f);
	glm::vec3 gPyramidScale(2.0f, 1.5f,2.0f);
	glm::vec3 gPyramidRotation(3.0f, 3.0f, 1.0f);
	glm::vec3 gMetalBoxAPosition(0.0f, 1.0f, -2.0f);
	glm::vec3 gMetalBoxAScale(1.0f, 2.5f, 6.0f);
	glm::vec3 gMetalBoxARotation(0.0, 3.0f, 1.0f);
	glm::vec3 gMetalBoxBPosition(0.5f, 1.0f, -3.5f);
	glm::vec3 gMetalBoxBScale(0.5, 2.5f, 1.5f);
	glm::vec3 gMetalBoxBRotation(0.0f, 3.0f, 1.0f);
	glm::vec3 gPlanePosition(-3.0f, -0.1f, -1.0f);
	glm::vec3 gPlaneScale(2.0f, 1.5f, 2.0f);
	glm::vec3 gPlaneRotation(0.0, 1.0f, 0.0f);
	glm::vec3 gBoxPosition(2.5f, 2.9f, -3.0f);
	glm::vec3 gBoxScale(6.0f, 3.25f, 2.15f);
	glm::vec3 gBoxRotation(0.0f, 0.0f, -1.0f);
	// Cube and light color
	glm::vec3 gObjectColor(1.f, 0.2f, 0.0f);
	glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);

	// Light position and scale
	glm::vec3 gLightPosition(0.0f, 10.0f, 0.0f);
	glm::vec3 gLightScale(8.0f,0.5f,8.0f);
	glm::vec3 gLightRotation(0.0f, 0.0f, 1.0f);

	// Lamp animation
	bool gIsLampOrbiting = true;
}
/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // normals
out vec3 vertexFragmentPos;
out vec2 vertexTextureCoordinate; // texture coords



//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
	vertexFragmentPos = vec3(model * vec4(position, 1.0f));
	vertexNormal = mat3(transpose(inverse(model))) * normal; //gets normal vectors
	vertexTextureCoordinate = textureCoordinate;
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,

	in vec3 vertexNormal;
in vec3 vertexFragmentPos; // incoming fragment
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform sampler2D uTexture;
uniform vec2 uvScale;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;



void main()
{
	//Calculate Ambient lighting*/
	float ambientStrength = 0.5f; // Set ambient or global lighting strength
	vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

	  //Calculate Diffuse lighting*/
	vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
	vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
	float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
	vec3 diffuse = impact * lightColor; // Generate diffuse light color

	//Calculate Specular lighting*/
	float specularIntensity = 2.0f; // Set specular light strength
	float highlightSize = 10.0f; // Set specular highlight size
	vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
	vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
	//Calculate specular component
	float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
	vec3 specular = specularIntensity * specularComponent * lightColor;

	vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

	vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

	fragmentColor = vec4(phong, 1.0);
}
);
/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

		//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);
/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

	out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
	fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);
void flipImageVertically(unsigned char* image, int width, int height, int channels) {
	for (int j = 0; j < height / 2; ++j) {
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;
		for (int i = width * channels; i > 0; --i) {
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}
// define functions
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UCreateCube(GLMesh& cube);
void UCreateBox(GLMesh& box);
void UCreatePlane(GLMesh& Plane);
void UDestroyMesh(GLMesh& mesh, GLMesh& cube);
void URender();
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

int main(int argc, char* argV[]) {

	if (!UInitialize(argc, argV, &gWindow))
		return EXIT_FAILURE;
	
		//return EXIT_FAILURE;
	
	// create meshes
	UCreateMesh(gMesh);
	UCreateCube(gCube);
	UCreateBox(gBox);
	UCreatePlane(gPlane);
	// Create Shader
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;

	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
		return EXIT_FAILURE;
	// load texture

	const char* texFilename = "../res/textures/TableTop2.jpg";
	if (!UCreateTexture(texFilename, gTableTexture))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	glUseProgram(gProgramId);

	// We set the texture as texture unit 0
	glUniform1i(glGetUniformLocation(gProgramId, "uTextureBase"), 0);
	// We set the texture as texture unit 1
	glUniform1i(glGetUniformLocation(gProgramId, "uTextureExtra"), 1);
	const char* metalFilename = "../res/textures/metal.png";
	if (!UCreateTexture(metalFilename, gMetalTexture))
	{
		cout << "Failed to load texture " << metalFilename << endl;
		return EXIT_FAILURE;
	}
	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	glUseProgram(gProgramId);
	
	// We set the texture as texture unit 2
	glUniform1i(glGetUniformLocation(gProgramId, "uTextureBase"), 2);
	// We set the texture as texture unit 3
	glUniform1i(glGetUniformLocation(gProgramId, "uTextureExtra"), 3);

	const char* grayFilename = "../res/textures/Gray.jpg";
	if (!UCreateTexture(grayFilename, gGrayTexture))
	{
		cout << "Failed to load texture " << grayFilename << endl;
		return EXIT_FAILURE;
	}
	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	glUseProgram(gProgramId);

	// We set the texture as texture unit 5
	glUniform1i(glGetUniformLocation(gProgramId, "uTextureBase"), 4);
	// We set the texture as texture unit 4
	glUniform1i(glGetUniformLocation(gProgramId, "uTextureExtra"), 5);

	//sets background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// render loop
	while (!glfwWindowShouldClose(gWindow)) {
		// per-frame timing
		double currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;
		
		
		
		//input
		UProcessInput(gWindow);
		
		// Render
		glClear(GL_COLOR_BUFFER_BIT);
		
		URender();
		
		glfwPollEvents();

	}
	//Release Mesh
	
	//Release Shader

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
	//if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
		//if (perspective == 'p') {
			//perspective = 'o';
		//}
		//else if (perspective == 'o') {
			//perspective = 'p';
		//}
			
		//projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	
	//if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		//projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
}
// glfw window size change
void UResizeWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
// glfw mouse movements
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}
	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos;

	gLastX = xpos;
	gLastY = ypos;

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
// glfw window size change


// Render frame
void URender() {
	 //Lamp orbits around the origin
	//const float angularVelocity = glm::radians(45.0f);
	//if (gIsLampOrbiting)
	//{
	//	glm::vec4 newPosition = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gLightPosition, 1.0f);
	//	gLightPosition.x = newPosition.x;
	//	gLightPosition.y = newPosition.y;
	//	gLightPosition.z = newPosition.z;
	//}

	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Pyramid
	
	glBindVertexArray(gMesh.vao);
	//set shader
	glUseProgram(gProgramId);
	// Model Matrix
	glm::mat4 model = glm::translate(gPyramidPosition) * glm::rotate(0.0f, gPyramidRotation) * glm::scale(gPyramidScale);

	// camera/ view transformation
	glm::mat4 view = gCamera.GetViewMatrix();

	// Creates a perspective projection
	glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gProgramId, "view");
	GLint projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
	GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
	GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

	//bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gGrayTexture);

	// Draw Triangle
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nIndices);
	
	//Metal Box A
	
		glBindVertexArray(gBox.vao);
		//set shader
		glUseProgram(gProgramId);
		// Model Matrix
		glm::mat4 modelBoxA = glm::translate(gMetalBoxAPosition) * glm::rotate(0.0f, gMetalBoxARotation) * glm::scale(gMetalBoxAScale);
				
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelBoxA));
			

		//bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gMetalTexture);

		// Draw Triangle
		glDrawArrays(GL_TRIANGLES, 0, gBox.nIndices);
	
	
	//Metal Box B
	
		glBindVertexArray(gBox.vao);
		//set shader
		glUseProgram(gProgramId);
		// Model Matrix
		glm::mat4 modelBoxB = glm::translate(gMetalBoxBPosition) * glm::rotate(0.0f, gMetalBoxBRotation) * glm::scale(gMetalBoxBScale);

	

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelBoxB));



		//bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gMetalTexture);

		// Draw Triangle
		glDrawArrays(GL_TRIANGLES, 0, gBox.nIndices);
	
	//Plane
	
		glBindVertexArray(gPlane.vao);
		//set shader
		glUseProgram(gProgramId);
		// Model Matrix
		glm::mat4 modelPlane = glm::translate(gPlanePosition) * glm::rotate(0.0f, gPlaneRotation) * glm::scale(gPlaneScale);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPlane));

		//bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTableTexture);

		// Draw Triangle
		glDrawArrays(GL_TRIANGLES, 0, 6);
	
	//Gray Box
	glBindVertexArray(gBox.vao);
	// Set the shader to be used
	glUseProgram(gProgramId);

	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelGrayBox = glm::translate(gBoxPosition) * glm::rotate(10.0f, gBoxRotation) * glm::scale(gBoxScale);

	
	// Pass matrix data to the Lamp Shader program's matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGrayBox));
	
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gGrayTexture);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gBox.nIndices);
	
	// LAMP: draw lamp
	//----------------
	glBindVertexArray(gCube.vao);
	glUseProgram(gLampProgramId);

	//Transform the smaller cube used as a visual que for the light source
	model = glm::translate(gLightPosition) * glm::rotate(0.0f, gLightRotation) * glm::scale(gLightScale);

	// Reference matrix uniforms from the Lamp Shader program
	modelLoc = glGetUniformLocation(gLampProgramId, "model");
	viewLoc = glGetUniformLocation(gLampProgramId, "view");
	projLoc = glGetUniformLocation(gLampProgramId, "projection");

	// Pass matrix data to the Lamp Shader program's matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glDrawArrays(GL_TRIANGLES, 0, gCube.nIndices);
	// Deactivate the Vertex Array Object and shader program
	glBindVertexArray(0);
	glUseProgram(0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}
//Create Mesh
void UCreateMesh(GLMesh& mesh) {
	// Vertex Data
	GLfloat verts[] =
	{

		// Positions          // Color                  // Texture Coordinates
		 -0.5f, 0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Vertex 0
		 -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,// Vertex 1
		  0.0f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Vertex 2
		 -0.5f, 0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Vertex 3
		  0.5f, 0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Vertex 4
		  0.0f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Vertex 5
		  0.5f, 0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Vertex 6
		  0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Vertex 7
		  0.0f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Vertex 8
		 -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Vertex 9
		  0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Vertex 10
		  0.0f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Vertex 11
		 -0.5f, 0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Vertex 12
		 -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Vertex 13
		  0.5f, 0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Vertex 14
		  0.5f, 0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Vertex 15
		  0.5f, 0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // Vertex 16
		 -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Vertex 17
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerColor = 4;
	const GLuint floatsPerUV = 2;

	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Data for the indices
	GLushort indices[] = { 0, 1, 2,  // Triangle 1
						   3, 4, 5,  // Triangle 2
						   6, 7, 8,  // Triangle 3
						   9, 10, 11,  // Triangle 4
						   12, 13, 14,  // Triangle 5
						   15, 16, 17,  // Triangle 6
	};
	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);

}
//Create Cube
void UCreateCube(GLMesh& cube) {
	// Position and Color data
	GLfloat verts[] = {
		//Positions          //Normals
		// --------------------------------------
		//Back Face          //Negative Z Normals
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   //Front Face         //Positive Z Normals
	  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	  //Left Face          //Negative X Normals
	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 //Right Face         //Positive X Normals
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	 //Bottom Face        //Negative Y Normals
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	//Top Face           //Positive Y Normals
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;

	cube.nIndices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal));

	glGenVertexArrays(1, &cube.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(cube.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(1, cube.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, cube.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal);// The number of floats before each

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
}
//Create Box
void UCreateBox(GLMesh& box) {
	// Position and Color data
	GLfloat verts[] = {
		//Positions          //Normals
		// ------------------------------------------------------
		//Back Face          //Negative Z Normal  Texture Coords.
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	   //Front Face         //Positive Z Normal
	  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

	  //Left Face          //Negative X Normal
	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 //Right Face         //Positive X Normal
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 //Bottom Face        //Negative Y Normal
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	//Top Face           //Positive Y Normal
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	box.nIndices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerUV));

	glGenVertexArrays(1, &box.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(box.vao);

	// Create VBO
	glGenBuffers(2, box.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, box.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU


	// Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}
//Create Plane
void UCreatePlane(GLMesh& Plane) {
	float verts[]{
		// Vertex Positions    // Colors (r,g,b,a)
		 -5.0f, 0.0f, -5.0f, 0.0f, 0.0f, // front left corner of plane
		  5.0f, 0.0f, -5.0f, 1.0f, 0.0f, // front right corner of plane
		  5.0f, 0.0f,  5.0f, 1.0f, 1.0f, // back right corner of plane
		  5.0f, 0.0f,  5.0f, 1.0f, 1.0f, // back right corner of plane
		 -5.0f, 0.0f,  5.0f, 0.0f, 1.0f, // back left corner of plane
		 -5.0f, 0.0f, -5.0f, 0.0f, 0.0f // front right corner of plane

	};
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerColor = 4;
	const GLuint floatsPerUV = 2;
	glGenVertexArrays(1, &Plane.vao); //generate VAOs 
	glBindVertexArray(Plane.vao);

	// create 2 buffers
	glGenBuffers(2, Plane.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, Plane.vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	//Create VAP
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(2);
}
//destroy mesh
void UDestroyMesh(GLMesh& mesh, GLMesh& cube) {
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
	glDeleteVertexArrays(1, &cube.vao);
	glDeleteBuffers(2, cube.vbos);

}
/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

	

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}

void UDestroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}


// creates shaders
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId) {
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}
//destroy shader
void UDestroyShaderProgram(GLuint programId) {
	glDeleteProgram(programId);
}