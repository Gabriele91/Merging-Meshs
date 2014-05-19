#ifndef SHADERDX_H
#define SHADERDX_H

#include <Config.h>
#include <Math3D.h>
#include <Utility.h>
#include <RenderDX.h>
#include <Render.h>
#include <Shader.h>

namespace Easy3D{

    
	class ShaderDX : public Shader {


	public:
		//costruttore
		ShaderDX(RenderDX* render);
		//distruttore
		virtual ~ShaderDX();
		//inizializza
		virtual void loadShader(const Utility::Path& vs,const Utility::Path& fs,const std::vector<String>& defines);

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

		uchar* getCpuVBuffer(){
			return &vBufferCpu[0];
		}
		uchar* getCpuPBuffer(){
			return &pBufferCpu[0];
		}
		size_t getCpuVBufferSize(){
			return vBufferCpu.size();
		}
		size_t getCpuPBufferSize(){
			return pBufferCpu.size();
		}

	protected:
		//imposta shader
		virtual void bind();
		virtual void unbind();
		virtual void uniform();

	private:

		friend class RenderDX;

		RenderDX* render{ nullptr };

		ID3D10Blob* vShaderBinary{ nullptr };
		ID3D10VertexShader *vShader{ nullptr };
		size_t vSizeConstantBuffer{ 0 };
		ID3D10Buffer*  vConstantBuffer10{ nullptr };


		ID3D10Blob* pShaderBinary{ nullptr };
		ID3D10PixelShader *pShader{ nullptr };
		size_t pSizeConstantBuffer{ 0 };
		ID3D10Buffer*  pConstantBuffer10{ nullptr };

		//cpu info
		DUNORDERED_MAP <String, size_t> vVariablesRef;
		DUNORDERED_MAP <String, size_t> vResourcesRef;
		DUNORDERED_MAP <String, size_t> pVariablesRef;
		DUNORDERED_MAP <String, size_t> pResourcesRef;
		std::vector<uchar> vBufferCpu;
		std::vector<uchar> pBufferCpu;


	};
};
#endif
