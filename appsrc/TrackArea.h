#ifndef TRACKAREA_H
#define TRACKAREA_H

#include <Config.h>
#include <Object.h>
#include <Render.h>
#include <Input.h>
#include "Camera.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Trackball.h"

namespace Easy3D{

	class TrackArea : public Input::MouseHandler {

		void calcRay(const Vec2& mouse);
		void rayCast();
        bool inViewport(const Vec2& point);
		virtual void onMousePress(Vec2 mouse, Key::Mouse bt);
		virtual void onMouseDown(Vec2 mouse, Key::Mouse bt);
        virtual void onMouseScroll(short scroll);

	public:

		struct Sphere{
			Vec3  point;
			float radius{ 1.0 };
		};

		struct Ray{
			Vec3 origin;
			Vec3 dir;
		};

		struct Segment{
			Vec3 vnear;
			Vec3 vfar;
		};

		//init
		TrackArea();
		void init(GeometryMaterial* material);
		virtual ~TrackArea();
		//draw mesh
		void draw();
		//set & get
		void setMesh(Mesh& obj);
		void setTrackball(Trackball& obj);
		void setViewport(const ViewportState& viewport);
		void setZoomVelocity(float zvelocity);
		void setZDistance(float zdistance);
		Sphere sphere;	//set and get	

	
	protected:

		Object   protation;
		Geometry geometry;
		Camera	 camera;
		Trackball *trackball;

		//zoom
		float scaleZoom{ 1.0 };
		float velocity{ 1.0 };
		//piking query
		Vec3 picking;
		//trackball
		Quaternion start;
		Vec3 from;
		Vec3 to;
		Ray ray;
		Segment segment;

	};
};

#endif