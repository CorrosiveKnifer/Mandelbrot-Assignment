//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	TextGenerator.cpp
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	TextGenerator
// Description:
//		Text Generator which loads .ttf information into characters.
//		Then can generate strings onto the screen.
//

//Self Includes
#include "TextGenerator.h"

//Local Includes
#include "Renderer.h"

//Constructor
TextGenerator::TextGenerator(std::string Font, unsigned int PixelSize)
{
	m_isFunctioning = true;

	std::string fontLoc("Resources/Fonts/");
	fontLoc += Font;

	m_fontName = Font;
	m_fontSize = PixelSize;
	m_color = glm::vec3(1.0f, 1.0f, 1.0f);

	//Create projection and program
	m_projMat = glm::ortho(0.0f, (float)Renderer::SCREEN_WIDTH, 0.0f, (float)Renderer::SCREEN_HEIGHT);
	m_renderProgram = Renderer::GetInstance().CreateProgram("Text.vs", "Text.fs");

	//FreeType Library:
	FT_Library Library;
	FT_Face Face;

	// Initialise library
	if (FT_Init_FreeType(&Library) != FT_Err_Ok)
	{
		std::cout << "ERROR: Freetype library error." << std::endl;
		m_isFunctioning = false;
		return;
	}

	//Load the font and create the face
	if (FT_New_Face(Library, fontLoc.c_str(), 0, &Face) != FT_Err_Ok)
	{
		std::cout << "ERROR: Freetype face error." << std::endl;
		m_isFunctioning = false;
		return;
	}

	FT_Set_Pixel_Sizes(Face, (FT_UInt)0, (FT_UInt)m_fontSize); //Set Pixel Size
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//Disable byte-alignment restriction.

	for (GLubyte CharIndex = 0; CharIndex < FontCharacterLimit; CharIndex++)
	{
		if (FT_Load_Char(Face, CharIndex, FT_LOAD_RENDER) != FT_Err_Ok)
		{
			std::cout << "ERROR: Freetype Glyph error: " << CharIndex << std::endl;
			continue;
		}
		m_characters[CharIndex] = {
			GenerateTexture(&Face),
			glm::ivec2(Face->glyph->bitmap.width, Face->glyph->bitmap.rows),
			glm::ivec2(Face->glyph->bitmap_left, Face->glyph->bitmap_top),
			(GLuint)Face->glyph->advance.x >> 6,
		};
	}

	FT_Done_Face(Face);
	FT_Done_FreeType(Library);

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	//Generate VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Create static EBO
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Create dynamic VBO
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	//Clear binds
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

//Deconstructor
TextGenerator::~TextGenerator()
{

}

//	RenderText( face )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Generates the Glyph information for each character's face.
//		
//	Param:
//		- FT_Face* 	|	Text to generate.
//
//	Return: GLuint	|	Texture ID	
//
void TextGenerator::RenderText(std::string text, glm::vec2 position)
{
	if (!m_isFunctioning)
		return;

	glUseProgram(m_renderProgram);
	glUniform3fv(glGetUniformLocation(m_renderProgram, "TextColor"), 1, glm::value_ptr(m_color));
	glUniformMatrix4fv(glGetUniformLocation(m_renderProgram, "ProjectionMat"), 1, GL_FALSE, glm::value_ptr(m_projMat));
	
	glBindVertexArray(m_VAO);

	glm::vec2 LocalPos = position;
	for (std::string::const_iterator iter = text.begin(); iter != text.end(); iter++)
	{
		Character* glyph = &m_characters[*iter];
		GLfloat xPos = LocalPos.x + glyph->Bearing.x;
		GLfloat yPos = LocalPos.y - (glyph->Size.y - glyph->Bearing.y);
		GLfloat width = static_cast<GLfloat>(glyph->Size.x);
		GLfloat height = static_cast<GLfloat>(glyph->Size.y);

		//Create data for VBO
		GLfloat vertices[4][4] = {
			{xPos, yPos + height, 0.0f, 0.0f},
			{xPos, yPos, 0.0f, 1.0f},
			{xPos + width, yPos, 1.0f, 1.0f},
			{xPos + width, yPos + height, 1.0f, 0.0f},
		};

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glyph->TextureID);
		glUniform1i(glGetUniformLocation(m_renderProgram, "TextTexture"), 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		LocalPos.x += glyph->Advance;
	}

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

//	GenerateTexture( face )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Generates the Glyph information for each character's face.
//		
//	Param:
//		- FT_Face* 	|	Text to generate.
//
//	Return: GLuint	|	Texture ID	
//
GLuint TextGenerator::GenerateTexture(FT_Face* face)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLsizei width = (*face)->glyph->bitmap.width;
	GLsizei height = (*face)->glyph->bitmap.rows;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, (*face)->glyph->bitmap.buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}
