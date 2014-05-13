#ifndef CAMERA_H
#define CAMERA_H

#include <Config.h>
#include <Object.h>
#include <Render.h>


namespace Easy3D{

	class Camera : public Object {

		Vec4 viewport;
		Mat4 projection;
		Mat4 view;
		//save tmp info proj
		float tNear, tFar;
		//changed update matrix
		virtual const Matrix4x4& getGlobalMatrix();
		Vec3 unProjec(const Vec3& win);

	public:

		//
		Camera() :Object(){}
		//set prospetive
		void setViewport(const Vec4& viewport);
		void setViewport(const ViewportState& viewport);
		void setPerspective(float angle, float n, float f);
		void setPerspective(float angle, float spectre, float n, float f);
		void setPerspective(float left, float right, float bottom, float top, float n, float f);
		void setOrtogonal(float left, float right, float bottom, float top, float n, float f);
		//return matrix
		const Matrix4x4& getProjectionMatrix() const;
		const Matrix4x4& getViewMatrix();
		const Vec4& getViewport() const {
			return viewport;
		}
		//utility
		Vec2 getClipPointFrom3DSpace(const Vec3& point);
		Vec2 getScreenPointFrom3DSpace(const Vec3& point);
		Vec2 getViewPointFrom3DSpace(const Vec3& point);
		Vec3 getPointFromDepth(const Vec2& point);
		Vec3 getPointFrom2DClipSpace(const Vec2& point);
		Vec3 getPointFrom2DScreen(const Vec2& point);
		Vec3 getPointFrom2DView(const Vec2& point);
		Vec3 getNormalPointFrom2DClipSpace(const Vec2& point);
		Vec3 getNormalPointFrom2DScreen(const Vec2& point);
		Vec3 getNormalPointFrom2DView(const Vec2& point);
	};


};


#endif