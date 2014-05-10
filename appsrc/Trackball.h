#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <Config.h>
#include <Math3D.h>
#include <Object.h>
#include <Screen.h>
#include <Input.h>
#include <Render.h>
#include <Shader.h>
#include "Mesh.h"
#include "Camera.h"

namespace Easy3D{

	class Trackball :public Object,
					 public Input::MouseHandler
	{
		
		Shader*	shader{ nullptr };
		BaseInputLayout* il{ nullptr };
		CMat4::ptr model{ nullptr };
		CMat4::ptr view{ nullptr };
		CMat4::ptr proj{ nullptr };
		Mesh trackball3D;

	public:

		Trackball();
		void init();
		virtual ~Trackball();
		void draw(Camera& camera);
	};

};


#endif