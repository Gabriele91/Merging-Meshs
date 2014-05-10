#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "Trackball.h"

using namespace Easy3D;

Trackball::Trackball(){}

void Trackball::init(){
	String rpath = Application::instance()->appResourcesDirectory();
	Render& r = *Application::instance()->getRender();

	//init shader
	shader = r.createShader();
	if (r.getRenderDriver() == OPENGL_DRIVER)
		shader->loadShader(rpath + "/shader/trackball/trackball.vs.glsl",
						   rpath + "/shader/trackball/trackball.fs.glsl", { "OpenGL_3_2 true" });
	else if (r.getRenderDriver() == DIRECTX_DRIVER)
		shader->loadShader(rpath + "/shader/trackball/trackball.vs.hlsl",
						   rpath + "/shader/trackball/trackball.fs.hlsl", { "DirectX_10 true" });

	proj = shader->getConstMat4("projection")->shared();
	view = shader->getConstMat4("view")->shared();
	model = shader->getConstMat4("model")->shared();

	AttributeList aList = {
		{ "inPosition", ATT_FLOAT3 },
		{ "inColor", ATT_FLOAT4 }
	};
	il = r.createIL(shader, aList);

	//init mesh
	trackball3D.format(Mesh::POSITION3D | Mesh::COLOR, (361*2)*3+1);
	float rad = 0;
	for (size_t i = 0; i != 360; ++i){

		rad = Math::torad((float)i);
		trackball3D.vertex({ std::cos(rad), std::sin(rad), 0 });
		trackball3D.color(Vec4(1.0, 0.0, 0.0, 1.0));

		rad = Math::torad((float)i+1);
		trackball3D.vertex({ std::cos(rad), std::sin(rad), 0 });
		trackball3D.color(Vec4(1.0, 0.0, 0.0, 1.0));

	}
	for (size_t i = 0; i != 360; ++i){

		rad = Math::torad((float)i);
		trackball3D.vertex({ 0, std::sin(rad), std::cos(rad) });
		trackball3D.color(Vec4(0.0, 1.0, 0.0, 1.0));

		rad = Math::torad((float)i+1);
		trackball3D.vertex({ 0, std::sin(rad), std::cos(rad) });
		trackball3D.color(Vec4(0.0, 1.0, 0.0, 1.0));

	}
	for (size_t i = 0; i != 360; ++i){
		rad = Math::torad((float)i);
		trackball3D.vertex({ std::cos(rad), 0, std::sin(rad) });
		trackball3D.color(Vec4(0.0, 0.0, 1.0, 1.0));

		rad = Math::torad((float)i + 1);
		trackball3D.vertex({ std::cos(rad), 0, std::sin(rad) });
		trackball3D.color(Vec4(0.0, 0.0, 1.0, 1.0));
	}
	trackball3D.mode(TypeDraw::DRAW_LINES);
	trackball3D.bind();
}
Trackball::~Trackball(){
	Render* r = Application::instance()->getRender();
	if (il)
		r->deleteIL(il);
	if (shader)
		r->deleteShader(shader);
}

void Trackball::draw(Camera& camera){
	Render& r = *Application::instance()->getRender();

	//save context
	auto cullstate = r.getCullFaceState();
	auto blendstate = r.getBlendState();

	//draw
	r.setCullFaceState(CullFace::DISABLE);
    r.setBlendState({ BLEND::ONE, BLEND::ZERO });
	r.bindShader(shader);

	proj->setValue(camera.getProjectionMatrix());
	view->setValue(camera.getViewMatrix());
	model->setValue(getGlobalMatrix());

	trackball3D.draw(il);

	r.unbindShader();

	//reset context
	r.setBlendState(blendstate);
	r.setCullFaceState(cullstate);
}

