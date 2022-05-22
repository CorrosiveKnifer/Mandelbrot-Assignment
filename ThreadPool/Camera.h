#ifndef _CAMERA_H_
#define _CAMERA_H_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	Camera.h
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	Camera
// Description:
//		Camera is preset to a type based on being perspective or orthographic.
//		Used to store the camera's view and projection matrix.
//

//Local Includes
#include "Renderer.h" //<--Includes Dependenies

//Enums:
enum CamType { Perspective, Orthographic };

struct Proj
{
	unsigned int width, height;
	float nearDist, farDist, POV;
};

class Camera
{
	//Member Functions:
public:
	Camera(Camera::CamType type);
	~Camera();

	//Sets
	void SetView(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
	void SetProjection(Proj projection);
	void SetPOV(float pov);

	//Gets
	const glm::mat4 GetCamMatrix();
protected:

private:
	Camera();

	void CalculateViewMatrix();
	void CalculateProjection();
	//Member Data:
public:

protected:
	CamType m_type;
	glm::mat4 m_viewMat, m_projMat;

	//Projection
	Proj m_currentProj;

	//View
	glm::vec3 m_pos;
	glm::vec3 m_forward;
	glm::vec3 m_up;
private:

};


#endif // !_CAMERA_H_
