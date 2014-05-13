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

//get a projectate point
Vec2 Camera::getClipPointFrom3DSpace(const Vec3& point){

	Vec4 vpp(point, 1.0);
	vpp = view.mul(vpp);
	vpp = projection.mul(vpp);
	if (vpp.w == 0) return Vec2::ZERO;

	vpp.x /= vpp.w;
	vpp.y /= vpp.w;
	vpp.z /= vpp.w;

	/* Map x, y and z to range 0-1 */
	vpp.x = (vpp.x + 1.0f) * 0.5f;
	vpp.y = (vpp.y + 1.0f) * 0.5f;
	vpp.z = (vpp.z + 1.0f) * 0.5f;

	return vpp.xy();
}
Vec2 Camera::getScreenPointFrom3DSpace(const Vec3& point){
	//get from clip space
	Vec2 vpp = getClipPointFrom3DSpace(point);
	//clip to viewport
	Vec2 screen(Application::instance()->getScreen()->getSize());
	Vec2 pscreen = vpp*screen;
	//flip y
	return Vec2(pscreen.x, screen.y - pscreen.y);
}
Vec2 Camera::getViewPointFrom3DSpace(const Vec3& point){
	//get from clip space
	Vec2 vpp = getClipPointFrom3DSpace(point);
	//clip to viewport
	Vec2 screen(viewport.z,viewport.w);
	Vec2 pscreen = vpp*screen + viewport.xy();
	//flip y
	return Vec2(pscreen.x, screen.y - pscreen.y);
}


Vec3 Camera::unProjec(const Vec3& win){
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
	return obj;
}

Vec3 Camera::getPointFromDepth(const Vec2& win){

	Render& r = *Application::instance()->getRender();
	//window space to clip space
	//size screen
	float screenH = Application::instance()->getScreen()->getHeight();
	//inverse scree Y
	Vec2 point(win.x, screenH - win.y);
	//ray
	Vec3 start = unProjec(Vec3(point, 0.0));
	Vec3 end = unProjec(Vec3(point, 1.0));
	//direction
	Vec3 dir(start - end);
	dir.normalize();
	//zdistance
	float distance = projection[14] / (r.getDepth(point) * -2.0 + 1.0 - projection[10]) *-1;
	//endpoint
	Vec3 endpos = getPosition() - dir*distance;

	return endpos;
}

Vec3 Camera::getPointFrom2DClipSpace(const Vec2& point) {
	//clip to projection
	Vec2 pProj = projection.getInverse().mul(Easy3D::Vector4D(point, 0.0, 1.0)).xy();
	//peojection to world (n.b. getGlobalMatrix()==getGlobalView().getInverse())
	Vec3 world = getGlobalMatrix().mul(Vec4(pProj, 0.0, 1.0)).xyz();
	//return
	return world;
}
Vec3 Camera::getPointFrom2DScreen(const Vec2& point) {
	//window space to clip space
	Vec2 screen(Application::instance()->getScreen()->getSize());

	Vec2 clipPoint((2.0f * point.x) / screen.x - 1.0f,
		1.0f - (2.0f * point.y) / screen.y);

	//calc point in 3D wolrd
	return getPointFrom2DClipSpace(clipPoint);
}
Vec3 Camera::getPointFrom2DView(const Vec2& point) {
	//window space to clip space
	Vec2 screen(viewport.z, viewport.w);
	Vec2 offset(viewport.x, viewport.y);

	Vec2 clipPoint((2.0f * (point.x - offset.x)) / screen.x - 1.0f,
				    1.0f - (2.0f * (point.y - offset.y)) / screen.y);

	//calc point in 3D wolrd
	return getPointFrom2DClipSpace(clipPoint);
}

Vec3 Camera::getNormalPointFrom2DClipSpace(const Vec2& point){
	//clip to projection
	Vec2 pProj = projection.getInverse().mul(Easy3D::Vector4D(point, 0.0, 1.0)).xy();
	//peojection to world (n.b. getGlobalMatrix()==getGlobalView().getInverse())
	Vec3 world = getGlobalMatrix().mul(Vec4(pProj, -1.0, 0.0)).xyz();
	//return
	return world;
}
Vec3 Camera::getNormalPointFrom2DScreen(const Vec2& point){
	//window space to clip space
	Vec2 screen(Application::instance()->getScreen()->getSize());
	Vec2 clipPoint((2.0f * point.x) / screen.x - 1.0f,
		1.0f - (2.0f * point.y) / screen.y);
	//calc point in 3D wolrd
	return getNormalPointFrom2DClipSpace(clipPoint);
}
Vec3 Camera::getNormalPointFrom2DView(const Vec2& point){
	//window space to clip space
	Vec2 screen(viewport.z, viewport.w);
	Vec2 offset(viewport.x, viewport.y);

	Vec2 clipPoint((2.0f * (point.x - offset.x)) / screen.x - 1.0f,
					1.0f - (2.0f * (point.y - offset.y)) / screen.y);
	//calc point in 3D wolrd
	return getNormalPointFrom2DClipSpace(clipPoint);
}