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

	Easy3D::Shader* shobj;
	
    CMat4::ptr ptrCProjection;
    CMat4::ptr ptrCView;
    CMat4::ptr ptrCModel;
    CVec3::ptr ptrCLight;
	CVec4::ptr ptrCDiffuse;
	BaseInputLayout *bil;
	Geometry geometry;
	Geometry geometry2;
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
		geometry2.init();
		//load models
		String rspath = Application::instance()->appResourcesDirectory();
		model1.loadOFF(rspath + "/meshs/faccia000.off", Mesh::OFF_VERTEX_NORMALS);
		model2.loadOFF(rspath + "/meshs/cube.off", Mesh::OFF_VERTEX_NORMALS_SLOW);
		
        //set mesh
		geometry.setMesh(&model1);
		geometry2.setMesh(&model2);
        
        //left
        vieportLeft=getRender().getViewportState().viewport*Vec4(0,0,0.5,1.0);
		//init track area
		trackAreaLeft.setCamera(cameraLeft);
		trackAreaLeft.attach(geometry);
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
		trackAreaRight.attach(geometry2);
		trackAreaRight.setTurnIntensity(5.0);
        trackAreaRight.setZoomVelocity(0.1);
        //init camera
		cameraRight.setViewport(vieportRight);
		cameraRight.setPerspective(45.0f, 0.1f, 1000.0f);
		cameraRight.setPosition(Vec3(0, 0, 4.0));
        
	}
	
	//rotation = 
	//Quaternion::fromLookRotation(camera.getPointFrom2DView(getInput().getMouse()), Vec3(0,1,0)).getInverse();

	void run(float dt){

		if (getInput().getKeyDown(Key::F))
			getRender().setCullFaceState(CullFace::FRONT);
		if (getInput().getKeyDown(Key::B))
			getRender().setCullFaceState(CullFace::BACK);
		if (getInput().getKeyDown(Key::D))
			getRender().setCullFaceState(CullFace::DISABLE);


		//clear
		getRender().doClear();
		//camera left
		getRender().setViewportState(vieportLeft);
		//draw model
		geometry.draw(cameraLeft);
		//draw trackball
		trackball.setPosition(geometry.getPosition());
		trackball.setScale(Vec3::ONE*1.5);
		trackball.setRotation(geometry.getRotation());
		trackball.draw(cameraLeft);


		//camera right
		getRender().setViewportState(vieportRight);
		//draw model
		geometry2.draw(cameraRight);
		//draw trackball
		trackball.setPosition(geometry2.getPosition());
		trackball.setScale(Vec3::ONE*1.5);
        trackball.setRotation(geometry2.getRotation());
		trackball.draw(cameraRight);


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