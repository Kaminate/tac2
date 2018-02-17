/*!
\author Nathan Park

\par Project: Tac Engine

\brief

*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacSystem.h"
#include "TacRunningAverage.h"
#include "TacTimer.h"
#include "TacGLFW.h"
#include "TacCallback.h"
#include "TacEntity.h"
#include "TacComponentCreator.h"
#include "TacTweak.h" // AntTweakBar
#include "TacNetwork.h" // Winsock
#include "TacResourceManager.h"

#include <map>
#include <exception>
#include <ostream>
#include <functional>
//#include <vld.h> // For the dll

namespace Tac
{
  class Core
  {
  public:
    typedef std::map<Entity::ID, Entity *> EntityContainer;

    TacDLLAPI Core(
      const std::string & gameName , 
      unsigned w , 
      unsigned h );
    TacDLLAPI ~Core();
    TacDLLAPI void AddSystem(System::Type sysType, System * sys);
    TacDLLAPI System * GetSystem(System::Type sysType);

    TacDLLAPI bool Init();
    TacDLLAPI void Run();
    TacDLLAPI float GetElapsedTime() const;
    TacDLLAPI void SetRenderCallback(SPCallback callback);
    TacDLLAPI void SetDebugDrawStart(std::function<void()> callback)
    {
      mDebugDrawCallback = callback;
    }

    TacDLLAPI Window & GetWindow();

#ifdef TACTWEAK
    TacDLLAPI Tweak & GetTweak();
#endif

    TacDLLAPI Entity * CreateEntity();
    TacDLLAPI void DeleteEntity(Entity::ID entityID);
    TacDLLAPI void ClearEntities();
    TacDLLAPI Entity * GetEntity(Entity::ID entityId);
    TacDLLAPI SPComp CreateComponent(Comp::Type compType);
    TacDLLAPI const EntityContainer & GetEntities();

    TacDLLAPI bool Load(const std::string & filename);
    TacDLLAPI bool Save(const std::string & filename) const;
    //TacDLLAPI bool SaveArchitype(const std::string & filename, const Entity * toSave) const;
    TacDLLAPI void StopRunning();

    ResourceManager * mResourceManager;
    Timer mResourceManagerTimer;

    void PrintToStatus(const std::string & str);

  private:
    bool mInit;
    Entity::ID sID;
    std::string mGameName;

    static const float sTimestep;

    typedef std::map<System::Type, System*> SystemMap;
    SystemMap mSystems;
    bool mRunning;
    float mElapsedTime;
    Timer mTimer;

    SPCallback mRenderCallback;
    std::function<void()> mDebugDrawCallback;

    GLFW mGlfw;
    Window mWindow;
#ifdef TACTWEAK

    Tweak mTweak;
#endif
    Winsock mWinsock; // maybe todo: move this into pimpl

    EntityContainer mEntities;
    ComponentCreator mCreator;
    CreatorMap mCreatorMap;

    std::ofstream mStatus;
  };

  DefineSharedPtr(Core);
} // Tac
