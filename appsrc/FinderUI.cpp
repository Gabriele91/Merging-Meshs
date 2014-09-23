//
//  FinderUI.cpp
//  Merging Meshs
//
//  Created by Gabriele Di Bari on 18/09/14.
//  Copyright (c) 2014 Gabriele Di Bari. All rights reserved.
//
#include <stdafx.h>

#if defined(PLATFORM_UNIX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif defined(PLATFORM_WINDOW)
#include <windows.h>
#include <Userenv.h>
#include <atlstr.h>
#pragma comment(lib, "userenv.lib")
#endif

#include "FinderUI.h"
#include <Debug.h>
using namespace Easy3D;




FinderOpenOff::FinderOpenOff(const String& name)
:currentPath(Application::instance()->appResourcesDirectory())
{
#if defined(PLATFORM_UNIX)
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    currentPath=String(homedir);
#elif defined(PLATFORM_WINDOW)
    TCHAR szHomeDirBuf[MAX_PATH] = { 0 };
    
    HANDLE hToken = 0;
	DEBUG_ASSERT_REPLACE( OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) );
    
    DWORD BufSize = MAX_PATH;
	DEBUG_ASSERT_REPLACE(GetUserProfileDirectory(hToken, szHomeDirBuf, &BufSize));
    
    CloseHandle( hToken );
    //to c++ string
    CString cstring( szHomeDirBuf );
    CT2CA pszConvertedAnsiString(cstring);
    currentPath=String(pszConvertedAnsiString);
#endif
    setName(name);
    buildDirsList();
    buildPathsList();
}

//draw
void FinderOpenOff::draw()
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

void FinderOpenOff::setName(const String& argname)
{
    name=argname;
    windowName="Open, "+argname;
    windowChildName="ListPath, "+argname;
}

void FinderOpenOff::setCallBackLoad(DFUNCTION<void (const String& path)> fun)
{
    cbLoad=fun;
}

void FinderOpenOff::listDirs(std::vector<String>& dirs)
{
    String path=currentPath.getDirectory();
    path.split("/", dirs);
}

String FinderOpenOff::getPathFromListPaths()
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

void FinderOpenOff::buildPathsList()
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

void FinderOpenOff::buildDirsList()
{
    //update dirs
    subDirs.clear();
    subDirs.push_back("..");
    currentPath.getSubDirs(subDirs);
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


//contructor
FinderSaveOff::FinderSaveOff(const String& name)
:currentPath(Application::instance()->appResourcesDirectory())
{
#if defined(PLATFORM_UNIX)
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	currentPath=String(homedir);
#elif defined(PLATFORM_WINDOW)
	TCHAR szHomeDirBuf[MAX_PATH] = { 0 };

	HANDLE hToken = 0;
	DEBUG_ASSERT_REPLACE(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken));

	DWORD BufSize = MAX_PATH;
	DEBUG_ASSERT_REPLACE(GetUserProfileDirectory(hToken, szHomeDirBuf, &BufSize));

	CloseHandle(hToken);
	//to c++ string
	CString cstring(szHomeDirBuf);
	CT2CA pszConvertedAnsiString(cstring);
	currentPath = String(pszConvertedAnsiString);
#endif
    setName(name);
    buildDirsList();
    buildPathsList();
    text[0]='\0';
}

//draw
void FinderSaveOff::draw()
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
            ImGui::Button( fileInfo.file );
        }
        ImGui::EndChild();
        if(ImGui::Combo("URL", &citem, superPathsList.c_str()))
        {
            currentPath= getPathFromListPaths();
            buildDirsList();
            buildPathsList();
        }
        //file name
        ImGui::InputText("File Name", text, 255);
        if(ImGui::Button("Save") && cbSave)
        {
            if (strlen(text))
            {
                String filepath(currentPath.getDirectory());
                filepath += '/';
                filepath += text;
                size_t hasOff = filepath.rfind(".off");
                if (hasOff != String::npos)
                    cbSave(filepath);
                else
                    cbSave(filepath + ".off");
            }
            visible=false;
        }
        //
        ImGui::End();
    }
}

void FinderSaveOff::setName(const String& argname)
{
    name=argname;
    windowName="Open, "+argname;
    windowChildName="ListPath, "+argname;
}

void FinderSaveOff::setCallBackSave(DFUNCTION<void (const String& path)> fun)
{
    cbSave=fun;
}

void FinderSaveOff::listDirs(std::vector<String>& dirs)
{
    String path=currentPath.getDirectory();
    path.split("/", dirs);
}

String FinderSaveOff::getPathFromListPaths()
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

void FinderSaveOff::buildPathsList()
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

void FinderSaveOff::buildDirsList()
{
    //update dirs
    subDirs.clear();
    subDirs.push_back("..");
    currentPath.getSubDirs(subDirs);
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