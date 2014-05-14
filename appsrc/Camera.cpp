#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include <Screen.h>
#include "Camera.h"

using namespace Easy3D;

void Camera::setViewport(const Vec4& viewport){
	this->viewport = viewport;
}
void Camera::setViewport(const ViewportState& viewport){
	this->viewport = viewport.viewport;
}
void Camera::setPerspective(float angle, float n, float f){
	//set Perspective
	setPerspective(angle, viewport.z / viewport.w, n, f);
}
void Camera::setPerspective(float angle, float spectre, float n, float f){
	//save near and far
	tNear = n;
	tFar = f;
	//set Perspective
	projection = Application::instance()->getRender()->calculatesProjection(angle, spectre, n, f);
}
void Camera::setPerspective(float left, float right, float bottom, float top, float n, float f){
	//save near and far
	tNear = n;
	tFar = f;
	//set Perspective
	projection = Application::instance()->getRender()->calculatesProjection(left, right, bottom, top, n, f);
}
void Camera::setOrtogonal(float left, float right, float bottom, float top, float n, float f){
	//save near and far
	tNear = n;
	tFar = f;
	//set Perspective
	projection = Application::instance()->getRender()->calculatesOrto(left, right, bottom, top, n, f);
}
const Matrix4x4& Camera::getGlobalMatrix(){
	if (isChange())
		view = this->Object::getGlobalMatrix().getInverse();
	return Object::getGlobalMatrix();
}
const Matrix4x4& Camera::getProjectionMatrix() const {
	return projection;
}
const Matrix4x4& Camera::getViewMatrix() {
	getGlobalMatrix();
	return view;
}

Vec3 Camera::unproject(const Vec3& win) const{
	//compute inverse
	Mat4 inverse = (projection.mul(view)).getInverse();
	//compute x
	Vec4 tmp(win, 1.0);
	tmp.x = (tmp.x - (viewport.x)) / (viewport.z);
	tmp.y = (tmp.y - (viewport.y)) / (viewport.w);
	tmp = tmp * 2.0 - 1.0;
	Vec4 obj = inverse.mul(tmp);
	//in the and
	obj/=obj.w;
	//
	return obj.xyz();
}
Vec3 Camera::picking(const Vec2& win){
	//calc direction
	Render& r = *Application::instance()->getRender();
	//calc direction
	Vec3 dir=direction(win);
	//distance
	float distance = 0;
	if (r.getRenderDriver() == OPENGL_DRIVER)
		distance = projection[14] / (r.getDepth(invScreenY(win)) * -2.0 + 1.0 - projection[10]) *-1;
	else if (r.getRenderDriver() == DIRECTX_DRIVER)
		distance = projection[14] / (r.getDepth(win) + projection[10]);
	//endpoint
	Vec3 endpos = getPosition() - dir*distance;

	return endpos;
}
Vec3 Camera::direction(const Vec2& win){
	//calc direction
	Render& r = *Application::instance()->getRender();
	//inverse scree Y
	Vec2 point(invScreenY(win));
	//ray
	Vec3 start = unproject(Vec3(point, 0.0));
	Vec3 end = unproject(Vec3(point, 1.0));
	//direction
	Vec3 dir(start - end);
	dir.normalize();
	//return
	return dir;
}
Vec2 Camera::invScreenX(const Vec2& screen) const{
	//size screen
	float screenW = Application::instance()->getScreen()->getWidth();
	//inverse scree Y
	return Vec2(screenW - screen.x,screen.y);
}
Vec2 Camera::invScreenY(const Vec2& screen) const{
	//size screen
	float screenH = Application::instance()->getScreen()->getHeight();
	//inverse scree Y
	return Vec2(screen.x, screenH - screen.y);
}

Vec2 Camera::toClip(const Vec2& point) const{
	Vec2 tmp;
	tmp.x = (point.x - (viewport.x)) / (viewport.z);
	tmp.y = (point.y - (viewport.y)) / (viewport.w);
	return tmp;
}