//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	CTask.cpp
// Author(s)    |	Provided and updated by Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	CTask
// Description:
//		Task class to calculate the diverging/converging nature of pixels on the screen.
//		Each task is a section of the screen.
//

//Self Include
#include "Task.h"

//Library Include
#include <iostream>
#include <thread>

//Local Include
#include "SceneManager.h"
#include "GameScene.h"
#include "Complex.h"
#include "iniParser.h"
#include "Renderer.h"

//Static variables
unsigned int CTask::LIMIT = 30;

//Default Constructor
CTask::CTask()
	:m_startX(0), m_startY(0), m_sizeX(0), m_sizeY(0)
{

	
}

//Main Constructor
CTask::CTask(int _startXLoc, int _startYLoc, unsigned int _sizeX, unsigned int _sizeY)
	: m_startX(_startXLoc), m_startY(_startYLoc), m_sizeX(_sizeX), m_sizeY(_sizeY)
{
	
}

//Destructor
CTask::~CTask()
{

}

//Copy Constructor
CTask::CTask(const CTask& other)
{
	this->m_startX = other.m_startX;
	this->m_startY = other.m_startY;
	this->m_sizeX = other.m_sizeX;
	this->m_sizeY = other.m_sizeY;
}

//Equals operator
CTask& CTask::operator=(const CTask& other)
{
	this->m_startX = other.m_startX;
	this->m_startY = other.m_startY;
	this->m_sizeX = other.m_sizeX;
	this->m_sizeY = other.m_sizeY;

	return *this;
}

//Functor
void CTask::operator()() const
{
	//Sleep to simulate work being done
	GameScene* scene = dynamic_cast<GameScene*>(SceneManager::TopScene());
	for (unsigned int i = m_startX; i < m_startX + m_sizeX; i++)
	{
		//Exceeds pixel limit:
		if (i >= static_cast<unsigned int>(Renderer::SCREEN_WIDTH))
		{
			break;
		}

		for (unsigned int j = m_startY; j < m_startY + m_sizeY; j++)
		{
			if (j < static_cast<unsigned int>(Renderer::SCREEN_HEIGHT))
			{
				Pixel* p = &(scene->GetPixel(i, j));
				p->IsDivergent = Iterate(*p);
			}
			else
			{
				//Exceeds pixel limit:
				break;
			}
		}
	}
}

//	Iterate( _pixel )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Iterates until the provided pixel showes it's diverging/converging nature.
//		
//	Param:
//		- GameScene::Pixel&		|	pixel to iterate on.
//
//	Return: bool	|	True if the pixel is divergent.
//
bool CTask::Iterate(Pixel& _pixel) const
{
	Complex curr(0.0f, 0.0f);

	//Iterate for n times
	for (unsigned int i = 0; i < LIMIT; i++)
	{
		//Create a copy
		Complex temp(0.0f, 0.0f);
		temp.Plus(curr);

		//Iterate
		curr.Multi(curr); //Z_n *= Z_n
		curr.Plus(_pixel.c); //Z_n += c

		if (curr.Moduli() > 2.0f)
		{
			//Diverging too far
			_pixel.alpha = glm::floor(0xFF * i / (float)LIMIT);
			return true;
		}
	}
	return false;
}

//	Iterate( _pixel )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Returns the string representation of this task.
//		
//	Param:
//		- n/a	|
//
//	Return: std::string		|	string representation of this task.
//
std::string CTask::ToString()
{
	std::string output = "Task (" + std::to_string(m_startX) + ", " + std::to_string(m_startY) + ")";
	output += " Size: " + std::to_string(m_sizeX) + ", " + std::to_string(m_sizeY)+". ";
	return output;
}
