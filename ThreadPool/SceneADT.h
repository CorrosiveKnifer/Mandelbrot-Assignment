#ifndef _SCENE_H_
#define _SCENE_H_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	SceneADT.h
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	SceneADT (Abstract)
// Description:
//		An abstract data type for decending scene class that might be created.
//		This class allows the GLManager to switch between scene classes.
//

//Dependeny Includes
#include <glew.h>
#include <freeglut.h>

class SceneADT
{
public:
	//Destructor
	virtual ~SceneADT(){};

	//Abstract functions:
	virtual void Draw() = 0;
	virtual void Process(float dT) = 0;
	virtual void OnMousePositionChange() = 0;
	virtual void OnMouseButtonChange() = 0;
	virtual void OnMouseWheelChange() = 0;
	virtual void OnKeyChange() = 0;
	virtual void OnSpecialChange() = 0;

protected:
	//Default Constructor
	SceneADT(){};

private:

	//Member Data:
public:

protected:

private:

};

#endif // !_SCENE_H_
