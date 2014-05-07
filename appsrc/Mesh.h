#ifndef MESH_H
#define MESH_H
#include <Config.h>
#include <Math3D.h>
#include <Color.h>
#include <Render.h>
#include <Utility.h>

namespace Easy3D{

    class Mesh{
    public:
        
        
        enum VertexField {
            POSITION2D=1,
            POSITION3D=2,
            COLOR     =4,
            NORMAL    =8,
            UV        =16
        };
        
        enum OFFCompute{
            OFF_NO_NORMALS,
            OFF_VERTEX_NORMALS
        };
        
		//load mesh from off file
		void loadOFF(const Utility::Path& path,OFFCompute normals=OFF_NO_NORMALS);
        
        //begin create mash
		void format(uchar type, size_t vsize=0, size_t isize=0);

        //like opengl 1.4
        void vertex(const Vec2& vt);
        void vertex(const Vec3& vt);
        void normal(const Vec3& normal);
        void color(const Vec4& color);
        void color(const Color& color);
        void uv(const Vec2& uv);

		//bind
		bool bind(bool deletecpuinfo=true);

		//like OpenGL 2.X, 3.X, 4.x
		void vbuffer(const byte* b);

		//like OpenGL 2.X, 3.X, 4.x
		void ibuffer(const uint* b);

		//index
		void index(uint i);

        //draw mode
        void mode(TypeDraw m);
       
        //draw element
        void draw();
        
        //get box
        const AABox& getBox(){ return  mBox; };
        
    private:
        
        //draw mode
        TypeDraw dMode;
        //burning box
        AABox mBox;
        //size of vertex
        size_t vSize;
        //cpu buffers
		size_t currentIndex{ 0 };
		std::vector<uint> indexs;
		size_t currentVertex{ 0 };
		std::vector<byte> vertexs;
        //gpu buffer
        uint sBVertex;
        BaseVertexBufferObject* bVertex{ NULL };
        uint sBIndex;
        BaseIndextBufferObject* bIndex{ NULL };
        //calc size
        static size_t attSize(uchar type);
		void calcVertexSize(uchar type);
		void addFild(const float* b, size_t size);
		void addVertexCPage(size_t next);
		void addIndexCPage();
        
        //loader utils
        Easy3D::Vec3 __OFFGetV(size_t i);
        Easy3D::Vec3 __OFFGetN(size_t i);
        void __OFFSetN(size_t i,const Easy3D::Vec3& n);
        void __OFFSumN(size_t i,const Easy3D::Vec3& n);
        void __OFFNormalizeNormals();
        void __OFFVNormals();

        
    };
    
}

#endif