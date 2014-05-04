#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include "Mesh.h"

using namespace Easy3D;


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
void Mesh::addFild(const float* b,size_t size){
    Math::memcpy(&vertexs[0], (const byte*)b, sizeof(float)*size);
    currentVertex += size;
}

//begin create mash
void Mesh::begin(uchar type, size_t size){
    calcVertexSize(type);
    vertexs.resize(size);
    currentVertex=0;
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
void buffer(const byte* b, size_t size){
    Math::memcpy(&vertexs[0], b, size);
    currentVertex+=size;
}

//bind
bool end();