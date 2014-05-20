#ifndef RENDERDX_H
#define RENDERDX_H

#include <Config.h>
#include <Math3D.h>
#include <Render.h>
#include <Blend.h>
#include <Color.h>
#include <Render.h>
#include <d3d10.h>
#include <d3dx10.h>

namespace Easy3D {
    
    ////////////
	class Application;
	class Screen;
    ////////////
    
    class BaseTexture;
    class BaseRenderTexture;
    class BaseVertexBufferObject;
    class BaseIndextBufferObject;
    class BaseInputLayout;
    class Shader;
    
	class RenderDX : public Render {

		friend  class Application;
		friend  class Screen;
		friend  class ShaderDX;
		friend  class UniformTexture;
		friend  class UniformRenderTexture;
		virtual void __renderInit();

    public:
        
		RenderDX();
		~RenderDX();
		virtual RenderDriver getRenderDriver(){ return DIRECTX_DRIVER; };
        virtual const ClearColorState& getClearColorState() const;
		virtual void setClearColorState(const ClearColorState& cf);
		virtual void doClear();
        
		virtual const ZBufferState& getZBufferState() const;
		virtual void setZBufferState(const ZBufferState& cf);
        
		virtual const CullFaceState& getCullFaceState() const;
		virtual void setCullFaceState(const CullFaceState& cf);
        
		virtual const ViewportState& getViewportState() const;
		virtual void setViewportState(const ViewportState& vs);
        
		virtual const BlendState& getBlendState() const;
		virtual void setBlendState(const BlendState& bs);

		//projection matrix

		virtual Mat4 calculatesOrto(float left, float right, float bottom, float top, float vNear, float vFar) const;
		virtual Mat4 calculatesProjection(float fovy, float aspect, float vNear, float vFar) const;
		virtual Mat4 calculatesProjection(float left, float right, float bottom, float top, float vNear, float vFar) const;

        //BO
		//virtual BaseVertexBufferObject* createVBO(const byte* vbo, size_t size, size_t strip, size_t offset)=0;
		virtual BaseVertexBufferObject* createVBO(const byte* vbo, size_t stride, size_t n);
		virtual BaseIndextBufferObject* createIBO(const uint* ibo, size_t size);
        
		virtual void bindVBO(BaseVertexBufferObject*);
		virtual void bindIBO(BaseIndextBufferObject*);
        
		virtual void deleteVBO(BaseVertexBufferObject*);
		virtual void deleteIBO(BaseIndextBufferObject*);
        
        //draw
		virtual void drawArrays(TypeDraw type, uint n);
		virtual void drawElements(TypeDraw type, uint n);
		/*
			Shader
		*/
		virtual Shader* createShader();
		virtual void deleteShader(Shader*);
		virtual void bindShader(Shader* shader);
		virtual void unbindShader();

        //IL=Input Layaut
		virtual BaseInputLayout* createIL(Shader*, const AttributeList& atl);
		virtual void deleteIL(BaseInputLayout*);
		virtual void bindIL(BaseInputLayout*);
		virtual void unbindIL(BaseInputLayout* il);

		//DEPTH
		virtual float getDepth(const Vec2& pixel);
		//RGBA
		virtual Vec4  getColor(const Vec2& pixel);

        //TEXTURE
        //texture functions
		virtual BaseTexture* sendTexture2D(size_t w, size_t h, void* byte, TextureType type);
		virtual BaseRenderTexture* createRenderTexture(size_t w, size_t h, size_t zbuffer, RenderTextureType type);

		void uniformVSTexture(uint n, BaseTexture*);
		void uniformVSTexture(uint n, BaseRenderTexture*);
		void uniformPSTexture(uint n, BaseTexture*);
		void uniformPSTexture(uint n, BaseRenderTexture*);

		virtual void deleteTexture(BaseTexture*);
		virtual void deleteTexture(BaseRenderTexture*);

		virtual void beginRenderToTexture(BaseRenderTexture*);
		virtual void endRenderToTexture(BaseRenderTexture*);
	
		//dx method
		void swap();
		void acquireContext();

	protected:
		/* CONTEXT */
		ViewportState viewportState;
		BlendState blendState;
		CullFaceState cullFaceState;
		ZBufferState zBufferState;
		ClearColorState clearColorState;
		GLuint		 vaoAttributes;
		bool         contextInit;
		ShaderDX*    currentShader{ nullptr };
		/// Query buffers
		ID3D10Texture2D* g_pQueryTextureColor1x1{ nullptr };
		ID3D10Texture2D* g_pQueryTextureDepth1x1{ nullptr };
		/// d3d device
		ID3D10Device* d3dDevice{ nullptr };
		/// d3d swap
		IDXGISwapChain* d3dSwap{ nullptr };
		/// zbuffer & target view
		ID3D10RenderTargetView*     g_pRenderTargetView{ nullptr };
		ID3D10Texture2D*            g_pDepthStencil{ nullptr };
		ID3D10DepthStencilView*     g_pDepthStencilView{ nullptr };
		// zbuffer state
		ID3D10DepthStencilState*    g_depthStencilState{ nullptr };         //enable
		ID3D10DepthStencilState*    g_depthDisabledStencilState{ nullptr }; //disable
		// render state
		ID3D10RasterizerState*    g_cullface_back{ nullptr };     //back
		ID3D10RasterizerState*    g_cullface_front{ nullptr };	  //front
		ID3D10RasterizerState*    g_cullface_disable{ nullptr };  //disable
		//current buffer
		ID3D10RenderTargetView*  currentRenderTargetView{ nullptr };
		ID3D10DepthStencilView* currentDepthStencilView{ nullptr };
		void setRenderTargets(ID3D10RenderTargetView* renderTargetView, 
							  ID3D10DepthStencilView* depthStencilView);
		// blend state
		ID3D10BlendState*		  g_pBlendState{ nullptr };
		void dxCreateBlend(bool blend,GLenum src,GLenum dst);
	};

};

#endif