#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "TrackArea.h"

using namespace Easy3D;


void TrackArea::onMousePress(Vec2 mouse, Key::Mouse bt){
	if (!objToRot) return;
	if (!camera) return;
	//start rotation
	start = objToRot->getRotation();
	//start to pic
	from = camera->getPointFrom2DView(mouse).getNormalize();
}
void TrackArea::onMouseDown(Vec2 mouse, Key::Mouse bt){
	if (!objToRot) return;
	if (!camera) return;
	//rote to
	to = camera->getPointFrom2DView(mouse).getNormalize();
	//from to
	Vec3 cross = from.cross(to);
	//to quaternion
	if (cross.length()> 0.5e-6){
		Quaternion turn = Quaternion::fromAxisAngle(cross, asin(sqrt(cross.dot(cross))) * turnIntensity);
		//update rotation
		objToRot->setRotation(start.mul(turn));
	}
	/*
	Quaternion turn;
	if (cross.length()>1.0e-5){
		turn.x = cross.x;
		turn.y = cross.y;
		turn.z = cross.z;
		turn.w = from.dot(to);
	}
	//
	objToRot->setRotation(start.mul(turn));
	*/
}


TrackArea::TrackArea(){
	//reg input
	Application::instance()->getInput()->addHandler((Input::MouseHandler*)this);
}
TrackArea::~TrackArea(){
	//unreg input
	Application::instance()->getInput()->removeHandler((Input::MouseHandler*)this);
}
void TrackArea::setCamera(Camera& obj){
	//set new camera
	camera = &obj;
}
void TrackArea::attach(Object& obj){
	objToRot = &obj;
}
void TrackArea::setTurnIntensity(float trnIt){
	turnIntensity = trnIt;
}