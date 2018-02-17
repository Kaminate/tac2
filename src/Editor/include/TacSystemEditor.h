/*!
\author Nathan Park
\brief
*/
#pragma once

#include "TacSystem.h"
#include "TacTweak.h"
#include "TacEntity.h"
#include "TacSelectedEntity.h"
#include "TacSystemInput.h"
#include "TacVector2.h"

namespace Tac
{
  class EditorSystem: public System
  {
  public:
    EditorSystem();
    ~EditorSystem();
    virtual void Init() override;
    virtual void Update(float dt);
    virtual void UpdateFakeTime(float dt);
    void SetCurrentLevelPath(const std::string & levelPath);
    const std::string & GetCurrentLevelPath();

    void CLearList();
    void AddEntityToList(Entity * gameObj);
    void RemEntityFromList(Entity * gameObj);
    void SelectEntity(Entity * gameObj);
    void DeleteSelected();
    //void DeselectEntity();

    bool OnMsg(const InputMsg & msg); // callback for mInputMsgHandler

    Entity::ID GetIdToSelect();

#ifdef TACTWEAK
    static void TW_CALL CallbackOpenLevel(void * data);
    static void TW_CALL CallbackSaveLevelAs(void * data);
    static void TW_CALL CallbackSaveLevel(void * data);
    static void TW_CALL CallbackCreateEntity(void * data);
    static void TW_CALL CallbackOnSelectEntity(void * data);
#endif
    void OpenLevel();
    void SaveLevel();
    void SaveLevelAs();


    void OpenLevel(const std::string & filename);

  private:

#ifdef TACTWEAK
    SPTweakBar mTweakMenu;
#endif

    SPSelectedEntity mSelectedEntity;

    std::string mLevel;
    Entity::ID mIDToSelect;
    std::shared_ptr<InputMsgHandler> mInputMsgHandler; // calls OnMsg()
    float mCameraMovementSpeed;
    float mMaxDegPerSec;
    Vector2 mCamDegPerSec;
    Vector2 mCamNonSelDegPerSec;
    Vector2 mMouseClickPos;
    bool mCamInvertedHorizControls;
    bool mCamInvertedVertiControls;
  };

} // Tac
