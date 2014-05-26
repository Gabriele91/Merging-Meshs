#include <stdafx.h>
#include <Application.h>
#include <Game.h>
#include <Input.h>
#include <Screen.h>
#include <Render.h>
#include <Shader.h>
#include <Math3D.h>
#include <Object.h>
#include <Debug.h>

#include "Mesh.h"
#include "Camera.h"
#include "Trackball.h"
#include "TrackArea.h"
#include "Geometry.h"

using namespace Easy3D;

class MyGame : public Easy3D::Game {
public:

	//materials
	TrackballMaterial matTrackball;
	PointsMaterial	  matPoints;
	GeometryMaterial  matGeometry;
	//model 1
	Mesh modelLeft;
	TrackArea trackAreaLeft;
	//model 2
	Mesh modelRight;
	TrackArea trackAreaRight;
	//others
	Trackball trackball;

    

	MyGame() :Game("Easy3D exemple", 1280, 720,32,60){}

	void start(){
		//resource
		String rspath = Application::instance()->appResourcesDirectory();
		//init renders
		Vec4 viewport(0, 0, 1280, 720);
		getRender().setViewportState(viewport);
		getRender().setZBufferState(true);
		getRender().setBlendState(BlendState(BLEND::ONE, BLEND::ZERO));
		getRender().setCullFaceState(CullFace::DISABLE);
		//materials
		matTrackball.init();
		matPoints.init();
		matGeometry.init();
		//init trackball
		trackball.init(&matTrackball);
		////////////////
		//LEFT
		//init first
		modelLeft.loadOFF(rspath + "/meshs/all.off",false);
		//init track 
		trackAreaLeft.init(&matGeometry, &matPoints);
		trackAreaLeft.setMesh(modelLeft);
		trackAreaLeft.setTrackball(trackball);
		trackAreaLeft.sphere.radius = 1.5;
		trackAreaLeft.setZoomVelocity(0.1);
		trackAreaLeft.setZDistance(4);
		trackAreaLeft.setViewport(
			Vec4(0, 0,
				 viewport.z*0.5, viewport.w)
			);
		////////////////
		//RIGHT
		//init first ship/tet3dcc2/m299/head/m332/m355/m1003/m1372
		modelRight.loadOFF(rspath + "/meshs/ship.off", false);
		//init track 
		trackAreaRight.init(&matGeometry, &matPoints);
		trackAreaRight.setMesh(modelRight);
		trackAreaRight.setTrackball(trackball);
		trackAreaRight.sphere.radius = 1.5;
		trackAreaRight.setZoomVelocity(0.1);
		trackAreaRight.setZDistance(4);
		trackAreaRight.setViewport(
			Vec4(viewport.z*0.5, 0, 
				 viewport.z*0.5, viewport.w)
			);
		////////////////

	}
	
	void run(float dt){

		//clear
		getRender().doClear();
		//draw left
		trackAreaLeft.draw();
		//draw right
		trackAreaRight.draw();
        
	}
	void end(){

	}

};

int main(){
	Easy3D::Application::create("Easy3DExemple", 
												OPENGL_DRIVER
												//DIRECTX_DRIVER
												);
	Easy3D::Application::instance()->exec(new MyGame());
	delete Easy3D::Application::instance()->getGame();
	return 0;
}