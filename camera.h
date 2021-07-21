#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <iostream>
#include <fstream>
#include <GLEW/glew.h>
//Include GLM header files
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iomanip>      // std::setprecision
//enume for camera movement
enum Camera_Movement {
	FORWARD, //!<  move FORWARD
	BACKWARD, //!<  move BACKWARD
	LEFT, //!<  move LEFT
	RIGHT //!<  move RIGHT
};

const GLfloat YAW = 0.0f; //!< Movement Yaw rotation
const GLfloat PITCH = 0.0f; //!< Movement Pitch rotation
const GLfloat SPEED = 5.0f; //!< Movement speed
const GLfloat MOUSE_SENSITIVTY = 0.05f; //!< mouse movement sensativity
const GLfloat MOUSE_ZOOM = 45.0f;  //!< mouse zoom in and out speed
const float  MAX_PITCH_ANGLE = 89.0f; //!< max allowed pitch angle

class Camera
{
public:
	Camera(glm::vec3 pos = glm::vec3(0.0, 0.0, 2.0), 
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), 
		GLfloat yaw = YAW, GLfloat pitch = PITCH) 
		:position(pos), forward(0.0, 0.0, -1.0), viewUp(up),
		moveSpeed(SPEED), mouse_zoom(MOUSE_ZOOM), mouse_sensitivity(MOUSE_SENSITIVTY),
		yawAngle(yaw), pitchAngle(pitch) //!< set camera vectors
	{
		this->updateCameraVectors(); //!< updates camera movements
	}
public:
	
	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->forward, this->viewUp); //!< camera view direction
	} //!< view matrix for camera

	
	void handleKeyPress(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->moveSpeed * deltaTime; //!< movement speed
		switch (direction)
		{
		case FORWARD:
			this->position += this->forward * velocity;
			break;
		case BACKWARD:
			this->position -= this->forward * velocity;
			break;
		case LEFT:
			this->position -= this->side * velocity;
			break;
		case RIGHT:
			this->position += this->side * velocity;
			break;
		default:
			break;
		} //!< switch  statement for camera movement
	}//!< key press handler
	
	void handleMouseMove(GLfloat xoffset, GLfloat yoffset)
	{
		
		xoffset *= this->mouse_sensitivity; //!< sets x offset to mouse sensativity
		yoffset *= this->mouse_sensitivity; //!< sets y offset to mouse sensativity

		this->pitchAngle += yoffset; //!< gets pitch angle
		this->yawAngle += xoffset; //!< gets yaw angle

		this->normalizeAngle(); //!< normalised angle
		this->updateCameraVectors(); //!< updated camera vector
	} //!< handles mouse movement
	
	void handleMouseScroll(GLfloat yoffset)
	{
		if (this->mouse_zoom >= 1.0f && this->mouse_zoom <= MOUSE_ZOOM)
			this->mouse_zoom -= this->mouse_sensitivity * yoffset;
		if (this->mouse_zoom <= 1.0f)
			this->mouse_zoom = 1.0f;
		if (this->mouse_zoom >= 45.0f)
			this->mouse_zoom = 45.0f;
	} //!< camera scroll handler
	
	void normalizeAngle()
	{
		if (this->pitchAngle > MAX_PITCH_ANGLE)
			this->pitchAngle = MAX_PITCH_ANGLE; 
		if (this->pitchAngle < -MAX_PITCH_ANGLE)
			this->pitchAngle = -MAX_PITCH_ANGLE;
		if (this->yawAngle < 0.0f)
			this->yawAngle += 360.0f;
	} //!< normalises angle

	void updateCameraVectors()
	{
		glm::vec3 forward;
		forward.x = -sin(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle));
		forward.y = sin(glm::radians(this->pitchAngle));
		forward.z = -cos(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle));
		this->forward = glm::normalize(forward); //!< normalise forward 
		
		glm::vec3 side;
		side.x = cos(glm::radians(this->yawAngle));
		side.y = 0;
		side.z = -sin(glm::radians(this->yawAngle));
		this->side = glm::normalize(side); //!< normalise side
	} //!< update camera movement
public:
	glm::vec3 forward,up, side, viewUp, position;  //!< camera movements, view and position
	GLfloat yawAngle, pitchAngle;  // rotation angles
	GLfloat moveSpeed, mouse_sensitivity, mouse_zoom;  //!< mouse and camera speed and mouse scroll
};

#endif