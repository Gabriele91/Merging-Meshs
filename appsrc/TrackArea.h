#ifndef TRACKAREA_H
#define TRACKAREA_H

#include <Config.h>
#include <Object.h>
#include <Render.h>
#include <Input.h>
#include "Camera.h"

namespace Easy3D{

	class TrackArea : public Input::MouseHandler {

		Object *objToRot{ nullptr };
		Camera *camera{ nullptr };
		float radius{ 1.0 };
        float scaleZoom{ 1.0 };
        float velocity{ 1.0 };
		Quaternion start;
		Vec3 from;
		Vec3 to;

		Vec3 smap(const Vec2& mouse);
        bool inViewport(const Vec2& point);
		virtual void onMousePress(Vec2 mouse, Key::Mouse bt);
		virtual void onMouseDown(Vec2 mouse, Key::Mouse bt);
        virtual void onMouseScroll(short scroll);

	public:

		TrackArea();
		virtual ~TrackArea();
		void setCamera(Camera& obj);
		void attach(Object& obj);
		void setRadius(float radius);
		float getRadius();
		void setZoomVelocity(float zvelocity);

	};
};

#endif