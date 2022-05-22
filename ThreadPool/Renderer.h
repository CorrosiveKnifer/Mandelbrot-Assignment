#ifndef _RENDERER_H_
#define _RENDERER_H_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	Renderer.h
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com), Callan Moore
// Class(es)	|	Renderer
// Description:
//		A class incharage of interacting with OpenGl to create programs, VAOs and textures.
//

// Dependency Includes
#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//Library Includes
#include <string>
#include <map>

//Bitwise
#define BUFFER_POS (1 << 0)
#define BUFFER_COL (1 << 1)
#define BUFFER_TEX (1 << 2)

//Forward Declaration
class Camera;
class TextGenerator;
class Texture;

//struct Texture
//{
//	Texture() { w = 0; h = 0; textureID = 0; };
//	int w, h;
//	GLuint textureID;
//};

class Renderer
{
	//Member Functions:
public:
	static Renderer& GetInstance();
	static void DestroyInstance();

	GLuint CreateProgram(const char* VertexShaderFilename, const char* FragmentShaderFilename);
	GLuint CreateVAO(uint32_t type, GLsizeiptr vertCount, GLsizeiptr indCount, GLfloat* vertData, GLuint* indData);
	Texture* CreateBlankTexture(const char* name, unsigned int width, unsigned int height);
	Texture* CreateTexture(const char* TextureFilename);
	void SetTexture(const char* textureName, unsigned char* pixelInfo);

	TextGenerator* CreateTextGenerator(std::string font, unsigned int fontSize);

	glm::mat4 GetCameraMatrix();

	static unsigned int GetVectorSize(uint32_t type);

protected:

private:
	Renderer();
	~Renderer();
	Renderer(const Renderer& renderer);
	Renderer& operator=(const Renderer& renderer);

	GLuint GetShader(const char* shaderName);
	Texture* GetTexture(const char* TextureFilename);
	TextGenerator* GetTextGenerator(std::string font, unsigned int fontSize);

	GLuint CreateShader(GLenum shaderType, const char* shaderName);
	GLuint CreateVBO(GLfloat* data, GLsizeiptr size);
	GLuint CreateEBO(GLuint* data, GLsizeiptr size);

	
	std::string ReadShaderFile(const char* filename);
	void PrintErrorDetails(bool isShader, GLuint id, const char* name);

	//Member Data:
public:
	static int SCREEN_WIDTH;
	static int SCREEN_HEIGHT;

protected:
	static Renderer* sm_pInstance;
	Camera* m_pMainCam;

private:
	std::map<std::string, GLuint> m_pShaders;
	std::map<std::string, Texture*> m_pTextures;
	std::map<std::string, TextGenerator*> m_TextGenerators;
	glm::vec3 worldScale;
};

#endif // !_RENDERER_H_