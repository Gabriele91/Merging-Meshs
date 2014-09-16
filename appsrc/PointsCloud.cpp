#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Eigen/Eigen>
#include "PointsCloud.h"
using namespace Easy3D;

inline static Eigen::MatrixXf centroidToZero(const std::list<Vec3>& pvect, Vec3& center){
	//calc center
	center = Easy3D::Vec3::ZERO;
	for (const auto& p : pvect){
		center += p;
	}
	center /= pvect.size();
	//ouput
	Eigen::MatrixXf v(pvect.size(), 3);
	size_t row = 0;
	//translate
	for (auto& p : pvect){
		v.row(row) = Eigen::Vector3f((const float*)(p - center));
		++row;
	}
	return v;
}


void PointsCloud::init(PointsMaterial *pmat){
	//init
	material = pmat;
	//init geometry
	geometry = new Mesh();
}
void PointsCloud::setObject(Object* obj){
	object=obj;
}

void PointsCloud::buildMesh(){

	//build mesh points (too slow)
	if (points.size()){
		geometry->format(Mesh::POSITION3D, points.size());
		for (auto& point : points)
			geometry->vertex(point);
		geometry->bind();
		geometry->mode(TypeDraw::DRAW_POINTS);
	}
	//update flag
	ischange = false;
}

void PointsCloud::draw(Camera& camera){
	if (!points.size()) return;
	if (ischange) buildMesh();
	material->setCamera(&camera);
	material->setObject(object);
    material->setSize({ 0.01f , 0.01f });
	material->draw(*geometry);
}
PointsCloud::~PointsCloud(){
	if (geometry)
		delete geometry;
}
void PointsCloud::addPoint(const Vec3& newv3){
	ischange = true;
	points.push_back(newv3);
}

Mat4 PointsCloud::calcSVD(const PointsCloud& target){
	Vec3 centroidP, centroidQ;

	Eigen::MatrixXf v_0 = centroidToZero(target.points, centroidP); // is V^T  = J
	Eigen::MatrixXf v_1 = centroidToZero(points, centroidQ);		// is V'^T = J'

	//C=VV'^T -> C=J^T * J'
	Eigen::MatrixXf covariance = v_0.transpose()  * v_1;

	//calc svd 	//Eigen::ComputeFullU | Eigen::ComputeFullV ??
	auto csvd = covariance.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
	//calc R=U_l*U_r^T
	Eigen::Matrix3f eigenR = csvd.matrixU() * csvd.matrixV().transpose();

	//calc t= p - R*q
	Eigen::Vector3f cp((const float*)centroidP);
	Eigen::Vector3f cq((const float*)centroidQ);
	Eigen::Vector3f t(cp - eigenR*cq);

	//eigenR to R+Translate
	Mat4 rotoTranslate;
	rotoTranslate.setRotMatrix(eigenR.data());
	rotoTranslate.transpose(); //column major to row major
	rotoTranslate.addTranslation((const float*)t.data());
	//return
	return rotoTranslate;
}
