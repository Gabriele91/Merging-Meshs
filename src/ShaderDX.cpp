#include <stdafx.h>
#include <ShaderDX.h>
#include <RenderDX.h>
#include <Application.h>
#include <Debug.h>
#include <D3D10.h>
#include <Dxerr.h>
////////////////////////
//http://nxproject.googlecode.com/svn/nGENE/nGENE%20Proj/ShaderDX10.cpp
////////////////////////
namespace Easy3D{
	////////////////////////

	DFORCEINLINE RenderDX& getRender(){
		return *(RenderDX*)Application::instance()->getRender();
	}
	////////////////////////
#define DX_ASSERT_MSG(hr) dxAssertMsg(hr, #hr ,__FILE__,__LINE__)
	inline static bool dxAssertMsg(HRESULT hr, const char* strcommand, const char* file, int line){
		if (FAILED(hr)){
			Debug::message() << "DirectX: " << DXGetErrorDescription(hr) << "\n";
			Debug::doassert(0, "DirectX Render"/*strcommand*/, file, line);
			return false;
		}
		return true;
	}

	inline static bool dxShaderError(ID3D10Blob* dxerrors = NULL){
		if (dxerrors){
			String errors;
			char* compileErrors = (char*)(dxerrors->GetBufferPointer());
			for (size_t i = 0; i != dxerrors->GetBufferSize(); ++i){
				errors += compileErrors[i];
			}
			DEBUG_ASSERT_MSG(0, "DirectX Shader compiling error:\n" << errors);
			return false;
		}
		return true;
	}


	//create buffer
	inline static ID3D10Buffer* genBufferObject(ID3D10Device* device, size_t size)
	{
		ID3D10Buffer* buffer = NULL;
		D3D10_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = size;
		bd.Usage = D3D10_USAGE_DYNAMIC;
		bd.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		DX_ASSERT_MSG(device->CreateBuffer(&bd, NULL, &buffer));

		return buffer;
	}

	DFORCEINLINE static void getContants(ID3D10Device *d3dDevice,
		const String& prefix,
		DUNORDERED_MAP <String, size_t>& mapVars,
		ID3D10Blob* pCode,
		ID3D10Buffer** pBuffer,
		size_t& size){
		HRESULT hr;
		ID3D10ShaderReflection* pReflection = NULL;
		hr = D3D10ReflectShader((void*)pCode->GetBufferPointer(),
			pCode->GetBufferSize(),
			&pReflection);

		if (FAILED(hr))
		{
			DEBUG_ASSERT_MSG(0, "Shader reflection could not be obtained. Reason: " << DXGetErrorDescription(hr));
			return;
		}

		D3D10_SHADER_DESC desc;
		DX_ASSERT_MSG(pReflection->GetDesc(&desc));
		//init
		ID3D10ShaderReflectionConstantBuffer* pConstantBuffer = NULL;
		size_t valueOffset = 0;
		size = 0;
		//calc size
		for (uint i = 0; i < desc.ConstantBuffers; ++i)
		{
			pConstantBuffer = pReflection->GetConstantBufferByIndex(i);

			uint count = 0;
			D3D10_SHADER_BUFFER_DESC bufferDesc;
			DX_ASSERT_MSG(pConstantBuffer->GetDesc(&bufferDesc));
			if (bufferDesc.Type == D3D10_CT_TBUFFER)
				continue;

			count = bufferDesc.Variables;

			ID3D10ShaderReflectionVariable* pConstant = NULL;
			D3D10_SHADER_VARIABLE_DESC constantDesc;
			for (uint j = 0; j < count; ++j)
			{
				pConstant = pConstantBuffer->GetVariableByIndex(j);
				pConstant->GetDesc(&constantDesc);
				mapVars[prefix + '.' + constantDesc.Name] = valueOffset;
				valueOffset += constantDesc.Size;
			}
		}
		//save size buffer
		size = Math::multipleOfX(valueOffset, 16);
		//
		if (valueOffset)
		{
			(*pBuffer) = genBufferObject(d3dDevice, size);
		}

	}

	DFORCEINLINE static void getResources(ID3D10Device *d3dDevice,
										  const String& prefix,
										  DUNORDERED_MAP <String, size_t>& mapResource,
										  ID3D10Blob* pCode){
		HRESULT hr;
		ID3D10ShaderReflection* pReflection = NULL;
		hr = D3D10ReflectShader((void*)pCode->GetBufferPointer(),
			pCode->GetBufferSize(),
			&pReflection);

		if (FAILED(hr))
		{
			DEBUG_ASSERT_MSG(0, "Shader reflection could not be obtained. Reason: " << DXGetErrorDescription(hr));
			return;
		}

		D3D10_SHADER_DESC desc;
		DX_ASSERT_MSG(pReflection->GetDesc(&desc));
		//init
		size_t valueOffset = 0;
		//calc size
		for (uint i = 0; i < desc.BoundResources; ++i)
		{
			D3D10_SHADER_INPUT_BIND_DESC resourceDesc;
			pReflection->GetResourceBindingDesc(i, &resourceDesc);
			if (resourceDesc.Type == D3D10_SIT_TEXTURE){
				mapResource[prefix + '.' + resourceDesc.Name] = resourceDesc.BindPoint;
			}
			else if (resourceDesc.Type == D3D10_SIT_SAMPLER){
				//void
			}
		}
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
	//costruttore
	ShaderDX::ShaderDX(RenderDX* render) :render(render){}
	//distruttore
#define releaseDX(x) if(x){ x->Release(); x=nullptr; } 
	ShaderDX::~ShaderDX(){
		releaseDX(vShaderBinary)
			releaseDX(vConstantBuffer10)
			releaseDX(pShaderBinary)
			releaseDX(pConstantBuffer10)
	}

	//inizializza
	void ShaderDX::loadShader(const Utility::Path& vs,
		const Utility::Path& ps,
		const std::vector<String>& defines){

		HRESULT hr = NULL;
		DWORD vShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		//vShaderFlags |= D3D10_SHADER_DEBUG;
		//vShaderFlags |= D3D10_SHADER_FORCE_VS_SOFTWARE_NO_OPT;
#endif

		String vShaderFile = textFileRead(vs);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		ID3D10Blob* vErrors = NULL;
		hr = D3D10CompileShader(vShaderFile, vShaderFile.size(), String(vs.getFilename()), NULL, NULL, "main", "vs_4_0", vShaderFlags, &vShaderBinary, &vErrors);
		dxShaderError(vErrors);
		DX_ASSERT_MSG(hr);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		DX_ASSERT_MSG(render->d3dDevice->CreateVertexShader((DWORD*)vShaderBinary->GetBufferPointer(), vShaderBinary->GetBufferSize(), &vShader));
		getContants(render->d3dDevice, "vs", vVariablesRef, vShaderBinary, &vConstantBuffer10, vSizeConstantBuffer);
		getResources(render->d3dDevice, "vs", vResourcesRef, vShaderBinary);
		if (vSizeConstantBuffer)
			vBufferCpu.resize(vSizeConstantBuffer);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		DWORD pShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		//pShaderFlags |= D3D10_SHADER_DEBUG;
		//pShaderFlags |= D3D10_SHADER_FORCE_PS_SOFTWARE_NO_OPT;
#endif
		String pShaderFile = textFileRead(ps);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		ID3D10Blob* pErrors = NULL;
		hr = D3D10CompileShader(pShaderFile, pShaderFile.size(), String(ps.getFilename()), NULL, NULL, "main", "ps_4_0", pShaderFlags, &pShaderBinary, &pErrors);
		dxShaderError(pErrors);
		DX_ASSERT_MSG(hr);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		DX_ASSERT_MSG(render->d3dDevice->CreatePixelShader((DWORD*)pShaderBinary->GetBufferPointer(), pShaderBinary->GetBufferSize(), &pShader));
		getContants(render->d3dDevice, "ps", pVariablesRef, pShaderBinary, &pConstantBuffer10, pSizeConstantBuffer);
		getResources(render->d3dDevice, "ps", pResourcesRef, pShaderBinary);
		if (pSizeConstantBuffer)
			pBufferCpu.resize(pSizeConstantBuffer);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	}

	///////////////////
	inline static void sendBinaryData(ID3D10Buffer* pConst, size_t offset, const void *value, size_t bsize){
		char* pBuffer = NULL;
		pConst->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pBuffer);
		void* pTemp = pBuffer + offset;
		memcpy(pTemp, value, bsize);
		pConst->Unmap();
	}
	///////////////////
	//uniform whit name
	//get consts

	class UniformDXVS : public Uniform < UniformDXVS > {

		ShaderDX* shader{ nullptr };;
		size_t offset{ 0 };
		size_t bsize{ 0 };

	public:
		UniformDXVS(ShaderDX* shader,
			size_t offset,
			size_t bsize)
			:shader(shader)
			, offset(offset)
			, bsize(bsize){}

		virtual void  set(const void* value, size_t s, size_t n){
			DEBUG_ASSERT(shader->getCpuVBufferSize() >= (offset + bsize*s + bsize*n));
			Math::memcpy((&shader->getCpuVBuffer()[offset]) + bsize*s, (Easy3D::byte*)value, bsize*n);
		}
		virtual void  set(const void* value) {
			DEBUG_ASSERT(shader->getCpuVBufferSize() >= (offset + bsize));
			Math::memcpy((&shader->getCpuVBuffer()[offset]), (Easy3D::byte*)value, bsize);
		}
		virtual void* get(){
			return NULL;
		}
		virtual const void* get() const {
			return NULL;
		}
		virtual ~UniformDXVS(){
		}
	};
	class UniformDXPS : public Uniform < UniformDXPS > {

		ShaderDX* shader{ nullptr };;
		size_t offset{ 0 };
		size_t bsize{ 0 };

	public:
		UniformDXPS(ShaderDX* shader,
			size_t offset,
			size_t bsize)
			:shader(shader)
			, offset(offset)
			, bsize(bsize){}

		virtual void  set(const void* value, size_t s, size_t n){
			DEBUG_ASSERT(shader->getCpuPBufferSize() >= (offset + bsize*s + bsize*n));
			Math::memcpy((&shader->getCpuPBuffer()[offset]) + bsize*s, (Easy3D::byte*)value, bsize*n);
		}
		virtual void  set(const void* value) {
			DEBUG_ASSERT(shader->getCpuPBufferSize() >= (offset + bsize));
			Math::memcpy(&(shader->getCpuPBuffer()[offset]), (Easy3D::byte*)value, bsize);
		}
		virtual void* get(){
			return NULL;
		}
		virtual const void* get() const {
			return NULL;
		}
		virtual ~UniformDXPS(){
		}
	};

	class UniformVSTexture : public Uniform < UniformTexture > {

		ShaderDX* shader{ nullptr };
		uint idtexture{ 0 };

	public:

		UniformVSTexture(ShaderDX* shader, uint idtexture)
			:shader(shader)
			,idtexture(idtexture){}

		virtual void  set(const void* value, size_t s, size_t n){
			if (n == 0) getRender().uniformVSTexture(idtexture, (BaseTexture*)value);
			else  getRender().uniformVSTexture(idtexture, (BaseRenderTexture*)value);
		}
		virtual void  set(const void* value) { /* void */ }
		virtual void* get(){ return NULL; }
		virtual const void* get() const { return NULL; }
		virtual ~UniformVSTexture(){}


	};
	class UniformPSTexture : public Uniform < UniformTexture > {

		ShaderDX* shader{ nullptr };
		uint idtexture{ 0 };

	public:

		UniformPSTexture(ShaderDX* shader, uint idtexture)
			:shader(shader)
			, idtexture(idtexture){}

		virtual void  set(const void* value, size_t s, size_t n){
			if (n == 0) getRender().uniformPSTexture(idtexture, (BaseTexture*)value);
			else getRender().uniformPSTexture(idtexture, (BaseRenderTexture*)value);
		}
		virtual void  set(const void* value) { /* void */ }
		virtual void* get(){ return NULL; }
		virtual const void* get() const { return NULL; }
		virtual ~UniformPSTexture(){}



	};

#define uniformMethod(name,type, bsize,ctype)\
	ctype* ShaderDX::name(const char *name){\
	if (name[0] && name[1] && '.' == name[2]){\
	    if ('v' == *name)\
			return (ctype*)new UniformDXVS(this, vVariablesRef[name], bsize);\
		else if ('p' == *name)\
			return (ctype*)new UniformDXPS(this, pVariablesRef[name], bsize);\
		else\
		   return nullptr;\
		}\
	else{\
		return (ctype*)new UniformDXVS(this, vVariablesRef[String("vs.") + name], bsize);\
		}\
	}
	uniformMethod(getConstInt, int, sizeof(int), CInt)
	uniformMethod(getConstFloat, float, sizeof(float), CFloat)
	uniformMethod(getConstVec2, Vec2, sizeof(float) * 2, CVec2)
	uniformMethod(getConstVec3, Vec3, sizeof(float) * 3, CVec3)
	uniformMethod(getConstVec4, Vec4, sizeof(float) * 4, CVec4)
	uniformMethod(getConstMat4, Mat4, sizeof(float) * 16, CMat4)

	uniformMethod(getConstIntArray, int, sizeof(int), CIntArray)
	uniformMethod(getConstFloatArray, float, sizeof(float), CFloatArray)
	uniformMethod(getConstVec2Array, Vec2, sizeof(float) * 2, CVec2Array)
	uniformMethod(getConstVec3Array, Vec3, sizeof(float) * 3, CVec3Array)
	uniformMethod(getConstVec4Array, Vec4, sizeof(float) * 4, CVec4Array)
	uniformMethod(getConstMat4Array, Mat4, sizeof(float) * 16, CMat4Array)



CTexture* ShaderDX::getConstTexture(const char *name){
	if (name[0] && name[1] && '.' == name[2]){
			if ('v' == *name)
				return (CTexture*)new UniformVSTexture(this, vVariablesRef[name]); 
			else if ('p' == *name)
				return (CTexture*)new UniformPSTexture(this, pVariablesRef[name]); 
			else
				return nullptr; 
	}
	else{
		return (CTexture*)new UniformVSTexture(this, vVariablesRef[String("vs.")+ name]);
	}
}
	
	//imposta shader
	void ShaderDX::bind(){
		render->d3dDevice->VSSetConstantBuffers(0, 1, &vConstantBuffer10);
		render->d3dDevice->VSSetShader(vShader);

		render->d3dDevice->PSSetConstantBuffers(0, 1, &pConstantBuffer10);
		render->d3dDevice->PSSetShader(pShader);

		render->d3dDevice->GSSetShader(NULL);
	}
	void ShaderDX::unbind(){
		render->d3dDevice->VSSetShader(NULL);
		render->d3dDevice->PSSetShader(NULL);
		render->d3dDevice->GSSetShader(NULL);
	}
	void ShaderDX::uniform(){
		if (vSizeConstantBuffer)
			sendBinaryData(vConstantBuffer10, 0, &vBufferCpu[0], vSizeConstantBuffer);
		if (pSizeConstantBuffer)
			sendBinaryData(pConstantBuffer10, 0, &pBufferCpu[0], pSizeConstantBuffer);
	}

};