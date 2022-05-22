//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	SceneManager.h
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	SceneManager
// Description:
//		Scene Manager that contains static functions to call SceneADT functions. 
//		This class also stores the Scene stack and is incharge of switching/freeing scenes.
//

//Parent Include
#include "SceneManager.h"

//Local Includes
#include "InputHandler.h"

//Static variables
float SceneManager::sm_prevTime = 0.0f;
std::stack<SceneADT*> SceneManager::m_pScenesStack;

//Default Constructor
SceneManager::SceneManager() 
{

}

//Destructor
SceneManager::~SceneManager()
{

}

//	PushScene( topScene)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Loads a the topScene provided onto the stack.
//		SceneManager takes ownership of the scene provided.
//		
//	Param:
//		- SceneADT*	|	Pointer to the new top scene.
//
//	Return: n/a		|	
//
void SceneManager::PushScene(SceneADT* topScene)
{
	m_pScenesStack.push(topScene);
	sm_prevTime = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
}

//	PopScene()
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Pops the top scene off the stack and frees it's memory.
//		
//	Param:
//		
//	Return: n/a		|	
//
SceneADT* SceneManager::PopScene(bool willDelete)
{
	SceneADT* temp = m_pScenesStack.top();
	m_pScenesStack.pop();

	if (willDelete)
	{
		delete temp; //Clear memory

		sm_prevTime = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
		return 0;
	}
	return temp;
}

//	EmptyScenes()
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Removes and frees all scenes on the stack.
//		
//	Param:
//		
//	Return: n/a		|	
//
void SceneManager::EmptyScenes()
{
	while (!m_pScenesStack.empty())
	{
		SceneADT* temp = m_pScenesStack.top();
		m_pScenesStack.pop();
		delete temp; //Clear memory
	}
}

//	Draw()
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Function in charge of rendering to the back buffer and switching buffers.
//		
//	Param:
//		
//	Return: n/a		|	
//
void SceneManager::Draw()
{
	//Clear BackBuffer
	glClear(GL_COLOR_BUFFER_BIT);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->Draw();

	//Switch Buffer
	glutSwapBuffers();
}

//	Process()
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Calculates the time since the last call and processes the next frame.
//		
//	Param:
//		
//	Return: n/a		|	
//
void SceneManager::Process()
{
	glutPostRedisplay();

	float currentTime = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	float dt = (currentTime - sm_prevTime);
	sm_prevTime = currentTime;

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->Process(dt);

	//InputHandler::GetInstance().Process(dt);
}

//	OnMouseMove(x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the position of the mouse in the inputHandler when it is moved,
//		and prompt the top scene that a mouse position changed.
//
//	Param:
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void SceneManager::OnMouseMove(int x, int y)
{
	InputHandler::GetInstance().OnMouseMotion(x, y);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->OnMousePositionChange();
}

//	OnMouseClick(button, state, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the mouse buttons' state in the inputHandler,
//		and prompt the top scene that a mouse button changed.
//		
//	Param:
//		- int		|	Button used on the mouse.
//		- state		|	Current status of the button.
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void SceneManager::OnMouseClick(int button, int state, int x, int y)
{
	InputHandler::GetInstance().OnMouseClick(button, state);
	InputHandler::GetInstance().OnMouseMotion(x, y);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->OnMouseButtonChange();
	
}

//	OnKeyPressed(key, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the key status in the InputHandler,
//		and prompt the top scene that a key changed.
//		
//	Param:
//		- unsigned char		|	Key pressed down.
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void SceneManager::OnKeyPressed(unsigned char key, int x, int y)
{
	InputHandler::GetInstance().OnKeyPressDown(key);
	InputHandler::GetInstance().OnMouseMotion(x, y);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->OnKeyChange();
}

//	OnKeyReleased(key, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the key status in the InputHandler,
//		and prompt the top scene that a key changed.
//		
//	Param:
//		- unsigned char		|	Key released.
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void SceneManager::OnKeyReleased(unsigned char key, int x, int y)
{
	InputHandler::GetInstance().OnKeyReleased(key);
	InputHandler::GetInstance().OnMouseMotion(x, y);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->OnKeyChange();
}

//	OnSpecialPressed(key, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the special key status in the InputHandler,
//		and prompt the top scene that a special key changed.
//		
//	Param:
//		- unsigned char		|	Special Key pressed.
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void SceneManager::OnSpecialPressed(int key, int x, int y)
{
	InputHandler::GetInstance().OnSpecialPressed(key);
	InputHandler::GetInstance().OnMouseMotion(x, y);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->OnSpecialChange();

}

//	OnSpecialReleased(key, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the special key status in the InputHandler,
//		and prompt the top scene that a special key changed.
//		
//	Param:
//		- unsigned char		|	Special Key released.
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void SceneManager::OnSpecialReleased(int key, int x, int y)
{
	InputHandler::GetInstance().OnSpecialReleased(key);
	InputHandler::GetInstance().OnMouseMotion(x, y);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->OnSpecialChange();
}

//	OnMouseWheel(wheel, dir, x, y)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Updates the mouse wheel status in the InputHandler,
//		and prompt the top scene that a mouse wheel changed.
//		
//	Param:
//		- int		|	Mouse wheel which updated, also handles more wheels.
//		- int		|	Direction of the wheel, +1 = forward, -1 = backwards.
//		- int		|	x coordinate of the mouse location.
//		- int		|	y coordinate of the mouse location.
//		
//	Return: n/a		|	
//
void SceneManager::OnMouseWheel(int wheel, int dir, int x, int y)
{
	InputHandler::GetInstance().OnMouseWheel(wheel, dir);
	InputHandler::GetInstance().OnMouseMotion(x, y);

	if (!m_pScenesStack.empty())
		m_pScenesStack.top()->OnMouseWheelChange();
}
