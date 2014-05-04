#include <stdafx.h>
#include <Screen.h>
#include <Render.h>
#include <Application.h>
///////////////////////
using namespace Easy3D;

//inizialize render context
void Screen::initRender(){
	Application::instance()->getRender()->__renderInit();
}