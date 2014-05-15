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
		Vec3 unproject(const Vec3& win) const;
		Vec2 invScreenX(const Vec2& screen) const;
		Vec2 invScreenY(const Vec2& screen) const;
		Vec3 picking(const Vec2& point);
		Vec3 direction(const Vec2& point);
		void ray(const Vec2& point, Vec3& origin, Vec3& dir);
		Vec2 toClip(const Vec2& point) const;
	};


};


#endif