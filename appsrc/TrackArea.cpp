#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "TrackArea.h"

using namespace Easy3D;


//init
TrackArea::TrackArea(){
	//reg input
	Application::instance()->getInput()->addHandler((Input::MouseHandler*)this);
}
void TrackArea::init(GeometryMaterial* material){
	//set geometry
	geometry.init(material);
	//attach geometry to protation
	protation.addChild(&geometry,false);
	//camera
	//camera.setPerspective(45.0f, 0.1f, 1000.0f);
}
TrackArea::~TrackArea(){
	//unreg input
	Application::instance()->getInput()->removeHandler((Input::MouseHandler*)this);
}

//utility function
bool TrackArea::inViewport(const Vec2& point){

	Vec2 min = camera.getViewport().xy();
	Vec2 max = Vec2(camera.getViewport().z, camera.getViewport().w) + min;

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
	segment.vfar = w + ray.dir*(-B + sqrtD) / (2.0f*A);
}
void TrackArea::calcRay(const Vec2& mouse){
	camera.ray(mouse, ray.origin, ray.dir);
}
//draw mesh
void TrackArea::draw(){

	//get render & input
	Render& render = *Application::instance()->getRender();
	Input&  input = *Application::instance()->getInput();
	//camera left
	render.setViewportState(camera.getViewport());
	//draw model
	geometry.draw(camera);
	//save info query
	picking = camera.picking(input.getMouse());

	//from mouse
	//if (getInput().getMouseHit(Key::BUTTON_MIDDLE))
	//	geometry.setMove(-cameraLeft.picking(getInput().getMouse()), true);
	//if (getInput().getKeyDown(Key::R))
	//	geometry.setPosition(Vec3::ZERO);
	//draw picking
	//trackball.setPosition(cameraLeft.picking(getInput().getMouse()));
	//trackball.setScale(Vec3::ONE*.05);
	//trackball.setRotation(Quaternion::fromEulero(Vec3::ZERO));
	//trackball.draw(cameraLeft);
	//draw trackball
	//trackball.setPosition(trackAreaLeft.sphere.point);
	//trackball.setScale(trackAreaLeft.sphere.radius*Vec3::ONE);
	//trackball.setRotation(pivot1.getRotation(true));
	//trackball.draw(cameraLeft);
}
//set & get
void TrackArea::setMesh(Mesh& obj){
	geometry.setMesh(&obj);
}
void TrackArea::changeMaterial(GeometryMaterial* material){
	geometry.changeMaterial(material);
}
void TrackArea::setViewport(const ViewportState& viewport){
	//camera
	camera.setViewport(viewport);
	camera.setPerspective(45.0f, 0.1f, 1000.0f);
}
void TrackArea::setZoomVelocity(float zvelocity){
    velocity=zvelocity;
}
void TrackArea::setZDistance(float zdistance) {
	camera.setPosition({0,0,zdistance});
}
//events
void TrackArea::onMouseScroll(short scroll){
	if (!inViewport(Application::instance()->getInput()->getMouse())) return;
	scaleZoom -= scroll*velocity*Application::instance()->getLastDeltaTime();
	scaleZoom = Math::max(scaleZoom, 0.0f);
	protation.setScale(scaleZoom*Vec3::ONE);
}
void TrackArea::onMousePress(Vec2 mouse, Key::Mouse bt){
	if (!inViewport(mouse)) return;
	//cases
	switch (bt){
		case Key::BUTTON_LEFT:
			//start rotation
			start = protation.getRotation();
			//rote from
			calcRay(mouse);
			rayCast();
			from = segment.vnear;
		break;
		case Key::BUTTON_MIDDLE:
			geometry.setPosition(Vec3::ZERO);
		break;
		case Key::BUTTON_RIGHT:
			geometry.setMove(-picking, true);
		break;
	default:
		break;
	}
}
void TrackArea::onMouseDown(Vec2 mouse, Key::Mouse bt){
	if (!inViewport(mouse)) return;
	if (bt != Key::BUTTON_LEFT) return;
	//rote from
	calcRay(mouse);
	rayCast();
	to = segment.vnear;
	//from to
	Vec3 cross = from.cross(to).getNormalize();
	//quad
	Quaternion turn;
	if (cross.length()> 1.0e-6){
		//rote
		float alpha = (from - to).length() / sphere.radius;
		//quaternion turn
		Quaternion turn = Quaternion::fromAxisAngle(cross, alpha);
		//update rotation
		protation.setRotation(start.mul(turn));
	}
}
