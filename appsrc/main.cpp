#include <stdafx.h>
#include <Application.h>
#include <Game.h>
#include <Input.h>
#include <Screen.h>
#include <Render.h>
#include <Shader.h>
#include <Math3D.h>
#include <Object.h>
#include <Debug.h>

#include "Mesh.h"
#include "Camera.h"
#include "Trackball.h"
#include "TrackArea.h"
#include "Geometry.h"
#include "Editorui.h"

#define DRAW_MODELS
using namespace Easy3D;
#define UI_SIZE 200


class MyGame : public Easy3D::Game {
public:

	//materials
	TrackballMaterial matTrackball;
	PointsMaterial	  matPoints;
	GeometryMaterial  matGeometry;
	//model 1
	TrackArea trackAreaLeft;
	//model 2
	TrackArea trackAreaRight;
	//others
	Trackball trackball;
    //last mesh
    Mesh* lastMesh{ nullptr };
    //editor
    EditorUI ui;
    //states
    enum LoadState
    {
        NO_LOAD,
        ONE_LOAD,
        ALL_LOAD
    };
    LoadState lstate{ NO_LOAD };
    

	MyGame() :Game("Easy3D exemple", 1280, 720,32,60){}
    
    //rspath + "/meshs/faccia000.off" ship/tet3dcc2/m299/head/m332/m355/m1003/m1372
    void loadModel(const String& path)
    {
        //screen size
        Vec2 ssize=getScreen().getSize();
        
        switch (lstate) {
            case NO_LOAD:
            {
                //init
                Mesh* mesh=new Mesh;
                mesh->loadOFF(path, false);
                lastMesh=mesh;
                //init track
                trackAreaLeft.addMesh(*mesh);
                trackAreaLeft.sphere.radius = 1.5;
                trackAreaLeft.setZoomVelocity(0.1);
                trackAreaLeft.setZDistance(4);
                //viewport
                trackAreaLeft.setViewport(Vec4(UI_SIZE, 0,  ssize.x-UI_SIZE, ssize.y) );
                //new state
                lstate=ONE_LOAD;
                break;
            }
            case ONE_LOAD:
            {
                //init
                Mesh* mesh=new Mesh();
                mesh->loadOFF(path, false);
                lastMesh=mesh;
                //init track
                trackAreaRight.addMesh(*mesh);
                trackAreaRight.sphere.radius = 1.5;
                trackAreaRight.setZoomVelocity(0.1);
                trackAreaRight.setZDistance(4);
                //viewport
                trackAreaLeft.setViewport(Vec4(UI_SIZE, 0,  ssize.x*0.5-UI_SIZE*0.5, ssize.y) );
                trackAreaRight.setViewport( Vec4(ssize.x*0.5+UI_SIZE*0.5, 0, ssize.x*0.5-UI_SIZE*0.5, ssize.y) );
                //new state
                lstate=ALL_LOAD;
            }
            break;
                
            default:
                break;
        }
    }

    void drawModels()
    {
        switch (lstate) {
            case ALL_LOAD:
                trackAreaRight.draw();
            case ONE_LOAD:
                trackAreaLeft.draw();
            default:  break;
        }
    }
    
    void mergeMesh()
    {
		if (lstate != ALL_LOAD || !lastMesh)
		{
			ui.showWrongMessage("can't merge mesh");
			return;
		}
        //screen size
        Vec2 ssize=getScreen().getSize();
        //add mesh to area
        if(!trackAreaLeft.addMeshsSVD(trackAreaRight))
        {
            ui.showWrongMessage("can't merge mesh");
            return;
        }
        //viewport
        trackAreaLeft.setViewport(Vec4(UI_SIZE, 0,  ssize.x-UI_SIZE, ssize.y) );
        //new state
        lstate=ONE_LOAD;
    }
    
    void saveModels(const String& path)
    {
		if (lstate == NO_LOAD || !lastMesh)
		{
			ui.showWrongMessage("can't save mesh");
			return;
		}
        //screen size
        Vec2 ssize=getScreen().getSize();
        //add mesh to area
        if(!trackAreaLeft.saveMeshs(path))
        {
            ui.showWrongMessage("can't save mesh");
            return;
        }
        //viewport
        trackAreaLeft.setViewport(Vec4(UI_SIZE, 0,  ssize.x-UI_SIZE, ssize.y) );
        //new state
        lstate=ONE_LOAD;
    }
    
	void start(){
		//resource
		String rspath = Application::instance()->appResourcesDirectory();
		//init renders
		Vec4 viewport(0, 0, 1280, 720);
		getRender().setViewportState(viewport);
        getRender().setClearColorState({64,64,64,255});
		getRender().setZBufferState(true);
		getRender().setBlendState(BlendState(BLEND::ONE, BLEND::ZERO));
		getRender().setCullFaceState(CullFace::DISABLE);
		//materials
		matTrackball.init();
		matPoints.init();
		matGeometry.init();
		matGeometry.setColor({1.0,1.0,1.0,1.0});
		//init trackball
		trackball.init(&matTrackball);
        //init track area
        //left
        trackAreaLeft.init(&matGeometry, &matPoints);
        trackAreaLeft.setTrackball(trackball);
        //right
        trackAreaRight.init(&matGeometry, &matPoints);
        trackAreaRight.setTrackball(trackball);
        //init ui
        ui.setSizeLeft(UI_SIZE);
        ui.setCallBackLoad([this](const String& path)
                           {
                               loadModel(path);
                           });
        ui.setCallBackSave([this](const String& path)
                           {
                               saveModels(path);
                           });
        ui.setCallBackSVD([this]()
                         {
                             mergeMesh();
                         });
		#if 0
		Mesh m1;
		m1.addMeshOFF(modelRight, {
			1, 0, 0, 0,
			0, 0.5, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		});
		m1.saveOFF(rspath + "/meshs/faccia045.off");
		#endif
		
	}
	
	void run(float dt){
		//clear
		getRender().doClear();
		//draw left
        drawModels();
        //draw ui
        ui.draw();
	}
	void end(){
	}

};

int main(){
	Easy3D::Application::create("Easy3DExemple", 
												//OPENGL_DRIVER
												DIRECTX_DRIVER
												);
	Easy3D::Application::instance()->exec(new MyGame());
	delete Easy3D::Application::instance()->getGame();
	return 0;
}