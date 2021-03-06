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
#include "PointsCloud.h"

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
		void init(GeometryMaterial* gmap,PointsMaterial* pmat);
		virtual ~TrackArea();
		//draw mesh
		void draw();
		//set & get
		void addMesh(Mesh& obj);
        void removeMesh(const Mesh& obj);
		void setTrackball(Trackball& obj);
		void setViewport(const ViewportState& viewport);
		void setZoomVelocity(float zvelocity);
		void setZDistance(float zdistance);
        bool saveMeshs(const String&  path);
		Sphere sphere;	//set and get
        //add mesh by svd
        bool addMeshsSVD(TrackArea& in);
        //lock and unlock
        void lock();
        void unlock();
        bool isLocked(){
            return locked;
        }
        void resetPosition()
        {
            for(auto& geom:geometies)
                geom.setPosition(Vec3::ZERO);
        }

	
	protected:

		//geometry info
		GeometryMaterial*     matgeom{ nullptr };
		std::list<Geometry> geometies;
		//scene elements
		Object   protation;
		Camera	 camera;
		Trackball *trackball{ nullptr };
		//point cloud
		PointsCloud cldpoints;
		void addPoint(const Vec3& point);
		//zoom
		float scaleZoom{ 1.0 };
		float velocity{ 1.0 };
		//piking query
		Vec3 picking;
        //lock/unlock
        bool locked{ true };
		//trackball
		Quaternion start;
		Vec3 from;
		Vec3 to;
		Ray ray;
		Segment segment;

	};
};

#endif