#ifndef SHADER_H
#define SHADER_H

#include <Config.h>
#include <Math3D.h>
#include <Utility.h>
#include <Render.h>

namespace Easy3D{

	template <class T>
	class Uniform{
	public:
		typedef DS_PTR<T> ptr;
		ptr shared(){ return ptr((T*)this); }
		virtual ~Uniform(){};
	protected:
		virtual void  set(const void*) = 0;
		virtual void  set(const void*, size_t i,size_t n) = 0;
		virtual void* get() = 0;
		virtual const void* get() const = 0;
	};

	class CTexture : public Uniform<CTexture> {
		public:
			CTexture(){}

			void enable(BaseTexture* tex){
				this->set(tex, (size_t)0, (size_t)0);
			}
			void enable(BaseRenderTexture* tex){
				this->set(tex, (size_t)0, (size_t)1);
			}
			virtual void disable()=0;

			operator const BaseTexture*() const{
				return (const BaseTexture*)(this->get());
			}
			operator const BaseRenderTexture*() const{
				return (const BaseRenderTexture*)(this->get());
			}
	};

	class CInt : public Uniform<CInt> {
	public:
		CInt(int i){
			set(&i);
		}
		operator int() const{
			return *((int*)(this->get()));
		}
		void setValue(int i){
			set(&i);
		}
		int operator = (int i){
			set(&i);
			return i;
		}
	};
	class CFloat : public Uniform<CFloat> {
	public:
		CFloat(float f){
			set(&f);
		}
		operator float() const{
			return *((float*)(this->get()));
		}
		void setValue(float f){
			set(&f);
		}
		float operator = (float f){
			set(&f);
			return f;
		}
	};
	class CVec2 : public Uniform<CVec2> {
	public:
		CVec2(const Vec2& v2){
			set(&v2.x);
		}
		operator const Vec2&() const{
			return *((Vec2*)(this->get()));
		}
		void setValue(const Vec2& v2){
			set(&v2.x);
		}
		const Vec2& operator = (const Vec2& v2){
			set(&v2.x);
			return v2;
		}
	};
	class CVec3 : public Uniform<CVec3> {
	public:
		CVec3(const Vec3& v3){
			set(&v3.x);
		}
		operator const Vec3&() const{
			return *((Vec3*)(this->get()));
		}
		void setValue(const Vec3& v3){
			set(&v3.x);
		}
		const Vec3& operator = (const Vec3& v3){
			set(&v3.x);
			return v3;
		}
	};
	class CVec4 : public Uniform<CVec4> {
	public:
		CVec4(const Vec4& v4){
			set(&v4.x);
		}
		operator const Vec4&() const{
			return *((Vec4*)(this->get()));
		}
		void setValue(const Vec4& v4){
			set(&v4.x);
		}
		const Vec4& operator = (const Vec4& v4){
			set(&v4.x);
			return v4;
		}
	};
	class CMat4 : public Uniform<CMat4> {
	public:
		CMat4(const Mat4 & m4){
			set(&m4.m00);
		}
		operator const Mat4&() const{
			return *((Mat4*)(this->get()));
		}
		void setValue(const Mat4& m4){
			set(&m4.m00);
		}
		const Mat4& operator = (const Mat4& m4){
			set(&m4.m00);
			return m4;
		}
	};
	
	class CIntArray : public Uniform<CIntArray> {
	public:
		void set(int i, size_t n){
			Uniform::set(&i, n, 1);
		}
		void set(int* i, size_t n){
			Uniform::set(i, 0, n);
		}
		int get(size_t n) const{
			return ((const int*)Uniform::get())[n];
		}
	};
	class CFloatArray : public Uniform<CFloatArray>{
	public:
		void set(float f, size_t n){
			Uniform::set(&f, n, 1);
		}
		void set(float* f, size_t n){
			Uniform::set(f, 0, n);
		}
		float get(size_t n) const{
			return ((const float*)Uniform::get())[n];
		}
	};
	class CVec2Array : public Uniform<CVec2Array> {
	public:
		void set(const Vec2& v2, size_t n){
			Uniform::set(&v2, n, 1);
		}
		void set(Vec2* v2, size_t n){
			Uniform::set(v2, 0, n);
		}
		Vec2 get(size_t n) const{
			return ((const Vec2*)Uniform::get())[n];
		}
	};
	class CVec3Array : public Uniform<CVec3Array> {
	public:
		void set(const Vec3& v3, size_t n){
			Uniform::set(&v3, n, 1);
		}
		void set(Vec3* v3, size_t n){
			Uniform::set(v3, 0, n);
		}
		Vec3 get(size_t n) const{
			return ((const Vec3*)Uniform::get())[n];
		}
	};
	class CVec4Array : public Uniform<CVec4Array> {
	public:
		void set(const Vec4& v4, size_t n){
			Uniform::set(&v4, n, 1);
		}
		void set(Vec4* v4, size_t n){
			Uniform::set(v4, 0, n);
		}
		Vec4 get(size_t n) const{
			return ((const Vec4*)Uniform::get())[n];
		}
	};
	class CMat4Array : public Uniform<CMat4Array> {
	public:
		void set(const Mat4& v4, size_t n){
			Uniform::set(&v4, n, 1);
		}
		void set(Mat4* v4, size_t n){
			Uniform::set(v4, 0, n);
		}
		Mat4 get(size_t n) const{
			return ((const Mat4*)Uniform::get())[n];
		}
	};

	class Shader{

	public:

		Shader(){};
		virtual ~Shader(){};
		virtual void loadShader(const Utility::Path& vs, const Utility::Path& fs){ loadShader(vs, fs, {}); }
		virtual void loadShader(const Utility::Path& vs, const Utility::Path& fs, const Utility::Path& gs){ loadShader(vs, fs, gs,{}); }
		virtual void loadShader(const Utility::Path& vs, const Utility::Path& fs, const std::vector<String>& defines) = 0;
		virtual void loadShader(const Utility::Path& vs, const Utility::Path& fs, const Utility::Path& gs, const std::vector<String>& defines) = 0;


		//get consts
		virtual CTexture* getConstTexture(const char *name) = 0;
		virtual CInt* getConstInt(const char *name) = 0;
		virtual CFloat* getConstFloat(const char *name) = 0;
		virtual CVec2* getConstVec2(const char *name) = 0;
		virtual CVec3* getConstVec3(const char *name) = 0;
		virtual CVec4* getConstVec4(const char *name) = 0;
		virtual CMat4* getConstMat4(const char *name) = 0;
		virtual CIntArray* getConstIntArray(const char *name) = 0;
		virtual CFloatArray* getConstFloatArray(const char *name) = 0;
		virtual CVec2Array* getConstVec2Array(const char *name) = 0;
		virtual CVec3Array* getConstVec3Array(const char *name) = 0;
		virtual CVec4Array* getConstVec4Array(const char *name) = 0;
		virtual CMat4Array* getConstMat4Array(const char *name) = 0;
		
	protected:

		//imposta shader
		virtual void bind() = 0;
		virtual void unbind() = 0;
		virtual void uniform() = 0;

	};
};
#endif
