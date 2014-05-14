#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "TrackArea.h"

using namespace Easy3D;

void TrackArea::onMouseScroll(short scroll){
    if (!objToRot) return;
    if (!inViewport(Application::instance()->getInput()->getMouse())) return;
    scaleZoom -= scroll*velocity*Application::instance()->getLastDeltaTime();
    scaleZoom = Math::max(scaleZoom,0.0f);
    objToRot->setScale(scaleZoom*Vec3::ONE);
}

bool TrackArea::inViewport(const Vec2& point){
    if (!camera) return false;

    Vec2 min=camera->getViewport().xy();
    Vec2 max=Vec2(camera->getViewport().z,camera->getViewport().w)+min;
    
    return (min.x <= point.x  &&  point.x <= max.x &&
            min.y <= point.y  &&  point.y <= max.y);
    
}


Vec3 TrackArea::smap(const Vec2& mouse){
	//center
	// 0 to 1
	auto pcenter = camera->toClip(camera->invScreenY(mouse));
	//-1 to 1
	pcenter = pcenter * 2.0 - 1.0;
	//square length
	float length = pcenter.dot(pcenter);
	//If the point is mapped outside of the sphere... (length > radius squared)
	if (length > (radius*radius)){
		float norm;
		//Compute a normalizing factor (radius / sqrt(length))
		norm = radius / std::sqrt(length);
		//Return the "normalized" vector, a point on the sphere
		return Vec3(pcenter*norm,0.0);
	}
	else {   //Else it's on the inside
		//Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
		return Vec3(pcenter, std::sqrt((radius*radius) - length));
	}
}

void TrackArea::onMousePress(Vec2 mouse, Key::Mouse bt){
	if (!objToRot) return;
    if (!inViewport(mouse)) return;
    //start rotation
	start = objToRot->getRotation();
	//rote from
	from = smap(mouse);
}
void TrackArea::onMouseDown(Vec2 mouse, Key::Mouse bt){
	if (!objToRot) return;
    if (!inViewport(mouse)) return;
	//rote to
	to = smap(mouse);
	//from to
	Vec3 cross = from.cross(to);
	//quad
	Quaternion turn;
	if (cross.length()>1.0e-5){
		turn.x = cross.x;
		turn.y = cross.y;
		turn.z = cross.z;
		turn.w = from.dot(to);
	}
	//
	objToRot->setRotation(start.mul(turn));
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

void TrackArea::setRadius(float r){
	radius = r;
}
float TrackArea::getRadius(){
	return radius;
}
void  TrackArea::setZoomVelocity(float zvelocity){
    velocity=zvelocity;
}
