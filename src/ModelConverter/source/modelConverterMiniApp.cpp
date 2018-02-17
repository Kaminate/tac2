///////////////////////////////
// modelConverterMiniApp.cpp //
///////////////////////////////
#include "modelConverterMiniApp.h"
#include "TacCore.h"
#include "TacGeometryData.h"

namespace Tac
{
  converter::objgui::objgui(
    const std::string & fullpath,
    converter * owner) 
    : mCloseButtonPressed(false)
    , mOBJdata(fullpath)
    , mOwner(owner)
  {
    mTriMeshThings.push_back(
      Window::FileTypeThing("tac mesh file (.tacmesh)", "tacmesh"));

#ifdef TACTWEAK
    mTweakBar = owner->GetOwner()->GetCore()->GetTweak().Create(fullpath);
    mTweakBar->AddButton("Close", "Close",
      TweakBar::callbackHelper<objgui>::TweakCallback, 
      new TweakBar::callbackHelper<objgui>(
      this, &objgui::OnClose));

    mTweakBar->AddButton("Save Averaged Normals", "Save Averaged Normals",
      TweakBar::callbackHelper<objgui>::TweakCallback, 
      new TweakBar::callbackHelper<objgui>(
      this, &objgui::saveAveragedNormals));

    mTweakBar->AddButton("Save Per-Face Normals", "Save Per-Face Normals",
      TweakBar::callbackHelper<objgui>::TweakCallback, 
      new TweakBar::callbackHelper<objgui>(
      this, &objgui::savePerFaceNormals));
#endif // TACTWEAK
  }

  void converter::objgui::savePerFaceNormals()
  {
    Save(ObjDataV2::NormalType::eFace);
  }

  void converter::objgui::saveAveragedNormals()
  {
    Save(ObjDataV2::NormalType::eAveraged);
  }

  bool converter::objgui::CloseButtonPressed() const 
  {
    return mCloseButtonPressed;
  }

  void converter::objgui::OnClose()
  {
    mCloseButtonPressed = true;
  }

  void converter::objgui::Save(ObjDataV2::NormalType normType)
  {
    std::string filename;
    AppSystem * app = mOwner->GetOwner();
    Core * core = app->GetCore();
    Window & wind = core->GetWindow();

    if(wind.SaveFile(filename, mTriMeshThings))
    {
      GeometryData geomdata;
      mOBJdata.Fill(geomdata, normType);
      geomdata.Save(filename);
    }
  }

  ////////////////////
  // modelconverter //
  ////////////////////

  converter::converter(AppSystem * owner) : MiniApp(owner)
  {
#ifdef TACTWEAK
    mTweakBar = GetOwner()->GetCore()->GetTweak().Create("model converter");
    mTweakBar->SetPosition(10,10);
    mTweakBar->SetSize(270,400);
    mTweakBar->AddComment("comment", "hi");
    mTweakBar->AddButton("Open OBJ file", "Open OBJ file", 
      TweakBar::callbackHelper<converter>::TweakCallback, 
      new TweakBar::callbackHelper<converter>(
      this, &converter::OnOpenObjFile));
#endif //TACTWEAK
    mOBJfileTHings.push_back(
      Window::FileTypeThing("object file (.obj)", "obj"));

  }
  converter::~converter()
  {
    for(objgui * model : mLoadedModels)
      delete model;
    mLoadedModels.clear();
  }
  void converter::OnOpenObjFile()
  {
    std::string filename;
    if(GetOwner()->GetCore()->GetWindow().OpenFile(filename, mOBJfileTHings))
    {
      mLoadedModels.insert(new objgui(filename, this));
    }
  }
  void converter::Update(float dt) 
  {
    dt;

    auto it = mLoadedModels.begin();
    while (it != mLoadedModels.end())
    {
      objgui * curModel = *it;
      if(curModel->CloseButtonPressed())
      {
        it = mLoadedModels.erase(it);
        delete curModel;
      }
      else
      {
        ++it;
      }
    }
  }
} // Tac
