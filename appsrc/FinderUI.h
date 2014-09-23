//
//  FinderUI.h
//  Merging Meshs
//
//  Created by Gabriele Di Bari on 18/09/14.
//  Copyright (c) 2014 Gabriele Di Bari. All rights reserved.
//

#ifndef FINDERUI_H
#define FINDERUI_H

#include <Config.h>
#include <Application.h>
#include <Utility.h>
#include "gui.h"

namespace Easy3D
{
    
class FinderOpenOff
{
    
public:
    
    //public atributes
    bool visible{false};
    
    //contructor
    FinderOpenOff(const String& name);
    //draw
    void draw();
    void setName(const String& argname);
    void setCallBackLoad(DFUNCTION<void (const String& path)> fun);
    
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
    
    void listDirs(std::vector<String>& dirs);
    String getPathFromListPaths();
    void buildPathsList();
    void buildDirsList();
};

class FinderSaveOff
{
    
public:
    
    //public atributes
    bool visible{false};
    
    //contructor
    FinderSaveOff(const String& name);
    //draw
    void draw();
    void setName(const String& argname);
    void setCallBackSave(DFUNCTION<void (const String& path)> fun);
    
private:
    
    char text[255];
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
    DFUNCTION<void (const String& path)> cbSave{ nullptr };
    
    void listDirs(std::vector<String>& dirs);
    String getPathFromListPaths();
    void buildPathsList();
    void buildDirsList();
    
};

};

#endif
