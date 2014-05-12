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
    Object pivot1;
    Object pivot2;
	Mesh model1;
	Mesh model2;
	TrackArea trackArea;
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
		model1.loadOFF(rspath + "/meshs/faccia000.off", Mesh::OFF_VERTEX_NORMALS);
		model2.loadOFF(rspath + "/meshs/cube.off", Mesh::OFF_VERTEX_NORMALS_SLOW);
		      
        //left
        vieportLeft=getRender().getViewportState().viewport*Vec4(0,0,0.5,1.0);
		//init track area
		trackAreaLeft.setCamera(cameraLeft);
		trackAreaLeft.attach(pivot1);
		trackAreaLeft.setTurnIntensity(5.0);
        trackAreaLeft.setZoomVelocity(0.1);
        //init camera
		cameraLeft.setViewport(vieportLeft);
		cameraLeft.setPerspective(45.0f, 0.1f, 1000.0f);
		cameraLeft.setPosition(Vec3(0, 0, 4.0));
        
        
        //right
        vieportRight=Vec4(vieportLeft.z,0,vieportLeft.z,vieportLeft.w);
		//init track area
		trackAreaRight.setCamera(cameraRight);
		trackAreaRight.attach(pivot2);
		trackAreaRight.setTurnIntensity(5.0);
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
		//draw model
        geometry.copyLocalTransform(pivot1);
		geometry.setMesh(&model1);
		geometry.draw(cameraLeft);
		//draw trackball
		trackball.setPosition(geometry.getPosition());
		trackball.setScale(Vec3::ONE*1.5);
		trackball.setRotation(geometry.getRotation());
		trackball.draw(cameraLeft);


		//camera right
		getRender().setViewportState(vieportRight);
		//draw model
        geometry.copyLocalTransform(pivot2);
		geometry.setMesh(&model2);
		geometry.draw(cameraRight);
		//draw trackball
		trackball.setPosition(geometry.getPosition());
		trackball.setScale(Vec3::ONE*1.5);
        trackball.setRotation(geometry.getRotation());
		trackball.draw(cameraRight);

		/*
		Debug::message() << "color: "
			<< getRender().getColor(getInput().getMouse()).toString()
			<< "\n";

		Debug::message() << "depth: "
			<< getRender().getDepth(getInput().getMouse())
			<< "\n";
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