#ifndef RENDER_H
#define RENDER_H

#include <Config.h>
#include <Math3D.h>
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

	enum RenderDriver{
		OPENGL_DRIVER,
		DIRECTX_DRIVER
	};

	enum CullFace{
		DISABLE = 0,
		FRONT,
		BACK
	};

	enum TypeDraw{
        DRAW_POINTS,
		DRAW_LINES,
        DRAW_LINE_LOOP,
		DRAW_TRIANGLES,
		DRAW_TRIANGLE_STRIP
	};

	enum TextureType{
		TX_RGBA,
		TX_RGB,
		TX_RG,
		TX_RED
	};
	enum RenderTextureType{
		RD_RGBA,
		RD_RGB,
		RD_SHADOW
	};

	enum AttributeType{
		ATT_FLOAT,
		ATT_FLOAT2,
		ATT_FLOAT3,
		ATT_FLOAT4,

		ATT_INT,
		ATT_INT2,
		ATT_INT3,
		ATT_INT4,

		ATT_UINT,
		ATT_UINT2,
		ATT_UINT3,
		ATT_UINT4,

		ATT_TLESS,
		ATT_TLESS2,
		ATT_TLESS3,
		ATT_TLESS4,
	};

	struct Attribute{
		String name;
		AttributeType type;
		size_t components() const {
			int itype = (int)type;
			return (size_t)((itype % 4)+1);
		}
		size_t size() const {
			return components() * 4;
		}
	};

	class AttributeList{

		typedef std::vector<Attribute> AList;
		AList list;
		size_t gSize;

	public:
		
		AttributeList(const std::initializer_list<Attribute>& atrs):gSize(0){
			for (const auto& at : atrs){
				push(at);
			}
		}
		void push(const Attribute& atr){
			list.push_back(atr);
			gSize += atr.size();
		}
		size_t size() const {
			return gSize;
		}
		size_t countAttribute() const{
			return list.size();
		}
		AList::iterator begin() { return list.begin(); }
		AList::iterator end() { return list.end(); }
		AList::const_iterator begin() const{ return list.begin(); }
		AList::const_iterator end() const{ return list.end(); }
	};

	struct ViewportState{

		Vec4 viewport;
		//costructors
		ViewportState(){}
		ViewportState(const Vec4& vp) : viewport(vp){}
		//operators
		bool operator==(const ViewportState& vs) const {
			return viewport == vs.viewport;
		}
		bool operator!=(const ViewportState& vs) const {
			return viewport != vs.viewport;
		}

	};

	struct BlendState{
		int src, dst, enable;
		//constructor
		BlendState() : enable(false){}
		BlendState(int src, int dst) :src(src), dst(dst), enable(true){}
		//operators
		bool operator==(const BlendState& bs) const {
			return src == bs.src && dst == bs.dst && enable == bs.enable;
		}
		bool operator!=(const BlendState& bs) const {
			return src != bs.src || dst != bs.dst || enable != bs.enable;
		}
	};

	struct CullFaceState{
		//face cull
		CullFace cullface;
		//cullface
		CullFaceState(CullFace culllFace = CullFace::DISABLE) :cullface(culllFace){}
		//operators
		bool operator==(const CullFaceState& cl)const{
			return cullface == cl.cullface;
		}
		bool operator!=(const CullFaceState& cl)const{
			return cullface != cl.cullface;
		}
		//cast operator
		operator int() const {
			return (int)cullface;
		}
		operator CullFace () const {
			return cullface;
		}

	};

	struct ZBufferState{
		//value
		bool zbuffer;
		//zbuffer
		ZBufferState(bool zBuffer = true) :zbuffer(zBuffer){}
		//operators
		bool operator==(const ZBufferState& zb)const{
			return zbuffer == zb.zbuffer;
		}
		bool operator!=(const ZBufferState& zb)const{
			return zbuffer != zb.zbuffer;
		}
		//cast operator
		operator bool() const{
			return zbuffer;
		}

	};

	struct ClearColorState{
		//color
		Color color;
		//color
		ClearColorState(const Color& color = Color()) :color(color){}
		ClearColorState(uchar *color) :color(Color(color)){}
		ClearColorState(uchar r, uchar g, uchar b, uchar a) :color(Color(r, g, b, a)){}
		//operators
		bool operator==(const ClearColorState& c)const{
			return color == c.color;
		}
		bool operator!=(const ClearColorState& c)const{
			return color != c.color;
		}
		//cast operator
		operator Color& (){
			return color;
		}
		operator const Color& () const{
			return color;
		}

	};
    
	class Render {

		friend class Application;
		friend class Screen;
		virtual void __renderInit()=0;
        
    public:
        
        Render(){};
		virtual ~Render(){};

		virtual RenderDriver getRenderDriver() = 0;
        virtual const ClearColorState& getClearColorState() const=0;
		virtual void setClearColorState(const ClearColorState& cf)=0;
		virtual void doClear()=0;
        
		virtual const ZBufferState& getZBufferState() const=0;
		virtual void setZBufferState(const ZBufferState& cf)=0;
        
		virtual const CullFaceState& getCullFaceState() const=0;
		virtual void setCullFaceState(const CullFaceState& cf)=0;
        
		virtual const ViewportState& getViewportState() const=0;
		virtual void setViewportState(const ViewportState& vs)=0;
        
		virtual const BlendState& getBlendState() const=0;
		virtual void setBlendState(const BlendState& bs)=0;

		//projection matrix

		virtual Mat4 calculatesOrto(float left, float right, float bottom, float top, float vNear, float vFar) const = 0;
		virtual Mat4 calculatesProjection(float fovy, float aspect, float vNear, float vFar) const = 0;
		virtual Mat4 calculatesProjection(float left, float right, float bottom, float top, float vNear, float vFar) const = 0;

        //BO
		//virtual BaseVertexBufferObject* createVBO(const byte* vbo, size_t size, size_t strip, size_t offset)=0;
		virtual BaseVertexBufferObject* createVBO(const byte* vbo, size_t stride, size_t n) = 0;
		virtual BaseIndextBufferObject* createIBO(const uint* ibo, size_t size) = 0;
        
        
		virtual void bindVBO(BaseVertexBufferObject*)=0;
		virtual void bindIBO(BaseIndextBufferObject*)=0;
        
		virtual void deleteVBO(BaseVertexBufferObject*)=0;
		virtual void deleteIBO(BaseIndextBufferObject*)=0;
        
        //draw
		virtual void drawArrays(TypeDraw type, uint n)=0;
		virtual void drawElements(TypeDraw type, uint n)=0;
        
        /*
         Shaders
         */
        virtual Shader* createShader()=0;
        virtual void deleteShader(Shader*)=0;
        virtual void bindShader(Shader* shader) = 0;
        virtual void unbindShader() = 0;
        //IL=Input Layaut
		virtual BaseInputLayout* createIL(Shader*, const AttributeList& atl) = 0;
        virtual void deleteIL(BaseInputLayout*)=0;
		virtual void bindIL(BaseInputLayout*)=0;
		virtual void unbindIL(BaseInputLayout* il)=0;
        
        //DEPTH
        virtual float getDepth(const Vec2& pixel)=0;
        //RGBA
        virtual Vec4  getColor(const Vec2& pixel)=0;
        
        //TEXTURE
        
        //texture functions
		virtual BaseTexture* sendTexture2D(size_t w, size_t h, void* byte, TextureType type)=0;
        virtual BaseRenderTexture* createRenderTexture(size_t w,size_t h,size_t zbuffer,RenderTextureType type)=0;
        
		virtual void enableTexture2D(BaseTexture*)=0;
		virtual void enableRenderTexture(BaseRenderTexture*) = 0;
        
		virtual void disableTexture2D(BaseTexture*)=0;
		virtual void disableRenderTexture(BaseRenderTexture*) = 0;
        
		virtual void deleteTexture2D(BaseTexture*)=0;
		virtual void deleteRenderTexture(BaseRenderTexture*)=0;
        
		virtual void enableRenderToTexture(BaseRenderTexture*) = 0;
		virtual void disableRenderToTexture(BaseRenderTexture*) = 0;

	};

};

#endif