//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	Renderer.cpp
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com), Callan Moore
// Class(es)	|	Renderer
// Description:
//		A class incharage of interacting with OpenGl to create programs, VAOs and textures.
//

//Parent Include
#include "Renderer.h"

//Local Includes
#include "Camera.h"
#include "TextGenerator.h"
#include "Texture.h"

//Library Includes
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include<iostream>
#include<fstream>
#include<vector>

//Static Variables
Renderer* Renderer::sm_pInstance = 0;
int Renderer::SCREEN_WIDTH = 400;
int Renderer::SCREEN_HEIGHT = 400;

//Default Constructor
Renderer::Renderer()
{
	m_pMainCam = new Camera(Orthographic);
	worldScale = glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT/ 2.0f, 1.0f);
}

//Destructor
Renderer::~Renderer()
{
	delete m_pMainCam;
	m_pMainCam = 0;

	std::map<std::string, Texture*>::iterator textureIter = m_pTextures.begin();
	while (textureIter != m_pTextures.end())
	{
		Texture* t = (*textureIter).second;
		delete t;
		textureIter = m_pTextures.erase(textureIter);
	}

	std::map<std::string, TextGenerator*>::iterator genIter = m_TextGenerators.begin();
	while (genIter != m_TextGenerators.end())
	{
		TextGenerator* t = (*genIter).second;
		delete t;
		genIter = m_TextGenerators.erase(genIter);
	}
}

//	GetInstance( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Get function for the only instance of this class. If the instance doesn't
//		exist then it will make one.
//		
//	Param:
//		- n/a	|
//
//	Return: Renderer&	|	Single instance of the Renderer
//
Renderer& Renderer::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new Renderer();
	}
	return *sm_pInstance;
}


//	DestroyInstance( )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Destroys the single instance of this class
//		
//	Param:
//		- n/a
//
//	Return: n/a		|
//
void Renderer::DestroyInstance()
{
	if (sm_pInstance != 0)
	{
		delete sm_pInstance;
		sm_pInstance = 0;
	}
}

//	CreateProgram( vertexShaderFilename , fragmentShaderFilename )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Creates, Attaches shaders to a program for graphical use. 
//
//	Param:
//		- const char*	|	vertex shader name (not location).
//		- const char* 	|	fragment shader name (not location).
//
//	Return: GLuint		|	Program ID
//
GLuint Renderer::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	GLuint program = 0;

	// Create the shaders from the filepath
	std::string vFile("Resources/Shaders/");
	vFile += vertexShaderFilename;
	std::string fFile("Resources/Shaders/");
	fFile += fragmentShaderFilename;

	//Creates shaders
	GLuint vShader(CreateShader(GL_VERTEX_SHADER, vFile.c_str()));
	GLuint fShader(CreateShader(GL_FRAGMENT_SHADER, fFile.c_str()));

	// Create the program handle, attach the shaders and link it
	program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		std::string programName = vertexShaderFilename + *fragmentShaderFilename;
		PrintErrorDetails(false, program, programName.c_str());
		return 0;
	}

	return program;
}

//	CreateShader( shaderType , shaderName )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Creates and compiles shader information. Will return shader if it already exists. 
//
//	Param:
//		- GLenum		|	Shader Type.
//		- const char* 	|	Shader location.
//
//	Return: GLuint		|	Shader ID
//
GLuint Renderer::CreateShader(GLenum shaderType, const char* shaderName)
{
	GLuint shaderID = GetShader(shaderName);

	if (shaderID != 0) //Shader exists?
	{
		return shaderID;
	}

	// Read the shader files and save the source code as strings
	std::string file = ReadShaderFile(shaderName);
	const GLchar* c_str = file.c_str();
	const GLint size = file.size();

	// Create the shader ID and create pointers for source code string and length
	shaderID = glCreateShader(shaderType);

	// Populate the Shader Object (ID) and compile
	glShaderSource(shaderID, 1, &c_str, &size);
	glCompileShader(shaderID);

	// Check for errors
	int compile_result = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result);
	if (compile_result == GL_FALSE)
	{
		PrintErrorDetails(true, shaderID, shaderName);
		return 0;
	}

	//Add to map
	m_pShaders.insert(std::pair<std::string, GLuint>(shaderName, shaderID));

	return shaderID;
}

//	ReadShaderFile( shaderName )
//
//	Author: Callan Moore (Provided with 3D Graphics Programming)
//	Access: private
//	Description:
//		Loads, compresses and prepares a shader file for openGl.
//
//	Param:
//		- const char* 	|	Shader location.
//
//	Return: std::string	|	string of shader source code.
//
std::string Renderer::ReadShaderFile(const char *filename)
{
	// Open the file for reading
	std::ifstream file(filename, std::ios::in);
	std::string shaderCode;

	// Ensure the file is open and readable
	if (!file.good()) {
		std::cout << "Cannot read file:  " << filename << std::endl;
		return "";
	}

	// Determine the size of of the file in characters and resize the string variable to accomodate
	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());

	// Set the position of the next character to be read back to the beginning
	file.seekg(0, std::ios::beg);
	// Extract the contents of the file and store in the string variable
	file.read(&shaderCode[0], shaderCode.size());
	file.close();

	return shaderCode;
}

//	PrintErrorDetails( isShader, id, name )
//
//	Author: Callan Moore (Provided with 3D Graphics Programming)
//	Access: private
//	Description:
//		Logs any errors within loading the shader information.
//
//	Param:
//		- boolean 		|	Boolean statement if the error is a shader.
//		- GLuint 		|	Id of the buffer.
//		- const char* 	|	Name of the buffer.
//
//	Return: n/a
//
void Renderer::PrintErrorDetails(bool isShader, GLuint id, const char* name)
{
	int infoLogLength = 0;
	// Retrieve the length of characters needed to contain the info log
	(isShader == true) ? glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength) : glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> log(infoLogLength);

	// Retrieve the log info and populate log variable
	(isShader == true) ? glGetShaderInfoLog(id, infoLogLength, NULL, &log[0]) : glGetProgramInfoLog(id, infoLogLength, NULL, &log[0]);
	std::cout << "Error compiling " << ((isShader == true) ? "shader" : "program") << ": " << name << std::endl;
	std::cout << &log[0] << std::endl;
}

//	GetShader( shaderName )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Returns the shader ID associated to the shader name. Returns 0 if the shader doesn't exist.
//
//	Param:
//		- const char* 	|	Shader name (not location).
//
//	Return: GLuint		|	Shader ID
//
GLuint Renderer::GetShader(const char* shaderName)
{
	std::map<std::string, GLuint>::iterator it;
	it = m_pShaders.find(shaderName);
	if (it != m_pShaders.end())
	{
		return (*it).second;
	}
	return 0;
}

//	CreateVAO( type,  vertCount, indCount, vertData, indData )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Creates a VAO, static VBO and EBO with the data provided.
//		Define which type using bitwise operators and {BUFFER_POS, BUFFER_COL, BUFFER_TEX}.
//
//	Param:
//		- uint32_t 		|	Bit type of the VAO.
//		- GLsizeiptr 	|	size of the vertices data.
//		- GLsizeiptr 	|	size of the indices data.
//		- GLfloat* 		|	pointer to the first vertices data.
//		- GLuint* 		|	pointer to the first indices data.
//
//	Return: GLuint		|	VAO ID
//
GLuint Renderer::CreateVAO(uint32_t type, GLsizeiptr vertCount, GLsizeiptr indCount,  GLfloat* vertData, GLuint* indData)
{
	GLuint bufferID;

	unsigned int vectSize = GetVectorSize(type);
	unsigned int vectOffset = 0;
	GLuint attributeID = 0;

	//Generate & bind VAO buffer
	glGenVertexArrays(1, &bufferID);
	glBindVertexArray(bufferID);

	//Generate & bind EBO
	GLuint EBO_ID = CreateEBO(indData, indCount);

	//Generate & bind VBO
	GLuint VBO_ID = CreateVBO(vertData, vertCount);

	//Position
	if (type & BUFFER_POS)
	{
		glVertexAttribPointer(attributeID, 3, GL_FLOAT, GL_FALSE, vectSize * sizeof(GLfloat), (GLvoid*)(vectOffset * sizeof(GLfloat)));
		glEnableVertexAttribArray(attributeID);
		vectOffset += 3;
		attributeID++;
	}

	//Color
	if (type & BUFFER_COL)
	{
		glVertexAttribPointer(attributeID, 4, GL_FLOAT, GL_FALSE, vectSize * sizeof(GLfloat), (GLvoid*)(vectOffset * sizeof(GLfloat)));
		glEnableVertexAttribArray(attributeID);
		vectOffset += 4;
		attributeID++;
	}
	
	//Texture
	if (type & BUFFER_TEX)
	{
		glVertexAttribPointer(attributeID, 2, GL_FLOAT, GL_FALSE, vectSize * sizeof(GLfloat), (GLvoid*)(vectOffset * sizeof(GLfloat)));
		glEnableVertexAttribArray(attributeID);
		vectOffset += 2;
		attributeID++;
	}
	
	return bufferID;
}

//	CreateVBO( data , size )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Generates, Binds and buffers vertices data into an VBO.
//
//	Param:
//		- GLfloat*		|	unsigned float array of each vertices.
//		- GLsizeiptr	|	size of the vertices array.
//
//	Return: GLuint		|	VBO buffer ID
//
GLuint Renderer::CreateVBO(GLfloat* data, GLsizeiptr size)
{
	GLuint bufferID;

	//Generate buffer
	glGenBuffers(1, &bufferID);

	//Bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);

	//Assign data to buffer
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	return bufferID;
}

//	CreateEBO( data , size )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Generates, Binds and buffers indice data into an EBO.
//
//	Param:
//		- GLuint*		|	unsigned integer array of each indice.
//		- GLsizeiptr	|	size of the indices array.
//
//	Return: GLuint		|	EBO buffer ID
//
GLuint Renderer::CreateEBO(GLuint* data, GLsizeiptr size)
{
	GLuint bufferID;

	//Generate buffer
	glGenBuffers(1, &bufferID);

	//Bind buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);

	//Assign data to buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	return bufferID;
}

//	GetVectorSize( type )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Converts the bitwise operator to determin the vector size.
//
//	Param:
//		- uint32_t		|	Bit variable.
//
//	Return: unsigned int|	The size of the vector
//
unsigned int Renderer::GetVectorSize(uint32_t type)
{
	unsigned int size = 0;

	if (type & BUFFER_POS)
	{
		size += 3;	//x, y, z,
	}

	if (type & BUFFER_COL)
	{
		size += 4; //r, g, b, a,
	}

	if (type & BUFFER_TEX)
	{
		size += 2; //u, v,
	}

	return size;
}

Texture* Renderer::CreateBlankTexture(const char* name, unsigned int width, unsigned int height)
{
	Texture* texture = GetTexture(name);

	if (texture != 0) // Texture exists?
	{
		return texture;
	}

	unsigned char* pixels = new unsigned char[(int)width * (int)height * 4];
	for (int i = 0; i < (int)width * (int)height * 4; i++)
		pixels[i] = std::rand() % 0xFF;

	GLuint tempTexture;
	glGenTextures(1, &tempTexture);
	glBindTexture(GL_TEXTURE_2D, tempTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	delete[] pixels;

	texture = new Texture(width, height, tempTexture);
	m_pTextures.insert(std::pair<std::string, Texture*>(name, texture));

	return texture;
}

//	CreateTexture( TextureFilename )
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Generates, binds and loads an image as a texture, then returns a reference to it.
//
//	Param:
//		- const char*	|	Texture name (not location).
//
//	Return: Texture*	|	reference to a Texture.
//
Texture* Renderer::CreateTexture(const char* TextureFilename)
{
	Texture* texture = GetTexture(TextureFilename);

	if (texture != 0) // Texture exists?
	{
		return texture;
	}

	GLuint id;
	int ImageWidth, ImageHeight, ImageComponents;

	//Create file location based on local directory
	std::string vFile("Resources/Textures/");
	vFile += TextureFilename;

	//Load image data
	unsigned char* ImageData = stbi_load(vFile.c_str(), &ImageWidth, &ImageHeight, &ImageComponents, 0);

	if (ImageData) //Image exist?
	{
		//Generate and Bind Texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		texture = new Texture(ImageWidth, ImageHeight, id);

		//Texture Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Toggle type
		GLint LoadComponents = (ImageComponents == 4) ? GL_RGBA : GL_RGB;

		//Load Texture into openGl and generate Mipmap
		glTexImage2D(GL_TEXTURE_2D, 0, LoadComponents, ImageWidth, ImageHeight, 0, LoadComponents, GL_UNSIGNED_BYTE, ImageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Add texture to map.
		m_pTextures.insert(std::pair<std::string, Texture*>(TextureFilename, texture));
	}
	else
	{
		texture = 0; //
		std::cout << "Failed to load texture: " << TextureFilename << std::endl;
	}

	//Delete any memory unused
	stbi_image_free(ImageData);

	return texture;
}

void Renderer::SetTexture(const char* textureName, unsigned char* pixelInfo)
{
	Texture* texture = GetTexture(textureName);

	if (texture != 0) // Texture exists?
	{
		glBindTexture(GL_TEXTURE_2D, texture->GetID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, (int)texture->GetSize().x, (int)texture->GetSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelInfo);
	}
}

//	CreateTextGenerator( font,  fontSize)
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Creates a text generator to dynamically renders text textures onto 
//		the screen, and returns a reference to it.
//		
//	Param:
//		- std::string	|	Font file name contained within "Resources/Fonts/"
//		- unsigned int	|	Size of the font to generate (Can not be changed). 
//
//	Return: TextGenerator*	|	reference to the text generator created.
//
TextGenerator* Renderer::CreateTextGenerator(std::string font, unsigned int fontSize)
{
	TextGenerator* gen = GetTextGenerator(font, fontSize);

	std::string name = font + " | "+ std::to_string(fontSize);

	if (gen != 0) // Generator exists?
	{
		return gen;
	}

	gen = new TextGenerator(font, fontSize);

	m_TextGenerators.insert(std::pair<std::string, TextGenerator*>(name, gen));

	return gen;
}

//	GetTextGenerator( font,  fontSize)
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Searches the TextGenerator map to see if the text generator exists.
//		
//	Param:
//		- std::string	|	Font file name contained within "Resources/Fonts/"
//		- unsigned int	|	Size of the font to generate (Can not be changed). 
//
//	Return: TextGenerator*	|	Reference to the text generator.
//
TextGenerator* Renderer::GetTextGenerator(std::string font, unsigned int fontSize)
{
	std::map<std::string, TextGenerator*>::iterator it;
	std::string name = font + " | " + std::to_string(fontSize);

	it = m_TextGenerators.find(name);
	if (it != m_TextGenerators.end())
	{
		return (*it).second;
	}
	return 0;
}

//	GetTexture( TextureFilename )
//
//	Author: Michael Jordan
//	Access: private
//	Description:
//		Gets the sprite if the sprite exists, else returning 0.
//
//	Param:
//		- const char*	|	texture name (not location).
//
//	Return: Texture*	|	pointer to Sprite structure.
//
Texture* Renderer::GetTexture(const char* TextureFilename)
{
	std::map<std::string, Texture*>::iterator it;
	it = m_pTextures.find(TextureFilename);
	if (it != m_pTextures.end())
	{
		return (*it).second;
	}
	return 0;
}

//	GetCameraMatrix()
//
//	Author: Michael Jordan
//	Access: public
//	Description:
//		Gets the projection times view matrix, scaled with the world scale.
//
//	Param:
//		- n/a		|
//
//	Return: glm::mat4	|	Camera Matrix
//
glm::mat4 Renderer::GetCameraMatrix()
{
	return glm::scale(m_pMainCam->GetCamMatrix(), worldScale);
}