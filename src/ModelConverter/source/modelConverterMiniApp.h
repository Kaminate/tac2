/////////////////////////////
// modelConverterMiniApp.h //
/////////////////////////////
#include <string>
#include "TacObjInfo.h"
#include "TacGLFW.h"
#include <TacTweak.h>
#include <TacSystemApplication.h>

namespace Tac
{
  class converter : public MiniApp
  {
  public:
    class objgui
    {
    public:
      objgui(const std::string & fullpath, converter * owner);
      void savePerFaceNormals();
      void saveAveragedNormals();
      bool CloseButtonPressed()const;
      void OnClose();

    private:
      void Save(ObjDataV2::NormalType type);

      bool mCloseButtonPressed;
      ObjDataV2 mOBJdata;
      converter * mOwner;
      std::vector<Window::FileTypeThing> mTriMeshThings;
#ifdef TACTWEAK
      SPTweakBar mTweakBar;
#endif
    };

    converter(AppSystem * owner);
    ~converter();
    void OnOpenObjFile();
    virtual void Update(float dt) override;

#ifdef TACTWEAK
    SPTweakBar mTweakBar;
#endif
    std::vector<Window::FileTypeThing> mOBJfileTHings;
    std::set<objgui*> mLoadedModels;
  };

} // Tac
