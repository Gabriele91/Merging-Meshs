#include <stdafx.h>
#include <OpenGLWindow.h>
#include <Debug.h>

//////////////////////////////////////////////////////
//OpenGL 3.2 only
PFNGLPROVOKINGVERTEXEXTPROC  glProvokingVertex=NULL;
//////////////////////////////////////////////////////
//vbo
PFNGLGENBUFFERSARBPROC glGenBuffers=NULL;
PFNGLBINDBUFFERARBPROC glBindBuffer=NULL;
PFNGLBUFFERSUBDATAARBPROC glBufferSubData=NULL;
PFNGLBUFFERDATAARBPROC glBufferData=NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffers=NULL;
//edit vbo
PFNGLMAPBUFFERARBPROC glMapBuffer=NULL;
PFNGLUNMAPBUFFERARBPROC glUnmapBuffer=NULL;
//vba
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays=NULL;
PFNGLBINDVERTEXARRAYPROC  glBindVertexArray=NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays=NULL;
//multi texture
PFNGLACTIVETEXTUREPROC glActiveTexture=NULL;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture=NULL;
PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElements=NULL;
//blend effect
PFNGLBLENDEQUATIONPROC glBlendEquation=NULL;
//vsync
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT=NULL;
//FBO NO EXT
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
PFNGLDRAWBUFFERSPROC glDrawBuffers = NULL;
//RBO NO EXT
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
 PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
//shader
PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray=NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray=NULL;
PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation=NULL;
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer=NULL;
PFNGLCREATESHADERPROC	glCreateShader=NULL;
PFNGLDELETESHADERPROC	glDeleteShader=NULL;
PFNGLSHADERSOURCEPROC	glShaderSource=NULL;
PFNGLCOMPILESHADERPROC	glCompileShader=NULL;
PFNGLCREATEPROGRAMPROC	glCreateProgram=NULL;
PFNGLDELETEPROGRAMPROC	glDeleteProgram=NULL;
PFNGLATTACHSHADERPROC	glAttachShader=NULL;
PFNGLDETACHSHADERPROC	glDetachShader=NULL;
PFNGLLINKPROGRAMPROC	glLinkProgram=NULL;
PFNGLUSEPROGRAMPROC		glUseProgram=NULL;
PFNGLGETUNIFORMLOCATIONPROC	glGetUniformLocation=NULL;
PFNGLBINDATTRIBLOCATIONPROC	glBindAttribLocation=NULL;
PFNGLUNIFORM1FPROC	glUniform1f=NULL;
PFNGLUNIFORM2FPROC	glUniform2f=NULL;
PFNGLUNIFORM3FPROC	glUniform3f=NULL;
PFNGLUNIFORM4FPROC	glUniform4f=NULL;
PFNGLUNIFORM1IPROC	glUniform1i=NULL;
PFNGLUNIFORM4UIPROC	glUniform4ui=NULL;
PFNGLUNIFORM4UIVPROC	glUniform4uiv=NULL;
PFNGLUNIFORM1IVPROC	glUniform1iv=NULL;
PFNGLUNIFORM2IVPROC	glUniform2iv=NULL;
PFNGLUNIFORM3IVPROC	glUniform3iv=NULL;
PFNGLUNIFORM4IVPROC	glUniform4iv=NULL;
PFNGLUNIFORM1FVPROC	glUniform1fv=NULL;
PFNGLUNIFORM2FVPROC	glUniform2fv=NULL;
PFNGLUNIFORM3FVPROC	glUniform3fv=NULL;
PFNGLUNIFORM4FVPROC	glUniform4fv=NULL;
PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv=NULL;
//Info Log
PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog=NULL;
PFNGLGETSHADERIVPROC	glGetShaderiv=NULL;
PFNGLGETPROGRAMIVPROC	glGetProgramiv=NULL;
PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog=NULL;

void Easy3D::initOpenGL3(){
	//out debug
	DEBUG_MESSAGE("Setup OpenGL3");
	DEBUG_MESSAGE("Vendor:" << glGetString(GL_VENDOR)  );
    DEBUG_MESSAGE("Renderer:" << glGetString(GL_RENDERER));
    DEBUG_MESSAGE("OpenGL Version: " << glGetString(GL_VERSION) );
    DEBUG_MESSAGE("GLSL Version:" << glGetString(GL_SHADING_LANGUAGE_VERSION) );
	//////////////////////////////////////////////////////
	//OpenGL 3.2,3.3,4.x only
	glProvokingVertex = (PFNGLPROVOKINGVERTEXEXTPROC)wglGetProcAddress("glProvokingVertex");
	if (!glProvokingVertex)
		glProvokingVertex = (PFNGLPROVOKINGVERTEXEXTPROC)wglGetProcAddress("glProvokingVertexEXT");
	if (!glProvokingVertex)
		glProvokingVertex = (PFNGLPROVOKINGVERTEXEXTPROC)wglGetProcAddress("glProvokingVertexNV");
	if (!glProvokingVertex)
		glProvokingVertex = (PFNGLPROVOKINGVERTEXEXTPROC)wglGetProcAddress("glProvokingVertexAMD");
	DEBUG_MESSAGE_IF(!glProvokingVertex, "glProvokingVertex not found");
	//////////////////////////////////////////////////////
	//VBO / edit vbo 
    glGenBuffers=(PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffers"); DEBUG_ASSERT(glGenBuffers);
    glBindBuffer=(PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBuffer"); DEBUG_ASSERT(glBindBuffer);
    glBufferData=(PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferData"); DEBUG_ASSERT(glBufferData);
    glDeleteBuffers=(PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffers"); DEBUG_ASSERT(glDeleteBuffers);
    glBufferSubData=(PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubData"); DEBUG_ASSERT(glBufferSubData);
    glMapBuffer=(PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBuffer"); DEBUG_ASSERT(glMapBuffer);
    glUnmapBuffer=(PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBuffer"); DEBUG_ASSERT(glUnmapBuffer);
	//vba
    glGenVertexArrays=(PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays"); DEBUG_ASSERT(glGenVertexArrays);
    glBindVertexArray=(PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray"); DEBUG_ASSERT(glBindVertexArray);
    glDeleteVertexArrays=(PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays"); DEBUG_ASSERT(glDeleteVertexArrays);
	//multi texture
    glActiveTexture=(PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glClientActiveTexture=(PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress("glClientActiveTexture");
    glDrawRangeElements=(PFNGLDRAWRANGEELEMENTSEXTPROC)wglGetProcAddress("glDrawRangeElementsEXT");
    //blend effect
    glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");
    //vsync //wglGetProcAddress
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
    if ( wglSwapIntervalEXT ) 
			wglSwapIntervalEXT(0); //nVidia
    //FBO
    glGenFramebuffers		 = (PFNGLGENFRAMEBUFFERSPROC)		  wglGetProcAddress("glGenFramebuffers");     DEBUG_ASSERT(glGenFramebuffers);
    glDeleteFramebuffers     = (PFNGLDELETEFRAMEBUFFERSPROC)    wglGetProcAddress("glDeleteFramebuffers");     DEBUG_ASSERT(glDeleteFramebuffers);
    glBindFramebuffer		 = (PFNGLBINDFRAMEBUFFERPROC)		  wglGetProcAddress("glBindFramebuffer");     DEBUG_ASSERT(glBindFramebuffer);
	glFramebufferTexture     = (PFNGLFRAMEBUFFERTEXTUREPROC)wglGetProcAddress("glFramebufferTexture");       DEBUG_ASSERT(glFramebufferTexture);
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus"); DEBUG_ASSERT(glCheckFramebufferStatus);
	glDrawBuffers            = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers"); DEBUG_ASSERT(glDrawBuffers);
    //RBO
    glGenRenderbuffers		= (PFNGLGENRENDERBUFFERSPROC)		  wglGetProcAddress("glGenRenderbuffersEXT");       DEBUG_ASSERT(glGenRenderbuffers);
    glDeleteRenderbuffers   = (PFNGLDELETERENDERBUFFERSPROC)    wglGetProcAddress("glDeleteRenderbuffersEXT");      DEBUG_ASSERT(glDeleteRenderbuffers);
    glBindRenderbuffer		= (PFNGLBINDRENDERBUFFERPROC)		  wglGetProcAddress("glBindRenderbufferEXT");       DEBUG_ASSERT(glBindRenderbuffer);
    glRenderbufferStorage	= (PFNGLRENDERBUFFERSTORAGEPROC)  wglGetProcAddress("glRenderbufferStorageEXT");        DEBUG_ASSERT(glRenderbufferStorage);
    glFramebufferRenderbuffer= (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbufferEXT"); DEBUG_ASSERT(glFramebufferRenderbuffer);
    
	//shader
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray"); DEBUG_ASSERT(glEnableVertexAttribArray);
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray"); DEBUG_ASSERT(glDisableVertexAttribArray);
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation"); DEBUG_ASSERT(glGetAttribLocation);
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer"); DEBUG_ASSERT(glVertexAttribPointer);

    glCreateShader=(PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader"); DEBUG_ASSERT(glCreateShader);
    glDeleteShader=(PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader"); DEBUG_ASSERT(glDeleteShader);
    glShaderSource=(PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"); DEBUG_ASSERT(glShaderSource);

    glCompileShader=(PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"); DEBUG_ASSERT(glCompileShader);
    glCreateProgram=(PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"); DEBUG_ASSERT(glCreateProgram);
    glDeleteProgram=(PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram"); DEBUG_ASSERT(glDeleteProgram);
    glAttachShader=(PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"); DEBUG_ASSERT(glAttachShader);
    glDetachShader=(PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader"); DEBUG_ASSERT(glDetachShader);
    glLinkProgram=(PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram"); DEBUG_ASSERT(glLinkProgram);

    glUseProgram=(PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetUniformLocation=(PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"); DEBUG_ASSERT(glGetUniformLocation);
    glBindAttribLocation=(PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation"); DEBUG_ASSERT(glBindAttribLocation);
    glUniform1f=(PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"); DEBUG_ASSERT(glUniform1f);
    glUniform2f=(PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f"); DEBUG_ASSERT(glUniform2f);
    glUniform3f=(PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f"); DEBUG_ASSERT(glUniform3f);
    glUniform4f=(PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"); DEBUG_ASSERT(glUniform4f);
    glUniform1i=(PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"); DEBUG_ASSERT(glUniform1i);
    glUniform4ui=(PFNGLUNIFORM4UIPROC)wglGetProcAddress("glUniform4ui"); DEBUG_ASSERT(glUniform4ui);
    glUniform4uiv=(PFNGLUNIFORM4UIVPROC)wglGetProcAddress("glUniform4uiv"); DEBUG_ASSERT(glUniform4uiv);
    glUniform1iv=(PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv"); DEBUG_ASSERT(glUniform1iv);
    glUniform2iv=(PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv"); DEBUG_ASSERT(glUniform2iv);
    glUniform3iv=(PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv"); DEBUG_ASSERT(glUniform3iv);
    glUniform4iv=(PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv"); DEBUG_ASSERT(glUniform4iv);
    glUniform1fv=(PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv"); DEBUG_ASSERT(glUniform1fv);
    glUniform2fv=(PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv"); DEBUG_ASSERT(glUniform2fv);
    glUniform3fv=(PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv"); DEBUG_ASSERT(glUniform3fv);
    glUniform4fv=(PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv"); DEBUG_ASSERT(glUniform4fv);
    glUniformMatrix4fv=(PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv"); DEBUG_ASSERT(glUniformMatrix4fv);
    //log compilatore shader
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog"); DEBUG_ASSERT(glGetShaderInfoLog);
    glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv"); DEBUG_ASSERT(glGetShaderiv);
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv"); DEBUG_ASSERT(glGetProgramiv);
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog"); DEBUG_ASSERT(glGetProgramInfoLog);
}