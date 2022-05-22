#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	SceneManager.h
// Author(s)    |	SceneManager Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	SceneManager
// Description:
//		Scene Manager that contains static functions to call
//		SceneADT functions. This class also stores the Scene stack and is incharge of
//		switching/freeing scenes.
//

//Local Includes
#include "SceneADT.h"

//Library Include
#include <stack>

class SceneManager
{
	//Static Functions:
public:
	//Stack management
	static void PushScene(SceneADT* topScene);
	static SceneADT* PopScene(bool willDelete = true);
	static void EmptyScenes();
	static SceneADT* TopScene() { return m_pScenesStack.top(); };

	//SceneADT functions:
	static void Draw();
	static void Process();
	static void OnMouseMove(int x, int y);
	static void OnMouseClick(int button, int state, int x, int y);
	static void OnKeyPressed(unsigned char key, int x, int y);
	static void OnKeyReleased(unsigned char key, int x, int y);
	static void OnSpecialPressed(int key, int x, int y);
	static void OnSpecialReleased(int key, int x, int y);
	static void OnMouseWheel(int wheel, int dir, int x, int y);

protected:

private:
	SceneManager();
	~SceneManager();

	//Static Data:
public:

protected:
	static std::stack<SceneADT*> m_pScenesStack;
	static float sm_prevTime;

private:

};

#endif // !_GLMANAGER_H_