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
#include "TrackballMaterial.h"

namespace Easy3D{

	class Trackball :public Object,
					 public Input::MouseHandler
	{
		
		TrackballMaterial *material{ nullptr };
		Mesh trackball3D;

	public:

		void init(TrackballMaterial *material);
		virtual ~Trackball();
		void draw(Camera& camera);
	};

};


#endif