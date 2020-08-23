#include "offscreenBuffer.h"
#include <iostream>


OffScreenBuffer::OffScreenBuffer(GLuint width, GLuint height) :
	myFramebufferTexture_Depth   (GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST),
	myFramebufferTexture_Color   (GL_RGB              , GL_RGB, GL_FLOAT  , GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST),
	myFramebufferTexture_GlobalID(GL_RGB              , GL_RGB, GL_FLOAT  , GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST),
	myFramebufferTexture_WorldPos(GL_RGB32F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST)
{
	Confuse = false;
	Shake = false;
	Chaos = false;

	myShader.loadShader("../shaders/postProcessor.vert", "../shaders/postProcessor.frag");
	myWidth = width;
	myHeight = height;

	GLenum err;

	//====== Generate Framebuffer Object =================
	glGenFramebuffers(1, &myFBO_Main);
	glBindFramebuffer(GL_FRAMEBUFFER, myFBO_Main);

	//---------- Depth buffer, depth attachment ---------
	this->myFramebufferTexture_Depth.Generate(myWidth, myHeight, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, myFramebufferTexture_Depth.ID, 0);

	//------------ Color texture, attachment 0 -----------
	this->myFramebufferTexture_Color.Generate(myWidth, myHeight, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myFramebufferTexture_Color.ID, 0);

	//------ Unique GlobalID texture, attachment 1 -------
	this->myFramebufferTexture_GlobalID.Generate(myWidth, myHeight, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, myFramebufferTexture_GlobalID.ID, 0);
	
	//------ World Position texture, attachment 2 -------
	this->myFramebufferTexture_WorldPos.Generate(myWidth, myHeight, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, myFramebufferTexture_WorldPos.ID, 0);

	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	//check if valid
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::OffScreenBuffer: Failed to initialize FBO" << std::endl;
	else
		std::cout << "OffScreenBuffer FBO creation complete\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//set up fullscreen quad
	GLfloat vertices[] =
	{
		// Pos        // Tex

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &fullscreenQuad_VAO);
	glBindVertexArray(fullscreenQuad_VAO);

	glGenBuffers(1, &fullscreenQuad_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuad_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//set up shader sampler2d and uniforms
	myShader.setInt("scene", 0);
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset  },  // top-left
		{  0.0f,    offset  },  // top-center
		{  offset,  offset  },  // top-right
		{ -offset,  0.0f    },  // center-left
		{  0.0f,    0.0f    },  // center-center
		{  offset,  0.0f    },  // center - right
		{ -offset, -offset  },  // bottom-left
		{  0.0f,   -offset  },  // bottom-center
		{  offset, -offset  }   // bottom-right    
	};
	glUniform2fv(glGetUniformLocation(myShader.ID, "offsets"), 9, (GLfloat*)offsets);
	GLint edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(myShader.ID, "edge_kernel"), 9, edge_kernel);
	GLfloat blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv(glGetUniformLocation(myShader.ID, "blur_kernel"), 9, blur_kernel);
}

void OffScreenBuffer::BeginOffscreenRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, myFBO_Main);
}

void OffScreenBuffer::EndOffscreenRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void OffScreenBuffer::DrawToScreen()
{
	// Set uniforms/options
	myShader.use();
	myShader.setFloat("time", (float) globalWorld->gameState.timeInfo.currentGameTime);
	myShader.setBool("confuse", Confuse);
	myShader.setBool("chaos", Chaos);
	myShader.setBool("shake", Shake);

	// Render textured quad
	glActiveTexture(GL_TEXTURE0);
	myFramebufferTexture_Color.Bind();
	glBindVertexArray(fullscreenQuad_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

void OffScreenBuffer::resize(int width, int height)
{
	myWidth = width;
	myHeight = height;

	myFramebufferTexture_Depth.Resize(width, height);
	myFramebufferTexture_Color.Resize(width, height);
	myFramebufferTexture_GlobalID.Resize(width, height);
	myFramebufferTexture_WorldPos.Resize(width, height);

	glViewport(0, 0, width, height);
}


void OffScreenBuffer::bindTexture_WorldPos(GLuint slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	myFramebufferTexture_WorldPos.Bind();
}

void OffScreenBuffer::bindTexture_Depth(GLuint slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	myFramebufferTexture_Depth.Bind();
}

void OffScreenBuffer::bindTexture_Color(GLuint slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	myFramebufferTexture_Color.Bind();

}



void OffScreenBuffer::setDrawBuffers_ColorOnly()
{
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachments);

}

void OffScreenBuffer::setDrawBuffers_All()
{
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
}







OffScreenBuffer::~OffScreenBuffer()
{
	glDeleteFramebuffers(1, &myFBO_Main);
	glDeleteBuffers(1, &fullscreenQuad_VBO);
	glDeleteVertexArrays(1, &fullscreenQuad_VAO);
}