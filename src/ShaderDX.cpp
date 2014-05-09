#include <stdafx.h>
#include <ShaderDX.h>
#include <Application.h>
#include <Debug.h>
#include <D3D10.h>
#include <Dxerr.h>
////////////////////////
//http://nxproject.googlecode.com/svn/nGENE/nGENE%20Proj/ShaderDX10.cpp
////////////////////////
using namespace Easy3D;
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

	ID3D10ShaderReflectionConstantBuffer* pConstantBuffer = NULL;
	size_t valueOffset = 0;

	for (uint i = 0; i < desc.BoundResources; ++i)
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

DFORCEINLINE static String textFileRead(const Utility::Path &path) {
	String out;
	/////////////////////////////////////////////////////////////////////
	//cpu load
	//get raw file
	void *data=NULL; size_t len=0;
	Application::instance()->loadData(path,data,len);
	out=(char*)data;
	//free raw file
	free(data);
	/////////////////////////////////////////////////////////////////////
	return out;
}
DFORCEINLINE static bool logError(unsigned int shader,int status) {

	if(!status)
	{
	GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog =(char*) malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			DEBUG_MESSAGE("Error compiling shader:\n" << infoLog);
			free(infoLog);
		}
		return false;
	}
	return true;

}
//costruttore
ShaderDX::ShaderDX(RenderDX* render):render(render){}
//distruttore
ShaderDX::~ShaderDX(){}

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
	ID3D10Blob* vErrors=NULL;
	hr = D3D10CompileShader(vShaderFile, vShaderFile.size(), String(vs.getFilename()), NULL, NULL, "main", "vs_4_0", vShaderFlags, &vShaderBinary, &vErrors);
	dxShaderError(vErrors);
	DX_ASSERT_MSG(hr);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	DX_ASSERT_MSG(render->d3dDevice->CreateVertexShader((DWORD*)vShaderBinary->GetBufferPointer(), vShaderBinary->GetBufferSize(), &vShader));
	getContants(render->d3dDevice, "vs", vVariablesRef, vShaderBinary, &vConstantBuffer10, vSizeConstantBuffer);
	if (vSizeConstantBuffer) vBufferCpu.resize(vSizeConstantBuffer);
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
	if (pSizeConstantBuffer) pBufferCpu.resize(pSizeConstantBuffer);
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

class UniformDXVS : public Uniform<UniformDXVS>{

	ShaderDX* shader;
	size_t offset;
	size_t bsize;

public:
	UniformDXVS(ShaderDX* shader,
		size_t offset,
		size_t bsize)
		:shader(shader)
		, offset(offset)
		, bsize(bsize){}
	
	virtual void  set(const void* value, size_t s, size_t n){
		memcpy((&shader->getCpuVBuffer()[offset]) + bsize*s, value, bsize*n);
	}
	virtual void  set(const void* value) {
		memcpy(&(shader->getCpuVBuffer()[offset]), value, bsize);
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
class UniformDXPS : public Uniform<UniformDXPS> {

	ShaderDX* shader;
	size_t offset;
	size_t bsize;

public:
	UniformDXPS(ShaderDX* shader,
		size_t offset,
		size_t bsize)
		:shader(shader)
		, offset(offset)
		, bsize(bsize){}

	virtual void  set(const void* value, size_t s, size_t n){
		memcpy((&shader->getCpuPBuffer()[offset]) + bsize*s, value, bsize*n);
	}
	virtual void  set(const void* value) {
		memcpy(&(shader->getCpuPBuffer()[offset]), value, bsize);
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

CInt* ShaderDX::getConstInt(const char *name){
	if ('v'==*name)
		return (CInt*)new UniformDXVS(this, vVariablesRef[name], sizeof(int));
	if ('p' == *name)
		return (CInt*)new UniformDXPS(this, pVariablesRef[name], sizeof(int));
	return NULL;
}
CFloat* ShaderDX::getConstFloat(const char *name){
	if ('v' == *name)
		return (CFloat*)new UniformDXVS(this, vVariablesRef[name], sizeof(float));
	if ('p' == *name)
		return (CFloat*)new UniformDXPS(this, pVariablesRef[name], sizeof(float));
	return NULL;
}
CVec2* ShaderDX::getConstVec2(const char *name){
	if ('v' == *name)
		return (CVec2*)new UniformDXVS(this, vVariablesRef[name], sizeof(Vec2));
	if ('p' == *name)
		return (CVec2*)new UniformDXVS(this, pVariablesRef[name], sizeof(Vec2));
	return NULL;
}
CVec3* ShaderDX::getConstVec3(const char *name){
	if ('v' == *name)
		return (CVec3*)new UniformDXVS(this, vVariablesRef[name], sizeof(Vec3));
	if ('p' == *name)
		return (CVec3*)new UniformDXPS(this, pVariablesRef[name], sizeof(Vec3));
	return NULL;
}
CVec4* ShaderDX::getConstVec4(const char *name){
	if ('v' == *name)
		return (CVec4*)new UniformDXVS(this, vVariablesRef[name], sizeof(Vec4));
	if ('p' == *name)
		return (CVec4*)new UniformDXPS(this, pVariablesRef[name], sizeof(Vec4));
	return NULL;
}
CMat4* ShaderDX::getConstMat4(const char *name){
	if ('v' == *name)
		return (CMat4*)new UniformDXVS(this, vVariablesRef[name], sizeof(Mat4));
	if ('p' == *name)
		return (CMat4*)new UniformDXPS(this, pVariablesRef[name], sizeof(Mat4));
	return NULL;
}
CIntArray* ShaderDX::getConstIntArray(const char *name){
	if ('v' == *name)
		return (CIntArray*)new UniformDXVS(this, vVariablesRef[name], sizeof(int));
	if ('p' == *name)
		return (CIntArray*)new UniformDXPS(this, pVariablesRef[name], sizeof(int));
	return NULL;
}
CFloatArray* ShaderDX::getConstFloatArray(const char *name){
	if ('v' == *name)
		return (CFloatArray*)new UniformDXVS(this, vVariablesRef[name], sizeof(float));
	if ('p' == *name)
		return (CFloatArray*)new UniformDXPS(this, pVariablesRef[name], sizeof(float));
	return NULL;
}
CVec2Array* ShaderDX::getConstVec2Array(const char *name){
	if ('v' == *name)
		return (CVec2Array*)new UniformDXVS(this, vVariablesRef[name], sizeof(Vec2));
	if ('p' == *name)
		return (CVec2Array*)new UniformDXVS(this, pVariablesRef[name], sizeof(Vec2));
	return NULL;
}
CVec3Array* ShaderDX::getConstVec3Array(const char *name){
	if ('v' == *name)
		return (CVec3Array*)new UniformDXVS(this, vVariablesRef[name], sizeof(Vec3));
	if ('p' == *name)
		return (CVec3Array*)new UniformDXVS(this, pVariablesRef[name], sizeof(Vec3));
	return NULL;
}
CVec4Array* ShaderDX::getConstVec4Array(const char *name){
	if ('v' == *name)
		return (CVec4Array*)new UniformDXVS(this, vVariablesRef[name], sizeof(Vec4));
	if ('p' == *name)
		return (CVec4Array*)new UniformDXVS(this, pVariablesRef[name], sizeof(Vec4));
	return NULL;
}
CMat4Array* ShaderDX::getConstMat4Array(const char *name){
	if ('v' == *name)
		return (CMat4Array*)new UniformDXVS(this, vVariablesRef[name], sizeof(Mat4));
	if ('p' == *name)
		return (CMat4Array*)new UniformDXPS(this, pVariablesRef[name], sizeof(Mat4));
	return NULL;
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
