#ifndef SHADERGL_H
#define SHADERGL_H

#include <Config.h>
#include <Math3D.h>
#include <Utility.h>
#include <Shader.h>

namespace Easy3D{
    
	class ShaderGL : public Shader {

	public:
		//costruttore
		ShaderGL();
		//distruttore
		virtual ~ShaderGL();
		//inizializza
		virtual void loadShader(const Utility::Path& vs,
                        const Utility::Path& fs,
                        const std::vector<String>& defines);

		//get consts
		virtual CInt* getConstInt(const char *name);
		virtual CFloat* getConstFloat(const char *name);
		virtual CVec2* getConstVec2(const char *name);
		virtual CVec3* getConstVec3(const char *name);
		virtual CVec4* getConstVec4(const char *name);
		virtual CMat4* getConstMat4(const char *name);
		virtual CIntArray* getConstIntArray(const char *name);
		virtual CFloatArray* getConstFloatArray(const char *name);
		virtual CVec2Array* getConstVec2Array(const char *name);
		virtual CVec3Array* getConstVec3Array(const char *name);
		virtual CVec4Array* getConstVec4Array(const char *name);
		virtual CMat4Array* getConstMat4Array(const char *name);

	protected:

		//get uniform id
		uint getUniformID(const char *name);

		//imposta shader
		virtual void bind();
		virtual void unbind();
		virtual void uniform();
		//id programma
		uint programID();

		//friend class
		friend class RenderGL;

	private:
		uint shader_id;
		uint shader_vs;
		uint shader_fs;
		bool deleteProgram;


	};
};
#endif
