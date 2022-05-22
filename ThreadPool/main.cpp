//Dependeny Include
#include <glew.h>
#include <freeglut.h>

//Library Includes
#include <iostream>
//#include <vld.h>

#include "ThreadPool.h"
#include "WorkQueue.h"
#include "Task.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "Renderer.h"
#include "InputHandler.h"
#include "iniParser.h"

//Close everything
void ShutDown()
{
	//Delete and clear all singleton variables.
	SceneManager::EmptyScenes();
	InputHandler::DestroyInstance();
	Renderer::DestroyInstance();
	ThreadPool::DestroyInstance();
}

//Force size of the window
void Resize(int width, int height)
{
	glutReshapeWindow(Renderer::SCREEN_WIDTH, Renderer::SCREEN_HEIGHT);
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(0));

	GameScene::LoadSettings();

	Renderer::SCREEN_WIDTH = IniParser::GetInstance().GetValueAsInt("Window", "Size");
	Renderer::SCREEN_HEIGHT = IniParser::GetInstance().GetValueAsInt("Window", "Size");

	//Start openGl
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(Renderer::SCREEN_WIDTH, Renderer::SCREEN_HEIGHT);
	glutCreateWindow("OpenGL Window");

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR 001: GLEW Initialization has failed.";
	}

	//Create window:
	SceneManager::PushScene(new GameScene());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glutDisplayFunc(SceneManager::Draw);
	glutIdleFunc(SceneManager::Process);

	glutMotionFunc(SceneManager::OnMouseMove);
	glutPassiveMotionFunc(SceneManager::OnMouseMove);

	glutMouseFunc(SceneManager::OnMouseClick);
	glutMouseWheelFunc(SceneManager::OnMouseWheel);

	glutKeyboardFunc(SceneManager::OnKeyPressed);
	glutKeyboardUpFunc(SceneManager::OnKeyReleased);
	glutSpecialFunc(SceneManager::OnSpecialPressed);
	glutSpecialUpFunc(SceneManager::OnSpecialReleased);
	
	glutReshapeFunc(Resize);
	glutCloseFunc(ShutDown);

	glutMainLoop();

	return 0;
}