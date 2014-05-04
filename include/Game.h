#ifndef GAME_H
#define GAME_H

#include <Config.h>
#include <EString.h>
#include <Application.h>

namespace Easy3D {

	class Game {

	public:


		Game(const String& name,
			 uint width,
			 uint height,
			 uint bites = 32,
			 uint FreamPerSecond = 60,
			 bool fullscreen = false,
			 int defaultNoAA = 0);

		virtual ~Game(){};

		virtual void start();
		virtual void run(float dt);
		virtual void end();

        
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