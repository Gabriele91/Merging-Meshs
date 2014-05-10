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
	Camera camera;
	Geometry geometry;
	Mesh model1;
	Mesh model2;
	Trackball trackball;

	//ogic
	bool g1org2;
	float zDelta;
	float increment;
	Quaternion rotation;
    

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
		model2.loadOFF(rspath + "/meshs/cone.off", Mesh::OFF_VERTEX_NORMALS_SLOW);
		//set mesh
		geometry.setMesh(&model1);

        //init camera
		camera.setViewport(getRender().getViewportState());
		camera.setPerspective(45.0f, 0.1f, 1000.0f);
		camera.setPosition(Vec3(0, 0, 4.0));
        rotation = Quaternion::fromEulero(Vec3(0, 0, 0));
		zDelta = 1.0;
		g1org2 = false;
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

		if (getInput().getKeyDown(Key::LEFT))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3(0.0, Math::PI*dt, 0)));
		if (getInput().getKeyDown(Key::RIGHT))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3(0.0, -Math::PI*dt, 0)));
		if (getInput().getKeyDown(Key::UP))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3( Math::PI*dt, 0.0, 0.0)));
		if (getInput().getKeyDown(Key::DOWN))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3(-Math::PI*dt, 0.0, 0.0)));
		//delta
		zDelta -= getInput().getScroll()*0.1*dt;
		//clear
		getRender().doClear();

		//camera left
		getRender().setViewportState(Vec4(0, 0, 1280 * 0.5, 720));
		camera.setViewport(getRender().getViewportState());
		camera.setPerspective(45.0f, 0.1f, 1000.0f);
		//rotation
		rotation = 
		Quaternion::fromLookRotation(camera.getPointFrom2DView(getInput().getMouse()), Vec3(0,1,0)).getInverse();
		//draw model
		geometry.setRotation(rotation);
		geometry.setScale(Vec3::ONE*zDelta);
		geometry.draw(camera);
		//draw trackball
		trackball.setPosition(geometry.getPosition());
		trackball.setScale(Vec3::ONE*1.5);
		trackball.setRotation(rotation);
		trackball.draw(camera);

		
		//camera right
		getRender().setViewportState(Vec4(1280 * 0.5, 0, 1280 * 0.5, 720));
		camera.setViewport(getRender().getViewportState());
		camera.setPerspective(45.0f, 0.1f, 1000.0f);
		//rotation
		rotation =
		Quaternion::fromLookRotation(camera.getPointFrom2DView(getInput().getMouse()), Vec3(0, 1, 0)).getInverse();
		//draw model
		geometry.setRotation(rotation);
		geometry.setScale(Vec3::ONE*zDelta);
		geometry.draw(camera);
		//draw trackball
		trackball.setPosition(geometry.getPosition());
		trackball.setScale(Vec3::ONE*1.5);
		trackball.setRotation(rotation);
		trackball.draw(camera);

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