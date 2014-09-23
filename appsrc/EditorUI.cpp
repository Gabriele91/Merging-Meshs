//
//  EditorUI.cpp
//  Merging Meshs
//
//  Created by Gabriele Di Bari on 18/09/14.
//  Copyright (c) 2014 Gabriele Di Bari. All rights reserved.
//
#include <stdafx.h>
#include <Screen.h>
#include "EditorUI.h"
using namespace Easy3D;

////////////////////////////////////////////////////////////////
//input hendler
WrongMsg::WrongMsg()
{
    Application::instance()->getInput()->addHandler(this);
}
WrongMsg::~WrongMsg()
{
    Application::instance()->getInput()->removeHandler(this);
}
void WrongMsg::draw()
{
    if(visible)
    {
        auto ssize=Application::instance()->getScreen()->getSize();
        ImGui::Begin("Wrong",&visible,ImVec2(400,100));
        ImGui::SetWindowPos(ssize*0.5-Vec2(200,50));
        ImGui::TextUnformatted(wrong);
        ImGui::End();
    }
}
void WrongMsg::onMousePress(Vec2 pos, Key::Mouse key)
{
    visible=false;
}
////////////////////////////////////////////////////////////////
EditorUI::EditorUI(float sizeLeft)
:openOffs("Off Files")
,saveOffs("Save Files")
,sizeLeft(sizeLeft)
{
    ImGui::Init();
    //costum style;
    ImGui::GetStyle().Colors[ImGuiCol_Border]           = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    ImGui::GetStyle().Colors[ImGuiCol_BorderShadow]     = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.10f, 0.10f, 0.30f, 0.15f);
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.10f, 0.10f, 0.50f, 1.00f);
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.10f, 0.10f, 0.30f, 1.00f);
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.10f, 0.10f, 0.20f, 1.00f);
    
    ImGui::GetStyle().Colors[ImGuiCol_TitleBg]          = ImVec4(0.10f, 0.10f, 0.50f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    
    ImGui::GetStyle().Colors[ImGuiCol_Header]           = ImVec4(0.10f, 0.10f, 0.50f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.10f, 0.10f, 0.20f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]     = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    
    ImGui::GetStyle().Colors[ImGuiCol_Button]           = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]     = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
}
EditorUI::~EditorUI()
{
    //ui
    ImGui::Free();
}

void EditorUI::setSizeLeft(float size)
{
    sizeLeft=size;
}
void EditorUI::setCallBackLoad(DFUNCTION<void(const String& path)> cbfun)
{
    openOffs.setCallBackLoad(cbfun);
}
void EditorUI::setCallBackSave(DFUNCTION<void(const String& path)> cbfun)
{
    saveOffs.setCallBackSave(cbfun);
}
void EditorUI::setCallBackSVD(DFUNCTION<void(void)> cbfun)
{
    doSVD=cbfun;
}
void EditorUI::showWrongMessage(const String& wrong)
{
    wrongMsg.wrong=wrong;
    wrongMsg.visible=true;
}
void EditorUI::draw()
{
    // Start the frame
    ImGui::StartFrame("SDV Editor",nullptr,
                      Vec2(sizeLeft,getScreen().getSize().y),
                      0.5,
                      ImGuiWindowFlags_NoTitleBar|
                      ImGuiWindowFlags_NoResize|
                      ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos(Vec2(0,0));
    /////////////////
    openOffs.visible^=ImGui::Button("load model");
    openOffs.draw();
    /////////////////
    saveOffs.visible^=ImGui::Button("save model");
    saveOffs.draw();
    /////////////////
    wrongMsg.draw();
    /////////////////
    if(ImGui::Button("execute svd")&&doSVD) doSVD();
    /////////////////
    if(ImGui::CollapsingHeader("Left Model"))
    {
        static float angle[3];
        static float color[4];
        ImGui::SliderFloat3("Rotation",angle,0,360);
        ImGui::ColorEdit4("Color", color);
    }
    if(ImGui::CollapsingHeader("Right Model"))
    {
        static float angle;
        ImGui::SliderAngle("pich", &angle);
        ImGui::SliderAngle("yaw", &angle);
        ImGui::SliderAngle("roll", &angle);
    }
    /////////////////
    ImGui::EndFrame();
}
////////////////////////////////////////////////////////////////

