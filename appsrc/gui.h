#ifndef GUI_H
#define GUI_H
#include <Config.h>
#include <Math3D.h>

#define IM_VEC2_CLASS_EXTRA                                     \
    ImVec2(const Easy3D::Vec2& f) { x = f.x; y = f.y; }         \
    operator Easy3D::Vec2() const { return Easy3D::Vec2(x,y); }
 
#define IM_VEC4_CLASS_EXTRA                                               \
    ImVec4(const Easy3D::Vec4& f) { x = f.x; y = f.y; z = f.z; w = f.w; } \
    operator Easy3D::Vec4() const { return Easy3D::Vec4(x,y,z,w); }

#include "ui/imgui.h"

namespace ImGui
{
	void Init();
    void Free();
	void StartFrame(const char* name = "Debug", bool* open = NULL, ImVec2 size = ImVec2(0,0), float fill_alpha = -1.0f, ImGuiWindowFlags flags = 0);
	void EndFrame();
};

#endif