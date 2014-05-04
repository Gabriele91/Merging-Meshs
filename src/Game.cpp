#include <stdafx.h>
#include <Game.h>
#include <Application.h>
#include <Screen.h>

///////////////////////
using namespace Easy3D;
///////////////////////

Game::Game(const String& app,
		   unsigned int width,
		   unsigned int height,
		   unsigned int bites,
		   unsigned int freamPerSecond,
		   bool fullscreen,
		   int dfAA){
Application::instance()
			->getScreen()
			->createWindow(app.c_str(),
							width,
							height,
							bites,
							freamPerSecond,
							fullscreen,
							(Screen::AntiAliasing)dfAA);
}

void Game::start(){

}
void Game::run(float dt){

}
void Game::end(){

}