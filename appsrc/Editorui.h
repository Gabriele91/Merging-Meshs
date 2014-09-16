#ifndef EDITORUI_H
#define EDITORUI_H

#include <Config.h>
#include "gui.h"

namespace Easy3D
{

class FinderOff
{
    
    public:
    
    //public atributes
    bool visible{false};
    
    //contructor
    FinderOff(const String& name)
    :currentPath(Application::instance()->appResourcesDirectory())
    {
        setName(name);
        buildDirsList();
        buildPathsList();
    }
    
    //draw
    void draw()
    {
        if(visible)
        {
            ImGui::Begin(windowName,&visible,ImVec2(400,400));
            ImGui::Text("Files:");
            ImGui::BeginChild(windowChildName,ImVec2(0,200),true);
            for(const auto& dir:subDirs)
            {
                if(ImGui::Button( dir ))
                {
                    Utility::Path newpath(currentPath.getDirectory()+"/"+dir);
                    currentPath=Utility::Path(newpath.getAbsolute());
                    buildDirsList();
                    buildPathsList();
                    break;
                };
            }
            for(const auto& fileInfo:offsFiles)
            {
                if(ImGui::Button( fileInfo.file ) && cbLoad)
                {
                    cbLoad(fileInfo.path);
                }
            }
            ImGui::EndChild();
            if(ImGui::Combo("URL", &citem, superPathsList.c_str()))
            {
                currentPath= getPathFromListPaths();
                buildDirsList();
                buildPathsList();
            }
            ImGui::End();
        }
    }
    
    void setName(const String& argname)
    {
        name=argname;
        windowName="Open, "+argname;
        windowChildName="ListPath, "+argname;
    }
    
    void setCallBackLoad(DFUNCTION<void (const String& path)> fun)
    {
        cbLoad=fun;
    }
    
    private:
    
    String name;
    String windowName;
    String windowChildName;
    Utility::Path currentPath;
    int citem{ 0 };
    //temps
    std::vector<String> subDirs;
    struct FileInfo{String path; String file;};
    std::vector<FileInfo> offsFiles;
    String superPathsList;
    //cb load
    DFUNCTION<void (const String& path)> cbLoad{ nullptr };
    
    inline void listDirs(std::vector<String>& dirs)
    {
        String path=currentPath.getDirectory();
        path.split("/", dirs);
    }
    
    String getPathFromListPaths()
    {
        String newpath;
        int item=0;
        int i=0;
        while( item <= citem )
        {
            if(superPathsList[i]=='\0')
            {
                if(item)
                    newpath+='/';
                ++item;
            }
            else
            {
                newpath+=superPathsList[i];
            }
            ++i;
        }
        return newpath;
    }
    
    void buildPathsList()
    {
        //build list
        superPathsList=String('\0',superPathsList.size());
        superPathsList=currentPath.getDirectory();
        //start
        citem=0;
        //path split
        for(auto& c:superPathsList)
            if(c=='/'){ c='\0'; ++citem; };
        //if unix add root
#ifdef PLATFORM_UNIX
        superPathsList="/"+superPathsList;
#endif
        superPathsList+="\0\0";
    }
    
    void buildDirsList()
    {
        //update dirs
        subDirs.clear();
        subDirs=currentPath.getSubDirs();
        //update off diles
        offsFiles.clear();
        auto files=currentPath.getFiles();
        for(const auto& file:files)
        {
            Utility::Path pathFile(file);
            if(pathFile.getExtension().toLower()=="off")
            {
                offsFiles.push_back({ currentPath.getPath()+pathFile.getFilename(), file });
            }
        }
    }
    
};

class EditorUI
{
    
    FinderOff openOffs;
    float  sizeLeft;
    DFUNCTION<void(void)> doSVD;
    
    public:

    EditorUI(float sizeLeft=0)
    :openOffs("Off Files")
    ,sizeLeft(sizeLeft)
    {
        ImGui::Init();
    }
    ~EditorUI()
    {
        ImGui::Free();
    }
    
    void setSizeLeft(float size)
    {
        sizeLeft=size;
    }
    
    void setCallBackLoad(DFUNCTION<void(const String& path)> cbfun)
    {
        openOffs.setCallBackLoad(cbfun);
    }
    void setCallBackSVD(DFUNCTION<void(void)> cbfun)
    {
        doSVD=cbfun;
    }
    
    void draw()
    {
        // Start the frame
        ImGui::StartFrame("SDV Editor",nullptr,
                          Vec2(sizeLeft,getScreen().getSize().y),
                          1,
                          ImGuiWindowFlags_NoTitleBar|
                          ImGuiWindowFlags_NoResize|
                          ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos(Vec2(0,0));
        /////////////////
        openOffs.visible^=ImGui::Button("load model");
        openOffs.draw();
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
    
    
    protected:
    
    
    Screen& getScreen(){
        return *Application::instance()->getScreen();
    }
    Input& getInput(){
        return *Application::instance()->getInput();
    }
    Audio& getAudio(){
        return *Application::instance()->getAudio();
    }
    Render& getRender(){
        return *Application::instance()->getRender();
    }
};


};

#endif