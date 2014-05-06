#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Eigen/Eigen>


inline static Eigen::MatrixXf centroidToZero(const std::vector<Easy3D::Vec3>& pvect, Easy3D::Vec3& center){
	//calc center
	center=Easy3D::Vec3::ZERO;
	for (const auto& p : pvect){
		center += p;
	}
	center /= pvect.size();
	//ouput
	Eigen::MatrixXf v(pvect.size(),3);
	size_t row = 0;
	//translate
	for (auto& p : pvect){
		v.row(row) = Eigen::Vector3f((const float*)(p - center));
		++row;
	}
	return v;
}

extern Easy3D::Mat4 computeSVD(const std::vector<Easy3D::Vec3>& p,const std::vector<Easy3D::Vec3>& q){

	Easy3D::Vec3 centroidP, centroidQ;

	//Easy3D::DEBUG_ASSERT(p.size() == q.size());

	Eigen::MatrixXf v_0=centroidToZero(p, centroidP); // is V^T  = J
	Eigen::MatrixXf v_1=centroidToZero(q, centroidQ); // is V'^T = J'
	
	//C=VV'^T -> C=J^T * J'
	Eigen::MatrixXf covariance = v_0.transpose()  * v_1;

	//calc svd 	//Eigen::ComputeFullU | Eigen::ComputeFullV ??
	auto csvd = covariance.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
	//calc R=U_l*U_r^T
	Eigen::Matrix3f eigenR = csvd.matrixU() * csvd.matrixV().transpose();
    
    //calc t= p - R*q
    Eigen::Vector3f cp((const float*)centroidP);
    Eigen::Vector3f cq((const float*)centroidQ);
    Eigen::Vector3f t(cp-eigenR*cq);
    
    //eigenR to R+Translate
    Easy3D::Mat4 rotoTranslate;
    rotoTranslate.setRotMatrix(eigenR.data());
    rotoTranslate.transpose(); //column major to row major
    rotoTranslate.addTranslation((const float*)t.data());
    
    return rotoTranslate;

}