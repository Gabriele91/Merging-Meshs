#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "Trackball.h"

using namespace Easy3D;

void Trackball::init(TrackballMaterial *tm){
	//save material
	material = tm;
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
}

void Trackball::draw(Camera& camera){
	//draw
	material->setCamera(&camera);
	material->setObject(this);
	material->draw(trackball3D);
}

