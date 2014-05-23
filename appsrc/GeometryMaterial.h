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

		//shader shadow
		Shader*	shadowSH{ nullptr };
		BaseInputLayout* shadowIL{ nullptr };
		CMat4::ptr shadowM{ nullptr };
		CMat4::ptr shadowV{ nullptr };
		CMat4::ptr shadowP{ nullptr };
		//shadow texture/camera
		BaseRenderTexture* shadowMap;
		Camera shadowCam;
		//shadow
		void initShadow();
		void drawShadow(const Mesh& m);

		//shader color
		Shader*	colorSH{ nullptr };
		BaseInputLayout* colorIL{ nullptr };
		//camera 
		CMat4::ptr colorP{ nullptr };
		CMat4::ptr colorV{ nullptr };
		//camera  shadow
		CMat4::ptr colorSWP{ nullptr };
		CMat4::ptr colorSWV{ nullptr };
		//model
		CMat4::ptr colorM{ nullptr };
		//light
		CVec3::ptr lightDir{ nullptr };
		CVec4::ptr lightDiffuse{ nullptr };
		CTexture::ptr colorTEX{ nullptr };
		//shadow
		void initColor();
		void drawColor(const Mesh& m);

		//draw context
		CullFaceState  ctxCFaces;
		BlendState     ctxBlend;
		ViewportState  ctxViewport;

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

		DFORCEINLINE void setObject(Object* obj){
			object = obj;
		}
		DFORCEINLINE void setCamera(Camera* cam){
			camera = cam;
		}

		DFORCEINLINE Camera& getCamera(){
			return shadowCam;
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