#ifndef POINTSMATERIAL_H
#define POINTSMATERIAL_H

#include <Config.h>
#include <Shader.h>
#include <Math3D.h>
#include <Object.h>
#include <Math3D.h>
#include "Camera.h"
#include "Material.h"

namespace Easy3D {


	class PointsMaterial : public Material {

		//shader input
		Shader*	shader{ nullptr };
		BaseInputLayout* il{ nullptr };
		CMat4::ptr model{ nullptr };
		CMat4::ptr view{ nullptr };
		CMat4::ptr proj{ nullptr };
		CVec2::ptr gsize{ nullptr };
		//uniform input
		Camera* camera{ nullptr };
		Object* object{ nullptr };
        Vec2    size;

	public:

		virtual ~PointsMaterial();
		virtual int  id();
		virtual void init();
		virtual void draw(const Mesh& m);

		DFORCEINLINE void setCamera(Camera* cam){
			camera = cam;
		}
		DFORCEINLINE void setObject(Object* obj){
			object = obj;
		}
		DFORCEINLINE void setSize(const Vec2& s){
			size = s;
		}

	};

};

#endif