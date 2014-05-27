#include <stdafx.h>
#include <Application.h>
#include "PointsMaterial.h"
///////////////////////
using namespace Easy3D;
///////////////////////


void PointsMaterial::init(){
	//get render
	Render& r = *Application::instance()->getRender();
	String  rpath = Application::instance()->appResourcesDirectory();
	//init shader
	shader = r.createShader();
	if (r.getRenderDriver() == OPENGL_DRIVER){
		shader->loadShader(rpath + "/shader/points/points.vs.glsl",
					  	   rpath + "/shader/points/points.fs.glsl",
					  	   rpath + "/shader/points/points.gs.glsl");
		gsize = shader->getConstVec2("size")->shared();
		proj = shader->getConstMat4("projection")->shared();
		view = shader->getConstMat4("view")->shared();
		model = shader->getConstMat4("model")->shared();
    }
	else if (r.getRenderDriver() == DIRECTX_DRIVER){
		shader->loadShader(rpath + "/shader/points/points.vs.hlsl",
						   rpath + "/shader/points/points.fs.hlsl",
						   rpath + "/shader/points/points.gs.hlsl");
		gsize = shader->getConstVec2("gs.size")->shared();
		proj = shader->getConstMat4("gs.projection")->shared();
		view = shader->getConstMat4("gs.view")->shared();
		model = shader->getConstMat4("gs.model")->shared();
    }

	AttributeList aList = {
		{ "inPosition", ATT_FLOAT3 }
	};
	il = r.createIL(shader, aList);
}
int  PointsMaterial::id(){
	return 0;
};
void PointsMaterial::draw(const Mesh& m) {
	Render& r = *Application::instance()->getRender();
	//save context
    auto ctxCZBuffer = r.getZBufferState();
	auto ctxCFaces = r.getCullFaceState();
	auto ctxBlend = r.getBlendState();
	//bind
	r.bindShader(shader);
	r.setZBufferState(false);
	r.setCullFaceState(CullFace::DISABLE);
	r.setBlendState({ BLEND::ONE, BLEND::ZERO });
	//camera coords
	proj->setValue(camera->getProjectionMatrix());
	view->setValue(camera->getViewMatrix());
	//model
	model->setValue(object->getGlobalMatrix());
	//size
	gsize->setValue(size);
	//draw
	m.draw(il);
	//unbind
	r.unbindShader();
	//reset context
	r.setZBufferState(ctxCZBuffer);
	r.setBlendState(ctxBlend);
	r.setCullFaceState(ctxCFaces);
}

PointsMaterial::~PointsMaterial(){
	Render* r = Application::instance()->getRender();
	if (il)
		r->deleteIL(il);
	if (shader)
		r->deleteShader(shader);
}

