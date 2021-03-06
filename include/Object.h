#ifndef OBJECT_H
#define OBJECT_H

#include <Config.h>
#include <Math3D.h>

namespace Easy3D{
	
	class Object
	{
	public:
		
		enum ParentMode{
			DISABLE_PARENT=0,
			ENABLE_PARENT=1,   //1
			ENABLE_SCALE=2,	   //10
			ENABLE_ALL=3,	   //11
		};


		Object();
		virtual ~Object();
		//methods
		void setScale(const Vector3D &scale,bool global=false);
		void setPosition(const Vector3D &position,bool global=false);
		void setRotation(const Quaternion& rotation, bool global = false);
		void setMove(const Vector3D &move, bool global = false);
		//inc
		void setTranslation(const Vector3D &translation);
		void setTurn(const Quaternion& rotation);
		//childs
		void addChild(Object *child, bool hard = true);
		void addChild(Object *child, ParentMode type, bool hard = true);
		void erseChild(Object *child);
        void clearListChilds()
        {
            //dt all childs
            for(auto& child:childs)
            {
                child->parent=nullptr;
                child->change();
            }
            //clear map
            childs.clear();
        }
		void changeParentMode(ParentMode type);
		//
		Vector3D getScale(bool global=false);
		Vector3D getPosition(bool global=false);
		Quaternion getRotation(bool global=false);
		ParentMode getParentMode() const;
        //
        void copyLocalTransform(const Object& local);
		//math
		void change();
		bool isChange(){ return changeValue; }
		virtual const Matrix4x4& getGlobalMatrix();
		//for each methods
		std::list<Object*>::iterator begin();
		std::list<Object*>::iterator end();
		std::list<Object*>::reverse_iterator rbegin();
		std::list<Object*>::reverse_iterator rend();
		//data
		void *data;
		        
	private:
        
		struct Transform3D{
			//
			Transform3D():scale(1.0f,1.0f,1.0f){}
			//values
			Vector3D position;
			Quaternion rotation;
			Vector3D scale;
			//cast
			operator float*() { return &position.x; }
			operator const float*() const { return &position.x; }
			//calc matrix
			Mat4 getMatrix();
			Mat4 getMatrixRT();
		};
		//local
		Transform3D transform;
		//global
        Matrix4x4 globalMat;
		bool changeValue;
		//childs
		std::list<Object*> childs;
		//parent
		ParentMode parentMode;
		Object* parent;
		bool deleteHard;
		Vector3D   getGlobalParentScale();
		//
	};



};

#endif