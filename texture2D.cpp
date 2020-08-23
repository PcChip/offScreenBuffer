#include <iostream>

#include "texture2D.h"


Texture2D::Texture2D()
	: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Data_Type(GL_UNSIGNED_BYTE), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}


Texture2D::Texture2D(GLuint InternalFormat, GLuint ImageFormat, GLuint DataType, GLuint WrapS, GLuint WrapT, GLuint FilterMin, GLuint FilterMax)
{
	Width = 0;
	Height = 0;
	Internal_Format = InternalFormat;
	Image_Format = ImageFormat;
	Data_Type = DataType;
	Wrap_S = WrapS;
	Wrap_T = WrapT;
	Filter_Min = FilterMin;
	Filter_Max = FilterMax;

	glGenTextures(1, &this->ID);
}


void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "OpenGL error _BEFORE_ Texture2D::Generate - Error:" << err << std::endl;
	}

#ifdef _DEBUG 
	std::cout << "Texture2D: Generating texture...\n";
#endif

	this->Width = width;
	this->Height = height;
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, Data_Type, data);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "OpenGL error in Texture2D::Generate - Error:" << err << std::endl;
	}


}

void Texture2D::Resize(int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &this->ID);
}

// https://stackoverflow.com/questions/48654723/multi-sampling-frame-render-objects-and-depth-buffers