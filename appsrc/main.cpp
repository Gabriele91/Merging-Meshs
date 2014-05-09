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

using namespace Easy3D;

class MyGame : public Easy3D::Game {
public:

	Easy3D::Shader* shobj;
	
    CMat4::ptr ptrCProjection;
    CMat4::ptr ptrCView;
    CMat4::ptr ptrCModel;
    CVec3::ptr ptrCLight;
    CVec4::ptr ptrCDiffuse;
    Mat4 projection;
    Mat4 view;
	Mat4 model;
	Mesh geometry;
	Mesh geometry2;
	BaseInputLayout *bil;

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
        
        Easy3D::String rspath=Easy3D::Application::instance()->appResourcesDirectory();
		shobj = getRender().createShader();

		if (getRender().getRenderDriver() == OPENGL_DRIVER){
			getRender().setCullFaceState(CullFace::BACK);
			shobj->loadShader(rspath + "/shader/normals.vs.glsl",
                              rspath + "/shader/normals.fs.glsl",
                              { "OpenGL_3_2 true" });

			ptrCProjection = shobj->getConstMat4("projection")->shared();
			ptrCView = shobj->getConstMat4("view")->shared();
			ptrCModel = shobj->getConstMat4("model")->shared();
			ptrCLight = shobj->getConstVec3("light")->shared();
			ptrCDiffuse = shobj->getConstVec4("diffuse")->shared();
		}

		if (getRender().getRenderDriver() == DIRECTX_DRIVER){
			getRender().setCullFaceState(CullFace::FRONT);
			shobj->loadShader(rspath + "/shader/normals.vs.hlsl",
                              rspath + "/shader/normals.fs.hlsl",
                              { "DirectX_10 true" });
			ptrCProjection = shobj->getConstMat4("vs.projection")->shared();
			ptrCView = shobj->getConstMat4("vs.view")->shared();
			ptrCModel = shobj->getConstMat4("vs.model")->shared();
			ptrCLight = shobj->getConstVec3("ps.light")->shared();
			ptrCDiffuse = shobj->getConstVec4("ps.diffuse")->shared();
		}

		AttributeList atl({
			{ "inPosition", ATT_FLOAT3 },
			{ "inNormal", ATT_FLOAT3 }
		});
		bil = getRender().createIL(shobj, atl);
		
		geometry.loadOFF(rspath + "/meshs/faccia000.off", Mesh::OFF_VERTEX_NORMALS);
		geometry2.loadOFF(rspath + "/meshs/cube.off", Mesh::OFF_VERTEX_NORMALS_SLOW);
		


        //init
		float wfactor = (float)getScreen().getWidth() / (float)getScreen().getHeight();
		projection = getRender().calculatesProjection(45.0f,wfactor,0.1f,1000.0f);

		Mat4 tranform=computeSVD(
		{
            { -1, 0, 0 },
            {  1, 0, 0 },
            {  0, 1, 0 }
		},
        {
            { -1, 2, 0 },
            {  1, 2, 0 },
            {  0, 1, 0 }
		});
        
        rotation = Quaternion::fromEulero(Vec3(0, 0, 0));
		zDelta = 1.0;
		g1org2 = false;
	}
	void run(float dt){

		if (getInput().getKeyDown(Key::F))
			getRender().setCullFaceState(CullFace::FRONT);
		if (getInput().getKeyDown(Key::B))
			getRender().setCullFaceState(CullFace::BACK);
		if (getInput().getKeyDown(Key::D))
			getRender().setCullFaceState(CullFace::DISABLE);
		if (getInput().getKeyHit(Key::A))
			g1org2 = !g1org2;

		if (getInput().getKeyDown(Key::LEFT))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3(0.0, Math::PI*dt, 0)));
		if (getInput().getKeyDown(Key::RIGHT))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3(0.0, -Math::PI*dt, 0)));
		if (getInput().getKeyDown(Key::UP))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3( Math::PI*dt, 0.0, 0.0)));
		if (getInput().getKeyDown(Key::DOWN))
			rotation = rotation.mul(Quaternion::fromEulero(Vec3(-Math::PI*dt, 0.0, 0.0)));

		auto posLight = getInput().getMouse() / getScreen().getSize();
		zDelta -= getInput().getScroll()*0.1*dt;
		//logic
		auto box = !g1org2 ? geometry.getBox() : geometry2.getBox();
		auto centroid = !g1org2 ? geometry.getCentroid() : geometry2.getCentroid();
		auto bsize = box.getSize();
        Object obj,objRelative;

        
        obj.addChild(&objRelative,false);
		objRelative.setTranslation(-centroid);

		obj.setTranslation(-Vec3(0, 0,(bsize.z*2 + centroid.z)*2)*zDelta);
        obj.setRotation(rotation);

		//draw
		getRender().doClear();
		//shader
		getRender().bindShader(shobj);
		getRender().bindIL(bil);
		//PVM
		ptrCProjection->setValue(projection);
		ptrCView->setValue(view);
        ptrCModel->setValue(objRelative.getGlobalMatrix());
		//light
		ptrCLight->setValue(Vec3(posLight.x - 0.5, 0.5 - posLight.y, 1.0));
        ptrCDiffuse->setValue({ 1.0, 1.0 , 1.0, 1.0 });
                
		if (!g1org2)
			geometry.draw();
		else
			geometry2.draw();

		//unbind 
		getRender().unbindShader();
		getRender().unbindIL(bil);
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