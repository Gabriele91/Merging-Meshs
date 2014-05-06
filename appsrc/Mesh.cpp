#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "Mesh.h"

using namespace Easy3D;
#define VBO_N_PAGE  256
#define IBO_N_PAGE  256
//utility

DFORCEINLINE static void textFileRead(String& out,const Utility::Path &path) {
	/////////////////////////////////////////////////////////////////////
	//cpu load
	//get raw file
	void *data = NULL; size_t len = 0;
	Application::instance()->loadData(path, data, len);
	out.resize(len + 1);
	out = (char*)data;
	//free raw file
	free(data);
	/////////////////////////////////////////////////////////////////////
}

void Mesh::loadOFF(const Utility::Path& path){
	DEBUG_ASSERT(!bVertex);
	//load file
	String textfile;
	textFileRead(textfile, path);
	//unsafe split
	std::vector<char*> lines;
	String::unsafe_split(&textfile, lines, '\n');
	//size==0 return
	if (!lines.size()) return;
	//parser state
	enum POFFState{
		SHOFF,
		COUNT,
		VERTEX,
		FACE
	};
	std::vector<String> values;
	POFFState state = SHOFF;
	size_t nvertex;
	size_t nface;
	size_t nindex;
	size_t iindex;
	//parser
	for (size_t i = 0; i != lines.size();++i){
		//get values
		values.resize(0);
		String::split(lines[i], values,' ');
		if (!values.size()) return;
		//parser
		switch (state)
		{
		case SHOFF:
			if (values[0][0] == 'O'&&
				values[0][1] == 'F'&&
				values[0][2] == 'F')
				state = COUNT;
			break;
		case COUNT:
			nvertex = values[0].toInt(); DEBUG_ASSERT(nvertex != 0);
			nface = values[1].toInt(); DEBUG_ASSERT(nface != 0);
			format(Mesh::POSITION3D, nvertex, nface * 3);
			state = VERTEX;
			break;
		case VERTEX:
			//add vertex
			vertex(Vec3(values[0].toDouble(), 
					    values[1].toDouble(), 
					    values[2].toDouble()));
			//next vertex
			--nvertex;
			if (!nvertex)
				state = FACE;
			break;
		case FACE:
			//nindex
			nindex = values[0].toInt();
			DEBUG_ASSERT(nindex>=3);
			//next
			// TRIANGLE_FAN (to do POLYGON) TO TRIANGLES
			for (iindex = 1; iindex != (nindex - 1); ++iindex){
				index(values[1].toInt());
				index(values[iindex + 1].toInt());
				index(values[iindex + 2].toInt());
			}
			//next face
			--nface;
			if (!nface)
				i = lines.size() - 1;/* break for */
			break;
		default: 
			break;
		}
	}
	//draw mode
	mode(DRAW_TRIANGLES);
	//bind mesh
	bind();
}

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

inline static size_t multipleOfX(size_t size, size_t x) //constexpr
{
	return ((size / x) + ((size % x) != 0)) * x;
}
//bind
bool Mesh::bind(bool force){
	Render& r = *Application::instance()->getRender();
	//vertex buffer
	DEBUG_ASSERT(currentVertex);
	bVertex = r.createVBO(&vertexs[0], vSize, /* vertexs.size() / vSize */  
											  Math::multipleOfX(currentVertex, vSize) / vSize);
	//index buffer
	if(currentIndex)
		bIndex = r.createIBO(&indexs[0],/* indexs.size() */
										   currentIndex);
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