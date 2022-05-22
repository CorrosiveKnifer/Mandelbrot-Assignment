#ifndef _INPUTHANDLER_H_
#define _INPUTHANDLER_H_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	InputHandler.h
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	InputHandler
// Description:
//		A singleton class in charge of tracking the current state of each key 
//		on the standard keyboard and the mouse buttons/position.
//

//Local Includes
#include "Renderer.h"

//Library Includes
#include <vector>

//Enums
enum ButtonState
{
	BUTTON_UP, BUTTON_DOWN, BUTTON_DOWN_FIRST
};

class InputHandler
{
	//Member Functions:
public:
	static InputHandler& GetInstance();
	static void DestroyInstance();

	//GLManager Func
	void OnKeyPressDown(unsigned char key);
	void OnKeyReleased(unsigned char key);
	void OnSpecialPressed(int key);
	void OnSpecialReleased(int key);
	void OnMouseClick(int button, int state);
	void OnMouseWheel(int wheel, int dir);
	void OnMouseMotion(int x, int y);

	const bool IsKeyReleased(unsigned char key);
	const bool IsKeyPressed(unsigned char key);
	const bool IsKeyPressedFirst(unsigned char key);
	const bool IsSpecialReleased(int key);
	const bool IsSpecialPressed(int key);
	const bool IsSpecialPressedFirst(int key);
	const bool IsMousePressed(int button);
	const bool IsMouseReleased(int button);
	const int GetMouseWheelDir(int wheel = 0);
	const glm::vec2 GetMousePos();

protected:
	ButtonState* GetSpecialLoc(int key);
private:
	InputHandler();
	~InputHandler();

	//Member Data:
public:

protected:

private:
	static InputHandler* sm_pInstance;

	ButtonState m_specialFunc[12] = { BUTTON_UP };
	ButtonState m_specialArrow[4] = { BUTTON_UP };
	ButtonState m_keyboard[255] = { BUTTON_UP };
	ButtonState m_mouse[3] = { BUTTON_UP };
	glm::vec2 m_mousePos;

	std::vector<std::pair<unsigned int, int>> m_mouseWheels;
};

#endif // !_INPUTHANDLER_H_