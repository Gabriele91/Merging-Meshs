#include <stdafx.h>
#include <Debug.h>
#include <Render.h>
#include <Shader.h>
#include <D3D10.h>
#include <Dxerr.h>
#include <Screen.h>
#include <RenderDX.h>
#include <ShaderDX.h>
#include <Application.h>
#include <WindowsScreen.h>
#include <EString.h>
#include <Dxerr.h>
//#define releaseDX(x) if(x){ x->Release(); delete x; x=NULL; } 
#define releaseDX(x) if(x){ x->Release(); x=nullptr; } 
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
#if defined( DEBUG ) || defined( _DEBUG )
#pragma comment(lib, "DXGI.lib")
static void debugNvidiaDriver(IDXGIAdapter** selectedAdapter, D3D10_DRIVER_TYPE* driverType){
	// Look for 'NVIDIA PerfHUD' adapter 
	// If it is present, override default settings 
	IDXGIFactory *pDXGIFactory;
	ID3D10Device *pDevice;
	HRESULT hRes;
	hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
	// Search for a PerfHUD adapter. 
	UINT nAdapter = 0;
	IDXGIAdapter* adapter = NULL;
	//default
	(*selectedAdapter) = NULL;
	(*driverType) = D3D10_DRIVER_TYPE_HARDWARE;
	//search
	while (pDXGIFactory->EnumAdapters(nAdapter, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		if (adapter){
			DXGI_ADAPTER_DESC adaptDesc;
			if (SUCCEEDED(adapter->GetDesc(&adaptDesc))){
				const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;

				// Select the first adapter in normal circumstances or the PerfHUD one if it exists.
				if (nAdapter == 0 || isPerfHUD)
					(*selectedAdapter) = adapter;

				if (isPerfHUD)
					(*driverType) = D3D10_DRIVER_TYPE_REFERENCE;

			}
		}
		++nAdapter;
	}
}
#endif

RenderDX::RenderDX() :contextInit(false){}
RenderDX::~RenderDX()
{
	if (contextInit){
		releaseDX(g_pRenderTargetView)
		releaseDX(g_pDepthStencil)
		releaseDX(g_pDepthStencilView)
		releaseDX(g_depthStencilState)
		releaseDX(g_stencilStateNoDepth)
		releaseDX(g_depthStateNoStencil)
		releaseDX(g_stateNoDepthNoStencil)
		releaseDX(g_cullface_back)
		releaseDX(g_cullface_front)
		releaseDX(g_cullface_disable)
		releaseDX(g_pBlendState)
		releaseDX(g_pQueryTextureColor1x1)
		releaseDX(g_pQueryTextureDepth1x1)
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


	IDXGIAdapter* selectedAdapter = NULL;
	D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;
	int createDeviceFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	debugNvidiaDriver(&selectedAdapter, &driverType);
	if (!selectedAdapter) createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
	// create a device class and swap chain class using the information in the scd struct
	DX_ASSERT_MSG(D3D10CreateDeviceAndSwapChain(
		          selectedAdapter,
				  driverType,
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
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc = {
		TRUE,                           // BOOL DepthEnable;
		D3D10_DEPTH_WRITE_MASK_ALL,     // D3D10_DEPTH_WRITE_MASK DepthWriteMask;
		D3D10_COMPARISON_LESS_EQUAL,    // D3D10_COMPARISON_FUNC DepthFunc;
		TRUE,                           // BOOL StencilEnable;
		(UINT8)0xFFFFFFFF,              // UINT8 StencilReadMask;
		(UINT8)0xFFFFFFFF,              // UINT8 StencilWriteMask;
		{                               // D3D10_DEPTH_STENCILOP_DESC FrontFace;
			D3D10_STENCIL_OP_KEEP,      // D3D10_STENCIL_OP StencilFailOp;
			D3D10_STENCIL_OP_INCR,      // D3D10_STENCIL_OP StencilDepthFailOp;
			D3D10_STENCIL_OP_KEEP,      // D3D10_STENCIL_OP StencilPassOp;
			D3D10_COMPARISON_ALWAYS,    // D3D10_COMPARISON_FUNC StencilFunc;
		},
		{                               // D3D10_DEPTH_STENCILOP_DESC BackFace;
			D3D10_STENCIL_OP_KEEP,      // D3D10_STENCIL_OP StencilFailOp;
			D3D10_STENCIL_OP_DECR,      // D3D10_STENCIL_OP StencilDepthFailOp;
			D3D10_STENCIL_OP_KEEP,      // D3D10_STENCIL_OP StencilPassOp;
			D3D10_COMPARISON_ALWAYS,    // D3D10_COMPARISON_FUNC StencilFunc;
		},
	};
	// Create the depth stencil state.
	DX_ASSERT_MSG(d3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_depthStencilState));
	//disable zbuffer
	depthStencilDesc.DepthEnable = false;
	// Create the state using the device.
	DX_ASSERT_MSG(d3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_stencilStateNoDepth));

	//no stencil 
	D3D10_DEPTH_STENCIL_DESC depthStencilDescNoStencil = {
		TRUE,                           // BOOL DepthEnable;
		D3D10_DEPTH_WRITE_MASK_ALL,     // D3D11_DEPTH_WRITE_MASK DepthWriteMask;
		D3D10_COMPARISON_LESS_EQUAL,    // D3D11_COMPARISON_FUNC DepthFunc;
		FALSE,                          // BOOL StencilEnable;
		0,                              // UINT8 StencilReadMask;
		0,                              // UINT8 StencilWriteMask;
		{                               // D3D11_DEPTH_STENCILOP_DESC FrontFace;
			D3D10_STENCIL_OP_KEEP,      // D3D11_STENCIL_OP StencilFailOp;
			D3D10_STENCIL_OP_KEEP,      // D3D11_STENCIL_OP StencilDepthFailOp;
			D3D10_STENCIL_OP_KEEP,      // D3D11_STENCIL_OP StencilPassOp;
			D3D10_COMPARISON_NEVER,     // D3D11_COMPARISON_FUNC StencilFunc;
		},
		{                               // D3D11_DEPTH_STENCILOP_DESC BackFace;
			D3D10_STENCIL_OP_KEEP,      // D3D11_STENCIL_OP StencilFailOp;
			D3D10_STENCIL_OP_KEEP,      // D3D11_STENCIL_OP StencilDepthFailOp;
			D3D10_STENCIL_OP_KEEP,      // D3D11_STENCIL_OP StencilPassOp;
			D3D10_COMPARISON_NEVER,     // D3D11_COMPARISON_FUNC StencilFunc;
		},
	};
	// Create the state using the device.
	DX_ASSERT_MSG(d3dDevice->CreateDepthStencilState(&depthStencilDescNoStencil, &g_depthStateNoStencil));
	//disable zbuffer
	depthStencilDescNoStencil.DepthEnable = false;
	// Create the state using the device.
	DX_ASSERT_MSG(d3dDevice->CreateDepthStencilState(&depthStencilDescNoStencil, &g_stateNoDepthNoStencil));
	//////////////////////////////////////////////////////////////////////////////////////////
	D3D10_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D10_RASTERIZER_DESC));
	wfdesc.FillMode = D3D10_FILL_SOLID;
	wfdesc.CullMode = D3D10_CULL_BACK;
	wfdesc.DepthClipEnable = true;
	wfdesc.FrontCounterClockwise = true;
	wfdesc.DepthBias = false;
	wfdesc.DepthBiasClamp = 0;
	wfdesc.SlopeScaledDepthBias = 0;
	//wfdesc.MultisampleEnable = false;
	//wfdesc.AntialiasedLineEnable = true;
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
	/////////////////////////////////////////////////////////////////////////////////////////
	//SET RANDER TARGETS
	setRenderTargets(g_pRenderTargetView, g_pDepthStencilView);
	/////////////////////////////////////////////////////////////////////////////////////////
	//Query textures - CPU Access buffer
	//Color buffer
	D3D10_TEXTURE2D_DESC StagedColorDesc = {
		1,//UINT Width;
		1,//UINT Height;
		1,//UINT MipLevels;
		1,//UINT ArraySize;
		DXGI_FORMAT_R8G8B8A8_UNORM,	   //DXGI_FORMAT Format;DXGI_FORMAT_R8G8B8A8_UNORM
		1, 0,                          //DXGI_SAMPLE_DESC SampleDesc;
		D3D10_USAGE_STAGING,           //D3D10_USAGE Usage;
		0,					           //UINT BindFlags;
		D3D10_CPU_ACCESS_READ,         //UINT CPUAccessFlags;
		0					           //UINT MiscFlags;
	};
	d3dDevice->CreateTexture2D(&StagedColorDesc, NULL, &g_pQueryTextureColor1x1);
	//Depth buffer
	D3D10_TEXTURE2D_DESC StagedDepthDesc = {
		1,//UINT Width;
		1,//UINT Height;
		1,//UINT MipLevels;
		1,//UINT ArraySize;
		DXGI_FORMAT_D32_FLOAT,	   	   //DXGI_FORMAT Format;
		1, 0,                          //DXGI_SAMPLE_DESC SampleDesc;
		D3D10_USAGE_STAGING,           //D3D10_USAGE Usage;
		0,					           //UINT BindFlags;
		D3D10_CPU_ACCESS_READ,         //UINT CPUAccessFlags;
		0					           //UINT MiscFlags;
	};
	d3dDevice->CreateTexture2D(&StagedDepthDesc, NULL, &g_pQueryTextureDepth1x1);
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
	if (currentRenderTargetView)
		d3dDevice->ClearRenderTargetView(currentRenderTargetView, color);
	if (currentDepthStencilView)
		d3dDevice->ClearDepthStencilView(currentDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
}

void RenderDX::setRenderTargets(ID3D10RenderTargetView* renderTargetView, ID3D10DepthStencilView* depthStencilView){
	if (renderTargetView)
		d3dDevice->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	else
		d3dDevice->OMSetRenderTargets(0, 0, depthStencilView);
	//save state
	currentRenderTargetView = renderTargetView;
	currentDepthStencilView = depthStencilView;
}

const ZBufferState& RenderDX::getZBufferState() const{
    return zBufferState;
}
void  RenderDX::setZBufferState(const ZBufferState& zb){
    
    if(zb.zbuffer)
		d3dDevice->OMSetDepthStencilState(g_depthStateNoStencil, 1);
	else
		d3dDevice->OMSetDepthStencilState(g_stateNoDepthNoStencil, 1);

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
	projection.setOrthoRHDX(left, right, bottom, top, vnear, vfar);
	return projection;
}
Mat4 RenderDX::calculatesProjection(float fovy, float aspect, float vnear, float vfar) const {
	Mat4 projection;
	projection.setPerspectiveRHDX(fovy, aspect, vnear, vfar);
	return projection;
}
Mat4 RenderDX::calculatesProjection(float left, float right, float bottom, float top, float vnear, float vfar) const {
	Mat4 projection;
	projection.setPerspectiveRHDX(left,right,bottom,top, vnear, vfar);
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
	void genBuffer(ID3D10Device* device, const uint* ibo, size_t size) {
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
BaseIndextBufferObject* RenderDX::createIBO(const uint* ibo, size_t size){
	auto ptr = new BaseIndextBufferObject();
	ptr->genBuffer(d3dDevice, ibo, size*sizeof(uint));
    return ptr;
}
void RenderDX::bindVBO(BaseVertexBufferObject* vbo){
    //glBindBuffer(GL_ARRAY_BUFFER,*vbo);
	d3dDevice->IASetVertexBuffers(0, 1, &vbo->buffer, &vbo->stride, &vbo->offset);
}
void RenderDX::bindIBO(BaseIndextBufferObject* ibo){
	d3dDevice->IASetIndexBuffer(*ibo, DXGI_FORMAT_R32_UINT, 0);
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
	auto newShader=new ShaderDX(this);
	return static_cast<Shader*>(newShader);
}
void RenderDX::deleteShader(Shader* inshader){
	auto sdx = dynamic_cast<ShaderDX*>(inshader);
	delete sdx;
}
void RenderDX::bindShader(Shader* inshader){
	currentShader = dynamic_cast<ShaderDX*>(inshader);
	currentShader->bind();
}
void RenderDX::unbindShader(){
	DEBUG_ASSERT(currentShader);
	currentShader->unbind();
	currentShader = nullptr;
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


//DEPTH
float RenderDX::getDepth(const Vec2& pixel){
	//create box
	D3D10_BOX srcBox;
	srcBox.left = pixel.x;
	srcBox.right = srcBox.left + 1;
	srcBox.top = pixel.y;
	srcBox.bottom = srcBox.top + 1;
	srcBox.front = 0;
	srcBox.back = 1;
	//get image
	d3dDevice->CopySubresourceRegion(g_pQueryTextureDepth1x1, 0, 0, 0, 0, g_pDepthStencil, 0, &srcBox);
	//mapped
	D3D10_MAPPED_TEXTURE2D  maptex2d;
	g_pQueryTextureDepth1x1->Map(0, D3D10_MAP_READ, 0, &maptex2d);
	//result
	float buffer = *((float*)maptex2d.pData);
	// copy data
	g_pQueryTextureDepth1x1->Unmap(0);

	return buffer;
}

//RGBA
Vec4  RenderDX::getColor(const Vec2& pixel){
	
	//create box
	D3D10_BOX srcBox;
	srcBox.left = pixel.x;
	srcBox.right = srcBox.left + 1;
	srcBox.top = pixel.y;
	srcBox.bottom = srcBox.top + 1;
	srcBox.front = 0;
	srcBox.back = 1;

	/////////////////////////////////////////////////////////////////////////////////////////
	//back
	ID3D10Texture2D* texture;
	ID3D10Resource* res;
	g_pRenderTargetView->GetResource(&res);
	res->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)&texture);
	//D3D10_TEXTURE2D_DESC dsc;
	//texture->GetDesc(&dsc);
	//get pixel
	d3dDevice->CopySubresourceRegion(g_pQueryTextureColor1x1, 0, 0, 0, 0, texture, 0, &srcBox);
	//release
	res->Release();
	/////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////
	//mapped
	D3D10_MAPPED_TEXTURE2D  maptex2d;
	g_pQueryTextureColor1x1->Map(0, D3D10_MAP_READ, 0, &maptex2d);
	//result
	uchar* color=(uchar*)maptex2d.pData;
	Vec4 buffer(Color(color[0],
					  color[1],
					  color[2],
					  color[3]).argbNormalize());
	// copy data
	g_pQueryTextureColor1x1->Unmap(0);
	/////////////////////////////////////////////////////////////////////////////////////////

	return buffer;
}
/*
 Textures
 */
class BaseTexture{

public:
	ID3D10Texture2D* texture2D{ nullptr };
	ID3D10SamplerState* sempler{ nullptr };
	ID3D10ShaderResourceView* resourceView{ nullptr };

	void createSemple(ID3D10Device* d3dDevice,
					  D3D10_FILTER filter,
					  D3D10_TEXTURE_ADDRESS_MODE mode, 
					  D3D10_COMPARISON_FUNC compfun){
		//input
		D3D10_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = filter;
		sampDesc.AddressU = mode;
		sampDesc.AddressV = mode;
		sampDesc.AddressW = mode;
		sampDesc.ComparisonFunc = compfun;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D10_FLOAT32_MAX;
		//create
		DX_ASSERT_MSG(d3dDevice->CreateSamplerState(&sampDesc, &sempler));
		//D3D10_FILTER_MIN_MAG_MIP_LINEAR
		//D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT

	}

	void createResourceView(ID3D10Device* d3dDevice, 
							DXGI_FORMAT format,
						    uint detailedMip=0,
							uint mipLivel=1){
		//create resource view
		D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = detailedMip;
		srvDesc.Texture2D.MipLevels = mipLivel;
		DX_ASSERT_MSG(d3dDevice->CreateShaderResourceView(texture2D, &srvDesc, &resourceView));
	}

};

BaseTexture* RenderDX::sendTexture2D(size_t w, size_t h, void* byte, TextureType type){
	
	DXGI_FORMAT dxTextureType = DXGI_FORMAT_R8G8B8A8_UNORM;
	size_t bsize=0;

	switch (type){
		case TX_RGBA8: dxTextureType = DXGI_FORMAT_R8G8B8A8_UNORM; bsize = 4; break;
		case TX_RG8: dxTextureType = DXGI_FORMAT_R8G8_UNORM; bsize = 2; break;
		case TX_R8: dxTextureType = DXGI_FORMAT_R8_UNORM; bsize = 1; break;
		case TX_RG16: dxTextureType = DXGI_FORMAT_R16G16_UNORM; bsize = 4; break;
		case TX_R16: dxTextureType = DXGI_FORMAT_R16_UNORM; bsize = 2; break;
		default:      break;
	}

	//Depth buffer
	D3D10_TEXTURE2D_DESC textureDesc = {
		w,//UINT Width;
		h,//UINT Height;
		1,//UINT MipLevels;
		1,//UINT ArraySize;
		dxTextureType,	   			   //DXGI_FORMAT Format;
		1, 0,                          //DXGI_SAMPLE_DESC SampleDesc;
		D3D10_USAGE_DEFAULT,           //D3D10_USAGE Usage;
	    D3D10_BIND_SHADER_RESOURCE ,   //UINT BindFlags;
		0,							   //UINT CPUAccessFlags;
		0					           //UINT MiscFlags;
	};
	//texture ptr
	BaseTexture* texture = new BaseTexture();
	//send memory
	D3D10_SUBRESOURCE_DATA sSubData;
	sSubData.pSysMem = byte;
	sSubData.SysMemPitch = (UINT)(w * bsize);
	sSubData.SysMemSlicePitch = (UINT)(w * h * bsize);
	d3dDevice->CreateTexture2D(&textureDesc, &sSubData, &texture->texture2D);
	//create semple
	texture->createSemple(d3dDevice,
						  D3D10_FILTER_MIN_MAG_MIP_POINT,
						  D3D10_TEXTURE_ADDRESS_WRAP,
						  D3D10_COMPARISON_NEVER);
	//create resource view
	texture->createResourceView(d3dDevice, textureDesc.Format);
	//ptr to the texture
	return texture;

}

class BaseRenderTexture : public BaseTexture{
	

public:

	ID3D10Texture2D* texture2Ddepth{ nullptr };
	ID3D10RenderTargetView* renderTargetView{ nullptr };
	ID3D10DepthStencilView* depthStencilView{ nullptr };

	void createRenderTargetView(ID3D10Device* d3dDevice, DXGI_FORMAT format, uint mipSlice=0){
		D3D10_RENDER_TARGET_VIEW_DESC tvmDesc;
		tvmDesc.Format = format;
		tvmDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		tvmDesc.Texture2D.MipSlice = mipSlice;
		DX_ASSERT_MSG(d3dDevice->CreateRenderTargetView(texture2D, &tvmDesc, &renderTargetView));
	}
	void createDepthStencilView(ID3D10Device* d3dDevice, DXGI_FORMAT format, uint mipSlice=0){
		D3D10_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = format;
		dsvDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = mipSlice;
		DX_ASSERT_MSG(d3dDevice->CreateDepthStencilView(texture2D, &dsvDesc, &depthStencilView));
	}
	void createRenderTexture(ID3D10Device* d3dDevice, 
							 uint width,
							 uint height,
							 uint mipLevels,
							 DXGI_FORMAT format, 
							 uint bindFlags){
		D3D10_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevels;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D10_USAGE_DEFAULT;
		textureDesc.BindFlags = bindFlags;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		DX_ASSERT_MSG(d3dDevice->CreateTexture2D(&textureDesc, NULL, &texture2D));
	}

	void addDepthStencil32Bit(ID3D10Device* d3dDevice,
							  uint width,
							  uint height){
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
		DX_ASSERT_MSG(d3dDevice->CreateTexture2D(&descDepth, NULL, &texture2Ddepth));
		// Create the depth stencil view
		D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		DX_ASSERT_MSG(d3dDevice->CreateDepthStencilView(texture2Ddepth, &descDSV, &depthStencilView));
	}
};

BaseRenderTexture* RenderDX::createRenderTexture(size_t w, 
	                                             size_t h, 
												 size_t zbuffer, 
												 RenderTextureType type){
	DXGI_FORMAT dxTextureType = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT targetType    = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT resViewType   = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D10_COMPARISON_FUNC cmpfunction = D3D10_COMPARISON_NEVER;
	D3D10_TEXTURE_ADDRESS_MODE mode   = D3D10_TEXTURE_ADDRESS_WRAP;
	uint bindFlags = D3D10_BIND_SHADER_RESOURCE;

	switch (type){
	case RD_RGBA:
		//texture
		dxTextureType = DXGI_FORMAT_R8G8B8A8_UNORM;
		bindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		//semple 
		mode = D3D10_TEXTURE_ADDRESS_WRAP;
		cmpfunction = D3D10_COMPARISON_NEVER;
		//semple 
		mode = D3D10_TEXTURE_ADDRESS_WRAP;
		//Sheder Resource View
		resViewType = DXGI_FORMAT_R8G8B8A8_UNORM;
		//RenderTarget
	    targetType = DXGI_FORMAT_R8G8B8A8_UNORM;
	break;
	case RD_SHADOW:
		//texture
		dxTextureType = DXGI_FORMAT_R32_TYPELESS;
		bindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE; 
		//semple 
		mode = D3D10_TEXTURE_ADDRESS_CLAMP;
		cmpfunction = D3D10_COMPARISON_LESS;
		//Sheder Resource View
		resViewType = DXGI_FORMAT_R32_FLOAT;
		//Depth Stencil View
		targetType = DXGI_FORMAT_D32_FLOAT;
	break;
	default:      break;
	}

	//texture ptr
	BaseRenderTexture* texture = new BaseRenderTexture();
	//create texture
	texture->createRenderTexture(d3dDevice, w, h, 1, dxTextureType, bindFlags);
	//no linear
	texture->createSemple(d3dDevice, D3D10_FILTER_MIN_MAG_MIP_POINT, mode, cmpfunction);
	//create resource view
	texture->createResourceView(d3dDevice, resViewType, 0, 1);
	// Create the render target view/ depth stecil.
	switch (type){
		case RD_RGBA: 
			texture->createRenderTargetView(d3dDevice, targetType);
			texture->addDepthStencil32Bit(d3dDevice, w, h);
		break;
		case RD_SHADOW:
			texture->createDepthStencilView(d3dDevice, targetType);
		break;
		default: break;
	}

	return texture;

}


void RenderDX::enableVSTexture(uint t, uint s, BaseTexture* texture){
	d3dDevice->VSSetShaderResources(t, 1, &texture->resourceView);
	d3dDevice->VSSetSamplers(s, 1, &texture->sempler);
}
void RenderDX::enablePSTexture(uint t, uint s, BaseTexture* texture){
	d3dDevice->PSSetShaderResources(t, 1, &texture->resourceView);
	d3dDevice->PSSetSamplers(s, 1, &texture->sempler);
}

void RenderDX::enableVSTexture(uint t, uint s, BaseRenderTexture* texture){
	d3dDevice->PSSetShaderResources(t, 1, &texture->resourceView);
	d3dDevice->PSSetSamplers(s, 1, &texture->sempler);
}
void RenderDX::enablePSTexture(uint t, uint s, BaseRenderTexture* texture){
	d3dDevice->PSSetShaderResources(t, 1, &texture->resourceView);
	d3dDevice->PSSetSamplers(s, 1, &texture->sempler);
}
void RenderDX::disableVSTexture(uint t, uint s){
	ID3D10SamplerState* sS = NULL;
	ID3D10ShaderResourceView* srV = NULL;
	d3dDevice->VSSetShaderResources(t, 1, &srV);
	d3dDevice->VSSetSamplers(s, 1, &sS);
}
void RenderDX::disablePSTexture(uint t, uint s){
	ID3D10SamplerState* sS = NULL;
	ID3D10ShaderResourceView* srV = NULL;
	d3dDevice->PSSetShaderResources(t, 1, &srV);
	d3dDevice->PSSetSamplers(s, 1, &sS);
}

void RenderDX::deleteTexture(BaseTexture* texture){
	releaseDX(texture->sempler)
	releaseDX(texture->resourceView)
	releaseDX(texture->texture2D)
	delete texture;
}
void RenderDX::deleteTexture(BaseRenderTexture* texture){
	releaseDX(texture->sempler)
	releaseDX(texture->renderTargetView)
	releaseDX(texture->depthStencilView)
	releaseDX(texture->resourceView)
	releaseDX(texture->texture2D)
	releaseDX(texture->texture2Ddepth)
	delete texture;
}

void RenderDX::beginRenderToTexture(BaseRenderTexture* texture){

	if (texture->renderTargetView && texture->depthStencilView)
		setRenderTargets(texture->renderTargetView, texture->depthStencilView);
	else if (texture->renderTargetView)
		setRenderTargets(texture->renderTargetView, g_pDepthStencilView);
	else
		setRenderTargets(0, texture->depthStencilView);
}
void RenderDX::endRenderToTexture(BaseRenderTexture*){
	//set screen
	setRenderTargets(g_pRenderTargetView, g_pDepthStencilView);
}


void RenderDX::swap(){
	DX_ASSERT_MSG(d3dSwap->Present(0, 0));
}
void RenderDX::acquireContext(){
	//void
}

};