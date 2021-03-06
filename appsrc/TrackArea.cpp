#include <stdafx.h>
#include <Math3D.h>
#include <Debug.h>
#include <Application.h>
#include "TrackArea.h"

using namespace Easy3D;


inline Quaternion pointTarget(const Vec3& target, const Vec3& point){
	Vec3 diff(target - point);
	float xzdist = std::sqrt(diff.x*diff.x + diff.z*diff.z);
	float pitch = std::atan2(diff.y, xzdist);
	float yaw = std::atan2(-diff.x, -diff.z);
	return Quaternion::fromEulero({ pitch, yaw, 0.0f });
}
//init
TrackArea::TrackArea(){
    //get input
    unlock();
}
TrackArea::~TrackArea(){
    //lock input
    lock();
}

void TrackArea::lock()
{
	//unreg input
    if(!locked)
    {
        Application::instance()->getInput()->removeHandler((Input::MouseHandler*)this);
        locked=true;
    }
}
void TrackArea::unlock()
{
	//reg input
    if(locked)
    {
        Application::instance()->getInput()->addHandler((Input::MouseHandler*)this);
        locked=false;
    }
}

void TrackArea::init(GeometryMaterial* gmap, PointsMaterial* pmat){
	//set materials
	matgeom=gmap;
	cldpoints.init(pmat);
	//camera
	//camera.setPerspective(45.0f, 0.1f, 1000.0f);
}


//utility function
bool TrackArea::inViewport(const Vec2& point){

	Vec2 min = camera.getViewport().xy();
	Vec2 max = Vec2(camera.getViewport().z, camera.getViewport().w) + min;

	return (min.x <= point.x  &&  point.x <= max.x &&
		min.y <= point.y  &&  point.y <= max.y);

}
void TrackArea::rayCast(){
	Vector3D w = ray.origin - sphere.point;
	float A = ray.dir.dot(ray.dir);
	float B = 2.0f*w.dot(ray.dir);
	float C = w.dot(w) - sphere.radius*sphere.radius;
	//calc d
	float D = B*B - 4.0f*A*C;
	float sqrtD = sqrt(D);
	//points
	segment.vnear = w + ray.dir*(-B - sqrtD) / (2.0f*A);
	segment.vfar = w + ray.dir*(-B + sqrtD) / (2.0f*A);
}
void TrackArea::calcRay(const Vec2& mouse){
	camera.ray(mouse, ray.origin, ray.dir);
}

//pikking
void TrackArea::addPoint(const Vec3& point){
    //
    Vec3  mostnear=Vec3::MAX;
    float mindist=Vec3::MAX.x;
    //
    for(auto& geometry:geometies){
        //calc mat inverse
        const Mat4& svdM    = geometry.getSVDMatrix();
        Mat4 modelinv       = geometry.getModelMatrix().getInverse();
        Vec3 vpoint         = modelinv.mul({ point, 1.0 }).xyz();
        //search the most near point
        Mesh& mesh = *geometry.getMesh();
        float tmpdistance = mindist;
        size_t ivertex = 0;
        //search
        for (size_t i = 0; i != mesh.sindex(); ++i){
            ivertex = mesh.getIndex(i);
            tmpdistance = mesh.getVertex3(ivertex, 0).distancePow2(vpoint);
            if (tmpdistance < mindist){
                mindist = tmpdistance;
                mostnear = svdM.mul(Vec4(mesh.getVertex3(ivertex, 0),1.0)).xyz();
            }
        }
    }
    if(mostnear!=Vec3::MAX)
    {
        //add
        cldpoints.addPoint(mostnear);
    }

}

//draw mesh
void TrackArea::draw(){

	//get render & input
	Render& render = *Application::instance()->getRender();
	Input&  input = *Application::instance()->getInput();
	//camera left
	render.setViewportState(camera.getViewport());
    //set shadow camera distance:
    auto& scam = matgeom->getShadowCamera();
    //dist
    Vec3 dir=Vec3(0.0, 0.9,.9).getNormalize();
    const float clen =4.0;
    Vec3 pos = dir*clen + dir*(protation.getScale(true) - 1.0);
    //set dist
    scam.setPosition(pos);
    //angle
    scam.setRotation(pointTarget(sphere.point, pos));
    
    for(auto& geometry:geometies){
        //draw model
        geometry.draw(camera);
        //save info query
        picking = camera.picking(input.getMouse());
        //draw trackball
        if (trackball){
            trackball->setPosition(sphere.point);
            trackball->setScale(sphere.radius*Vec3::ONE);
            trackball->setRotation(protation.getRotation(true));
            trackball->draw(camera);
        }
	}

    //draw selected points
	cldpoints.setObject(geometies.begin()->getRelative()/* <- scene scale & offest */);
    cldpoints.draw(camera);
}
//set & get
void TrackArea::addMesh(Mesh& obj){
    //add geometry
    geometies.resize(geometies.size()+1);
    //const
    auto& geometry=*(--geometies.end());
    //init
    geometry.init(matgeom);
	//attach geometry to protation
	protation.addChild(&geometry,false);
    //add mesh
	geometry.setMesh(&obj);
	//equal scale factor
	geometry.getRelative()->setScale(
                                     geometies.begin()->getRelative()->getScale()
                                    );
	geometry.getRelative()->setPosition(
                                       geometies.begin()->getRelative()->getPosition()
                                       );
}
void TrackArea::removeMesh(const Mesh& obj){
    //search
    for (auto i = geometies.begin(), e = geometies.end(); i !=e; ++i)
    {
        if(i->getMesh()==&obj)
        {
            protation.erseChild(&(*i));
            geometies.erase(i);
            break;
        }
    }
}

void TrackArea::setTrackball(Trackball& trk){
	trackball = &trk;
}
void TrackArea::setViewport(const ViewportState& viewport){
	//camera
	camera.setViewport(viewport);
	camera.setPerspective(45.0f, 0.1f, 1000.0f);
}
void TrackArea::setZoomVelocity(float zvelocity){
    velocity=zvelocity;
}
void TrackArea::setZDistance(float zdistance) {
	camera.setPosition({0,0,zdistance});
}

bool TrackArea::addMeshsSVD(TrackArea& in)
{
    if(in.cldpoints.points.size()<3) return false;
    if(in.cldpoints.points.size()!=cldpoints.points.size()) return false;
    //compute svd
    Mat4 rtran=in.cldpoints.calcSVD(cldpoints);
    //add all mesh
    for(auto& geom:in.geometies)
    {
        addMesh(*geom.getMesh());
        //get lst geometry
        auto& geometry=*(--geometies.end());
        //applay  svd
        geometry.setSVDMatrix(rtran);
    }
    //lcear points
    cldpoints.points.clear();
    //clear reference
    in.geometies.clear();
    in.protation.clearListChilds();
    in.cldpoints.points.clear();
    //return true
    return true;
}


bool TrackArea::saveMeshs(const String&  path)
{
    //I have some meshes?
    if(!geometies.size()) return false;
    //save
    Mesh tosave;
    for(auto& geom:geometies)
    {
        tosave.addMeshOFF(*geom.getMesh(), geom.getSVDMatrix());
    }
    return tosave.saveOFF(path);
}

//events
void TrackArea::onMouseScroll(short scroll){
	if (!inViewport(Application::instance()->getInput()->getMouse())) return;
	scaleZoom -= scroll*velocity*Application::instance()->getLastDeltaTime();
	scaleZoom = Math::max(scaleZoom, 0.0f);
	protation.setScale(scaleZoom*Vec3::ONE);
}
void TrackArea::onMousePress(Vec2 mouse, Key::Mouse bt){
	if (!inViewport(mouse)) return;
	//cases
	switch (bt){
		case Key::BUTTON_LEFT:
			//start rotation
			start = protation.getRotation();
			//rote from
			calcRay(mouse);
			rayCast();
			from = segment.vnear;
        break;
		case Key::BUTTON_RIGHT:
		//case Key::BUTTON_MIDDLE:
			//geometry.setPosition(Vec3::ZERO);
			addPoint(picking);
		break;
        case Key::BUTTON_MIDDLE:
            for(auto& geometry:geometies)
                geometry.setMove(-picking, true);
		break;
	default:
		break;
	}
}
void TrackArea::onMouseDown(Vec2 mouse, Key::Mouse bt){
	if (!inViewport(mouse)) return;
	if (bt != Key::BUTTON_LEFT) return;
	//rote from
	calcRay(mouse);
	rayCast();
	to = segment.vnear;
	//from to
	Vec3 cross = from.cross(to).getNormalize();
	//quad
	Quaternion turn;
	if (cross.length()> 1.0e-6){
		//rote
		float alpha = (from - to).length() / sphere.radius;
		//quaternion turn
		Quaternion turn = Quaternion::fromAxisAngle(cross, alpha);
		//update rotation
		protation.setRotation(start.mul(turn));
	}
}
