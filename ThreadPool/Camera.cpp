//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	Camera.cpp
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	Camera
// Description:
//		Camera is preset to a type based on being perspective or orthographic.
//		Used to store the camera's view and projection matrix.
//

//Self Include
#include "Camera.h"

//Default Constructor
Camera::Camera()
{
	m_type = CamType::Orthographic;

	//Default:
	SetView(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	//Default:
	m_currentProj.width = Renderer::SCREEN_WIDTH;
	m_currentProj.height = Renderer::SCREEN_HEIGHT;
	m_currentProj.nearDist = 0.1f;
	m_currentProj.farDist = 100.0f;
	m_currentProj.POV = 45.0f;

	CalculateViewMatrix();
	CalculateProjection();
}

//Main constructor
Camera::Camera(CamType type)
{
	m_type = type;
	
	//Default:
	SetView(
		glm::vec3(0.0f, 0.0f, 3.0f), 
		glm::vec3(0.0f, 0.0f, -1.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	//Default:
	m_currentProj.width = Renderer::SCREEN_WIDTH;
	m_currentProj.height = Renderer::SCREEN_HEIGHT;
	m_currentProj.nearDist = 0.1f;
	m_currentProj.farDist = 100.0f;
	m_currentProj.POV = 45.0f;

	CalculateViewMatrix();
	CalculateProjection();
}

//Destructor
Camera::~Camera()
{

}

//	SetView( position, forward, up)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates all variables associated with view matrix.
//		
//	Param:
//		- glm::vec3	|	Position of the camera in world space.
//		- glm::vec3	|	Forward vector of the camera in world space.
//		- glm::vec3	|	Up vector of the camera in world space.
//
//	Return: n/a		|	
//
void Camera::SetView(glm::vec3 position, glm::vec3 forward, glm::vec3 up)
{
	m_pos = position;
	m_forward = forward;
	m_up = up;

	//Update View Matrix
	CalculateViewMatrix();
}

//	SetProjection( projection)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the all projection variables if and only if the new POV is more than zero.
//		
//	Param:
//		- Proj		|	Projecion structure
//
//	Return: n/a		|	
//
void Camera::SetProjection(Proj projection)
{
	//Clamp above zero
	m_currentProj = (projection.POV > 0) ? projection : m_currentProj;

	//Update Projection matrix
	CalculateProjection();
}

//	SetPOV( pov )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the point of view if and only if the new POV is more than zero.
//		
//	Param:
//		- float		|	Point of view
//
//	Return: n/a		|	
//
void Camera::SetPOV(float pov)
{
	//Clamp above zero
	m_currentProj.POV = (pov > 0) ? pov : m_currentProj.POV;

	//Update Projection matrix
	CalculateProjection();
}

//	GetCamMatrix( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Returns the projection times the view matrix.
//		
//	Param:
//
//
//	Return: glm::mat4	|	Camera Matrix
//
const glm::mat4 Camera::GetCamMatrix()
{
	return m_projMat * m_viewMat;
}

//	CalculateViewMatrix( )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Updates the current view matrix.
//		
//	Param:
//
//
//	Return: n/a		|
//
void Camera::CalculateViewMatrix()
{
	m_viewMat = glm::lookAt(m_pos, m_pos + m_forward, m_up);
}

//	CalculateProjection( )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Updates the current projection matrix.
//		
//	Param:
//
//
//	Return: n/a		|
//
void Camera::CalculateProjection()
{
	float aspectRatio;
	float halfWidth, halfHeight;

	switch (m_type)
	{
	default:
	case Perspective:
		aspectRatio = (float)m_currentProj.width / (float)m_currentProj.height;
		m_projMat = glm::perspective(m_currentProj.POV, aspectRatio, m_currentProj.nearDist, m_currentProj.farDist);
		break;
	case Orthographic:
		halfWidth = m_currentProj.width / 2.0f;
		halfHeight = m_currentProj.height / 2.0f;
		m_projMat = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_currentProj.nearDist, m_currentProj.farDist);
		break;
	}
}