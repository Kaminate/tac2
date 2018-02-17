/*!
\author Nathan Park
\brief
*/
#pragma once

#include "TacTweak.h"
#include "TacEntity.h"
#include "TacSharedPtr.h"

namespace Tac
{
  class EditorSystem;
  class SelectedEntity
  {
  public:
    SelectedEntity(EditorSystem * editor, Entity * gameObject);

#ifdef TACTWEAK
    SPTweakBar GetTweak();
#endif

    //void OnSaveArchitype();
    EditorSystem * GetEditor();
    Entity * GetSelected();
    Comp::Type GetCurrentType();
    
  private:
#ifdef TACTWEAK

    SPTweakBar mTweakSelected;
#endif
    EditorSystem * mEditor; 
    Entity * mSelectedEntity;
    Comp::Type mComponentType;
  };
  DefineSharedPtr(SelectedEntity);

} // Tac
