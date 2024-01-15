// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Solar System", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	GLuint TextureSun = loadDDS("2k_sun.DDS");
	GLuint TextureMer = loadDDS("2k_mercury.DDS");
	GLuint TextureVen = loadDDS("2k_venus.DDS");
	GLuint TextureEar = loadDDS("2k_earth.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Read .obj file
	std::vector<glm::vec3> verticesSun;
	std::vector<glm::vec2> uvsSun;
	std::vector<glm::vec3> normalsSun; // Won't be used at the moment.
	bool resSun = loadOBJ("sphere.obj", verticesSun, uvsSun, normalsSun);

	std::vector<glm::vec3> verticesMer;
	std::vector<glm::vec2> uvsMer;
	std::vector<glm::vec3> normalsMer; // Won't be used at the moment.
	bool resMer = loadOBJ("sphere.obj", verticesMer, uvsMer, normalsMer); 

	std::vector<glm::vec3> verticesVen;
	std::vector<glm::vec2> uvsVen;
	std::vector<glm::vec3> normalsVen; // Won't be used at the moment.
	bool resVen = loadOBJ("sphere.obj", verticesVen, uvsVen, normalsVen); 

	std::vector<glm::vec3> verticesEar;
	std::vector<glm::vec2> uvsEar;
	std::vector<glm::vec3> normalsEar; // Won't be used at the moment.
	bool resEar = loadOBJ("sphere.obj", verticesEar, uvsEar, normalsEar);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, verticesSun.size() * sizeof(glm::vec3), &verticesSun[0], GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, verticesMer.size() * sizeof(glm::vec3), &verticesMer[0], GL_STATIC_DRAW);

	GLuint vertexbufferVen;
	glGenBuffers(1, &vertexbufferVen);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferVen);
	glBufferData(GL_ARRAY_BUFFER, verticesVen.size() * sizeof(glm::vec3), &verticesVen[0], GL_STATIC_DRAW);

	GLuint vertexbufferEar;
	glGenBuffers(1, &vertexbufferEar);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferEar);
	glBufferData(GL_ARRAY_BUFFER, verticesEar.size() * sizeof(glm::vec3), &verticesEar[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvsSun.size() * sizeof(glm::vec2), &uvsSun[0], GL_STATIC_DRAW);

	glm::vec3 centerOfRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	float orbitRadius = 10.0f; // Distance from the first object
	float orbitSpeed = 0.00015f; // Speed of rotation
	float orbitAngle = 0.0f; // Initial angle

	float rotationAngle = 0.0f;
	float rotationSpeed = 0.01f; 


	float rotationAngleMer = 0.0f;
	float rotationSpeedMer = 0.01f; 


	float rotationAngleVen = 0.0f;
	float rotationSpeedVen = 0.01f; 

	float rotationAngleEar = 0.41f;
	float rotationSpeedEar = 0.005f; 


	float orbitSpeedVen = 0.00011f;
	float orbitRadiusVen = 15.0f; 
	float orbitAngleVen = 0.0f; 

	float orbitSpeedEar = 0.00009f; 
	float orbitRadiusEar = 10.0f; 
	float orbitAngleEar = 0.0f; 

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Common setup for both spheres
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();


		// Sun

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureSun);
		glUniform1i(TextureID, 0);

		orbitAngle += orbitSpeed;
		if (orbitAngle > 360.0f) orbitAngle -= 360.0f;

		// Compute the orbit position
		float x = centerOfRotation.x + orbitRadius * cos(orbitAngle);
		float z = centerOfRotation.z + orbitRadius * sin(orbitAngle);
		glm::vec3 orbitPosition = glm::vec3(x, centerOfRotation.y, z);

		rotationAngle += rotationSpeed;
		if (rotationAngle > 360.0f) rotationAngle -= 360.0f; // Loop back to 0 degrees

		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		// Create a model matrix, apply scaling and then rotation
		glm::mat4 ModelMatrix1 = glm::mat4(1.0f);
		ModelMatrix1 = glm::scale(ModelMatrix1, scale);
		// Apply rotation around a chosen axis, e.g., Y-axis here
		ModelMatrix1 = glm::rotate(ModelMatrix1, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

		// Calculate MVP matrix
		glm::mat4 MVPSun = getProjectionMatrix() * getViewMatrix() * ModelMatrix1;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPSun[0][0]);


		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, verticesSun.size());


		// Mercury
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureMer); // Bind the second texture
		glUniform1i(TextureID, 0);

		glm::vec3 scale2 = glm::vec3(0.15f, 0.15f, 0.15f); // Scale
		glm::mat4 ModelMatrix2 = glm::scale(glm::mat4(1.0f), scale2);

		// Translation
		ModelMatrix2 = glm::translate(ModelMatrix2, orbitPosition);

		// Set the MVP matrix 
		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, verticesMer.size());


		// Venus

		orbitAngleVen += orbitSpeedVen;
		if (orbitAngleVen > 360.0f) orbitAngleVen -= 360.0f;

		float xVen = centerOfRotation.x + orbitRadiusVen * cos(orbitAngleVen);
		float zVen = centerOfRotation.z + orbitRadiusVen * sin(orbitAngleVen);
		glm::vec3 orbitPositionVen = glm::vec3(xVen, centerOfRotation.y, zVen);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureVen); // Bind the 3rd texture
		glUniform1i(TextureID, 0);


		glm::vec3 scaleVen = glm::vec3(0.25f, 0.25f, 0.25f); 
		glm::mat4 ModelMatrixVen = glm::scale(glm::mat4(1.0f), scaleVen);

		// Translation 
		ModelMatrixVen = glm::translate(ModelMatrixVen, orbitPositionVen);

		// Set the MVP matrix
		glm::mat4 MVPVen = ProjectionMatrix * ViewMatrix * ModelMatrixVen;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPVen[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferVen);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, verticesVen.size());


		// Earth

		rotationAngleEar += rotationSpeedEar;
		if (rotationAngleEar > 360.0f) rotationAngleEar -= 360.0f; // Loop back to 0 degrees

		orbitAngleEar += orbitSpeedEar;
		if (orbitAngleEar > 360.0f) orbitAngleEar -= 360.0f;

		float xEar = centerOfRotation.x + orbitRadiusEar * cos(orbitAngleEar);
		float zEar = centerOfRotation.z + orbitRadiusEar * sin(orbitAngleEar);
		glm::vec3 orbitPositionEar = glm::vec3(xEar, centerOfRotation.y, zEar);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureEar); // Bind the 4th texture
		glUniform1i(TextureID, 0);

		glm::vec3 scaleEar = glm::vec3(0.25f, 0.25f, 0.25f);

		glm::mat4 ModelMatrixEar = glm::translate(glm::mat4(1.0f), orbitPositionEar);
		ModelMatrixEar = glm::rotate(ModelMatrixEar, glm::radians(rotationAngleEar), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrixEar = glm::scale(ModelMatrixEar, scaleEar);

		glm::mat4 MVPEar = ProjectionMatrix * ViewMatrix * ModelMatrixEar;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPEar[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferEar);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, verticesEar.size());


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureSun);
	glDeleteTextures(1, &TextureMer);
	glDeleteTextures(1, &TextureVen);
	glDeleteTextures(1, &TextureEar);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
