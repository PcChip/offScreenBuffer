#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"

class Texture2D
{
public:
	// Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
	GLuint ID;
	// Texture image dimensions
	GLuint Width, Height; // Width and height of loaded image in pixels
	// Texture Format
	GLuint Internal_Format; // Format of texture object
	GLuint Image_Format; // Format of loaded image
	GLuint Data_Type;
						 // Texture configuration
	GLuint Wrap_S; // Wrapping mode on S axis
	GLuint Wrap_T; // Wrapping mode on T axis
	GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels
	// Constructor (sets default texture modes)
	Texture2D(); //GL_RGB, GL_LINEAR
	Texture2D(GLuint InternalFormat, GLuint ImageFormat, GLuint DataType, GLuint WrapS, GLuint WrapT, GLuint FilterMin, GLuint FilterMax);
	~Texture2D();
	// Generates texture from image data
	void Generate(GLuint width, GLuint height, unsigned char* data);
	void Resize(int width, int height);
	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;
};
