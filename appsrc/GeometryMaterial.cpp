#include <stdafx.h>
#include <Render.h>
#include <Utility.h>
#include <Application.h>
#include "GeometryMaterial.h"
///////////////////////
using namespace Easy3D;
///////////////////////



void GeometryMaterial::init(){
	//get render
	Render& r = *Application::instance()->getRender();
	String  rpath = Application::instance()->appResourcesDirectory();
	//init shader
	shader = r.createShader();
	if (r.getRenderDriver() == OPENGL_DRIVER){
		shader->loadShader(rpath + "/shader/geometry/geometry.vs.glsl",
						   rpath + "/shader/geometry/geometry.fs.glsl");
		lightDir = shader->getConstVec3("light")->shared();
		lightDiffuse = shader->getConstVec4("diffuse")->shared();
	}
	else if (r.getRenderDriver() == DIRECTX_DRIVER){
		shader->loadShader(rpath + "/shader/geometry/geometry.vs.hlsl",
						   rpath + "/shader/geometry/geometry.fs.hlsl");
		lightDir = shader->getConstVec3("ps.light")->shared();
		lightDiffuse = shader->getConstVec4("ps.diffuse")->shared();
	}

	proj = shader->getConstMat4("projection")->shared();
	view = shader->getConstMat4("view")->shared();
	model = shader->getConstMat4("model")->shared();

	AttributeList atl({
		{ "inPosition", ATT_FLOAT3 },
		{ "inNormal", ATT_FLOAT3 }
	});
	il = r.createIL(shader, atl);
}
int  GeometryMaterial::id(){
	return 1;
};
void GeometryMaterial::bind(){
	Render& r = *Application::instance()->getRender();
	//save context
	ctxCFaces =r.getCullFaceState();
	ctxBlend  =r.getBlendState();
	//bind
	r.bindShader(shader);
	r.setCullFaceState(CullFace::DISABLE);
	r.setBlendState({ BLEND::ONE, BLEND::ZERO });
}
void GeometryMaterial::draw(const Mesh& m) {
	m.draw(il);
}
void GeometryMaterial::unbind(){
	Render& r = *Application::instance()->getRender();
	r.unbindShader();
	//reset context
	r.setBlendState(ctxBlend);
	r.setCullFaceState(ctxCFaces);
}

GeometryMaterial::~GeometryMaterial(){
	Render* r = Application::instance()->getRender();
	if (il)
		r->deleteIL(il);
	if (shader)
		r->deleteShader(shader);
}