#ifndef POINTSCLOUD_H
#define POINTSCLOUD_H

#include <Math3D.h>
#include "Mesh.h"
#include "PointsMaterial.h"

namespace Easy3D{
	struct PointsCloud{

		//graphics
		Object* object{ nullptr };
		Mesh* geometry{ nullptr };
		PointsMaterial* material{ nullptr };
		void buildMesh();
		//cpu info
		std::list<Vec3> points;
		bool ischange{true};

	public:

		void init(PointsMaterial *material);
		void setObject(Object* obj);
		void draw(Camera& camera);
		virtual ~PointsCloud();

		void addPoint(const Vec3& newv3);
		const std::list<Vec3>& getPoints() const{
			return points;
		}

		Mat4 calcSVD(const PointsCloud& target);

	};
};


#endif