#ifndef __CTASK_H__
#define __CTASK_H__

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	CTask.h
// Author(s)    |	Provided and updated by Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	CTask
// Description:
//		Task class to calculate the diverging/converging nature of pixels on the screen.
//		Each task is a section of the screen.
//

#include <Windows.h>

#include "GameScene.h"

class CTask
{
public:
	CTask();
	CTask(int _startXLoc, int _startYLoc, unsigned int _sizeX, unsigned int _sizeY);

	~CTask();

	void operator()() const;

	std::string ToString();

	CTask(const CTask& other);
	CTask& operator=(const CTask& other);

	static unsigned int LIMIT;

private:
	

	bool Iterate(Pixel& _pixel) const;

	int m_startX, m_startY;
	unsigned int m_sizeX, m_sizeY;
};

#endif



