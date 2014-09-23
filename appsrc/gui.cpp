#include <stdafx.h>
#include <Application.h>
#include <Render.h>
#include <Screen.h>
#include <Input.h>
#include <Shader.h>
#include "Mesh.h"
#include "gui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ui/stb_image.h"
using namespace Easy3D;
#define SIZE_BUFFER 10000

struct gVector
{
    Vec2 pos;
    Vec2 uv;
    Vec4 color;
};


class ImGuiListener : public Input::KeyboardHandler
{
public:
    
    void  onKeyPress(Key::Keyboard key)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[key] = true;
        //ctrl
        if ( key == Key::LCTRL || key == Key::RCTRL )
            io.KeyCtrl=true;
        //shitf
        if( key == Key::LSHIFT || key == Key::RSHIFT )
            io.KeyShift=true;
    }
    void  onKeyRelease(Key::Keyboard key)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[key] = false;
        //ctrl
        if ( key == Key::LCTRL || key == Key::RCTRL )
            io.KeyCtrl=false;
        //shitf
        if( key == Key::LSHIFT || key == Key::RSHIFT )
            io.KeyShift=false;
    }
    void onStringInput(const String& str)
    {
        for (int i=0;str[i];++i)
            ImGui::GetIO().AddInputCharacter(str[i]);
    }
};

struct ImGuiContext
{
    ImGuiListener*   listener{nullptr};
    BaseTexture*     texture{nullptr};
    Shader*          shader{nullptr};
	BaseInputLayout* il{ nullptr };
	CVec4::ptr uscissor{ nullptr };
	CVec4::ptr uviewport{ nullptr };
    CMat4::ptr uproj{ nullptr };
    CTexture::ptr utexture{ nullptr };
    BaseVertexBufferObject* vb{ nullptr };
    std::vector<gVector> cpuvb;
}context;



static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
    if (cmd_lists_count == 0)
        return;
    
    //get render
	Render& r = *Application::instance()->getRender();
    
	//save context
    auto ctxCZBuffer = r.getZBufferState();
	auto ctxCFaces = r.getCullFaceState();
	auto ctxBlend = r.getBlendState();
    auto ctxViewport = r.getViewportState();
    
    //ptr
    size_t nvertex=0;
    //create buffer
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        const ImDrawVert* vtx_src = &cmd_list->vtx_buffer[0];
        for (size_t i = 0; i < cmd_list->vtx_buffer.size(); i++)
        {
            context.cpuvb[nvertex].pos = vtx_src->pos;
            context.cpuvb[nvertex].uv  = vtx_src->uv;
            
            context.cpuvb[nvertex].color.x = (float)((vtx_src->col >> 0) & 0xFF) / 255.0f;
            context.cpuvb[nvertex].color.y = (float)((vtx_src->col >> 8) & 0xFF) / 255.0f;
            context.cpuvb[nvertex].color.z = (float)((vtx_src->col >> 16) & 0xFF) / 255.0f;
            context.cpuvb[nvertex].color.w = (float)((vtx_src->col >> 24) & 0xFF) / 255.0f;
            ++nvertex;
            ++vtx_src;
        }
        DEBUG_ASSERT(SIZE_BUFFER>nvertex);
    }
    //send to gpu
    r.updateSteamVBO(context.vb,(const Easy3D::byte*)&(context.cpuvb[0].pos.x), nvertex*sizeof(gVector));

	//bind
	r.bindShader(context.shader);
	r.setZBufferState(false);
	r.setCullFaceState(CullFace::DISABLE);
	r.setBlendState({ BLEND::SRC::ALPHA, BLEND::ONE::MINUS::SRC::ALPHA });
    
    //projection
    const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	Mat4 proj = r.calculatesOrto(0, width, height, 0, -1.0, +1.0f);
    r.setViewportState(Vec4(0,0,width,height));
    
    
    //uniform
    context.uproj->setValue(proj);
    context.utexture->enable(context.texture);
	context.uviewport->setValue(Vec4(0, 0, width, height));
    
    //set buffers
    r.bindVBO(context.vb);
    r.bindIL(context.il);
    
    //draw
    int vtxOffset = 0;
    for (int n = 0; n < cmd_lists_count; n++)
    {
        // Render command list
        const ImDrawList* cmd_list = cmd_lists[n];
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
        {
            //uniform (fake) scissor
			context.uscissor->setValue(pcmd->clip_rect);
            //
            r.drawArrays(TypeDraw::DRAW_TRIANGLES, vtxOffset, pcmd->vtx_count);
            //
            vtxOffset += pcmd->vtx_count;
        }
    }
 	//unbind
	r.unbindShader();
    //reset context
	r.setZBufferState(ctxCZBuffer);
	r.setBlendState(ctxBlend);
	r.setCullFaceState(ctxCFaces);
    r.setViewportState(ctxViewport);
    
    
}

void ImGui::StartFrame(const char* name, bool* open, ImVec2 size, float fill_alpha, ImGuiWindowFlags flags)
{
    //immage gui
    ImGuiIO& io = ImGui::GetIO();
    //update dt
    io.DeltaTime=Application::instance()->getLastDeltaTime();
    //input
    Easy3D::Input& input=*Application::instance()->getInput();
    io.MousePos = input.getMouse();
    io.MouseDown[0] = input.getMouseDown(Key::BUTTON_LEFT);
    io.MouseDown[1] = input.getMouseDown(Key::BUTTON_RIGHT);
    //new frame
    ImGui::NewFrame(name,open,size,fill_alpha,flags);
    
}
void ImGui::EndFrame()
{
    ImGui::Render();
}

void ImGui::Init()
{
    //
    ImGuiIO& io = ImGui::GetIO();
    Easy3D::Screen& screen=*Application::instance()->getScreen();
    Easy3D::Input& input=*Application::instance()->getInput();
    Easy3D::Render& render=*Application::instance()->getRender();
    //init
    io.DisplaySize = screen.getSize();
	io.PixelCenterOffset = 0.5f;//0.0f;
    io.DeltaTime = 1.0f/60.0f;
    // Keyboard mapping. ImGui will use those indices to peek
    io.KeyMap[ImGuiKey_Tab] = Key::TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = Key::LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = Key::RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = Key::UP;
    io.KeyMap[ImGuiKey_DownArrow] = Key::DOWN;
    io.KeyMap[ImGuiKey_Home] = Key::HOME;
    io.KeyMap[ImGuiKey_End] = Key::END;
    io.KeyMap[ImGuiKey_Delete] = Key::KDELETE;
    io.KeyMap[ImGuiKey_Backspace] = Key::BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = Key::RETURN;
    io.KeyMap[ImGuiKey_Escape] = Key::ESCAPE;
    io.KeyMap[ImGuiKey_A] = Key::A;
    io.KeyMap[ImGuiKey_C] = Key::C;
    io.KeyMap[ImGuiKey_V] = Key::V;
    io.KeyMap[ImGuiKey_X] = Key::X;
    io.KeyMap[ImGuiKey_Y] = Key::Y;
    io.KeyMap[ImGuiKey_Z] = Key::Z;
    //draw call
    io.RenderDrawListsFn = ImImpl_RenderDrawLists;
    //////////////////////////////////////////////////////////////////
    //add hendler
    context.listener=new ImGuiListener();
    input.addHandler(context.listener);
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    //get font texture
    const void* pngBuffer;
    unsigned int pngSize;
    ImGui::GetDefaultFontData(NULL, NULL, &pngBuffer, &pngSize);
    //uncompress png
    int textureW, textureH, textureComp;
    void* texBuffer = stbi_load_from_memory((const unsigned char*)pngBuffer, (int)pngSize, &textureW, &textureH, &textureComp, 0);
    //send to gpu
    context.texture=
    render.sendTexture2D(textureW, textureH, (void*)texBuffer, TextureType::TX_RGBA8);
    //free data
    stbi_image_free(texBuffer);
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    //shaders
	String  rpath = Application::instance()->appResourcesDirectory();
    //init shader
	context.shader = render.createShader();
	if (render.getRenderDriver() == OPENGL_DRIVER){
		context.shader->loadShader(rpath + "/shader/gui/gui.vs.glsl",
                                   rpath + "/shader/gui/gui.fs.glsl");
		context.uproj = context.shader->getConstMat4("projection")->shared();
		context.utexture = context.shader->getConstTexture("textureFont")->shared();
		context.uscissor = context.shader->getConstVec4("scissor")->shared();
		context.uviewport = context.shader->getConstVec4("viewport")->shared();
    }
	else if (render.getRenderDriver() == DIRECTX_DRIVER){
		context.shader->loadShader(rpath + "/shader/gui/gui.vs.hlsl",
								   rpath + "/shader/gui/gui.fs.hlsl");
		context.uproj = context.shader->getConstMat4("vs.projection")->shared();
		context.utexture = context.shader->getConstTexture("ps.textureFont:sampleFont")->shared();
		context.uscissor = context.shader->getConstVec4("ps.scissor")->shared();
		context.uviewport = context.shader->getConstVec4("ps.viewport")->shared();
    }
	AttributeList aList = {
		{ "inPosition", ATT_FLOAT2 },
		{ "inCoords", ATT_FLOAT2 },
		{ "inColor", ATT_FLOAT4 }
	};
	context.il = render.createIL(context.shader, aList);
    //////////////////////////////////////////////////////////////////
    //create streaming buffer
    context.vb=render.createStreamVBO(nullptr,  sizeof(gVector),SIZE_BUFFER);
    context.cpuvb.resize(SIZE_BUFFER);
    
}

void ImGui::Free()
{
    Easy3D::Input& input=*Application::instance()->getInput();
    Easy3D::Render& render=*Application::instance()->getRender();
    //dt hendler
    input.removeHandler(context.listener);
    //dealloc
    delete context.listener;
    render.deleteShader(context.shader);
    render.deleteIL(context.il);
    render.deleteVBO(context.vb);
}