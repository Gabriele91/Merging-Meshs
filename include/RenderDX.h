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
        
        //TEXTURE
        //texture functions
		virtual BaseTexture* sendTexture2D(size_t w, size_t h, void* byte, TextureType type);
		virtual BaseRenderTexture* createRenderTexture(size_t w, size_t h, size_t zbuffer, RenderTextureType type);
        
		virtual void enableTexture2D(BaseTexture*);
		virtual void enableRenderTexture(BaseTexture*);
        
		virtual void disableTexture2D(BaseTexture*);
		virtual void disableRenderTexture(BaseTexture*);
        
		virtual void deleteTexture2D(BaseTexture*);
		virtual void deleteRenderTexture(BaseRenderTexture*);
        
		virtual void enableRenderToTexture(BaseRenderTexture*);
		virtual void disableRanderToTexture(BaseRenderTexture*);
	
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
		ShaderDX*    currentShader = NULL;
		/// d3d device
		ID3D10Device* d3dDevice = NULL;
		/// d3d swap
		IDXGISwapChain* d3dSwap = NULL;
		/// zbuffer & target view
		ID3D10RenderTargetView*     g_pRenderTargetView = NULL;
		ID3D10Texture2D*            g_pDepthStencil = NULL;
		ID3D10DepthStencilView*     g_pDepthStencilView = NULL;
		// zbuffer state
		ID3D10DepthStencilState*    g_depthStencilState = NULL;         //enable
		ID3D10DepthStencilState*    g_depthDisabledStencilState = NULL; //disable
		// render state
		ID3D10RasterizerState*    g_cullface_back = NULL;     //back
		ID3D10RasterizerState*    g_cullface_front = NULL;	//front
		ID3D10RasterizerState*    g_cullface_disable = NULL;  //disable
		// blend state
		ID3D10BlendState*		  g_pBlendState = NULL;
		void dxCreateBlend(bool blend,GLenum src,GLenum dst);
	};

};

#endif