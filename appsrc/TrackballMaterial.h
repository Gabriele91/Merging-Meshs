#ifndef TRACKBALLMATERIAL_H
#define TRACKBALLMATERIAL_H

#include <Config.h>
#include <Shader.h>
#include <Math3D.h>
#include "Material.h"

namespace Easy3D {

	class TrackballMaterial : public Material {

		//shader input
		Shader*	shader{ nullptr };
		BaseInputLayout* il{ nullptr };
		CMat4::ptr model{ nullptr };
		CMat4::ptr view{ nullptr };
		CMat4::ptr proj{ nullptr };
		//draw context
		CullFaceState  ctxCFaces;
		BlendState     ctxBlend;

	public:

		virtual ~TrackballMaterial();
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

	};

};

#endif