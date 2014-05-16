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


void TrackArea::rayCast(){
	Vector3D w = ray.origin - sphere.point;
	float A = ray.dir.dot(ray.dir);
	float B = 2.0f*w.dot(ray.dir);
	float C = w.dot(w) - sphere.radius*sphere.radius;
	//calc d
	float D = B*B - 4.0f*A*C;
	float sqrtD = sqrt(D);
	//points
	segment.vnear = w + ray.dir*(-B - sqrtD) / (2.0f*A);
	segment.vfar  = w + ray.dir*(-B + sqrtD) / (2.0f*A);
}

void TrackArea::calcRay(const Vec2& mouse){
	camera->ray(mouse,ray.origin,ray.dir);
}

void TrackArea::onMousePress(Vec2 mouse, Key::Mouse bt){
	if (!objToRot) return;
    if (!inViewport(mouse)) return;
    //start rotation
	start = objToRot->getRotation();
	//rote from
	calcRay(mouse);
	rayCast();
	from = segment.vnear;
}
void TrackArea::onMouseDown(Vec2 mouse, Key::Mouse bt){
	if (!objToRot) return;
	if (!inViewport(mouse)) return;
	//rote from
	calcRay(mouse);
	rayCast();
	to = segment.vnear;
#if 0
	//from to
	Vec3 cross = from.cross(to);
	//quad
	Quaternion turn;
	if (cross.getNormalize().length()> 1.0e-6){
		//rote
		float alpha = (from-to).length()/sphere.radius;
		//quaternion turn
		Quaternion turn = Quaternion::fromAxisAngle(cross, alpha);
		//update rotation
		objToRot->setRotation(start.mul(turn));
	}
#else
	//from to
	Vec3 cross = to.cross(from);
	//quad
	Quaternion turn;
	if (cross.getNormalize().length()> 1.0e-6){
		//rote
		float alpha = -(to-from).length() / sphere.radius;
		//quaternion turn
		Quaternion turn = Quaternion::fromAxisAngle(cross, alpha).getInverse();
		//update rotation
		objToRot->setRotation(turn.mul(start));
	}
#endif
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

void  TrackArea::setZoomVelocity(float zvelocity){
    velocity=zvelocity;
}
