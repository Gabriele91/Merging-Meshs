#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include <Screen.h>
#include "Geometry.h"
#include "Camera.h"

using namespace Easy3D;
void Geometry::init(GeometryMaterial* gm){
	///////////////////////////
	material=gm;
	///////////////////////////
	//add relative pivot
	addChild(&relative, false);	
	///////////////////////////
}
Geometry::~Geometry(){}

void Geometry::setMesh(Mesh* mesh){
	geometry = mesh;
    //calc offset
	auto bsize = geometry->getBox().getSize();
	auto minsize = Math::min(bsize.x, bsize.y, bsize.z);
	Vec3 factor = Vec3::ONE / minsize ;
	//calc offset
	relative.setScale(factor);
	//calc offset
	auto bcenter = geometry->getBox().getCenter();
	relative.setPosition(-bcenter * relative.getScale());
}
GeometryMaterial* Geometry::getMaterial() const{
	return material;
}
Mat4 Geometry::getModelMatrix(){
	return relative.getGlobalMatrix().mul(svdMatrix);
}
Object* Geometry::getRelative(){
	return &relative;
}

void Geometry::draw(Camera& camera){
	//render
	DEBUG_ASSERT(geometry);
	//update material
	material->setCamera(&camera);
	material->setObject(&relative);
    material->setSVDMatrix(&svdMatrix);
	material->setLightDir(lDir);
	material->setLightDiffuse(lDiffuse);
	//draw
	material->draw(*geometry);
}