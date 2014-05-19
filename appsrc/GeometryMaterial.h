#ifndef GEOMETRYMATERIAL_H
#define GEOMETRYMATERIAL_H

#include <Config.h>
#include <Object.h>
#include <Render.h>
#include <Shader.h>
#include <Math3D.h>
#include "Material.h"
#include "Camera.h"


namespace Easy3D {

	class GeometryMaterial : public Material {
		//shader input
		Shader*	shader{ nullptr };
		BaseInputLayout* il{ nullptr };
		//model
		CMat4::ptr model{ nullptr };
		CMat4::ptr view{ nullptr };
		CMat4::ptr proj{ nullptr };
		//light
		CVec3::ptr lightDir{ nullptr };
		CVec4::ptr lightDiffuse{ nullptr };
		//draw context
		CullFaceState  ctxCFaces;
		BlendState     ctxBlend;
		//uniform input
		Camera* camera;
		Object* object;
		Vec3 ldir;
		Vec4 ldiffuse;

	public:

		virtual ~GeometryMaterial();
		virtual int  id();
		virtual void init();
		virtual void draw(const Mesh& m);
		virtual void bind();
		virtual void unbind();

		DFORCEINLINE void setObject(Object* obj){
			object = obj;
		}
		DFORCEINLINE void setCamera(Camera* cam){
			camera = cam;
		}

		DFORCEINLINE void setLightDir(const Vec3& dir){
			ldir = dir;
		}
		DFORCEINLINE void setLightDiffuse(const Vec4& color){
			ldiffuse = color;
		}
	};
};

#endif