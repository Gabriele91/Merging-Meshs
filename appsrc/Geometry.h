#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <Config.h>
#include <Object.h>
#include <Render.h>
#include <Utility.h>
#include <Shader.h>
#include "Camera.h"
#include "Mesh.h"



namespace Easy3D{

	class Geometry : public Object{
		
		Object relative;
		Mesh* geometry{ nullptr };

		Shader*	shader{ nullptr };
		BaseInputLayout* il{ nullptr };
		//model
		CMat4::ptr model{ nullptr };
		CMat4::ptr view{ nullptr };
		CMat4::ptr proj{ nullptr };
		//light
		CVec3::ptr lightDir{ nullptr };
		CVec4::ptr lightDiffuse{ nullptr };
		Vec3 lDir{Vec3(0.0f,0.0f,1.0f)};
		Vec4 lDiffuse{Vec4::ONE};

	public:

		virtual ~Geometry();
		void init();
		void setMesh(Mesh* mesh);
		Mesh* getMesh(){ return geometry; }
		void draw(Camera& camera);

	};
};

#endif