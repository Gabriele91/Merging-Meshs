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
		Mat4 svdMatrix;
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
		void draw(Camera& camera);

		void setMesh(Mesh* mesh);
		Mesh* getMesh() const { return geometry; }
		GeometryMaterial* getMaterial() const;
		Mat4 getModelMatrix();
        void setSVDMatrix(const Mat4& mat)
        {
            svdMatrix=mat;
        }
        const Mat4& getSVDMatrix() const 
        {
            return svdMatrix;
        }
		Object* getRelative();

	};
};

#endif