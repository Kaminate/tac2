// Nathan Park
#include "TacSelectedEntity.h"
#include "TacSystemEditor.h"
#include "TacCore.h"
namespace Tac
{
#ifdef TACTWEAK
  void TW_CALL AddComponent(void * data)
  {
    SelectedEntity * selected = (SelectedEntity*) data;
    Entity * entity = selected->GetSelected();

    Comp::Type compType =selected->GetCurrentType() ;
    if (entity->HasComponent(compType))
      return;

    SPComp comp 
      = selected->GetEditor()->GetCore()->CreateComponent(compType);

    entity->AddComponent(compType, comp);
    if (comp->Initialize())
    {
      comp->AddToTweakBar(*selected->GetTweak());
    }
    else
    {
      entity->RemComponent(compType);
    }
  }
  void TW_CALL RemoveComponent(void * data)
  {
    SelectedEntity * selected = (SelectedEntity*) data;
    Entity * entity = selected->GetSelected();
    Comp::Type compType =selected->GetCurrentType() ;
    if (entity->HasComponent(compType))
    {
      Comp * comp = entity->GetComponent(compType);
      comp->RemoveFromTweakBar(*selected->GetTweak());
      entity->RemComponent(compType);
      // after removal (aka NOW), comp is an invalid pointer
    }
  }
#endif

  SelectedEntity::SelectedEntity( EditorSystem * editor, Entity * gameObj)
    : mEditor(editor)
    , mComponentType(Comp::Type::eCompTransform)
    , mSelectedEntity(gameObj)
  {
#ifdef TACTWEAK

    std::string barname = "Entity_" + std::to_string(gameObj->GetID());
    // create a new tweak bar
    mTweakSelected.reset();
    mTweakSelected = editor->GetCore()->GetTweak().Create(barname);
    mTweakSelected->SetSize(270, 300);
    mTweakSelected->SetPosition(50,250);
    mTweakSelected->AddButton("Add Comp", "Add Comp", AddComponent, this);
    mTweakSelected->AddButton("Rem Comp","Rem Comp",RemoveComponent, this);
    mTweakSelected->AddVar("Component Type","",
      editor->GetCore()->GetTweak().GetTypeComponent(),
      &mComponentType);
    //mTweakSelected->AddButton(
    //  "Save Architype As", this, &SelectedEntity::OnSaveArchitype, "");

    // add entity too bar
    mSelectedEntity->AddToTweakBar(*mTweakSelected);
#endif
  }

  Entity * SelectedEntity::GetSelected()
  {
    return mSelectedEntity;
  }

  Comp::Type SelectedEntity::GetCurrentType()
  {
    return mComponentType;
  }

  EditorSystem * SelectedEntity::GetEditor()
  {
    return mEditor;
  }

#ifdef TACTWEAK
  Tac::SPTweakBar SelectedEntity::GetTweak()
  {
    return mTweakSelected;

  }

  //void SelectedEntity::OnSaveArchitype()
  //{
  //  std::string filename;
  //  std::vector<Window::FileTypeThing> things;
  //  things.push_back(Window::FileTypeThing(
  //    "architype (.tacarchitype)", "tacarchitype"));

  //  Core * engine = GetEditor()->GetCore();
  //  if(engine->GetWindow().SaveFile(filename, things))
  //  {
  //    engine->SaveArchitype(filename, mSelectedEntity);
  //  }
  //}

#endif
}