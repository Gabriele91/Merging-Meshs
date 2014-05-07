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
inline void space_split(const char* str, std::vector<String>& out ){
	//define a space
	#define isspace(x) ((x) == ' '|| (x) == '\t')
	#define eatspaces(x,i) while (isspace(x[i])) ++i;
	#define eatnospaces(x,i) while ((!isspace(x[i])) && x[i]) ++i;
	//dec variables
	size_t s = 0, i = 0, iout = 0, size = 1; 
	out.resize(0);
	//jmp space
	eatspaces(str, i)
    //size of vector
	while (str[i]){
		//id is a space
		if (isspace(str[i])){
				//a new split
				++size;
				//jump space
				eatspaces(str, i)
		}
		++i;
    }
    out.resize(size);
	//restart
	i = 0;
	//jmp space
	eatspaces(str, i)
	//save first no space char
	s = i;
    //split
    while (str[i]) {
		if (isspace(str[i])){
			//copy string
            str_string(&str[s],i-s,out[iout]);
			//jump next space
			eatspaces(str, i)
			//next start
            s=i;
			//next string
            ++iout;
        }
		else
			 ++i;
    }
    //end case
    if (iout!=size){
        str_string(&str[s],i-s,out[iout]);
    }
	//undefs
	#undef isspace
	#undef eatspaces
	#undef eatnospaces
}


Easy3D::Vec3& Mesh::offV(size_t i){
	return *((Easy3D::Vec3*)&vertexs[vSize*i]);
}
Easy3D::Vec3& Mesh::offN(size_t i){
	return *((Easy3D::Vec3*)&vertexs[vSize*i + sizeof(float)* 3]);
}
void Mesh::offNormalize(){
	size_t nsize = sizeVertexs();
	for (size_t i = 0; i != nsize; ++i){
		offN(i).normalize();
    }
}
void Mesh::offComputeNormals(){
	size_t nsize = sizeIndexs();
	for (size_t i = 0; i != nsize; i += 3){
		auto v1 = offV(indexs[i+1]) - offV(indexs[i]);
		auto v2 = offV(indexs[i+2]) - offV(indexs[i]);
		auto norm = (v1.cross(v2)).getNormalize();

		offN(indexs[i])   += norm;
		offN(indexs[i+1]) += norm;
		offN(indexs[i+2]) += norm;
    }
    //normalize all normals
	offNormalize();
}

Easy3D::Vec3 Mesh::offFaceNormal(size_t i){
	auto v1 = offV(indexs[i + 1]) - offV(indexs[i]);
	auto v2 = offV(indexs[i + 2]) - offV(indexs[i]);
	return (v1.cross(v2)).getNormalize();
}
Easy3D::Vec3 Mesh::offFaceNormal(size_t v0, size_t v1, size_t v2){
	auto dv1 = offV(indexs[v1]) - offV(indexs[v0]);
	auto dv2 = offV(indexs[v2]) - offV(indexs[v0]);
	return (dv1.cross(dv2)).getNormalize();
}
float Mesh::offFaceArea(size_t i){
	auto v1 = offV(indexs[i + 1]) - offV(indexs[i]);
	auto v2 = offV(indexs[i + 2]) - offV(indexs[i]);
	auto v3 = v1.cross(v2);
	return 0.5f*std::sqrt(v3.x*v3.x + v3.y*v3.y + v3.z*v3.z);
}
inline float angleVectors(const Vec3& a,const Vec3& b){
	return std::acos( a.dot(b) / (a.length()*b.length()) );
}
float Mesh::offFaceAngle(size_t v0, size_t v1, size_t v2){
	return angleVectors(offV(indexs[v0]) - offV(indexs[v1]),
						offV(indexs[v0]) - offV(indexs[v2]));
}
void Mesh::offSlowComputeNormals(){
	//calc size
	size_t nsize = sizeIndexs();
	size_t adj = 0;
	size_t av = 0;
	// A as a face of Mesh
	//#pragma omp for
	for (size_t a = 0; a != nsize; a += 3){
		//for all vertex of A
		for (size_t v = 0; v != 3; ++v){
			//adjacency
			adj = 0;
			//vertex
			av = a + v;
			//for all face of mesh (exclude A)
			for (size_t b = 0; b != nsize; b += 3){
				//A!=B
				if (b == a) continue;
				//
				#if 0
				if (indexs[av] == indexs[b]){
					++adj;
					offN(indexs[av]) += offFaceNormal(b, b + 1, b + 2);
				}
				if (indexs[av] == indexs[b + 1]){
					++adj;
					offN(indexs[av]) += offFaceNormal(b + 1, b, b + 2);
				}
				if (indexs[av] == indexs[b + 2]){
					++adj;
					offN(indexs[av]) += offFaceNormal(b + 2, b, b + 1);
				}
				#elif 1
				auto bNA = offFaceNormal(b) *offFaceArea(b);
				if (indexs[av] == indexs[b]){
					++adj;
					offN(indexs[av]) += bNA * offFaceAngle(b, b + 1, b + 2);
				}
				if (indexs[av] == indexs[b + 1]){
					++adj;
					offN(indexs[av]) += bNA * offFaceAngle(b + 1, b, b + 2);
				}
				if (indexs[av] == indexs[b + 2]){
					++adj;
					offN(indexs[av]) += bNA * offFaceAngle(b + 2, b, b + 1);
				}
				#elif 0
				if (indexs[av] == indexs[b]){
					++adj;
					offN(indexs[av]) += offFaceNormal(b, b + 1, b + 2) * offFaceArea(b) * offFaceAngle(b, b + 1, b + 2);
				}
				if (indexs[av] == indexs[b + 1]){
					++adj;
					offN(indexs[av]) += offFaceNormal(b + 1, b, b + 2) * offFaceArea(b) * offFaceAngle(b + 1, b, b + 2);
				}
				if (indexs[av] == indexs[b + 2]){
					++adj;
					offN(indexs[av]) += offFaceNormal(b + 2, b, b + 1) * offFaceArea(b) * offFaceAngle(b + 2, b, b + 1);
				}
				#else
				if (indexs[av] == indexs[b] || 
					indexs[av] == indexs[b + 1] ||
					indexs[av] == indexs[b + 2]){
					++adj;
					offN(indexs[av]) += offFaceNormal(b);
				}
				#endif
			}
			//normalize
			if (adj)
				offN(indexs[av]).normalize();
		}
	}
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
		space_split(lines[i], values);
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
	if (normals == Mesh::OFF_VERTEX_NORMALS)
		offComputeNormals();
	else if (normals == Mesh::OFF_VERTEX_NORMALS_SLOW)
		offSlowComputeNormals();
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
size_t Mesh::sizeVertexs(){
	return (Math::multipleOfX(currentVertex, vSize) / vSize);
}
size_t Mesh::sizeIndexs(){
	return currentIndex;
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

//add a index
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
	sBVertex = (uint)sizeVertexs();
	sBIndex = (uint)sizeIndexs();
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