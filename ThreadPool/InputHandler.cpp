//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	InputHandler.cpp
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	InputHandler
// Description:
//		A singleton class in charge of tracking the current state of each key 
//		on the standard keyboard and the mouse buttons/position.
//

//Parent Include
#include "InputHandler.h"

//Static Variable
InputHandler* InputHandler::sm_pInstance = 0;

//Default Constructor
InputHandler::InputHandler()
{
	m_mousePos = glm::vec2(0, 0);
	for (unsigned int i = 0; i < 255; i++)
	{
		m_keyboard[i] = BUTTON_UP;
	}
}

//Destructor
InputHandler::~InputHandler()
{

}

//	GetInstance( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Get function for the only instance of this class. If the instance doesn't
//		exist then it will make one.
//		
//	Param:
//		- n/a
//
//	Return: InputHandler&	|	Single instance of the InputHandler
//
InputHandler& InputHandler::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new InputHandler();
	}
	return *sm_pInstance;
}

//	DestroyInstance( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Destroys the single instance of this class
//		
//	Param:
//		- n/a
//
//	Return: n/a		|
//
void InputHandler::DestroyInstance()
{
	if (sm_pInstance != 0)
	{
		delete sm_pInstance;
		sm_pInstance = 0;
	}
}

//	OnKeyPressDown( key, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the key pressed to a down state.
//		Note: character 'a' is considered different to 'A'.
//		
//	Param:
//		- unsigned char |	key being pressed down.
//
//	Return: n/a		|
//
void InputHandler::OnKeyPressDown(unsigned char key)
{
	//Switch on which key pressed:
	switch (m_keyboard[key])
	{
	case BUTTON_UP:
		m_keyboard[key] = BUTTON_DOWN_FIRST; //First call of down
		break;

	case BUTTON_DOWN_FIRST:
		m_keyboard[key] = BUTTON_DOWN; //After first call of down
		break;

	default:
	case BUTTON_DOWN:
		//Do Nothing
		break;
	}
}

//	OnKeyReleased( key, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the key released to a down state.
//		Note: character 'a' is considered different to 'A'.
//		
//	Param:
//		- unsigned char |	key being pressed down.
//
//	Return: n/a		|
//
void InputHandler::OnKeyReleased(unsigned char key)
{
	m_keyboard[key] = BUTTON_UP;
}

//	OnMouseClick( button, state, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the mouse button to a down state.
//		
//	Param:
//		- int		|	Button used on the mouse.
//		- state		|	Current status of the button.
//
//	Return: n/a		|
//
void InputHandler::OnMouseClick(int button, int state)
{
	if (button >= 3)
		return; //Invalid

	m_mouse[button] = (state == GLUT_DOWN) ? BUTTON_DOWN : BUTTON_UP;
}

//	OnMouseWheel( wheel, dir, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the mouse wheel direction state.
//		
//	Param:
//		- int		|	Wheel used on the mouse.
//		- int		|	Direction, +1 = forward or -1 backwards.
//
//	Return: n/a		|
//
void InputHandler::OnMouseWheel(int wheel, int dir)
{
	bool found = false;

	//Search for wheel (In rare case more wheels are present).
	for (unsigned int i = 0; i < m_mouseWheels.size(); i++)
	{
		if (m_mouseWheels.at(i).first == wheel)
		{
			m_mouseWheels.at(i).second = dir;
			found = true;
		}
	}

	//If not found, add to list of mouse wheels
	if (!found)
	{
		m_mouseWheels.push_back(std::pair<unsigned int, int>(wheel, dir));
	}
}

//	OnMouseMotion(x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the position of the mouse when it is moved.
//		
//	Param:
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void InputHandler::OnMouseMotion(int x, int y)
{
	m_mousePos = glm::vec2(x, y);
}

//	IsKeyReleased(key)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Check if the key has been released.
//		Note: character 'a' is considered different to 'A'.
//		
//	Param:
//		- unsigned char		|	key being checked.
//		
//	Return: bool	|	Status of the key being released
//
const bool InputHandler::IsKeyReleased(unsigned char key)
{
	return m_keyboard[key] == BUTTON_UP;
}

//	IsKeyPressed(key)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Check if the key has been pressed.
//		Note: character 'a' is considered different to 'A'.
//		
//	Param:
//		- unsigned char		|	key being checked.
//		
//	Return: bool	|	Status of the key being pressed
//
const bool InputHandler::IsKeyPressed(unsigned char key)
{
	return m_keyboard[key] == BUTTON_DOWN || m_keyboard[key] == BUTTON_DOWN_FIRST;
}

//	IsKeyPressedFirst(key)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Check if the key has been pressed for the first time.
//		Note: character 'a' is considered different to 'A'.
//		
//	Param:
//		- unsigned char		|	key being checked.
//		
//	Return: bool	|	Status of the key being pressed
//
const bool InputHandler::IsKeyPressedFirst(unsigned char key)
{
	return m_keyboard[key] == BUTTON_DOWN_FIRST;
}

//	IsMousePressed(button)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Check if the mouse button has been pressed.
//		
//	Param:
//		- int		|	mouse button being checked.
//		
//	Return: bool	|	Status of the button being pressed
//
const bool InputHandler::IsMousePressed(int button)
{
	if (button >= 3) 
		return false;
	return m_mouse[button] == BUTTON_DOWN;
}

//	IsMouseReleased(button)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Check if the mouse button has been released.
//		
//	Param:
//		- int		|	mouse button being checked.
//		
//	Return: bool	|	Status of the button being released
//
const bool InputHandler::IsMouseReleased(int button)
{
	if (button >= 3) 
		return false;
	return m_mouse[button] == BUTTON_UP;
}

//	GetMouseWheelDir( wheel)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Gets the current directional state of a mouse wheel.
//		
//	Param:
//		- int		|	Wheel on the mouse (Default = 0). 
//
//	Return: n/a		|
//
const int InputHandler::GetMouseWheelDir(int wheel)
{
	//Search for wheel
	for (unsigned int i = 0; i < m_mouseWheels.size(); i++)
	{
		if (m_mouseWheels.at(i).first == wheel)
		{
			return m_mouseWheels.at(i).second;
		}
	}

	//Not found, no dir.
	return 0;
}

//	GetMousePos()
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Returns the current mouse position
//		
//	Param:
//		- n/a		|
//		
//	Return: glm::vec2	|	Mouse Position
//
const glm::vec2 InputHandler::GetMousePos()
{
	return m_mousePos;
}

//	OnSpecialPressed( key )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the special key to the pressed state.
//		
//	Param:
//		- int		|	Special key which is being pressed. 
//
//	Return: n/a		|
//
void InputHandler::OnSpecialPressed(int key)
{
	ButtonState* specLoc = GetSpecialLoc(key);

	if (specLoc != 0)
	{
		switch (*specLoc)
		{
		case BUTTON_UP:
			*specLoc = BUTTON_DOWN_FIRST; //First call of down
			break;

		case BUTTON_DOWN_FIRST:
			*specLoc = BUTTON_DOWN; //After first call of down
			break;

		default:
		case BUTTON_DOWN:
			//Do Nothing
			break;
		}
	}
}

//	OnSpecialReleased( key )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the special key to the released state.
//		
//	Param:
//		- int		|	Special key which is being released. 
//
//	Return: n/a		|
//
void InputHandler::OnSpecialReleased(int key)
{
	ButtonState* specLoc = GetSpecialLoc(key);

	if (specLoc != 0)
	{
		*specLoc = BUTTON_UP;
	}
}

//	IsSpecialReleased( key )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Checks a special key if it has been released.
//		
//	Param:
//		- int		|	Special key which is being released. 
//
//	Return:	bool	|	status of the special key being released.
//
const bool InputHandler::IsSpecialReleased(int key)
{
	ButtonState* specLoc = GetSpecialLoc(key);

	if (specLoc != 0)
	{
		return *specLoc == BUTTON_UP;
	}
	
	return false;
}

//	IsSpecialPressed( key )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Checks a special key if it has been pressed.
//		
//	Param:
//		- int		|	Special key which is being pressed. 
//
//	Return: bool	|	status of the special key being pressed.
//
const bool InputHandler::IsSpecialPressed(int key)
{
	ButtonState* specLoc = GetSpecialLoc(key);

	if (specLoc != 0)
	{
		return *specLoc == BUTTON_DOWN || *specLoc == BUTTON_DOWN_FIRST;
	}

	return false;
}

//	IsSpecialPressedFirst( key )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Checks a special key if it has been pressed for the first time.
//		
//	Param:
//		- int		|	Special key which is being pressed for the first time. 
//
//	Return: bool	|	status of the special key being pressed for the first time.
//
const bool InputHandler::IsSpecialPressedFirst(int key)
{
	ButtonState* specLoc = GetSpecialLoc(key);

	if (specLoc != 0)
	{
		return *specLoc == BUTTON_DOWN_FIRST;
	}

	return false;
}

//	GetSpecialLoc( key )
//
//	Author: Michael Jordan
//	Access: protected
//	Description:
//		Private function to locate the internal location of a special key.
//		Only F1 to F12 and Directional keys are supported.
//		
//	Param:
//		- int		|	Special key to find. 
//
//	Return: ButtonState*	|	Special Key.
//
ButtonState* InputHandler::GetSpecialLoc(int key)
{
	if (key >= GLUT_KEY_F1 && key <= GLUT_KEY_F12)
	{
		return &m_specialFunc[key];

	}

	switch (key)
	{
	case GLUT_KEY_LEFT:
		return &m_specialArrow[0];

	case GLUT_KEY_RIGHT:
		return &m_specialArrow[1];

	case GLUT_KEY_UP:
		return &m_specialArrow[2];

	case GLUT_KEY_DOWN:
		return &m_specialArrow[3];

	default:
		//Rest is not supported.
		break;
	}

	return 0;
}