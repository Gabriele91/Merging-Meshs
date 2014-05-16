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
#include "Trackball.h"
#include "TrackArea.h"
#include "Camera.h"
#include "Geometry.h"

using namespace Easy3D;

class MyGame : public Easy3D::Game {
public:

	Geometry geometry;
	Object m1rot,m1p1;
	Object m2p1;
	Mesh model1;
	Mesh model2;
	Trackball trackball;
	Camera cameraLeft;
	Camera cameraRight;
    Vec4   vieportLeft;
    Vec4   vieportRight;
	TrackArea trackAreaLeft;
	TrackArea trackAreaRight;
    

	MyGame() :Game("Easy3D exemple", 1280, 720){}

	void start(){

		getRender().setViewportState(Vec4(0, 0, 1280, 720));
		getRender().setZBufferState(true);
		getRender().setBlendState(BlendState(BLEND::ONE, BLEND::ZERO));
		getRender().setCullFaceState(CullFace::DISABLE);
		//init renders
		trackball.init();
		geometry.init();
		//load models
		String rspath = Application::instance()->appResourcesDirectory();
		//model1.loadOFF(rspath + "/meshs/faccia000.off", Mesh::OFF_VERTEX_NORMALS);
		model2.loadOFF(rspath + "/meshs/cube.off", Mesh::OFF_VERTEX_NORMALS_SLOW);
		      
        //left
        vieportLeft=getRender().getViewportState().viewport*Vec4(0,0,0.5,1.0);
		//init track area
		trackAreaLeft.setCamera(cameraLeft);
		trackAreaLeft.attach(m1rot);
		trackAreaLeft.sphere.radius = 1.5;
        trackAreaLeft.setZoomVelocity(0.1);
        //init camera
		cameraLeft.setViewport(vieportLeft);
		cameraLeft.setPerspective(45.0f, 0.1f, 1000.0f);
		cameraLeft.setPosition(Vec3(0, 0, 4.0));
        //offset child
		m1rot.addChild(&m1p1,false);
        
        //right
        vieportRight=Vec4(vieportLeft.z,0,vieportLeft.z,vieportLeft.w);
		//init track area
		trackAreaRight.setCamera(cameraRight);
		trackAreaRight.attach(m2p1);
		trackAreaRight.sphere.radius = 1.5;
        trackAreaRight.setZoomVelocity(0.1);
        //init camera
		cameraRight.setViewport(vieportRight);
		cameraRight.setPerspective(45.0f, 0.1f, 1000.0f);
		cameraRight.setPosition(Vec3(0, 0, 4.0));
        
	}
	
	void run(float dt){

		//clear
		getRender().doClear();

		//camera left
		getRender().setViewportState(vieportLeft);
		m1p1.addChild(&geometry);
		//draw model
		geometry.setMesh(&model2);
		geometry.draw(cameraLeft);
		//draw trackball
		trackball.setPosition(trackAreaLeft.sphere.point);
		trackball.setScale(trackAreaLeft.sphere.radius*Vec3::ONE);
		trackball.setRotation(m1rot.getRotation());
		trackball.draw(cameraLeft);

		//pos pikking
		Vec3 camspace = cameraLeft.picking(getInput().getMouse());
        //from mouse
		if (getInput().getMouseHit(Key::BUTTON_MIDDLE))
			m1p1.setPosition({0,1,0}, true);
		trackball.setPosition(camspace);
		trackball.setScale(Vec3::ONE*.05);
		trackball.setRotation(Quaternion::fromEulero(Vec3::ZERO));
		trackball.draw(cameraLeft);
		//dt geometry
		m1p1.erseChild(&geometry);

		/*
		//camera right
		getRender().setViewportState(vieportRight);
		m2p1.addChild(&geometry);
		//draw model
		geometry.setMesh(&model2);
		geometry.draw(cameraRight);
		//draw trackball
		trackball.setPosition(trackAreaRight.sphere.point);
		trackball.setScale(trackAreaRight.sphere.radius*Vec3::ONE);
        trackball.setRotation(m2p1.getRotation());
		trackball.draw(cameraRight);
		//pikking
		Vec3 camspace2 = cameraRight.picking(getInput().getMouse());
        //from mouse
		if (getInput().getMouseHit(Key::BUTTON_RIGHT))
			m2p1.setPosition(m2p1.getPosition() - camspace2);
		trackball.setPosition(camspace2);
		trackball.setScale(Vec3::ONE*.05);
		trackball.setRotation(Quaternion::fromEulero(Vec3::ZERO));
		trackball.draw(cameraRight);
		//dt geometry
		m2p1.erseChild(&geometry);
		*/
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