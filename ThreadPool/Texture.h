#ifndef _TEXTURE_H_
#define _TEXTURE_H_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	Texture.h
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com),
// Class(es)	|	Texture
// Description:
//		A class which handles the binding and displaying of a Texture.
//

//Dependanies include
#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Texture
{
	//Member Functions:
public:
	Texture(unsigned int width, unsigned int height, GLuint ID) 
	{
		m_width = width;
		m_height = height;
		m_textureID = ID;
	};

	Texture(const Texture& origin) 
	{
		this->m_width = origin.m_width;
		this->m_height = origin.m_height;
		this->m_textureID = origin.m_textureID;
	};

	~Texture() {};

	void ApplyTexture(GLenum  slot, GLuint program)
	{
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glUniform1i(glGetUniformLocation(program, "ImageTexture"), 0);
	};

	//Get Functions
	glm::vec2 GetSize() { return glm::vec2(m_width, m_height); };
	GLuint GetID() { return m_textureID;  };

protected:

private:

	//Member Data:
public:

protected:
	unsigned int m_width;
	unsigned int m_height;
	GLuint m_textureID;

private:

};

#endif // !_TEXTURE_H_