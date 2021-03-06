#include <stdafx.h>
#include <Render.h>
#include <Utility.h>
#include <Application.h>
#include "TrackballMaterial.h"
///////////////////////
using namespace Easy3D;
///////////////////////


void TrackballMaterial::init(){
	//get render
	Render& r = *Application::instance()->getRender();
	String  rpath = Application::instance()->appResourcesDirectory();
	//init shader
	shader = r.createShader();
	if (r.getRenderDriver() == OPENGL_DRIVER)
		shader->loadShader(rpath + "/shader/trackball/trackball.vs.glsl",
		rpath + "/shader/trackball/trackball.fs.glsl");
	else if (r.getRenderDriver() == DIRECTX_DRIVER)
		shader->loadShader(rpath + "/shader/trackball/trackball.vs.hlsl",
		rpath + "/shader/trackball/trackball.fs.hlsl");

	proj = shader->getConstMat4("projection")->shared();
	view = shader->getConstMat4("view")->shared();
	model = shader->getConstMat4("model")->shared();

	AttributeList aList = {
		{ "inPosition", ATT_FLOAT3 },
		{ "inColor", ATT_FLOAT4 }
	};
	il = r.createIL(shader, aList);
}
int  TrackballMaterial::id(){
	return 0;
};
void TrackballMaterial::draw(const Mesh& m) {
	Render& r = *Application::instance()->getRender();
	//save context
	auto ctxCFaces = r.getCullFaceState();
	auto ctxBlend = r.getBlendState();
	//bind
	r.bindShader(shader);
	r.setCullFaceState(CullFace::DISABLE);
	r.setBlendState({ BLEND::ONE, BLEND::ZERO });
	//camera coords
	proj->setValue(camera->getProjectionMatrix());
	view->setValue(camera->getViewMatrix());
	//model
	model->setValue(object->getGlobalMatrix());
	//draw
	m.draw(il);
	//unbind
	r.unbindShader();
	//reset context
	r.setBlendState(ctxBlend);
	r.setCullFaceState(ctxCFaces);
}

TrackballMaterial::~TrackballMaterial(){
	Render* r = Application::instance()->getRender();
	if (il)
		r->deleteIL(il);
	if (shader)
		r->deleteShader(shader);
}