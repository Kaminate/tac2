// Nathan Park
#include "TacComponentTransform.h"
#include "TacSerializer.h"
#include "TacCore.h"
namespace Tac
{
  void TransformComp::Load( Loader & serializer )
  {
    serializer.Load(mPos);
    serializer.Load(mRot);
    serializer.Load(mSca);
  }

  void TransformComp::Save( Saver & serializer )
  {
    serializer.SaveFormatted("position", mPos);
    serializer.SaveFormatted("rotation", mRot);
    serializer.SaveFormatted("scale", mSca);
  }

  bool TransformComp::Initialize()
  {
    return true;
  }

  TransformComp::TransformComp()
    : mPos(0,0,0)
    , mRot(0,0,0)
    , mSca(1,1,1)
    , mFakePos(0,0,0)
    , mFakeRot(0,0,0)
  {

  }

#ifdef TACTWEAK
  void TransformComp::AddToTweakBar( TweakBar & bar )
  {
    //TwType twvec3 = GetSystem()->GetCore()->GetTweak().GetTypeVector3();
    //for(int i =0; i < 3; ++i)
    //{
    //  std::string name = "TransformPos" + (i + 'X');
    //  std::string props = "group=Pos label='Pos' step=1 ";
    //  bar.AddVar(name, props, TW_TYPE_FLOAT, &mPos[i]);
    //  name = "TransformSca" + (i + 'X');
    //  props = "group=Pos label='Pos' step=1 ";
    //}
    //bar.AddVar("Transform Rot", 
    //  "group=Tranform "
    //  "label='Rot '",
    //  TW_TYPE_DIR3F, &mRot);

    for(int i = 0; i < 3; ++i)
    {
      std::string axis("X");
      axis[0] += (char)i;
      bar.AddVar(
        "transformScale" + axis,
        "label='" + axis + "' step=0.5 min=0.5 group='Scale'",
        TW_TYPE_FLOAT, &mSca[i]);
      bar.AddVar(
        "transformRot" + axis,
        "label='" + axis + "' step=0.5236 min=0 group='Rotation'",
        TW_TYPE_FLOAT, &mRot[i]);
      bar.AddVar(
        "transformPos" + axis,
        "label='" + axis + "' step=0.5 group='Position'",
        TW_TYPE_FLOAT, &mPos[i]);
    }
    std::string toDefine(
      bar.GetName() + "/Scale group=Transform \n" +
      bar.GetName() + "/Rotation group=Transform \n" +
      bar.GetName() + "/Position group=Transform \n" );
      
    TwDefine(toDefine.c_str());
  }
  void TransformComp::RemoveFromTweakBar( TweakBar & bar )
  {
    bar.RemoveVar("Transform Pos");
    bar.RemoveVar("Transform Rot");
    bar.RemoveVar("Transform Scale");
  }
#endif
} // Tac
