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


Vec3 Camera::getPointFromDepth(const Vec2& point){
	Render& r = *Application::instance()->getRender();
#if 1
	float distance;
	if (r.getRenderDriver() == OPENGL_DRIVER)
		distance = projection[14] / (r.getDepth(point) * -2.0 + 1.0 - projection[10]) *-1;
	else if (r.getRenderDriver() == DIRECTX_DRIVER)
		distance = projection[14] / (r.getDepth(point) + projection[10]);

	//window space to clip space
	Vec2 screen(viewport.z, viewport.w);
	Vec2 offset(viewport.x, viewport.y);
	Vec2 clip((2.0f * (point.x - offset.x)) / screen.x - 1.0f,	1.0f - (2.0f * (point.y - offset.y)) / screen.y);
	//pos3D
	auto ppos = projection.mul(Easy3D::Vector4D(clip, 1.0, 1.0));
	//inv
	Vec4 wpos = getGlobalMatrix().mul(Vec4(ppos.x, ppos.y, 1.0, 1.0));

	return wpos.xyz();
#else
	//window space to clip space
	
    Vec2 screen(viewport.z, viewport.w);
	Vec2 offset(viewport.x, viewport.y);
	Vec2 cpixel(screen*0.5 + offset);
    
    float distance;
	if (r.getRenderDriver() == OPENGL_DRIVER)
		distance = projection[14] / (r.getDepth(cpixel) * -2.0 + 1.0 - projection[10]) *-1;
	else if (r.getRenderDriver() == DIRECTX_DRIVER)
		distance = projection[14] / (r.getDepth(cpixel) + projection[10]);

    //n.b. getGlobalMatrix()==getGlobalView().getInverse()
	Vec3 zpos = getGlobalMatrix().mul(Vec4(0,0, -distance, 1.0)).xyz();

    return Vec3(zpos);
#endif
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