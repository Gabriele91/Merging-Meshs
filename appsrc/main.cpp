#include <stdafx.h>
#include <Application.h>
#include <Game.h>
#include <Input.h>
#include <Render.h>
#include <Shader.h>
#include <Math3D.h>
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
    Mat4 projection;
    Mat4 view;
    Mat4 model;
	Mesh geometry;

	float increment;
    

    BaseVertexBufferObject *bvb;
    BaseInputLayout *bil;

	MyGame() :Game("Easy3D exemple", 640, 640){}

	void start(){

		getRender().setViewportState(Vec4(0, 0, 640, 640));
		getRender().setZBufferState(true);
		getRender().setBlendState(BlendState(BLEND::ONE, BLEND::ZERO));
        
        Easy3D::String rspath=Easy3D::Application::instance()->appResourcesDirectory();
		shobj = getRender().createShader();

		if (getRender().getRenderDriver() == OPENGL_DRIVER){
			getRender().setCullFaceState(CullFace::BACK);
			shobj->loadShader(rspath + "/shader/base.vs.glsl",
                              rspath + "/shader/base.fs.glsl",
                              { "OpenGL_3_2 true" });

			ptrCProjection = shobj->getConstMat4("projection");
			ptrCView = shobj->getConstMat4("view");
			ptrCModel = shobj->getConstMat4("model");
		}

		if (getRender().getRenderDriver() == DIRECTX_DRIVER){
			getRender().setCullFaceState(CullFace::FRONT);
			shobj->loadShader(rspath + "/shader/base.vs.hlsl",
                              rspath + "/shader/base.fs.hlsl",
                              { "DirectX_10 true" });
			ptrCProjection = shobj->getConstMat4("vs.projection");
			ptrCView = shobj->getConstMat4("vs.view");
			ptrCModel = shobj->getConstMat4("vs.model");
		}

		AttributeList atl({
			{ "inPosition", ATT_FLOAT3 },
			//{ "inColor", ATT_FLOAT4 }
		});
		bil = getRender().createIL(shobj, atl);

		geometry.loadOFF("meshs/faccia000.off");

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
        
        
	}
	void run(float dt){

		if (getInput().getKeyDown(Key::F))
			getRender().setCullFaceState(CullFace::FRONT);
		if (getInput().getKeyDown(Key::B))
			getRender().setCullFaceState(CullFace::BACK);

		getRender().doClear();
		//bind
		getRender().bindShader(shobj);
        
		ptrCProjection->setValue(projection);
		ptrCView->setValue(view);
        
        //getRender().bindVBO(bvb);
        getRender().bindIL(bil);
        
        ///draw 1
		model.setScale(Vec3(0.25, 0.25, 0.1));
		model.addTranslation(Vec3(0,0,0));
		model.addEulerRotation(Vec3(0, 0, increment+=1*dt));
		ptrCModel->setValue(model);
        //getRender().drawArrays(DRAW_TRIANGLES, 3);
		geometry.draw();
		///draw 2
		//model.setScale(Vec3(5, 5, 1));
		//model.addTranslation(Vec3(-0.5,0,-11));
		//ptrCModel->setValue(model);
		//getRender().drawArrays(DRAW_TRIANGLES, 3);
		geometry.draw();

		getRender().unbindShader();
        getRender().unbindIL(bil);
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