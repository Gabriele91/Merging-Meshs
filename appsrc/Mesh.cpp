#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "Mesh.h"

using namespace Easy3D;
#define VBO_N_PAGE  256
#define IBO_N_PAGE  256
//utility

size_t Mesh::attSize(uchar type){
    switch (type) {
        case VertexField::UV:
        case VertexField::POSITION2D:
            return sizeof(float)*2;
        break;
        case VertexField::NORMAL:
        case VertexField::POSITION3D:
            return sizeof(float)*3;
        break;
        case VertexField::COLOR:
            return sizeof(float)*4;
        break;
        default:
            return 0;
        break;
    }
}

void Mesh::calcVertexSize(uchar type){
    vSize=0;
    vSize+=attSize(type & VertexField::POSITION2D);
    vSize+=attSize(type & VertexField::POSITION3D);
    vSize+=attSize(type & VertexField::NORMAL);
    vSize+=attSize(type & VertexField::UV);
    vSize+=attSize(type & VertexField::COLOR);
}
void Mesh::addFild(const float* b, size_t size){
	addVertexCPage(sizeof(float)*size);
	Math::memcpy(&vertexs[currentVertex], (const byte*)b, sizeof(float)*size);	
	currentVertex += sizeof(float)*size;
}

void Mesh::addVertexCPage(size_t next){
	while ( (currentVertex+next) >=  vertexs.size() ){
		vertexs.resize(vertexs.size() + VBO_N_PAGE*vSize);
	}
}
void Mesh::addIndexCPage(){
	if ( currentIndex == indexs.size() ){
		indexs.resize(indexs.size() + IBO_N_PAGE);
	}
}
//begin create mash
void Mesh::format(uchar type, size_t vsize, size_t isize){
	calcVertexSize(type);
	vertexs = std::vector<byte> (vSize*(vsize != 0 ? vsize : VBO_N_PAGE));
	indexs = std::vector<uint> (isize != 0 ? isize : IBO_N_PAGE);
	currentVertex = 0;
	currentIndex = 0;
}
//like opengl 1.4
void Mesh::vertex(const Vec2& vt){
    addFild(vt,2);
}
void Mesh::vertex(const Vec3& vt){
    addFild(vt,3);
}
void Mesh::normal(const Vec3& normal){
    addFild(normal,3);
}
void Mesh::color(const Vec4& color){
    addFild(color,4);
}
void Mesh::color(const Color& ucolor){
    color(ucolor.argbNormalize());
}
void Mesh::uv(const Vec2& uv){
    addFild(uv,2);
}

//like OpenGL 2.X, 3.X, 4.x
void Mesh::vbuffer(const Easy3D::byte* b){
	Math::memcpy(&vertexs[0], b, vertexs.size());
	currentVertex = vertexs.size();
}
void Mesh::ibuffer(const Easy3D::uint* b){
	Math::memcpy((Easy3D::byte*)&indexs[0], (Easy3D::byte*)b, indexs.size()*sizeof(uint));
	currentIndex = indexs.size();
}

//
void Mesh::index(uint i){
	addIndexCPage();
	indexs[currentIndex] = i;
	++currentIndex;
}


//bind
bool Mesh::bind(bool force){
	Render& r = *Application::instance()->getRender();
	//vertex buffer
	DEBUG_ASSERT(currentVertex);
	bVertex=r.createVBO(&vertexs[0], vSize, vertexs.size() / vSize);
	//index buffer
	if(currentIndex)
		bIndex = r.createIBO(&indexs[0], indexs.size());
    //sizes
    sBVertex=(uint)(vertexs.size()/vSize);
    sBIndex=(uint)(indexs.size());
	//delete cpu info
	if (force){
		vertexs = std::vector<byte>(0);
		indexs = std::vector<uint>(0);
	}
	//return 
	return bVertex != NULL;
}

void Mesh::mode(TypeDraw m){
	dMode = m;
}

void Mesh::draw(){
	Render& r = *Application::instance()->getRender();
	r.bindVBO(bVertex);
	if (bIndex){
		r.bindIBO(bIndex);
		r.drawElements(dMode, sBIndex);
	}
	else{
		r.drawArrays(dMode, sBVertex);
	}
}