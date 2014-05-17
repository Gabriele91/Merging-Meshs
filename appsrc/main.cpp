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

#include "MathUtility.h"
#include "Mesh.h"
#include "Camera.h"
#include "Trackball.h"
#include "TrackArea.h"
#include "Geometry.h"

using namespace Easy3D;

class MyGame : public Easy3D::Game {
public:

	//materials
	TrackballMaterial trackballmat;
	GeometryMaterial  geometrymat;
	//model 1
	Mesh model1;
	Object pivot1;
	Geometry geometry1;
	//model 2
	Mesh model2;
	Object pivot2;
	Geometry geometry2;
	//others
	Trackball trackball;
	Camera cameraLeft;
    Vec4   vieportLeft;
	TrackArea trackAreaLeft;
    

	MyGame() :Game("Easy3D exemple", 1280, 720){}

	void start(){
		//resource
		String rspath = Application::instance()->appResourcesDirectory();
		//init renders
		getRender().setViewportState(Vec4(0, 0, 1280, 720));
		getRender().setZBufferState(true);
		getRender().setBlendState(BlendState(BLEND::ONE, BLEND::ZERO));
		getRender().setCullFaceState(CullFace::DISABLE);
		//materials
		trackballmat.init();
		geometrymat.init();
		//init trackball
		trackball.init(&trackballmat);
		////////////////
		//LEFT
		//init first
		model1.loadOFF(rspath + "/meshs/faccia000.off", Mesh::OFF_VERTEX_NORMALS);
		geometry1.init(&geometrymat);
		geometry1.setMesh(&model1);
		pivot1.addChild(&geometry1, false);	
		//init track area
		trackAreaLeft.setCamera(cameraLeft);
		trackAreaLeft.attach(pivot1);
		trackAreaLeft.sphere.radius = 1.5;
		trackAreaLeft.setZoomVelocity(0.1);
		//left viewport
		vieportLeft = getRender().getViewportState().viewport*Vec4(0, 0, 0.5, 1.0);
		//init camera
		cameraLeft.setViewport(vieportLeft);
		cameraLeft.setPerspective(45.0f, 0.1f, 1000.0f);
		cameraLeft.setPosition(Vec3(0, 0, 4.0));
		////////////////
		//RIGHT
		//init second
		model2.loadOFF(rspath + "/meshs/cube.off", Mesh::OFF_VERTEX_NORMALS_SLOW);
		geometry2.init(&geometrymat);
		geometry2.setMesh(&model2);
		pivot2.addChild(&geometry2, false);

	}
	
	void run(float dt){

		//clear
		getRender().doClear();
		//camera left
		getRender().setViewportState(vieportLeft);
		//draw model
		geometry1.draw(cameraLeft);
		//from mouse
		if (getInput().getMouseHit(Key::BUTTON_MIDDLE))
			geometry1.setMove(-cameraLeft.picking(getInput().getMouse()), true);
		if (getInput().getKeyDown(Key::R))
			geometry1.setPosition(Vec3::ZERO);
		//draw picking
		trackball.setPosition(cameraLeft.picking(getInput().getMouse()));
		trackball.setScale(Vec3::ONE*.05);
		trackball.setRotation(Quaternion::fromEulero(Vec3::ZERO));
		trackball.draw(cameraLeft);
		//draw trackball
		trackball.setPosition(trackAreaLeft.sphere.point);
		trackball.setScale(trackAreaLeft.sphere.radius*Vec3::ONE);
		trackball.setRotation(pivot1.getRotation(true));
		trackball.draw(cameraLeft);

        
	}
	void end(){

	}

};

int main(){
	Easy3D::Application::create("Easy3DExemple", 
												//OPENGL_DRIVER
												DIRECTX_DRIVER
												);
	Easy3D::Application::instance()->exec(new MyGame());
	delete Easy3D::Application::instance()->getGame();
	return 0;
}