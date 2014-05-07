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

inline void str_string(const char *in,size_t n,String& out){
    out.resize(n);
    Math::memcpy((Easy3D::byte*)&out[0],(Easy3D::byte*)in, n);
}
inline void fast_split(const char* str, std::vector<String>& out, char delimiter ){
    //dec variables
    out.resize(0);
    size_t s=0,i=0,iout=0,size=1;
    //size of vector
    while(str[i++]){
        if(str[i]==delimiter)
            ++size;
    }
    out.resize(size);
    i=0;
    //split
    while (str[i]) {
        if(str[i]==delimiter){
            str_string(&str[s],i-s,out[iout]);
            s=i+1;
            ++iout;
        }
        ++i;
    }
    //end case
    if (iout!=size){
        str_string(&str[s],i-s,out[iout]);
    }
}



Easy3D::Vec3 Mesh::__OFFGetV(size_t i){
    Easy3D::Vec3 out;
    Math::memcpy((Easy3D::byte*)&out.x,
                 (Easy3D::byte*)&vertexs[vSize*i],
                 sizeof(float)*3);
    return out;
}
Easy3D::Vec3 Mesh::__OFFGetN(size_t i){
    Easy3D::Vec3 out;
    Math::memcpy((Easy3D::byte*)&out.x,
                 (Easy3D::byte*)&vertexs[vSize*i+sizeof(float)*3],
                 sizeof(float)*3);
    return out;
}
void Mesh::__OFFSetN(size_t i,const Easy3D::Vec3& n){
    Math::memcpy((Easy3D::byte*)&vertexs[vSize*i+sizeof(float)*3],
                 (Easy3D::byte*)&n.x,
                 sizeof(float)*3);
}
void Mesh::__OFFSumN(size_t i,const Easy3D::Vec3& n){
    __OFFSetN(i,__OFFGetN(i)+n);
}
void Mesh::__OFFNormalizeNormals(){
    size_t nvertex= Math::multipleOfX(currentVertex, vSize) / vSize;
    for(size_t i=0;i!=nvertex;++i){
        Easy3D::Vec3 n=__OFFGetN(i);
        n.normalize();
        __OFFSetN(i,n);
    }
}
void Mesh::__OFFVNormals(){
    for(size_t i=0;i!=currentIndex;i+=3){
        auto p=__OFFGetV(indexs[i]);
        auto v1=p-__OFFGetV(indexs[i+1]);
        auto v2=p-__OFFGetV(indexs[i+2]);
        auto norm=v1.cross(v2);
        __OFFSumN(indexs[i],norm);
        __OFFSumN(indexs[i+1],norm);
        __OFFSumN(indexs[i+2],norm);
    }
    //normalize normals
    __OFFNormalizeNormals();
}


void Mesh::loadOFF(const Utility::Path& path,OFFCompute normals){
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
    //type mesh
    uchar type;                         type =Mesh::POSITION3D;
    if(normals!=Mesh::OFF_NO_NORMALS)   type|=Mesh::NORMAL;
    //
	size_t nvertex=0;
	size_t nface=0;
	size_t nindex=0;
	size_t iindex=0;
	//parser
	for (size_t i = 0; i != lines.size();++i){
		//get values
		values.resize(0);
		fast_split(lines[i], values,' ');
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
			format(type, nvertex, nface * 3);
			state = VERTEX;
			break;
        case VERTEX:
            //add vertex
            vertex(Vec3(values[0].toDouble(),
                        values[1].toDouble(),
                        values[2].toDouble()));
            //add normal
            if(normals!=Mesh::OFF_NO_NORMALS)
                normal(Vec3::ZERO);
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
    //normals
    if(normals!=Mesh::OFF_NO_NORMALS)
        __OFFVNormals();
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
    mBox=AABox();
}
//like opengl 1.4
void Mesh::vertex(const Vec2& vt){
    mBox.addPoint(Vec3(vt,0.0));
    addFild(vt,2);
}
void Mesh::vertex(const Vec3& vt){
    
    mBox.addPoint(vt);
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
	DEBUG_ASSERT(currentVertex);
    
    //sizes
    sBVertex=(uint)(Math::multipleOfX(currentVertex, vSize) / vSize);
    sBIndex=(uint)(currentIndex);
    //vertex buffer
	bVertex = r.createVBO(&vertexs[0], vSize, /* vertexs.size() / vSize */ sBVertex);
	//index buffer
	if(currentIndex)
		bIndex = r.createIBO(&indexs[0],/* indexs.size() */ sBIndex);

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