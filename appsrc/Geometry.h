#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <Config.h>
#include <Object.h>
#include <Render.h>
#include <Utility.h>
#include <Shader.h>
#include "Camera.h"
#include "Mesh.h"
#include "GeometryMaterial.h"



namespace Easy3D{

	class Geometry : public Object{
		
		Object relative;
		Mesh* geometry{ nullptr };
		//
		GeometryMaterial* material{ nullptr };
		//
		Vec3 lDir{Vec3(0.0f,0.0f,1.0f)};
		Vec4 lDiffuse{Vec4::ONE};

	public:

		virtual ~Geometry();
		void init(GeometryMaterial* gm);
		void setMesh(Mesh* mesh);
		Mesh* getMesh(){ return geometry; }
		void draw(Camera& camera);
		void changeMaterial(GeometryMaterial* gm);

	};
};

#endif