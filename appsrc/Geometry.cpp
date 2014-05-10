#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include <Screen.h>
#include "Geometry.h"
#include "Camera.h"

using namespace Easy3D;
void Geometry::init(){
	///////////////////////////
	String rpath = Application::instance()->appResourcesDirectory();
	Render& r = *Application::instance()->getRender();
	///////////////////////////
	//init shader
	shader = r.createShader();
	if (r.getRenderDriver() == OPENGL_DRIVER){
		shader->loadShader(rpath + "/shader/meshs/meshs.vs.glsl",
						   rpath + "/shader/meshs/meshs.fs.glsl",{ "OpenGL_3_2 true" });
		lightDir     = shader->getConstVec3("light")->shared();
		lightDiffuse = shader->getConstVec4("diffuse")->shared();
	}
	else if (r.getRenderDriver() == DIRECTX_DRIVER){
		shader->loadShader(rpath + "/shader/meshs/meshs.vs.hlsl",
						   rpath + "/shader/meshs/meshs.fs.hlsl",{ "DirectX_10 true" });
		lightDir     = shader->getConstVec3("ps.light")->shared();
		lightDiffuse = shader->getConstVec4("ps.diffuse")->shared();
	}

	proj   = shader->getConstMat4("projection")->shared();
	view   = shader->getConstMat4("view")->shared();
	model  = shader->getConstMat4("model")->shared();

	AttributeList atl({
		{ "inPosition", ATT_FLOAT3 },
		{ "inNormal", ATT_FLOAT3 }
	});
	il = r.createIL(shader, atl);
	///////////////////////////
	//add relative pivot
	addChild(&relative, false);	
	///////////////////////////
}
Geometry::~Geometry(){
	Render* r = Application::instance()->getRender();
	if (il)
		r->deleteIL(il);
	if (shader)
		r->deleteShader(shader);
}
void Geometry::setMesh(Mesh* mesh){
	geometry = mesh;
}
void Geometry::draw(Camera& camera){
	//render
	Render& r = *Application::instance()->getRender();
	DEBUG_ASSERT(geometry);
	//save context
	auto cullstate = r.getCullFaceState();
	auto blendstate = r.getBlendState();
	//calc offset
	auto bsize = geometry->getBox().getSize();
	auto bcenter = geometry->getBox().getCenter();
	auto minsize = Math::min(bsize.x, bsize.y, bsize.z);
	Vec3 factor = Vec3::ONE / minsize ;
	//calc offset
	relative.setScale(factor);
	relative.setPosition(-bcenter * factor * getScale());
	//draw
	r.setCullFaceState(CullFace::DISABLE);
	r.setBlendState({ BLEND::ONE, BLEND::ZERO });
	r.bindShader(shader);

	proj->setValue(camera.getProjectionMatrix());
	view->setValue(camera.getViewMatrix());
	model->setValue(relative.getGlobalMatrix());
	lightDir->setValue(lDir);
	lightDiffuse->setValue(lDiffuse);

	geometry->draw(il);

	r.unbindShader();
	//reset context
	r.setBlendState(blendstate);
	r.setCullFaceState(cullstate);
}