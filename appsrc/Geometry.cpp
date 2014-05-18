#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include <Screen.h>
#include "Geometry.h"
#include "Camera.h"

using namespace Easy3D;
Geometry::~Geometry(){}
void Geometry::init(GeometryMaterial* gm){
	///////////////////////////
	changeMaterial(gm);
	///////////////////////////
	//add relative pivot
	addChild(&relative, false);	
	///////////////////////////
}
void Geometry::changeMaterial(GeometryMaterial* gm){
	material = gm;
}
void Geometry::setMesh(Mesh* mesh){
	geometry = mesh;
    //calc offset
	auto bsize = geometry->getBox().getSize();
	auto minsize = Math::min(bsize.x, bsize.y, bsize.z);
	Vec3 factor = Vec3::ONE / minsize ;
	//calc offset
	relative.setScale(factor);
}
void Geometry::draw(Camera& camera){
	//render
	DEBUG_ASSERT(geometry);
	//calc offset
	auto bcenter = geometry->getBox().getCenter();
	relative.setPosition(-bcenter * relative.getScale());
	//draw
	material->bind();

	material->setProj(camera.getProjectionMatrix());
	material->setView(camera.getViewMatrix());
	material->setModel(relative.getGlobalMatrix());
	material->setLightDir(lDir);
	material->setLightDiffuse(lDiffuse);

	material->draw(*geometry);

	material->unbind();
}