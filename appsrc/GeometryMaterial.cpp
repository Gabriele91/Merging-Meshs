#include <stdafx.h>
#include <Render.h>
#include <Utility.h>
#include <Application.h>
#include "GeometryMaterial.h"
///////////////////////
using namespace Easy3D;
///////////////////////


void GeometryMaterial::initShadow(){
	//get render
	Render& r = *Application::instance()->getRender();
	String  rpath = Application::instance()->appResourcesDirectory();
	//init shader
	shadowSH = r.createShader();
	if (r.getRenderDriver() == OPENGL_DRIVER)
		shadowSH->loadShader(rpath + "/shader/geometry/geometryShadow.vs.glsl",
							 rpath + "/shader/geometry/geometryShadow.fs.glsl");
	else if (r.getRenderDriver() == DIRECTX_DRIVER)
		shadowSH->loadShader(rpath + "/shader/geometry/geometryShadow.vs.hlsl",
							 rpath + "/shader/geometry/geometryShadow.fs.hlsl");

	shadowP = shadowSH->getConstMat4("projection")->shared();
	shadowV = shadowSH->getConstMat4("view")->shared();
	shadowM = shadowSH->getConstMat4("model")->shared();

	AttributeList atl({
		{ "inPosition", ATT_FLOAT3 },
		{ "inNormal", ATT_FLOAT3 }
	});
	shadowIL = r.createIL(shadowSH, atl);
	////////////////////////////////////////////////////////
	// shadow map
	auto viewport = Vec4(0, 0, 1024, 1024);
	shadowCam.setViewport(viewport);
	shadowMap = r.createRenderTexture(viewport.z, viewport.w, 32, RD_SHADOW);
	shadowCam.setPerspective(45.0f, 0.1f, 1000.0f);
	shadowCam.setPosition({ 1, 0, 3 });
	shadowCam.setRotation(Quaternion::fromEulero(Vec3(0,Math::PI*.1,0)));
	////////////////////////////////////////////////////////
}
void GeometryMaterial::initColor(){
	//get render
	Render& r = *Application::instance()->getRender();
	String  rpath = Application::instance()->appResourcesDirectory();
	//init shader
	colorSH = r.createShader();
	if (r.getRenderDriver() == OPENGL_DRIVER){
		colorSH->loadShader(rpath + "/shader/geometry/geometry.vs.glsl",
							rpath + "/shader/geometry/geometry.fs.glsl");
		lightDir = colorSH->getConstVec3("light")->shared();
		lightDiffuse = colorSH->getConstVec4("diffuse")->shared();
		colorTEX = colorSH->getConstTexture("shadowMap")->shared();
	}
	else if (r.getRenderDriver() == DIRECTX_DRIVER){
		colorSH->loadShader(rpath + "/shader/geometry/geometry.vs.hlsl",
						    rpath + "/shader/geometry/geometry.fs.hlsl");
		lightDir = colorSH->getConstVec3("ps.light")->shared();
		lightDiffuse = colorSH->getConstVec4("ps.diffuse")->shared();
		colorTEX = colorSH->getConstTexture("ps.shadowMap")->shared();
	}
	//camera 
	colorP = colorSH->getConstMat4("projection")->shared();
	colorV = colorSH->getConstMat4("view")->shared();
	//shadow camera
	colorSWP = colorSH->getConstMat4("projectionSW")->shared();
	colorSWV = colorSH->getConstMat4("viewSW")->shared();
	//model
	colorM = colorSH->getConstMat4("model")->shared();
	
	AttributeList atl({
		{ "inPosition", ATT_FLOAT3 },
		{ "inNormal", ATT_FLOAT3 }
	});
	colorIL = r.createIL(colorSH, atl);
}

void GeometryMaterial::init(){
	//init shadow shader
	initShadow();
	//init shadow color
	initColor();
}
int  GeometryMaterial::id(){
	return 1;
};
void GeometryMaterial::bind(){
	Render& r = *Application::instance()->getRender();
	//save context
	ctxCFaces   =r.getCullFaceState();
	ctxBlend    =r.getBlendState();
	ctxViewport =r.getViewportState();
	//bind
	r.setCullFaceState(CullFace::DISABLE);
	r.setBlendState({ BLEND::ONE, BLEND::ZERO });
}

void GeometryMaterial::drawShadow(const Mesh& m){
	//get render
	Render& r = *Application::instance()->getRender();
	//CREATE A SHADOW MAP
	r.beginRenderToTexture(shadowMap);

	r.setViewportState(shadowCam.getViewport());
	r.doClear();

	r.bindShader(shadowSH);

	shadowP->setValue(shadowCam.getProjectionMatrix());
	shadowV->setValue(shadowCam.getViewMatrix());
	shadowM->setValue(object->getGlobalMatrix());

	m.draw(shadowIL);

	r.endRenderToTexture(shadowMap);
	r.unbindShader();
}

void GeometryMaterial::drawColor(const Mesh& m){
	//get render
	Render& r = *Application::instance()->getRender();
	//DRAW MESH
	r.setViewportState(ctxViewport);

	r.bindShader(colorSH);
	//shadow map
	colorTEX->setValue(shadowMap);
	//shadow camera
	colorSWP->setValue(shadowCam.getProjectionMatrix());
	colorSWV->setValue(shadowCam.getViewMatrix());
	//camera coords
	colorP->setValue(camera->getProjectionMatrix());
	colorV->setValue(camera->getViewMatrix());
	//model
	colorM->setValue(object->getGlobalMatrix());
	//set light info
	lightDir->setValue(ldir);
	lightDiffuse->setValue(ldiffuse);
	//draw
	m.draw(colorIL);
	//end draw
	r.unbindShader();
}


void GeometryMaterial::draw(const Mesh& m) {
	//build shadow
	drawShadow(m);
	//draw model
	drawColor(m);
}
void GeometryMaterial::unbind(){
	Render& r = *Application::instance()->getRender();
	//reset context
	r.setBlendState(ctxBlend);
	r.setCullFaceState(ctxCFaces);
	r.setViewportState(ctxViewport);
}

GeometryMaterial::~GeometryMaterial(){
	Render* r = Application::instance()->getRender();
	if (colorIL)
		r->deleteIL(colorIL);
	if (colorSH)
		r->deleteShader(colorSH);
	if (shadowIL)
		r->deleteIL(shadowIL);
	if (shadowSH)
		r->deleteShader(shadowSH);
	if (shadowMap)
		r->deleteTexture(shadowMap);
}