
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

//Self Include
#include "GameScene.h"

//Local Includes
#include "Renderer.h"
#include "InputHandler.h"
#include "ThreadPool.h"
#include "IniParser.h"
#include "Task.h"

//Library Includes
#include <sstream>
#include <iomanip>

//Constructor
GameScene::GameScene()
{
	//Load array of pixel information
	m_pPixels = new Pixel*[Renderer::SCREEN_WIDTH];
	for (int i = 0; i < Renderer::SCREEN_WIDTH; i++)
	{
		m_pPixels[i] = new Pixel[Renderer::SCREEN_HEIGHT];
	}

	//Box vertices
	GLfloat vertices[] = {
		//Pos (x, y, z)			Uv(u, v)
		 1.0f,  1.0f, 0.0f,		0.0f, 1.0f, 	   //Top - Right
		-1.0f,  1.0f, 0.0f,		0.0f, 0.0f,	   //Top - Left
		-1.0f, -1.0f, 0.0f,		1.0f, 0.0f,	   //Bottem - Left
		 1.0f, -1.0f, 0.0f,		1.0f, 1.0f,	   //Bottem - Right
	};

	//Box Indices
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	//Size of each array
	unsigned int sizeV = sizeof(vertices) / sizeof(GLfloat);
	unsigned int sizeI = sizeof(indices) / sizeof(GLuint);

	//Calculate list count
	m_verticesCount = sizeV / 7;
	m_indicesCount = sizeI;
	
	//Load into dynamic variables
	m_pVertices = new GLfloat[sizeV];
	for (unsigned int i = 0; i < sizeV; i++)
	{
		m_pVertices[i] = vertices[i];
	}

	//Load into dynamic variables
	m_pIndices = new GLuint[sizeI];
	for (unsigned int i = 0; i < sizeI; i++)
	{
		m_pIndices[i] = indices[i];
	}

	//Render program using MandelBrotVert and MandelBrotFrag shaders
	m_program = Renderer::GetInstance().CreateProgram("MandelBrotVert.vs", "MandelBrotFrag.fs");
	
	//Create a Vertex Array Object
	m_VAO_ID = Renderer::GetInstance().CreateVAO(BUFFER_POS | BUFFER_TEX, sizeV * sizeof(GLfloat), sizeI * sizeof(GLuint), m_pVertices, m_pIndices);
	
	//Load text texture into memory
	m_text25Gen = Renderer::GetInstance().CreateTextGenerator("Exo-Bold.ttf", 15);

	//Set text color
	m_text25Gen->SetColour(glm::vec3(1.0, 0.0, 0.0));

	//Screen texture
	m_texture = Renderer::GetInstance().CreateBlankTexture("Render", Renderer::SCREEN_WIDTH, Renderer::SCREEN_HEIGHT);

	//Set up cursor boarder
	SetUpBorder();

	//Load file settings
	LoadSettings();

	//Start Thread pool
	ThreadPool::GetInstance().Initialize();
	ThreadPool::GetInstance().Start();
}

//Destructor
GameScene::~GameScene()
{
	for (int i = 0; i < Renderer::SCREEN_WIDTH; i++)
	{
		delete[] m_pPixels[i];
	}
	delete[] m_pPixels;
	m_pPixels = 0;

	delete[] m_pVertices;
	m_pVertices = 0;

	delete[] m_pIndices;
	m_pIndices = 0;

	//Does not own m_text25Gen
}

//	SetUpBorder( )
//
//	Author: Michael Jordan
//	Access: protected
//	Description:
//		Dynamically set up the VAO of the the ui border.
//		
//	Param:
//		- n/a	|	
//
//	Return: n/a		|	
//
void GameScene::SetUpBorder()
{
	//Note: Vertices are dynmically loaded based on position data.
	
	//Cursor indices
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0,

		4, 1, 6,
		6, 5, 4,

		8, 9, 6,
		6, 7, 9, 

		0, 10, 11,
		11, 7, 0,
	};

	//Create new Vertex Array Object
	glGenVertexArrays(1, &m_BorderVAO);
	glBindVertexArray(m_BorderVAO);

	//Size of each array
	unsigned int sizeI = sizeof(indices) / sizeof(GLuint);

	m_borderCount = sizeI;

	//Preload Indices
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Set up vertices acceptance...
	glGenBuffers(1, &m_BorderVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_BorderVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sizeI * 3, NULL, GL_DYNAMIC_DRAW);

	//Shader layouts:
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	//Clear
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//	Draw( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Renders all objects to the back buffer.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::Draw()
{
	//Load shader:
	glUseProgram(m_program);

	//Bind array
	glBindVertexArray(m_VAO_ID);

	//Change image color based on settings
	glUniform3fv(glGetUniformLocation(m_program, "Color"), 1, glm::value_ptr(m_pixelColor));

	//Change transform location
	glUniformMatrix4fv(glGetUniformLocation(m_program, "Transform"), 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	//Apply current texture to screen object
	m_texture->ApplyTexture(GL_TEXTURE0, m_program);

	//Draw screen object
	glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);

	//Draw Cursor Border
	DrawBorder();

	//Generate appropriate text
	std::stringstream temp;
	temp << std::fixed << std::setprecision(2) << m_currZoom << "x (" << std::setprecision(2) << m_zoom << ")";

	std::stringstream temp2;
	temp2 << std::fixed << std::setprecision(2) << m_tempPos.x << ", ";
	temp2 << std::fixed << std::setprecision(2) << m_tempPos.y;

	//Draw Text
	m_text25Gen->SetColour(glm::vec3(0, 0, 0));
	for (unsigned int i = 0; i < 2; i++)
	{
		if (i == 1)
		{
			m_text25Gen->SetColour(glm::vec3(200/255.0f, 160/255.0f, 0.0f));
		}
		m_text25Gen->RenderText("Zoom: " + temp.str(), glm::vec2(5 - i * 1, 5 + i * 1));
		m_text25Gen->RenderText("Pos: " + temp2.str(), glm::vec2(80 + temp.str().length() * 12 - i * 1, 5+ i * 1));
	}
	
}

//	DrawBorder( )
//
//	Author: Michael Jordan
//	Access: protected
//	Description:
//		Renders the ui border to the back buffer.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::DrawBorder()
{
	glDisable(GL_BLEND);
	glm::vec2 mousePos = InputHandler::GetInstance().GetMousePos();

	//Convert origin to center
	mousePos.x = (mousePos.x - (Renderer::SCREEN_WIDTH / 2)) / (Renderer::SCREEN_WIDTH / 2);
	mousePos.y = (-mousePos.y + (Renderer::SCREEN_HEIGHT / 2)) / (Renderer::SCREEN_HEIGHT / 2);

	//Load shader:
	glUseProgram(m_program);

	//Border VAO
	glBindVertexArray(m_BorderVAO);
	glm::mat4 transMat = glm::translate(glm::mat4(), glm::vec3(mousePos.x, mousePos.y, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(m_program, "Transform"), 1, GL_FALSE, glm::value_ptr(transMat));
	
	if (m_HasWorkSent)
	{
		glUniform3fv(glGetUniformLocation(m_program, "Color"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	}
	else
	{
		glUniform3fv(glGetUniformLocation(m_program, "Color"), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	
	float scale = 1.0f / (m_zoom / m_currZoom);

	//Calculate Edges
	float leftEdge = -(Renderer::SCREEN_WIDTH / 2.0f) * scale;
	float rightEdge = (Renderer::SCREEN_WIDTH / 2.0f) * scale;
	float sectWidth = (Renderer::SCREEN_WIDTH / 2.0f);

	//Calculate Edges
	float topEdge = (Renderer::SCREEN_HEIGHT / 2.0f) * scale;
	float botEdge = -(Renderer::SCREEN_HEIGHT / 2.0f) * scale;
	float sectHeight = (Renderer::SCREEN_HEIGHT / 2.0f);

	//pixel
	float width = 1 / (Renderer::SCREEN_WIDTH * 0.5f);
	float height = 1 / (Renderer::SCREEN_HEIGHT * 0.5f);

	//Default locations
	GLfloat vertices[] = {
		rightEdge / sectWidth,  topEdge / sectHeight,            0.0f, //[0] Top - Right
	    leftEdge / sectWidth,  topEdge / sectHeight,             0.0f, //[1] Top - Left
	    leftEdge / sectWidth,  (topEdge / sectHeight) - height,  0.0f, //[2] Top Border Bottem - Left
		rightEdge / sectWidth,  (topEdge / sectHeight) - height, 0.0f, //[3] Top Border Bottem - Right

		(leftEdge / sectWidth) + width, topEdge / sectHeight,    0.0f, //[4] Left border Top - Right
		(leftEdge / sectWidth) + width, botEdge / sectHeight,    0.0f, //[5] Left border Bottem - Right
	    leftEdge / sectWidth,  botEdge / sectHeight,             0.0f, //[6] Bottem - Left
		rightEdge / sectWidth,  botEdge / sectHeight,            0.0f, //[7] Bottem - Right

		leftEdge / sectWidth,  (botEdge / sectHeight) + height,  0.0f, //[8]Bottem border Top - Left
		rightEdge / sectWidth,  (botEdge / sectHeight) + height, 0.0f, //[9]Bottem border Top - Right

		(rightEdge / sectWidth) - width,  topEdge / sectHeight,  0.0f, //[10] Right Border Top - Right
		(rightEdge / sectWidth) - width,  botEdge / sectHeight - height, 0.0f, //[11] Right Border Bottem - Right
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_BorderVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawElements(GL_TRIANGLES, m_borderCount, GL_UNSIGNED_INT, 0);

	//Clear useage
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glEnable(GL_BLEND);
}

//	Process( dt )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Process the information of each object inbetween frames.
//		
//	Param:
//		- float		|	Change in time since last call.
//
//	Return: n/a		|
//
void GameScene::Process(float dT)
{
	if (!m_HasWorkSent)
	{
		float scale = 1.0f / m_currZoom;
		float stepPerPixel = (Renderer::SCREEN_WIDTH * scale) /(Renderer::SCREEN_WIDTH);
		SendTasks(Renderer::SCREEN_WIDTH * scale, Renderer::SCREEN_HEIGHT * scale, scale);
	}
	else
	{
		if (m_IsTiming)
		{
			if (!ThreadPool::GetInstance().HasItemsRemaining())
			{
				m_endWork = std::chrono::high_resolution_clock::now();
				auto time = std::chrono::duration_cast<std::chrono::microseconds>(m_endWork - m_startWork).count();
				float seconds = time / 1000000.0f;
				m_IsTiming = false;
				std::cout << "[MAIN THREAD]: Tasks finished! Total Time Taken: " << seconds << " seconds." << std::endl;

				unsigned char* pixels = new unsigned char[Renderer::SCREEN_WIDTH * Renderer::SCREEN_HEIGHT * 4];
				for(unsigned int i = 0; i < Renderer::SCREEN_WIDTH; i++)
				{
					for (unsigned int j = 0; j < Renderer::SCREEN_HEIGHT; j++)
					{
						pixels[((i * Renderer::SCREEN_HEIGHT + j) * 4) + 0] = 0xFF;//(!m_pPixels[i][j].IsDivergent) ? 0xFF : 0x00;
						pixels[((i * Renderer::SCREEN_HEIGHT + j) * 4) + 1] = 0xFF; //(!m_pPixels[i][j].IsDivergent) ? 0xFF : 0x00;
						pixels[((i * Renderer::SCREEN_HEIGHT + j) * 4) + 2] = 0xFF; //(!m_pPixels[i][j].IsDivergent) ? 0xFF : 0x00;
						pixels[((i * Renderer::SCREEN_HEIGHT + j) * 4) + 3] = m_pPixels[i][j].alpha; //(!m_pPixels[i][j].IsDivergent) ? 0xFF : 0x00;
					}
				}
				Renderer::GetInstance().SetTexture("Render", pixels);
				delete[] pixels;
			}
		}
	}
}

//	SendTasks( width,  height, stepPerPixel )
//
//	Author: Michael Jordan
//	Access: protected
//	Description:
//		Updates each pixel information with new complex numbers,
//		then sends divided tasks to the threadpool.
//		
//	Param:
//		- float		|	Change in time since last call.
//
//	Return: n/a		|
//
void GameScene::SendTasks(float width, float height, float stepPerPixel)
{
	//Mandlebrot constants
	float ReMin = 0.0f;
	float ReMax = 4.0f;
	float ImMin = -0.0f;
	float ImMax = 4.0f;

	//Calcuate the offset
	float originX = m_origin.x * (Renderer::SCREEN_WIDTH / 2.0f);
	float originY = m_origin.y * (Renderer::SCREEN_HEIGHT / 2.0f);

	std::cout << "[MAIN THREAD]: Generating Data..." << std::endl;

	//Update Pixel information
	for (int i = 0; i < Renderer::SCREEN_WIDTH; i++)
	{
		for (int j = 0; j < Renderer::SCREEN_HEIGHT; j++)
		{
			//Calculate the current cordinates at pixel location.
			float x = (-width / 2.0f) + originX + (i * stepPerPixel);
			float y = (height / 2.0f) + originY - (j * stepPerPixel);

			//Complex number for this pixel
			m_pPixels[i][j].c = Complex(
				(ReMin + (x / (float)(width)) * (ReMax - ReMin)) * stepPerPixel,
				(ImMin + (y / (float)(height)) * (ImMax - ImMin)) * stepPerPixel
			);

			//Reset iteration variables
			m_pPixels[i][j].IsDivergent = false;
			m_pPixels[i][j].alpha = 0xFF;
		}
	}

	//Get Divisions from the file.
	const int divisions = IniParser::GetInstance().GetValueAsInt("Tread", "WorkDivisionsCount");

	//Get Limit and accuracy from the file.
	CTask::LIMIT = IniParser::GetInstance().GetValueAsInt("Tread", "IterateLimit");

	//Get colour information from the file.
	m_pixelColor.r = IniParser::GetInstance().GetValueAsInt("Colour", "R") / 255.0f;
	m_pixelColor.g = IniParser::GetInstance().GetValueAsInt("Colour", "G") / 255.0f;
	m_pixelColor.b = IniParser::GetInstance().GetValueAsInt("Colour", "B") / 255.0f;

	//Safety checks:
	assert(divisions > 0);
	assert(CTask::LIMIT != 0);

	//Get reference to ThreadPool
	ThreadPool& threadPool = ThreadPool::GetInstance();

	int sizeX = (Renderer::SCREEN_WIDTH) / divisions;
	int sizeY = (Renderer::SCREEN_HEIGHT) / divisions;

	std::cout << "[MAIN THREAD]: Sending Data Packets..." << std::endl;

	//// The main thread writes items to the WorkQueue
	for (int i = 0; i < divisions + 1; i++)
	{
		//X coordinate start
		int startX = i * sizeX;

		for (int j = 0; j < divisions + 1; j++)
		{
			//Y coordinate start
			int startY = j * sizeY;
			
			//Send task
			threadPool.Submit(CTask(startX, startY, sizeX, sizeY));
		}
	}

	//Start Timer
	m_startWork = std::chrono::high_resolution_clock::now();

	//Update conditions
	m_IsTiming = true;
	m_HasWorkSent = true;
}

//	LoadSettings( )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Loads the settings.ini file if it exists. If it doesn't exist,
//		it will be created.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::LoadSettings()
{
	//Load file
	if (!IniParser::GetInstance().LoadIniFile("Resources/Files/Settings.ini"))
	{
		//If file doesn't exist, create it:
		IniParser::GetInstance().CreateIniFile("Resources/Files/Settings.ini");

		IniParser::GetInstance().AddNewValue("Tread", "ThreadCount", "8");
		IniParser::GetInstance().AddNewValue("Tread", "WorkDivisionsCount", "20");
		IniParser::GetInstance().AddNewValue("Tread", "IterateLimit", "30");

		IniParser::GetInstance().AddNewValue("Colour", "R", "255");
		IniParser::GetInstance().AddNewValue("Colour", "G", "255");
		IniParser::GetInstance().AddNewValue("Colour", "B", "255");

		IniParser::GetInstance().AddNewValue("Window", "Size", "900");

		IniParser::GetInstance().SaveIniFile();
	}
}

//	OnMousePositionChange( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Communicated with the scene manager to be prompt this scene that a mouse has moved.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::OnMousePositionChange()
{
	if (m_HasWorkSent)
	{
		float scale = 1.0f / m_currZoom;
		float width = Renderer::SCREEN_WIDTH * scale;
		float height = Renderer::SCREEN_HEIGHT * scale;

		
		m_tempPos = InputHandler::GetInstance().GetMousePos();

		//Convert origin to center
		m_tempPos.x = ((m_tempPos.x - (Renderer::SCREEN_WIDTH / 2)) / (Renderer::SCREEN_WIDTH / 2));
		m_tempPos.y = ((-m_tempPos.y + (Renderer::SCREEN_WIDTH / 2)) / (Renderer::SCREEN_WIDTH / 2));
	}
}

//	OnMouseButtonChange( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Communicated with the scene manager to be prompt this scene that a mouse 
//		button has changed state.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::OnMouseButtonChange()
{
	if (m_HasWorkSent)
	{
		if (InputHandler::GetInstance().IsMousePressed(GLUT_LEFT_BUTTON))
		{
			m_origin += (m_tempPos) / m_currZoom;
			m_currZoom = glm::clamp(m_zoom, 1.0f, m_zoomMax);
			m_HasWorkSent = false;
		}
	}
}

//	OnMouseWheelChange( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Communicated with the scene manager to be prompt this scene that a mouse 
//		wheel has changed state.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::OnMouseWheelChange()
{
	if (m_HasWorkSent)
	{
		int dir = InputHandler::GetInstance().GetMouseWheelDir(0);
		
		if(dir == -1 || m_zoom < m_currZoom)
		{
			m_zoom = glm::clamp(m_zoom + dir * m_currZoom * 0.125f, 1.0f, m_zoomMax);
		}
		else
		{
			m_zoom = glm::clamp(m_zoom + dir * m_currZoom * 0.5f, 1.0f, m_zoomMax);
		}
	}
}

//	OnKeyChange( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Communicated with the scene manager to be prompt this scene that a key 
//		has changed state.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::OnKeyChange()
{
	if (InputHandler::GetInstance().IsKeyPressedFirst('r') || InputHandler::GetInstance().IsKeyPressedFirst('R'))
	{
		//Restart
		m_currZoom = 1.0f;
		m_zoom = 1.0f;
		m_tempPos = glm::vec2(0.0f, 0.0f);
		m_origin = glm::vec2(0.0f, 0.0f);
		m_HasWorkSent = false;
	}

	if (InputHandler::GetInstance().IsKeyPressed('z') || InputHandler::GetInstance().IsKeyPressed('Z'))
	{
		m_zoomRate = 0.5f * m_currZoom;
	}
	else
	{
		m_zoomRate = m_currZoom;
	}
}

//	OnSpecialChange( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Communicated with the scene manager to be prompt this scene that a key 
//		has changed state.
//		
//	Param:
//		- n/a		|
//
//	Return: n/a		|
//
void GameScene::OnSpecialChange()
{
	//Do nothing
}

//	GetPixel( i, j )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Gets a reference to the pixel at screen location ( i, j ).
//		
//	Param:
//		- n/a		|
//
//	Return: Pixel&	|	reference to ( i, j ) pixel.
//
Pixel& GameScene::GetPixel(int i, int j)
{
	return m_pPixels[i][j];
}
