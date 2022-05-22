#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	GameScene.h
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	GameScene
// Description:
//		Contains main game loop.
//

//Parent Include
#include "SceneADT.h"
#include "Complex.h"
#include "TextGenerator.h"
#include "Texture.h"

//Library Includes
#include <Chrono>

struct Pixel
{
	Complex c = Complex(0.0f, 0.0f);
	bool IsDivergent = true;
	unsigned char alpha = 0xFF;
};

class GameScene : public SceneADT
{
	//Member Functions:
public:
	GameScene();
	virtual ~GameScene();

	static void LoadSettings();

	virtual void Draw();
	virtual void Process(float dT);
	virtual void OnMousePositionChange();
	virtual void OnMouseButtonChange();
	virtual void OnMouseWheelChange();
	virtual void OnKeyChange();
	virtual void OnSpecialChange();

	Pixel& GetPixel(int i, int j);
	
protected:
	void SetUpBorder();

	void DrawBorder();

	void SendTasks(float width, float height, float stepPerPixel);

private:
	
	//Member Data:
public:

protected:
	std::chrono::high_resolution_clock::time_point m_startWork;
	std::chrono::high_resolution_clock::time_point m_endWork;

	bool m_HasWorkSent;
	bool m_IsTiming;
	
	GLuint m_program;
	GLuint m_VAO_ID;
	GLuint m_BorderVAO;
	GLuint m_BorderVBO;

	GLfloat* m_pVertices;
	GLuint* m_pIndices;

	TextGenerator* m_text25Gen;

	unsigned int m_verticesCount;
	unsigned int m_indicesCount;

	unsigned int m_borderCount;

	float m_currZoom;
	float m_zoom;
	float m_zoomRate;

	glm::vec3 m_pixelColor;

	glm::vec2 m_origin;
	glm::vec2 m_tempPos;
	Pixel** m_pPixels;

	Texture* m_texture;
private:
};

#endif // !_GAME_SCENE_H_