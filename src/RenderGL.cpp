#include <stdafx.h>
#include <Render.h>
#include <RenderGL.h>
#include <Application.h>
#include <Screen.h>
#include <Shader.h>
#include <ShaderGL.h>
#include <Debug.h>

///////////////////////
namespace Easy3D{


RenderGL::RenderGL(){}
RenderGL::~RenderGL()
{
	if (contextInit)
		glDeleteVertexArrays(1, &vaoAttributes);
}

void RenderGL::__renderInit(){
	//OPENGL
	contextInit=true;
	//attributes vao
	glGenVertexArrays(1, &vaoAttributes);
	glBindVertexArray(vaoAttributes);
    //DirectX like topology
    //note : GL_FIRST_VERTEX_CONVENTION_EXT?
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
    //DirectX like z buffer
    glDepthRange(0.0f,1.0f);
    glDepthFunc( GL_LESS );
    //find errors:
	CHECK_GPU_ERRORS();
	//view port
	setViewportState(Vec4(0,
		                  0,
						  Application::instance()->getScreen()->getWidth(),
						  Application::instance()->getScreen()->getHeight())); 
	CHECK_GPU_ERRORS();
	//enable culling
	setCullFaceState(CullFaceState(CullFace::BACK)); CHECK_GPU_ERRORS();
	//enable z buffer
	setZBufferState(ZBufferState(true)); CHECK_GPU_ERRORS();
	//set clear color
	setClearColorState(Color(25, 128, 255, 255)); CHECK_GPU_ERRORS();
}

const ClearColorState& RenderGL::getClearColorState() const{
    return clearColorState;
}
void  RenderGL::setClearColorState(const ClearColorState& clearColor){
    glClearColor(clearColor.color.rNormalize(),
                 clearColor.color.gNormalize(),
                 clearColor.color.bNormalize(),
                 clearColor.color.aNormalize());
	clearColorState = clearColor;
}

void RenderGL::doClear(){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

const ZBufferState& RenderGL::getZBufferState() const{
    return zBufferState;
}
void  RenderGL::setZBufferState(const ZBufferState& zb){
    
    if(zb.zbuffer)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
	zBufferState = zb;
}

const CullFaceState& RenderGL::getCullFaceState() const{
    return cullFaceState;
}
void  RenderGL::setCullFaceState(const CullFaceState& cf){
	if(cf.cullface==CullFace::DISABLE)
        glDisable( GL_CULL_FACE );
    else{
        glEnable( GL_CULL_FACE );
        glCullFace(cf.cullface==CullFace::FRONT?GL_FRONT:GL_BACK);
    }
	cullFaceState = cf;
}

const ViewportState& RenderGL::getViewportState() const {
    //return
	return viewportState;
}
void  RenderGL::setViewportState(const ViewportState& vs){
    glViewport(vs.viewport.x,vs.viewport.y,vs.viewport.z,vs.viewport.w);
	viewportState = vs;
}

const BlendState& RenderGL::getBlendState() const{
	return blendState;
}
void  RenderGL::setBlendState(const BlendState& bs){
	//restore old blend state
	if (bs.enable) {
		glEnable(GL_BLEND);
		glBlendFunc(bs.src, bs.dst);
		//save blend
		blendState = bs;
	}
	else {
		glBlendFunc(GL_ONE, GL_ZERO);
		glDisable(GL_BLEND);
		//save blend
		blendState.src = GL_ONE;
		blendState.dst = GL_ZERO;
		blendState.enable = false;
	}
}


//projection matrix
//projection matrix
Mat4 RenderGL::calculatesOrto(float left, float right, float bottom, float top, float vnear, float vfar) const {
	Mat4 projection;
	projection.setOrtho(left, right, bottom, top, vnear, vfar);
	return projection;
}
Mat4 RenderGL::calculatesProjection(float fovy, float aspect, float vnear, float vfar) const {
	Mat4 projection;
	projection.setPerspective(fovy, aspect, vnear, vfar);
	return projection;
}
Mat4 RenderGL::calculatesProjection(float left, float right, float bottom, float top, float vnear, float vfar) const {
	Mat4 projection;
	projection.setPerspective(left, right, bottom, top, vnear, vfar);
	return projection;
}
/*
 VBO & IBO
 */
class BufferObject{
public:
    GLuint idBuffer;
    BufferObject(GLuint idBuffer=0):idBuffer(idBuffer){}
    inline operator GLuint() const{
        return idBuffer;
    }
    inline operator GLuint*(){
        return &idBuffer;
    }
    void genBuffer(){
        glGenBuffers(1, &idBuffer);
    }
};

class BaseVertexBufferObject : public BufferObject {
public:
    BaseVertexBufferObject(GLuint idBuffer=0):BufferObject(idBuffer){}
};
class BaseIndextBufferObject : public BufferObject  {
public:
    BaseIndextBufferObject(GLuint idBuffer=0):BufferObject(idBuffer){}
};

BaseVertexBufferObject* RenderGL::createVBO(const byte* vbo, size_t stride, size_t n){
    auto ptr=new BaseVertexBufferObject();
    ptr->genBuffer();
    glBindBuffer(GL_ARRAY_BUFFER, *ptr);
	glBufferData(GL_ARRAY_BUFFER, stride*n, vbo, GL_STATIC_DRAW);
    return ptr;
}
BaseIndextBufferObject* RenderGL::createIBO(const ushort* ibo, size_t size){
    auto ptr=new BaseIndextBufferObject();
    ptr->genBuffer();
    glBindBuffer(GL_ARRAY_BUFFER, *ptr);
    glBufferData(GL_ARRAY_BUFFER,  size, *ptr, GL_STATIC_DRAW);
    return ptr;
}
void RenderGL::bindVBO(BaseVertexBufferObject* vbo){
	DEBUG_ASSERT(currentShader);
	currentShader->uniform();
    glBindBuffer(GL_ARRAY_BUFFER,*vbo);
}
void RenderGL::bindIBO(BaseIndextBufferObject* ibo){
	DEBUG_ASSERT(currentShader);
	currentShader->uniform();
    glBindBuffer(GL_ARRAY_BUFFER,*ibo);
}
void RenderGL::deleteVBO(BaseVertexBufferObject* vbo){
    glDeleteBuffers(1, *vbo);
    delete vbo;
}
void RenderGL::deleteIBO(BaseIndextBufferObject* ibo){
    glDeleteBuffers(1, *ibo);
    delete ibo;
}
/*
 Draw
 */
inline static GLuint getOpenGLDrawType(TypeDraw type){
    switch (type){
        case DRAW_POINTS: return GL_POINTS; break;
        case DRAW_LINES: return GL_LINES;  break;
        case DRAW_LINE_LOOP: return GL_LINE_LOOP;  break;
        case DRAW_TRIANGLES: return GL_TRIANGLES; break;
        case DRAW_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;  break;
        default: return 0; break;
    }
}
void RenderGL::drawArrays(TypeDraw type, uint n){
    glDrawArrays(getOpenGLDrawType(type), 0, n);
}
void RenderGL::drawElements(TypeDraw type, uint n){
    glDrawElements(getOpenGLDrawType(type), n, GL_UNSIGNED_SHORT, (void*)NULL);
}
/*
 InputLayout
 */
class BaseInputLayout{
public:
    
    struct AInput{
        GLint attribute;
        size_t size;
        size_t strip;
        size_t offset;
    };
    
    GLuint idshader;
    std::vector<AInput> layouts;
	typedef std::vector<AInput>::iterator itLayouts;
    void addAttribute(const String& name,
                 size_t size,
                 size_t strip,
                 size_t offset){
        
        layouts.push_back({
            glGetAttribLocation(idshader, name),
            size,
            strip,
            offset
        });
        
    }
};
  
    
    
Shader* RenderGL::createShader(){
    return (Shader*)new ShaderGL();
}
void RenderGL::deleteShader(Shader* s){
        delete ((ShaderGL*)s);
}
void RenderGL::bindShader(Shader* shader){
	currentShader = (ShaderGL*)shader;
	currentShader->bind();
}
void RenderGL::unbindShader(){
	currentShader->unbind();
	currentShader = NULL;
}

BaseInputLayout* RenderGL::createIL(Shader* shader,const AttributeList& atl){
    auto il=new BaseInputLayout();
    //shade shader id
    il->idshader=((ShaderGL*)shader)->programID();
    //offset
    size_t offset=0;
    //add params
    for(auto at:atl){
        il->addAttribute(at.name,at.components(),atl.size(),offset);
        offset+=at.size();
    }
    //return
    return  il;
}
/*
void RenderGL::addAttribute(BaseInputLayout *il,
                            const String& name,
                            size_t size,
                            size_t strip,
                            size_t offset){
    il->layouts.push_back({
        glGetAttribLocation(il->idshader, name),
        size,
        strip,
        offset
    });
    
}*/
void RenderGL::bindIL(BaseInputLayout* il){
    #ifndef GL_BUFFER_OFFSET
        #define GL_BUFFER_OFFSET(x) ((char *)NULL + (x))
    #endif
	for (size_t i = 0; i != il->layouts.size();++i){
		glEnableVertexAttribArray(il->layouts[i].attribute);
		glVertexAttribPointer(il->layouts[i].attribute,
							 (int)il->layouts[i].size,                   //nvalues 1-2-3-4
                             GL_FLOAT,									 //type value
                             GL_FALSE,									 //normalize -> no
							 (int)il->layouts[i].strip,                  //size of structure
							 GL_BUFFER_OFFSET(il->layouts[i].offset));   //offset param
    }
}
void RenderGL::unbindIL(BaseInputLayout* il){
	for (size_t i = 0; i != il->layouts.size(); ++i){
		glDisableVertexAttribArray(il->layouts[i].attribute);
    }
}
void RenderGL::deleteIL(BaseInputLayout* il){
    delete  il;
}
/*
 Textures
 */
BaseTexture* RenderGL::sendTexture2D(size_t w, size_t h, void* byte, TextureType type){ return NULL; }
BaseRenderTexture* RenderGL::createRenderTexture(size_t w,size_t h,size_t zbuffer,RenderTextureType type){ return NULL; }

void RenderGL::enableTexture2D(BaseTexture*){}
void RenderGL::enableRenderTexture(BaseTexture*){}

void RenderGL::disableTexture2D(BaseTexture*){}
void RenderGL::disableRenderTexture(BaseTexture*){}

void RenderGL::deleteTexture2D(BaseTexture*){}
void RenderGL::deleteRenderTexture(BaseRenderTexture*){}

void RenderGL::enableRenderToTexture(BaseRenderTexture*){}
void RenderGL::disableRanderToTexture(BaseRenderTexture*){}
};