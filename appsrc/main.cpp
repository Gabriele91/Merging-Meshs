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
	
    CMat4* ptrCProjection;
    CMat4* ptrCView;
    CMat4* ptrCModel;
    CVec3* ptrCLight;
    CVec4* ptrCDiffuse;
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
	Quaternion rotconst;
	Quaternion rotation;
    

	MyGame() :Game("Easy3D exemple", 640, 640){}

	void start(){

		getRender().setViewportState(Vec4(0, 0, 640, 640));
		getRender().setZBufferState(true);
		getRender().setBlendState(BlendState(BLEND::ONE, BLEND::ZERO));
        
        Easy3D::String rspath=Easy3D::Application::instance()->appResourcesDirectory();
		shobj = getRender().createShader();

		if (getRender().getRenderDriver() == OPENGL_DRIVER){
			getRender().setCullFaceState(CullFace::BACK);
			shobj->loadShader(rspath + "/shader/normals.vs.glsl",
                              rspath + "/shader/normals.fs.glsl",
                              { "OpenGL_3_2 true" });

			ptrCProjection = shobj->getConstMat4("projection");
			ptrCView = shobj->getConstMat4("view");
			ptrCModel = shobj->getConstMat4("model");
			ptrCLight = shobj->getConstVec3("light");
			ptrCDiffuse = shobj->getConstVec4("diffuse");
		}

		if (getRender().getRenderDriver() == DIRECTX_DRIVER){
			getRender().setCullFaceState(CullFace::FRONT);
			shobj->loadShader(rspath + "/shader/normals.vs.hlsl",
                              rspath + "/shader/normals.fs.hlsl",
                              { "DirectX_10 true" });
			ptrCProjection = shobj->getConstMat4("vs.projection");
			ptrCView = shobj->getConstMat4("vs.view");
			ptrCModel = shobj->getConstMat4("vs.model");
			ptrCLight = shobj->getConstVec3("ps.light");
			ptrCDiffuse = shobj->getConstVec4("ps.diffuse");
		}

		AttributeList atl({
			{ "inPosition", ATT_FLOAT3 },
			{ "inNormal", ATT_FLOAT3 }
		});
		bil = getRender().createIL(shobj, atl);
		
		geometry.loadOFF(rspath + "/meshs/faccia000.off", Mesh::OFF_VERTEX_NORMALS);
		geometry2.loadOFF(rspath + "/meshs/cube.off", Mesh::OFF_VERTEX_NORMALS);
		
		//geometry.loadOFF(rspath + "/meshs/ship.off", Mesh::OFF_VERTEX_NORMALS);
		//geometry2.loadOFF(rspath + "/meshs/ship.off", Mesh::OFF_VERTEX_NORMALS_SLOW);

		g1org2 = false;

        //init
        projection=getRender().calculatesProjection(45.0f, 0.75f, 0.1f, 1000.0f);
		
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
        
		rotconst = Quaternion::fromEulero(Vec3(0.0, .01, 0));
        rotation=Quaternion::fromEulero(Vec3(0, 0, 0));
		zDelta = 1.0;
	}
	void run(float dt){

		if (getInput().getKeyDown(Key::F))
			getRender().setCullFaceState(CullFace::FRONT);
		if (getInput().getKeyDown(Key::B))
			getRender().setCullFaceState(CullFace::BACK);
		if (getInput().getKeyHit(Key::A)){
			g1org2 = !g1org2;
			Debug::message() << (!g1org2 ? "g1" : "g2") << '\n';
		}
		auto posLight = getInput().getMouse() / getScreen().getSize();
		zDelta -= getInput().getScroll()*0.1*dt;
		//logic
		auto box = geometry.getBox();
		auto size = box.getSize();
		auto msize = Math::max(size.x, size.y, size.z);
        Object obj,objRelative;
        
        obj.addChild(&objRelative,false);
		objRelative.setTranslation(-box.getCenter());

		obj.setTranslation(-Vec3(0, 0, msize)*zDelta);
        rotation=rotation.mul(rotconst);
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