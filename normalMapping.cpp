
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
// GLM Include 
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

// include shader header
#include "shader.h"
// include camera header
#include "camera.h" 
// include texture header
#include "texture.h"
// include model header
#include "model.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); //!< callback function for Key. 
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos); //!< call back function for mouse movement. needs x and y pos
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset); //!< call back function for mouse scrollwheel. needs x and y offset

void do_movement();//!< Movement

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;//!<Window Width and Height
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;

bool firstMouseMove = true;//!<mouse move
bool keyPressedStatus[1024]; //!<key status
GLfloat deltaTime = 0.0f; //!<time start at 0
GLfloat lastFrame = 0.0f;//!<frames start at 0 
Camera camera(glm::vec3(0.0f, 1.8f, 4.0f));//!<camera position
glm::vec3 lampPos(0.5f, 1.5f, 0.8f);//!< Lamp light possition
bool bNormalMapping = false;//!<Normal mapping set to false 
Model objModel; //!< loads the model


int main(int argc, char** argv)
{

	if (!glfwInit())	
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl; //!<desplays error message 
		return -1; 
	} //!< If cant initilise GLFW 

	// OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl; //!<Log message
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//window
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Advanced Shaders. Normal Mapping, Blinn - Phong and Model Loading", NULL, NULL); //!<create window 
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl; //!< desplay error message
		glfwTerminate(); //!< terminated the winow
		return -1;
	}//!< if there is not window then ...
	
	glfwMakeContextCurrent(window);//!< 	
	glfwSetKeyCallback(window, key_callback); //!< key callback
	glfwSetCursorPosCallback(window, mouse_move_callback);//!< coursor position callnacl
	glfwSetScrollCallback(window, mouse_scroll_callback);	//!< scroll wheel callback
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //!< input mode for cursor disabled

	
	glewExperimental = GL_TRUE; 

	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);//!<  view port for window

	// Model path
	std::ifstream modelPath("modelPath.txt");//!< text file for model path
	if (!modelPath)
	{
		std::cerr << "Error::could not read model path file." << std::endl; //!< Error message
		glfwTerminate(); //!< Terminate GLFW
		std::system("pause"); //!< Pause the ststem
		return -1;
	} //!< correct load model

	std::string modelFilePath; // set model filepath
	std::getline(modelPath, modelFilePath); //!< get model path and filepath
	if (!objModel.loadModel(modelFilePath))
	{
		glfwTerminate(); //!< terminate load model
		std::system("pause"); //!< pause the system
		return -1;
	}//!< if model doesnt load

	// Textures
	//GLuint diffuseMap = TextureHelper::load2DTexture("../../resources/textures/brickwall.jpg"); //!< diffuse map texture file path
	//GLuint normalMap = TextureHelper::load2DTexture("../../resources/textures/brickwall_normal.jpg");//!< normal map texture file path


	// Shaders
	Shader shader("scene.vertex", "scene.frag"); //!< load shader files 

	glEnable(GL_DEPTH_TEST);//!< enables GL_DEPTH_TEST
	
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime(); //!< Current frame
		deltaTime = currentFrame - lastFrame; //!< calculate time
		lastFrame = currentFrame; 
		glfwPollEvents(); //!< GLFW event poller
		do_movement(); //!< movement

		// clear the window
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f); //!< set the window colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //!< clear colour and depth buffer

		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); //!< Camera projection 
		glm::mat4 view = camera.getViewMatrix(); //!< camera view

		// using the shader
		shader.use();
		
		GLint lightAmbientLoc = glGetUniformLocation(shader.programId, "light.ambient"); //!<  creates ambiant location
		GLint lightDiffuseLoc = glGetUniformLocation(shader.programId, "light.diffuse"); //!< creates diffuse location
		GLint lightSpecularLoc = glGetUniformLocation(shader.programId, "light.specular"); //!< creates specular location
		GLint lightPosLoc = glGetUniformLocation(shader.programId, "light.position"); //!< creates light possition location

		glUniform3f(lightAmbientLoc, 0.3f, 0.3f, 0.3f); //!< ambiant light location
		glUniform3f(lightDiffuseLoc, 0.6f, 0.6f, 0.6f); //!< diffuse light location
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);  //!< specular light location
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z); //!< light position
		
		GLint viewPosLoc = glGetUniformLocation(shader.programId, "viewPos"); //!< view position
		glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z); //!< camera position
		 
		lightPosLoc = glGetUniformLocation(shader.programId, "lightPos"); //!< light position location for light
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);  //!< light position
		
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection)); //!< projection matrix
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view)); //!< view matrix
		glm::mat4 model; //!< model metrix
		
		//model = glm::rotate(model, (GLfloat)glfwGetTime() * -2, glm::normalize(glm::vec3(0.0, 1.0, 0.0))); //!< model rotation
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model)); //!< gets model matrix
		glUniform1i(glGetUniformLocation(shader.programId, "normalMapping"), bNormalMapping);
		//!< gets normal map 
		objModel.draw(shader); //!< draws the shader model

		std::cout << "using normal mapping " << (bNormalMapping ? "true" : "false") << std::endl; //!< message for using normal mapping

		glBindVertexArray(0); //!< binds vertex array
		glUseProgram(0); //!<  uses program
		glfwSwapBuffers(window); //!< swaps buffers
	}
	
	glfwTerminate(); //!< terminates GLFW
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyPressedStatus[key] = true; //!< set pressed key status to true when key is pressed
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;//!< set pressed key status to false when key is released
	} //!< if keycode is in range
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); //call close the window fucntion
	} //!< if escape key is pressed
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		bNormalMapping = !bNormalMapping; //!< turn normal mapping on and off
	} //!< if N key is pressed

} //!< keyboard callback

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) 
	{
		lastX = xpos; //!< last x coordinate
		lastY = ypos; //!< last y coordinate
		firstMouseMove = false; //!< disables first mouse movement
	}//!< If there is first mouse movement then...

	GLfloat xoffset = lastX - xpos; //!< get x offset
	GLfloat yoffset = lastY - ypos; //!< get y offset

	lastX = xpos; //!< set last x to be x position
	lastY = ypos; //!< set last y to be y position

	camera.handleMouseMove(xoffset, yoffset); //!<camera handles mouse movement
}//!< mouse movement callback

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset); //1< camera handles mouse scroll 
} //!< mouse scroll callback // comment
// movement
     

void do_movement()
{

	if (keyPressedStatus[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime); //!< move forward when W is pressed
	if (keyPressedStatus[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime); //!< move bacwards when S is pressed
	if (keyPressedStatus[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime); //!< move left when A is pressed
	if (keyPressedStatus[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime); //!< move right when D is pressed
} //!< Do movement for camera 