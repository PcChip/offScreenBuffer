#pragma once
#include "texture2d.h"
#include "teShader.h"
#include "TranceEngine.h"
//#include "modules/te2graphicsmodule.h"

extern TranceEngine* globalWorld;

class OffScreenBuffer
{
	public: 
		void BeginOffscreenRender(); //binds and clears
		void EndOffscreenRender(); //binds framebuffer 0
		void DrawToScreen(); //draws fullscreen quad

		void resize(int width, int height);

		void bindTexture_WorldPos(GLuint slot);
		void bindTexture_Depth(GLuint slot);
		void bindTexture_Color(GLuint slot);
		void setDrawBuffers_ColorOnly(); //my FBO must already be bound
		void setDrawBuffers_All(); //my FBO must already be bound

		OffScreenBuffer(GLuint width, GLuint height);
		~OffScreenBuffer();

	private:
		GLuint myFBO_Main; //for most objects, writes globalID
		GLuint myFBO_SecondPass; //for transparencies and water

		//========= Textures for first FBO =========
		Texture2D myFramebufferTexture_Depth;
		Texture2D myFramebufferTexture_Color;
		Texture2D myFramebufferTexture_GlobalID;
		Texture2D myFramebufferTexture_WorldPos;

		//========= Texture for second FBO =========
		Texture2D myFramebufferTexture_SecondPass_Color;

		GLuint myWidth = 0;
		GLuint myHeight = 0;

		//====== Items needed to draw fullscreen quad to FBO 0 =====
		teShader myShader;

		GLuint fullscreenQuad_VAO;
		GLuint fullscreenQuad_VBO;

		GLboolean Confuse, Chaos, Shake = GL_FALSE;


};