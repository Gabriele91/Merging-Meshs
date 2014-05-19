#include <stdafx.h>
#include <ShaderGL.h>
#include <RenderGL.h>
#include <Application.h>
#include <Debug.h>
////////////////////////
namespace Easy3D{


	DFORCEINLINE RenderGL& getRender(){
		return *(RenderGL*)Application::instance()->getRender();
	}
	DFORCEINLINE static String textFileRead(const Utility::Path &path) {
		String out;
		/////////////////////////////////////////////////////////////////////
		//cpu load
		//get raw file
		void *data = NULL; size_t len = 0;
		Application::instance()->loadData(path, data, len);
		out = (char*)data;
		//free raw file
		free(data);
		/////////////////////////////////////////////////////////////////////
		return out;
	}
	DFORCEINLINE static bool logError(unsigned int shader, int status) {

		if (!status)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen > 1)
			{
				char* infoLog = (char*)malloc(sizeof(char) * infoLen);
				glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
				DEBUG_MESSAGE("Error compiling shader:\n" << infoLog);
				free(infoLog);
			}
			return false;
		}
		return true;

	}

	ShaderGL::ShaderGL()
		:shader_vs(0),
		shader_fs(0),
		shader_id(0)
	{
	}

	void ShaderGL::loadShader(const Utility::Path& vsFile,
		const Utility::Path& fsFile,
		const std::vector<String>& defines){
		//delete last shader
		deleteProgram();
		GLint compiled = 0, linked = 0;
		// load shaders files
		String fileVS = textFileRead(vsFile);
		String fileFS = textFileRead(fsFile);
		//list define
		String definesString;
		for (const String& define : defines)
			definesString += "#define " + define + "\n";
		//precision highp or mediump (or lowp) float
		fileVS = "#version 150\n" +
			definesString +
			"#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
			"#define lerp        mix                        \n"
			"#line 0\n" +
			fileVS;
		fileFS = "#version 150\n" +
			definesString +
			"#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
			"#define lerp        mix                        \n"
			"#define bestp\n"
			"#define highp\n"
			"#define mediump\n"
			"#define lowp\n"
			"#line 0\n" +
			fileFS;
		//create a pixel shader
		shader_fs = glCreateShader(GL_FRAGMENT_SHADER);
		//create a vertex shader
		shader_vs = glCreateShader(GL_VERTEX_SHADER);
		//send source
		const char* cVSfile = fileVS;
		const char* cFSfile = fileFS;
		glShaderSource(shader_vs, 1, &(cVSfile), 0);
		glShaderSource(shader_fs, 1, &(cFSfile), 0);
		//compiling
		compiled = 0;
		glCompileShader(shader_vs);
		glGetShaderiv(shader_vs, GL_COMPILE_STATUS, &compiled);
		if (!logError(shader_vs, compiled)){ glDeleteShader(shader_vs); }

		compiled = 0;
		glCompileShader(shader_fs);
		glGetShaderiv(shader_fs, GL_COMPILE_STATUS, &compiled);
		if (!logError(shader_fs, compiled)){ glDeleteShader(shader_fs); }
		//made a shader program
		shader_id = glCreateProgram();
		glAttachShader(shader_id, shader_vs);
		glAttachShader(shader_id, shader_fs);
		glLinkProgram(shader_id);
		//get link status
		glGetProgramiv(shader_id, GL_LINK_STATUS, &linked);
		//out errors
		if (!linked){
			GLint infoLen = 0;
			glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen > 1){
				char* infoLog = (char*)malloc(sizeof(char) * infoLen);
				glGetProgramInfoLog(shader_id, infoLen, NULL, infoLog);
				DEBUG_MESSAGE("Error linking program:\n" << infoLog);
				free(infoLog);
			}
			DEBUG_MESSAGE("Error linking\n");
			//"stacca" gli schader dal shader program
			glDetachShader(shader_id, shader_fs);
			glDetachShader(shader_id, shader_vs);
			//cancella gli shader
			glDeleteShader(shader_fs);
			glDeleteShader(shader_vs);
			//cancella lo shader program
			glDeleteProgram(shader_id);
			shader_vs = shader_fs = shader_id = 0;
		}

	}
	//distruttore
	inline void ShaderGL::deleteProgram(){
		if (shader_id){
			glUseProgram(0);
			//"stacca" gli schader dal shader program
			glDetachShader(shader_id, shader_fs);
			glDetachShader(shader_id, shader_vs);
			//cancella gli shader
			glDeleteShader(shader_fs);
			glDeleteShader(shader_vs);
			//cancella lo shader program
			glDeleteProgram(shader_id);
			//to null
			shader_fs = 0;
			shader_vs = 0;
			shader_id = 0;
		}
	}
	ShaderGL::~ShaderGL() {
		deleteProgram();
	}
	//imposta shader
	void ShaderGL::bind(){
		//start texture uniform
		ntexture = -1;
		//uniform parogram shaders
		glUseProgram(shader_id);
	}


#define UniformGLT(CName,T,uniform)\
    class CName : public Uniform<CName> {\
        GLuint id{0};\
    public:\
        CName( GLuint id): id(id){}\
        virtual void  set(const void* value, size_t s, size_t n){\
             uniform (id+s, (GLuint)n, (const T*)value); \
	        }\
        virtual void  set(const void* value) {\
            uniform (id, 1, (const T*)value);\
	        }\
        virtual void* get(){ return NULL;}\
        virtual const void* get() const { return NULL; }\
        virtual ~CName(){}\
	    };


	UniformGLT(UniformGLInt, int, glUniform1iv)
	UniformGLT(UniformGLFloat, float, glUniform1fv)
	UniformGLT(UniformGLVec2, float, glUniform2fv)
	UniformGLT(UniformGLVec3, float, glUniform3fv)
	UniformGLT(UniformGLVec4, float, glUniform4fv)

	class UniformGLMat4 : public Uniform < UniformGLMat4 > {
		GLuint id{ 0 };
	public:
		UniformGLMat4(GLuint id) : id(id){}
		virtual void  set(const void* value, size_t s, size_t n){
			glUniformMatrix4fv(id + s, (GLuint)n, false, (const float*)value);
		}
		virtual void  set(const void* value) {
			glUniformMatrix4fv(id, 1, false, (const float*)value);
		}
		virtual void* get(){ return NULL; }
		virtual const void* get() const { return NULL; }
		virtual ~UniformGLMat4(){}
	};

	class UniformGLTexture : public Uniform < UniformGLTexture > {
		GLuint id{ 0 };
		ShaderGL* shader{ nullptr };
	public:
		UniformGLTexture(GLuint id, ShaderGL* shader) : id(id), shader(shader){}
		virtual void  set(const void* value, size_t s, size_t n){
			//next
			++shader->ntexture;
			//texture type
			if (n == 0) getRender().enableTexture((BaseTexture*)value, shader->ntexture);
			else        getRender().enableTexture((BaseRenderTexture*)value, shader->ntexture);
			//uniform
			glUniform1i(id, shader->ntexture);
		}
		virtual void  set(const void* value) {
			//void
		}
		virtual void* get(){ return NULL; }
		virtual const void* get() const { return NULL; }
		virtual ~UniformGLTexture(){}
	};


	CTexture* ShaderGL::getConstTexture(const char *name){
		return (CTexture*)new UniformGLTexture(getUniformID(name), this);
	}
	CInt* ShaderGL::getConstInt(const char *name){
		return (CInt*)new UniformGLInt(getUniformID(name));
	}
	CFloat* ShaderGL::getConstFloat(const char *name){
		return (CFloat*)new UniformGLFloat(getUniformID(name));
	}
	CVec2* ShaderGL::getConstVec2(const char *name){
		return (CVec2*)new UniformGLVec2(getUniformID(name));
	}
	CVec3* ShaderGL::getConstVec3(const char *name){
		return (CVec3*)new UniformGLVec3(getUniformID(name));
	}
	CVec4* ShaderGL::getConstVec4(const char *name){
		return (CVec4*)new UniformGLVec4(getUniformID(name));
	}
	CMat4* ShaderGL::getConstMat4(const char *name){
		return (CMat4*)new UniformGLMat4(getUniformID(name));
	}
	CIntArray* ShaderGL::getConstIntArray(const char *name){
		return (CIntArray*)new UniformGLInt(getUniformID(name));
	}
	CFloatArray* ShaderGL::getConstFloatArray(const char *name){
		return (CFloatArray*)new UniformGLFloat(getUniformID(name));
	}
	CVec2Array* ShaderGL::getConstVec2Array(const char *name){
		return (CVec2Array*)new UniformGLVec2(getUniformID(name));
	}
	CVec3Array* ShaderGL::getConstVec3Array(const char *name){
		return (CVec3Array*)new UniformGLVec3(getUniformID(name));
	}
	CVec4Array* ShaderGL::getConstVec4Array(const char *name){
		return (CVec4Array*)new UniformGLVec4(getUniformID(name));
	}
	CMat4Array* ShaderGL::getConstMat4Array(const char *name){
		return (CMat4Array*)new UniformGLMat4(getUniformID(name));
	}
	//get uniform id
	uint ShaderGL::getUniformID(const char *name){ return glGetUniformLocation(shader_id, name); }


	//disabilita shader
	void ShaderGL::unbind(){
		glUseProgram(0);
	}
	//id programma
	unsigned int ShaderGL::programID(){
		return shader_id;
	}

	void ShaderGL::uniform(){
		//disable all texture
		for (long nt = 0; nt <= ntexture; ++ntexture)
			getRender().disableTexture((uint)nt);
		//restart texture uniform
		ntexture = -1;
	}

};
