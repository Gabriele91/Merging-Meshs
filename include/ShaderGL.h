#ifndef SHADERGL_H
#define SHADERGL_H

#include <Config.h>
#include <Math3D.h>
#include <Utility.h>
#include <Shader.h>

namespace Easy3D{
	//sheder class declaretrion
	class ShaderGL : public Shader {
        
        void loadShader(bool enableGeometry,
                        const Utility::Path& vs,
                        const Utility::Path& fs,
                        const Utility::Path& gs,
                        const std::vector<String>& defines);

	public:
		//costruttore
		ShaderGL();
		//distruttore
		virtual ~ShaderGL();
		//inizializza
		virtual void loadShader(const Utility::Path& vs,
                                const Utility::Path& fs,
                                const std::vector<String>& defines){
            loadShader(false,vs,fs,"",defines);
        }
		virtual void loadShader(const Utility::Path& vs,
                                const Utility::Path& fs,
                                const Utility::Path& gs,
                                const std::vector<String>& defines){
            loadShader(true,vs,fs,gs,defines);
        }

		//get consts
		virtual CTexture* getConstTexture(const char *name);
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
		friend class UniformGLTexture;

		//count uniforms texture
		long ntexture{ -1 };


	private:
		uint shader_id{ 0 };
		uint shader_vs{ 0 };
		uint shader_fs{ 0 };
		uint shader_gs{ 0 };
		void deleteProgram();

	};
};
#endif
