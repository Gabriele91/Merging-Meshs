#ifndef MESH_H
#define MESH_H
#include <Config.h>
#include <Math3D.h>
#include <Color.h>
#include <Render.h>

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
        
        
    public:
        
        
        
        //begin create mash
        void begin(uchar type, size_t size);

        //like opengl 1.4
        void vertex(const Vec2& vt);
        void vertex(const Vec3& vt);
        void normal(const Vec3& normal);
        void color(const Vec4& color);
        void color(const Color& color);
        void uv(const Vec2& uv);

		//bind
		bool end();

        //like OpenGL 2.X, 3.X, 4.x
        void buffer(const byte* b, size_t size);
        
		//index
		void index(uint i);

        //draw mode
        void mode(TypeDraw m);
       
        //draw element
        void draw();
        
    private:
        
        //draw mode
        TypeDraw dMode;
        //burning box
        AABox mBox;
        //size of vertex
        size_t vSize;
        //cpu buffer
        byte* currentVertex{NULL};
        std::vector<byte> vertexs;
        std::vector<uint> indexs;
        //gpu buffer
        BaseVertexBufferObject* bVertex;
        BaseIndextBufferObject* bIndex;
        //calc size
        static size_t attSize(uchar type);
        void calcVertexSize(uchar type);
        void addFild(const float* b,size_t size);
        
    };
    
}

#endif