#ifndef GEOMETRYMATERIAL_H
#define GEOMETRYMATERIAL_H

#include <Config.h>
#include <Render.h>
#include <Shader.h>
#include <Math3D.h>
#include "Material.h"


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

	public:

		virtual ~GeometryMaterial();
		virtual int  id();
		virtual void init();
		virtual void draw(const Mesh& m);
		virtual void bind();
		virtual void unbind();

		DFORCEINLINE void setModel(const Mat4& m){
			model->setValue(m);
		}
		DFORCEINLINE void setView(const Mat4& v){
			view->setValue(v);
		}
		DFORCEINLINE void setProj(const Mat4& p){
			proj->setValue(p);
		}
		DFORCEINLINE void setLightDir(const Vec3& d){
			lightDir->setValue(d);
		}
		DFORCEINLINE void setLightDiffuse(const Vec4& c){
			lightDiffuse->setValue(c);
		}
	};
};

#endif