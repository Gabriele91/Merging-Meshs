#ifndef EDITORUI_H
#define EDITORUI_H

#include <Config.h>
#include <Application.h>
#include <Input.h>
#include "gui.h"
#include "FinderUI.h"

namespace Easy3D
{
  
class WrongMsg : public Input::MouseHandler
{
public:
    
    bool visible{false};
    String wrong;
    
    WrongMsg();
    ~WrongMsg();
    
    void draw();
    virtual void onMousePress(Vec2 pos, Key::Mouse key);
};
    
class EditorUI
{
    ImGuiStyle style;
    WrongMsg  wrongMsg;
    FinderOpenOff openOffs;
    FinderSaveOff saveOffs;
    float  sizeLeft;
    DFUNCTION<void(void)> doSVD;
    
    public:

    EditorUI(float sizeLeft=0);
    ~EditorUI();
    
    void setSizeLeft(float size);
    
    void setCallBackLoad(DFUNCTION<void(const String& path)> cbfun);
    void setCallBackSave(DFUNCTION<void(const String& path)> cbfun);
    void setCallBackSVD(DFUNCTION<void(void)> cbfun);
    void showWrongMessage(const String& wrong);
    
    void draw();
    
    
    protected:
    
    
    Screen& getScreen(){
        return *Application::instance()->getScreen();
    }
    Input& getInput(){
        return *Application::instance()->getInput();
    }
    Audio& getAudio(){
        return *Application::instance()->getAudio();
    }
    Render& getRender(){
        return *Application::instance()->getRender();
    }
};


};

#endif