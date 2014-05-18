#ifndef RENDERGL_H
#define RENDERGL_H

#include <Config.h>
#include <Math3D.h>
#include <Render.h>
#include <Blend.h>
#include <Color.h>

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
	class ShaderGL;
    
	class RenderGL : public Render {

		friend  class Application;
		friend  class Screen;
		virtual void __renderInit();
        
    public:
        
		RenderGL();
		~RenderGL();

		virtual RenderDriver getRenderDriver(){ return OPENGL_DRIVER; };
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
        
        //DEPTH
        virtual float getDepth(const Vec2& pixel);
        //RGBA
        virtual Vec4  getColor(const Vec2& pixel);
        
        /*
         Shaders
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
		virtual void enableRenderTexture(BaseRenderTexture*);
        
		virtual void disableTexture2D(BaseTexture*);
		virtual void disableRenderTexture(BaseRenderTexture*);
        
		virtual void deleteTexture2D(BaseTexture*);
		virtual void deleteRenderTexture(BaseRenderTexture*);
        
		virtual void enableRenderToTexture(BaseRenderTexture*);
		virtual void disableRenderToTexture(BaseRenderTexture*);

		//utility function
		GLuint getTBO(BaseTexture*);
		GLuint getFBO(BaseRenderTexture*);
		GLuint getTBO(BaseRenderTexture*);
		GLuint getRBO(BaseRenderTexture*);

	protected:
		/* CONTEXT */
		ViewportState viewportState;
		BlendState blendState;
		CullFaceState cullFaceState;
		ZBufferState zBufferState;
		ClearColorState clearColorState;
		GLuint		 vaoAttributes;
		bool         contextInit;
		ShaderGL*    currentShader=NULL;
	};

};

#endif