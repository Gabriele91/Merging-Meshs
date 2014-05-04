#include <stdafx.h>
#include <Render.h>
#include <RenderDX.h>
#include <Application.h>
#include <Screen.h>
#include <WindowsScreen.h>
#include <Shader.h>
#include <ShaderDX.h>
#include <Debug.h>
#include <EString.h>
#include <Dxerr.h>
//#define releaseDX(x) if(x){ x->Release(); delete x; x=NULL; } 
#define releaseDX(x) if(x){ x->Release(); x=NULL; } 
#define DX_ASSERT_MSG(hr) dxAssertMsg(hr, #hr ,__FILE__,__LINE__)

///////////////////////
namespace Easy3D{

inline static bool dxAssertMsg(HRESULT hr, const char* strcommand,const char* file,int line){
	if (FAILED(hr)){
		Debug::message() << "DirectX: " << DXGetErrorDescription(hr) << "\n";
		Debug::doassert(0, "DirectX Render"/*strcommand*/, file, line);
		return false;
	}
	return true;
}

static D3D10_BLEND ConvertBlendFunc(GLenum glBlend, bool isAlpha){
	D3D10_BLEND d3dBlend = D3D10_BLEND_ZERO;

	switch (glBlend){
		case GL_ZERO:                     d3dBlend = D3D10_BLEND_ZERO;                break;
		case GL_ONE:                      d3dBlend = D3D10_BLEND_ONE;                 break;
		case GL_SRC_COLOR:                d3dBlend = (isAlpha ? D3D10_BLEND_SRC_ALPHA : D3D10_BLEND_SRC_COLOR);           break;
		case GL_ONE_MINUS_SRC_COLOR:      d3dBlend = (isAlpha ? D3D10_BLEND_INV_SRC_ALPHA : D3D10_BLEND_INV_SRC_COLOR);   break;
		case GL_DST_COLOR:                d3dBlend = (isAlpha ? D3D10_BLEND_DEST_ALPHA : D3D10_BLEND_DEST_COLOR);         break;
		case GL_ONE_MINUS_DST_COLOR:      d3dBlend = (isAlpha ? D3D10_BLEND_INV_DEST_ALPHA : D3D10_BLEND_INV_DEST_COLOR); break;
		case GL_SRC_ALPHA:                d3dBlend = D3D10_BLEND_SRC_ALPHA;           break;
		case GL_ONE_MINUS_SRC_ALPHA:      d3dBlend = D3D10_BLEND_INV_SRC_ALPHA;       break;
		case GL_DST_ALPHA:                d3dBlend = D3D10_BLEND_DEST_ALPHA;          break;
		case GL_ONE_MINUS_DST_ALPHA:      d3dBlend = D3D10_BLEND_INV_DEST_ALPHA;      break;
		case GL_CONSTANT_COLOR:           d3dBlend = D3D10_BLEND_BLEND_FACTOR;        break;
		case GL_ONE_MINUS_CONSTANT_COLOR: d3dBlend = D3D10_BLEND_INV_BLEND_FACTOR;    break;
		case GL_CONSTANT_ALPHA:           d3dBlend = D3D10_BLEND_BLEND_FACTOR;        break;
		case GL_ONE_MINUS_CONSTANT_ALPHA: d3dBlend = D3D10_BLEND_INV_BLEND_FACTOR;    break;
		case GL_SRC_ALPHA_SATURATE:       d3dBlend = D3D10_BLEND_SRC_ALPHA_SAT;       break;
		default: break;
	}

	return d3dBlend;
}
static D3D10_BLEND_OP ConvertBlendOp(GLenum glBlendOp){
	D3D10_BLEND_OP d3dBlendOp = D3D10_BLEND_OP_ADD;

	switch (glBlendOp)
	{
		case GL_FUNC_ADD:              d3dBlendOp = D3D10_BLEND_OP_ADD;           break;
		case GL_FUNC_SUBTRACT:         d3dBlendOp = D3D10_BLEND_OP_SUBTRACT;      break;
		case GL_FUNC_REVERSE_SUBTRACT: d3dBlendOp = D3D10_BLEND_OP_REV_SUBTRACT;  break;
		default: break;
	}

	return d3dBlendOp;
}
static void glLikeBlendConfig(D3D10_BLEND_DESC& blendState, bool blend, GLenum src, GLenum dst){
	ZeroMemory(&blendState, sizeof(D3D10_BLEND_DESC));
	blendState.BlendEnable[0] = blend;
	blendState.SrcBlend = ConvertBlendFunc(src, 0);
	blendState.DestBlend = ConvertBlendFunc(dst, 0);
	blendState.BlendOp = D3D10_BLEND_OP_ADD;
	blendState.SrcBlendAlpha = ConvertBlendFunc(src, 1);
	blendState.DestBlendAlpha = ConvertBlendFunc(dst, 1);
	blendState.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
}


RenderDX::RenderDX() :contextInit(false){}
RenderDX::~RenderDX()
{
	if (contextInit){
		releaseDX(g_pRenderTargetView)
		releaseDX(g_pDepthStencil)
		releaseDX(g_pDepthStencilView)
		releaseDX(g_depthStencilState)
		releaseDX(g_depthDisabledStencilState)
		releaseDX(g_cullface_back)
		releaseDX(g_cullface_front)
		releaseDX(g_cullface_disable)
		releaseDX(g_pBlendState)
		releaseDX(d3dSwap)
		releaseDX(d3dDevice)
	}
}

void RenderDX::dxCreateBlend(bool blend, GLenum src, GLenum dst){
	if (g_pBlendState){
		releaseDX(g_pBlendState)
	}
	//blend state
	D3D10_BLEND_DESC blendState;
	glLikeBlendConfig(blendState, blend, src, dst);
	DX_ASSERT_MSG(d3dDevice->CreateBlendState(&blendState, &g_pBlendState));
	d3dDevice->OMSetBlendState(g_pBlendState, 0, 0xffffffff);
}


void RenderDX::__renderInit(){
	//OPENGL
	contextInit=true;
	//init
	//window
	HWND hWindow = ((WindowsScreen*)Application::instance()->getScreen())->hWind;
	RECT rc;
	GetClientRect(hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWindow;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	int createDeviceFlags=0;
#if defined( DEBUG ) || defined( _DEBUG )
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
	// create a device class and swap chain class using the information in the scd struct
	DX_ASSERT_MSG
		(D3D10CreateDeviceAndSwapChain(NULL,
									D3D10_DRIVER_TYPE_HARDWARE,
									NULL,
									createDeviceFlags,
									D3D10_SDK_VERSION,
									&scd,
									&d3dSwap,
									&d3dDevice));
	//////////////////////////////////////////////////////////////////////////////////////
	// create a texture for the depth buffer
	// Create a render target view
	ID3D10Texture2D* pBuffer;
	DX_ASSERT_MSG(d3dSwap->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBuffer));
	DX_ASSERT_MSG(d3dDevice->CreateRenderTargetView(pBuffer, NULL, &g_pRenderTargetView));
	DX_ASSERT_MSG(pBuffer->Release());

	// Create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	DX_ASSERT_MSG(d3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil));

	//////////////////////////////////////////////////////////////////////////////////////////
	//create depth desc state;
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
	//
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = (UINT8)0xFFFFFFFF;
	depthStencilDesc.StencilWriteMask = (UINT8)0xFFFFFFFF;
	//op if is front ace
	depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	//op if is back face
	depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	DX_ASSERT_MSG(d3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_depthStencilState));
	//disable zbuffer
	depthStencilDesc.DepthEnable = false;
	// Create the state using the device.
	DX_ASSERT_MSG(d3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_depthDisabledStencilState));
	//////////////////////////////////////////////////////////////////////////////////////////
	D3D10_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D10_RASTERIZER_DESC));
	wfdesc.FillMode = D3D10_FILL_SOLID;
	wfdesc.CullMode = D3D10_CULL_BACK;
	wfdesc.DepthClipEnable = true;
	DX_ASSERT_MSG(d3dDevice->CreateRasterizerState(&wfdesc, &g_cullface_back));
	wfdesc.CullMode = D3D10_CULL_FRONT;
	DX_ASSERT_MSG(d3dDevice->CreateRasterizerState(&wfdesc, &g_cullface_front));
	wfdesc.CullMode = D3D10_CULL_NONE;
	DX_ASSERT_MSG(d3dDevice->CreateRasterizerState(&wfdesc, &g_cullface_disable));
	//////////////////////////////////////////////////////////////////////////////////////////

	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	DX_ASSERT_MSG(d3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView));
	d3dDevice->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
	//////////////////////////////////////////////////////////////////////////////////////////
	//blend state
	dxCreateBlend(false, GL_ZERO, GL_ONE);
	//////////////////////////////////////////////////////////////////////////////////////
	//enable culling
	setCullFaceState(CullFaceState(CullFace::BACK)); 
	//enable z buffer
	setZBufferState(ZBufferState(true)); 
	//set clear color
	setClearColorState(Color(25, 128, 255, 255));
	// Setup the viewport
	setViewportState(Vec4(0, 0, width, height));

}

const ClearColorState& RenderDX::getClearColorState() const{
    return clearColorState;
}
void  RenderDX::setClearColorState(const ClearColorState& clearColor){
	clearColorState = clearColor;
}

void RenderDX::doClear(){
	float color[] = {
		clearColorState.color.rNormalize(),
		clearColorState.color.gNormalize(),
		clearColorState.color.bNormalize(),
		clearColorState.color.aNormalize()
	};
	d3dDevice->ClearRenderTargetView(g_pRenderTargetView, color);
	d3dDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
}

const ZBufferState& RenderDX::getZBufferState() const{
    return zBufferState;
}
void  RenderDX::setZBufferState(const ZBufferState& zb){
    
    if(zb.zbuffer)
		d3dDevice->OMSetDepthStencilState(g_depthStencilState, 1);
	else
		d3dDevice->OMSetDepthStencilState(g_depthDisabledStencilState, 1);

	zBufferState = zb;
}

const CullFaceState& RenderDX::getCullFaceState() const{
    return cullFaceState;
}
void  RenderDX::setCullFaceState(const CullFaceState& cf){
	//////////////////////////////////////////////////////////////////////////////////////////
	if(cf.cullface==CullFace::DISABLE)
		d3dDevice->RSSetState(g_cullface_disable);
	else if (cf.cullface == CullFace::FRONT)
		d3dDevice->RSSetState(g_cullface_front);
	else if (cf.cullface == CullFace::BACK)
		d3dDevice->RSSetState(g_cullface_back);
	//////////////////////////////////////////////////////////////////////////////////////////
	cullFaceState = cf;
}

const ViewportState& RenderDX::getViewportState() const {
    //return
	return viewportState;
}
void  RenderDX::setViewportState(const ViewportState& vs){
	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = vs.viewport.z;
	vp.Height = vs.viewport.w;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vs.viewport.x;
	vp.TopLeftY = vs.viewport.y;
	d3dDevice->RSSetViewports(1, &vp);
	viewportState = vs;
}

const BlendState& RenderDX::getBlendState() const{
	return blendState;
}
void  RenderDX::setBlendState(const BlendState& bs){
	//restore old blend state
	dxCreateBlend(bs.enable,bs.src,bs.dst);
	//save blend
	blendState = bs;
}


//projection matrix
Mat4 RenderDX::calculatesOrto(float left, float right, float bottom, float top, float vnear, float vfar) const {
	Mat4 projection;
	projection.setOrtho(left, right, bottom, top, vnear, vfar);
	projection[10] += 1;    // [-1 1] & [2fn]  -> [ 0 2]  & [2fn]
	projection[10] *= 0.5;  // [ 0 2] & [2fn]  -> [ 0 1]  & [2fn] 
	projection[10] -= 1;    // [ 0 1] & [2fn]  -> [-1 0]  & [2fn] 
	projection[14] *= 0.5;  // [-1 0] & [2fn]  -> [-1 0]  & [fn]
	return projection;
}
Mat4 RenderDX::calculatesProjection(float fovy, float aspect, float vnear, float vfar) const {
	Mat4 projection;
	projection.setPerspective(fovy, aspect, vnear, vfar);
	projection[10] += 1;    // [-1 1] & [2fn]  -> [ 0 2]  & [2fn]
	projection[10] *= 0.5;  // [ 0 2] & [2fn]  -> [ 0 1]  & [2fn] 
	projection[10] -= 1;    // [ 0 1] & [2fn]  -> [-1 0]  & [2fn] 
	projection[14] *= 0.5;  // [-1 0] & [2fn]  -> [-1 0]  & [fn]
	return projection;
}
Mat4 RenderDX::calculatesProjection(float left, float right, float bottom, float top, float vnear, float vfar) const {
	Mat4 projection;
	projection.setPerspective(left,right,bottom,top, vnear, vfar);
	projection[10] += 1;    // [-1 1] & [2fn]  -> [ 0 2]  & [2fn]
	projection[10] *= 0.5;  // [ 0 2] & [2fn]  -> [ 0 1]  & [2fn] 
	projection[10] -= 1;    // [ 0 1] & [2fn]  -> [-1 0]  & [2fn] 
	projection[14] *= 0.5;  // [-1 0] & [2fn]  -> [-1 0]  & [fn]
	return projection;
}

/*
 VBO & IBO
 */
class BufferObject{
public:
	ID3D10Buffer* buffer;
	BufferObject(ID3D10Buffer* idBuffer = 0) :buffer(idBuffer){}
	inline operator const ID3D10Buffer*() const{
		return buffer;
	}
	inline operator ID3D10Buffer*(){
		return buffer;
	}
	void free(){
		releaseDX(buffer);
	}
};

class BaseVertexBufferObject : public BufferObject {
public:
	size_t size;
	UINT stride;
	UINT offset;
	BaseVertexBufferObject(ID3D10Buffer*  buffer = 0) :BufferObject(buffer){}
	void genBuffer(ID3D10Device* device, const byte* vbo, size_t size) {
		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = size;
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData; 
		InitData.pSysMem = vbo;
		DX_ASSERT_MSG(device->CreateBuffer(&bd, &InitData, &buffer));
	}
};
class BaseIndextBufferObject : public BufferObject  {
public:
	BaseIndextBufferObject(ID3D10Buffer*  buffer = 0) :BufferObject(buffer){}
	void genBuffer(ID3D10Device* device, const ushort* ibo, size_t size) {
		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = size;
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = ibo;
		DX_ASSERT_MSG(device->CreateBuffer(&bd, &InitData, &buffer));
	}
};


BaseVertexBufferObject* RenderDX::createVBO(const byte* vbo, size_t stride, size_t n){
	auto ptr = new BaseVertexBufferObject();
	ptr->size = stride*n;
	ptr->stride = stride;
	ptr->offset = 0;
	ptr->genBuffer(d3dDevice, vbo, ptr->size);
    return ptr;
}
BaseIndextBufferObject* RenderDX::createIBO(const ushort* ibo, size_t size){
	auto ptr = new BaseIndextBufferObject();
	ptr->genBuffer(d3dDevice, ibo, size*sizeof(ushort));
    return ptr;
}
void RenderDX::bindVBO(BaseVertexBufferObject* vbo){
    //glBindBuffer(GL_ARRAY_BUFFER,*vbo);
	d3dDevice->IASetVertexBuffers(0, 1, &vbo->buffer, &vbo->stride, &vbo->offset);
}
void RenderDX::bindIBO(BaseIndextBufferObject* ibo){
	d3dDevice->IASetIndexBuffer(*ibo, DXGI_FORMAT_R16_UINT, 0);
    //glBindBuffer(GL_ARRAY_BUFFER,*ibo);
}
void RenderDX::deleteVBO(BaseVertexBufferObject* vbo){
    vbo->free();
    delete vbo;
}
void RenderDX::deleteIBO(BaseIndextBufferObject* ibo){
    ibo->free();
    delete ibo;
}
/*
 Draw
 */
inline static D3D10_PRIMITIVE_TOPOLOGY getDirectXDrawType(TypeDraw type){
	switch (type){
		case DRAW_POINTS: return D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;  break;
		case DRAW_LINES: return D3D10_PRIMITIVE_TOPOLOGY_LINELIST;  break;
		case DRAW_LINE_LOOP: return D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;  break;
		case DRAW_TRIANGLES: return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
		case DRAW_TRIANGLE_STRIP: return D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;  break;
		default: return D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED; break;
    }
}
void RenderDX::drawArrays(TypeDraw type, uint n){
	DEBUG_ASSERT(currentShader);
	currentShader->uniform();
	d3dDevice->IASetPrimitiveTopology(getDirectXDrawType(type));
	d3dDevice->Draw(n,0);
}
void RenderDX::drawElements(TypeDraw type, uint n){
	DEBUG_ASSERT(currentShader);
	currentShader->uniform();
	d3dDevice->IASetPrimitiveTopology(getDirectXDrawType(type));
	d3dDevice->DrawIndexed(n, 0, 0);
}
/*
 InputLayout
 */
class BaseInputLayout{
public:
	ID3D10InputLayout* pVertexLayout = NULL;
};
  
    
    
Shader* RenderDX::createShader(){
    return (Shader*)new ShaderDX(this);
}
void RenderDX::deleteShader(Shader* s){
    delete ((ShaderDX*)s);
}
void RenderDX::bindShader(Shader* shader){
	currentShader = (ShaderDX*)shader;
	currentShader->bind();
}
void RenderDX::unbindShader(){
	currentShader->unbind();
	currentShader = NULL;
}

inline static DXGI_FORMAT convertTypeIL(AttributeType type){
	switch (type)
	{
	case Easy3D::ATT_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
		break;
	case Easy3D::ATT_FLOAT2:
		return DXGI_FORMAT_R32G32_FLOAT;
		break;
	case Easy3D::ATT_FLOAT3:
		return DXGI_FORMAT_R32G32B32_FLOAT;
		break;
	case Easy3D::ATT_FLOAT4:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case Easy3D::ATT_INT:
		return DXGI_FORMAT_R32_SINT;
		break;
	case Easy3D::ATT_INT2:
		return DXGI_FORMAT_R32G32_SINT;
		break;
	case Easy3D::ATT_INT3:
		return DXGI_FORMAT_R32G32B32_SINT;
		break;
	case Easy3D::ATT_INT4:
		return DXGI_FORMAT_R32G32B32A32_SINT;
		break;
	case Easy3D::ATT_UINT:
		return DXGI_FORMAT_R32_UINT;
		break;
	case Easy3D::ATT_UINT2:
		return DXGI_FORMAT_R32G32_UINT;
		break;
	case Easy3D::ATT_UINT3:
		return DXGI_FORMAT_R32G32B32_UINT;
		break;
	case Easy3D::ATT_UINT4:
		return DXGI_FORMAT_R32G32B32A32_UINT;
		break;
	case Easy3D::ATT_TLESS:
		return DXGI_FORMAT_R32_TYPELESS;
		break;
	case Easy3D::ATT_TLESS2:
		return DXGI_FORMAT_R32G32_TYPELESS;
		break;
	case Easy3D::ATT_TLESS3:
		return DXGI_FORMAT_R32G32B32_TYPELESS;
		break;
	case Easy3D::ATT_TLESS4:
		return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		break;
	default:
		return DXGI_FORMAT_UNKNOWN;
		break;
	}
}

BaseInputLayout* RenderDX::createIL(Shader* shader, const AttributeList& atl){
	
	//get binary code vertex shader
	ID3D10Blob *vShaderBinary = ((ShaderDX*)shader)->vShaderBinary;
	
	//create input li
    auto il=new BaseInputLayout();
	
	//directx li
	D3D10_INPUT_ELEMENT_DESC *layout = new D3D10_INPUT_ELEMENT_DESC[atl.countAttribute()];
	
	//for each  attribute
	size_t i = 0;
	size_t offset = 0;

	//create attributes
	for (const auto& at : atl){
		layout[i].SemanticName = at.name;
		layout[i].SemanticIndex = 0;
		layout[i].Format = convertTypeIL(at.type);
		layout[i].InputSlot = 0;
		layout[i].AlignedByteOffset = offset;
		layout[i].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[i].InstanceDataStepRate = 0;
		/*
		next step
		*/
		offset += at.size();
		++i;
	}

	//alloc in render
	DX_ASSERT_MSG(d3dDevice->CreateInputLayout(layout, atl.countAttribute(), 
											   vShaderBinary->GetBufferPointer(), vShaderBinary->GetBufferSize(), 
											   &(il->pVertexLayout)));
	
	//release 
	delete layout;

    return  il;
}
void RenderDX::bindIL(BaseInputLayout* il){
	d3dDevice->IASetInputLayout(il->pVertexLayout);
}
void RenderDX::unbindIL(BaseInputLayout* il){
	d3dDevice->IASetInputLayout(il->pVertexLayout);
}
void RenderDX::deleteIL(BaseInputLayout* il){
	releaseDX(il->pVertexLayout)
    delete  il;
}
/*
 Textures
 */
BaseTexture* RenderDX::sendTexture2D(size_t w, size_t h, void* byte, TextureType type){ return NULL; }
BaseRenderTexture* RenderDX::createRenderTexture(size_t w, size_t h, size_t zbuffer, RenderTextureType type){ return NULL; }

void RenderDX::enableTexture2D(BaseTexture*){}
void RenderDX::enableRenderTexture(BaseTexture*){}

void RenderDX::disableTexture2D(BaseTexture*){}
void RenderDX::disableRenderTexture(BaseTexture*){}

void RenderDX::deleteTexture2D(BaseTexture*){}
void RenderDX::deleteRenderTexture(BaseRenderTexture*){}

void RenderDX::enableRenderToTexture(BaseRenderTexture*){}
void RenderDX::disableRanderToTexture(BaseRenderTexture*){}


void RenderDX::swap(){
	DX_ASSERT_MSG(d3dSwap->Present(0, 0));
}
void RenderDX::acquireContext(){
	//void
}

};