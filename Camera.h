#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

//Defines several possible options for camera movement. 
//Used as abstraction to stay away from window-system spcific input method
enum Camera_Movement {
	FORWARD, BACKWARD, LEFT, RIGHT
};

//default camera values
const float YAW = -90.0f;
//If we look at the x and z yaw triangle we see that a กำ of 0 result 
//in the camera's direction vector to point towards the positive x-axis.
//To make sure the camera points towards the negative z-axis(towards the object) by default, 
//we can give the yaw a value of a -90 degree rotate counter-clockwise
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 80.0f;

//An abstract camera class that processs input and calculates the 
//corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	//Camera Attributes
	glm::vec3 Position, Front, Up, Right, WorldUp;
	//Euler Angles
	float Yaw, Pitch;
	//Camera options
	float MovementSpeed, MouseSensitivity, Zoom/*fov*/;
	//fov : fow much we can see of the scene 
	//fov ก้ == scene's projected space ก้(give the illusion of zoom-in)

	//constructor with vectors
	Camera(glm::vec3 position = glm::vec3(.0f, .0f, .0f),
		glm::vec3 up = glm::vec3(.0f, 1.0f, .0f),
		float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(.0f, .0f, -1.0f)),
		MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	//constructor with scalar values
	Camera(float posX, float posY, float posZ,
		float upX, float upY, float upZ,
		float yaw, float pitch) : Front(glm::vec3(.0f, .0f, -1.0f)), // -z axis
		MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//keyboard input
	void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	//mouse input
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	//mouse-scroll input
	void ProcessMouseScroll(float yOffset)
	{
		Zoom -= (float)yOffset * 3/*to more faster*/;
		if (Zoom < 1.0f) Zoom = 0.0f;
		//if (Zoom > 90.0f) Zoom = 90.0f;
	}



private:
	//calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors() {
		//calculate the new Front vector
		glm::vec3 direction;
		direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		direction.y = sin(glm::radians(Pitch));
		direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(direction);
		//also re-calculate the Right and Up vector
		//normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif // !CAMERA_H

