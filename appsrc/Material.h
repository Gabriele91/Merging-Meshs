#ifndef MATERIAL_H
#define MATERIAL_H

#include <Config.h>
#include "Mesh.h"

namespace Easy3D {

	class Material{

	public:
		virtual ~Material(){}
		virtual int  id() = 0;
		virtual void init() = 0;
		virtual void draw(const Mesh& m) = 0;

	};

};

#endif