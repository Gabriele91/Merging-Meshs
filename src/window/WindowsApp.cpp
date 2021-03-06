#include "stdafx.h"
#include <Timer.h>
#include <Render.h>
#include <RenderGL.h>
#include <RenderDX.h>
#include <WindowsApp.h>
#include <WindowsScreen.h>
#include <WindowsInput.h>
#include <Shlobj.h>
#include <Debug.h>
//window
#include <direct.h>
//
///////////////////////
using namespace Easy3D;

WindowsApp::WindowsApp(const String& name, RenderDriver tRender) :Application(){
	if (tRender==OPENGL_DRIVER)
		render = (Render*)new RenderGL();
	else
		render = (Render*)new RenderDX();
	screen = (Screen*)new WindowsScreen();
	input = (Input*)new WindowsInput();
   //savename
   appname=name;
   /////////////////////////////////////
   //create appdirectory
   //init appdata folder
   TCHAR szPath[MAX_PATH];
   SHGetFolderPathA(((WindowsScreen*)getScreen())->hWind,
                    CSIDL_APPDATA|CSIDL_FLAG_CREATE,
                    NULL,
                    0,
                    szPath);
   dataPath = String(szPath) + '/' + appname;
   //create directory
   CreateDirectory(dataPath.c_str(),0);
   //get errors
   DWORD error=GetLastError();
   DEBUG_ASSERT(error != ERROR_PATH_NOT_FOUND);
   //DEBUG_ASSERT(error != ERROR_ALREADY_EXISTS);
   /////////////////////////////////////
   //not exit from loop
   doexit=false;
}

WindowsApp::~WindowsApp(){
	//delete screen
	delete screen;
	screen = NULL;
	//delete render context
	delete render;
	render = NULL;
	//delete input
	delete input;
	input = NULL;
}

bool WindowsApp::loadData(const String& path,void*& ptr,size_t &len){
	//open
	FILE *pfile=fopen(path,"rb");
	DEBUG_MESSAGE("load file: "<<path); 
	DEBUG_ASSERT_MSG(pfile,"error load file: "<<path);
		//get size
		fseek(pfile,0,SEEK_END);
		len=ftell(pfile);
		fseek(pfile,0,SEEK_SET);
		//read
		ptr=malloc(len*sizeof(char)+1);
		fread(ptr,len,1,pfile);
		(*((char*)ptr+len))='\0';
	//close
	fclose(pfile);
	return pfile!=NULL;
}

String WindowsApp::appDataDirectory(){
	return dataPath;
}

String WindowsApp::appWorkingDirectory(){
	char cCurrentPath[MAX_PATH];
	if(!_getcwd(cCurrentPath, sizeof(cCurrentPath))){
		DEBUG_MESSAGE("Error get working directory: "<<errno);
	}
	return cCurrentPath;
}

String WindowsApp::appResourcesDirectory(){
	return appWorkingDirectory();
}	

void WindowsApp::exit(){
	PostQuitMessage(0);
	doexit=true;
}

void WindowsApp::loop(){	
	//
	Timer timer;
	double msToSleep=1000.0/(static_cast<double>(screen->getFrameRate()));
	double millipass=0;
	double dt=0;
	double sleepTime=0;
	//start timer	
	timer.start();
	//set current context
	screen->acquireContext();
	//draw loop
	while( !input->getClose() && !doexit ) {
		//get timer values
		millipass=timer.getGetCounter();
		//calc dt and sleep time
		sleepTime=msToSleep-millipass;
		while(sleepTime>0 && sleepTime<60000 ){
			Sleep(sleepTime>10?1:0);
			millipass=timer.getGetCounter();
			sleepTime=msToSleep-millipass;
		}
		//calc dt
		dt=millipass/1000.0;
		timer.reset();
        //save dt
        lastDeltaTime=dt;
		//update
		update(dt);	
		//update opengl
		screen->swap();
    }
}

void WindowsApp::exec(Game *ptrMainInstance){
	mainInstance = ptrMainInstance;
	//start
	mainInstance->start();
	//loop
	loop();
	//end
	mainInstance->end();
}

void WindowsApp::update(float dt){
	input->update();
	mainInstance->run(dt);
}

bool WindowsApp::onlyPO2(){
	return true;
}