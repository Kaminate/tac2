/*!
\file Core.cpp
\brief
\author Nathan Park
\par
*/

#include "TacCore.h"
#include "TacSystem.h"
#include "TacVeryUtil.h"
#include <TacSerializer.h>
#include <TacErrorIf.h>

#include <thread>

namespace Tac
{

  const float Core::sTimestep = 1/60.0f;

  Core::Core(const std::string & gameName, unsigned w, unsigned h) 
    : mGameName(gameName)
    , mRunning(false) 
    , mElapsedTime(0) 
    , sID(Entity::InvalidId)
#ifdef TACTWEAK
    , mTweak(w, h)
#endif
    , mResourceManager (new ResourceManager())
    , mInit(false)
  {
    if (mGameName.empty())
      mGameName = "Tac Engine";
    RemoveSpaces(mGameName);

    if (w && h)
      mWindow.Open(w, h, mGameName.c_str());

    glfwSwapInterval(0); // very important

    mStatus.open("tacstatus.txt", 
      std::ofstream::out | std::ofstream::trunc);
    std::string cwd;
    GetCurrentDir(cwd);
    PrintToStatus("Current working directory: " + cwd);
  }

  Core::~Core()
  {
    ClearEntities();
    //exit(-1); // temp

    delete mResourceManager;

    // delete systems in reverse
    for (SystemMap::reverse_iterator rit = mSystems.rbegin(); 
      rit != mSystems.rend(); ++rit)
    {
      std::pair<System::Type const, System*> pair = *rit;
      System * curSys = pair.second;
      delete curSys;
    }
    mSystems.clear();



    // http://patorjk.com/software/taag, font name Irvit
    // (fucked up from escape character backslash \\)
    // too bad vs2012 doesn't support raw string literals (2013 does)
    std::cout <<
    "   ____                 _ _                 "      << std::endl <<
    "  / ___| ___   ___   __| | |__  _   _  ___  "      << std::endl <<
    " | |  _ / _ \\ / _ \\ / _` | '_ \\| | | |/ _ \\ "  << std::endl <<
    " | |_| | (_) | (_) | (_| | |_) | |_| |  __/ "      << std::endl <<
    "  \\____|\\___/ \\___/ \\__,_|_.__/ \\__, |\\___| "<< std::endl <<
    "                                |___/       "      << std::endl;


  }

  void Core::Run()
  {
    ErrorIf(!mInit, "Call Core::Init() before calling Core::Run()");
      
    mRunning = true;

    while (mRunning)
    {
      if (mWindow.IsOpen() && mWindow.ShouldClose())
        mRunning = false;

      mGlfw.PollEvents();

      if (mResourceManagerTimer.GetElapsedTime() > 0.5f)
      {
        mResourceManagerTimer.Reset();
        mResourceManager->UpdateResources();
      }

      if (mTimer.GetElapsedTime() >= sTimestep)
      {
        mElapsedTime += sTimestep;

        mTimer.Reset();

        mDebugDrawCallback();

        // update each system(timestep)
        for (std::pair<const System::Type, System*> & pair : mSystems)
        {
          System * currSystem = pair.second;
          currSystem->Update(sTimestep);
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
      else
      {
        for (std::pair<const System::Type, System*> & pair : mSystems)
        {
          System * currSystem = pair.second;
          currSystem->UpdateFakeTime(0);
        }

        if(mWindow.IsOpen() && mRenderCallback)
        {
          mRenderCallback->Execute();

#ifdef TACTWEAK
          // this crashes (memory access violation reading 0)
          // if there are no tweak windows ?
          glBindBuffer(GL_ARRAY_BUFFER, 0);
          glBindFramebuffer(GL_FRAMEBUFFER, 0);
          GLuint bufs[] = {GL_BACK_LEFT};
          glDrawBuffers(1, bufs);
          mTweak.Render();
#endif // TACTWEAK

          mWindow.SwapBuffers();
        }
      } // else timestep has not passed
    } // while (running)
  } // Core::Run()

  void Core::SetRenderCallback(SPCallback render)
  {
    mRenderCallback = render;
  }

  Window & Core::GetWindow()
  {
    return mWindow;
  }

  float Core::GetElapsedTime() const
  {
    return mElapsedTime;
  }

  Entity * Core::CreateEntity()
  {
    Entity * newEntity =new Entity(++sID);
    mEntities[sID] = newEntity;
    return newEntity;
  }

  void Core::DeleteEntity( Entity::ID objID)
  {
    auto it = mEntities.find(objID);
    auto pair = *it;
    Entity * gameObj = pair.second;
    delete gameObj;
    mEntities.erase(objID);
  }

  SPComp Core::CreateComponent(Comp::Type compType)
  {
    SPComp comp = mCreator.Create(compType);
    System::Type creatorType = mCreatorMap.GetCreator(compType);
    System * sys = mSystems[creatorType];
    ErrorIf(!sys, "System never created");
    sys->AddComp(compType, comp);
    comp->SetSystem(sys);
    return comp;
  }

  bool Core::Load( const std::string & filename )
  {
    Loader serializer(filename);
    bool success = serializer.Ok();
    if (success)
    {
      serializer.Load(mGameName);
      unsigned numEntities;
      serializer.Load(numEntities);
      while (numEntities--)
      {
        Entity * iEntity = CreateEntity(); 
        unsigned numComponents;
        std::string entityName;
        serializer.Load(entityName);
        serializer.Load(numComponents);
        while(numComponents--)
        {
          Comp::Type compType;
          serializer.LoadEnum((unsigned&)compType);
          SPComp comp = CreateComponent(compType);
          comp->Load(serializer);
          iEntity->AddComponent(compType, comp);
        }
        if (!iEntity->Initialize())
          DeleteEntity(iEntity->GetID());
      }
    }
    return success;
  }

  bool Core::Save( const std::string & filename ) const 
  {
    Saver fileSaver(filename);
    bool success = fileSaver.Ok();
    if (success)
    {
      fileSaver.SaveFormatted("game", mGameName);
      fileSaver.Tab();
      fileSaver.SaveFormatted("numEntities", mEntities.size());
      unsigned entityCounter = 0;
      for (auto & entityPair : mEntities)
      {
        char buffer[6];
        _itoa_s(entityCounter++, buffer, 10);
        std::string number(buffer);

        fileSaver.SaveFormatted("entity", "entity" + number);
        fileSaver.Tab();
        Entity * iEntity = entityPair.second;
        fileSaver.SaveFormatted("numComponents", iEntity->GetNumComps());
        for (
          unsigned iComp = 0; 
          iComp < Comp::Type::eCompCount; 
        ++iComp)
        {
          Comp::Type compType = (Comp::Type) iComp;
          Comp * comp = iEntity->GetComponent(compType);
          if (comp)
          {
            fileSaver.SaveFormatted("componentType", compType);
            fileSaver.Tab();
            comp->Save(fileSaver);
            fileSaver.DeTab();
          }
        }
        fileSaver.DeTab();
      }
    }
    return success;
  }

  void Core::AddSystem( System::Type sysType, System * sys )
  {
    mSystems[sysType] = sys;
    sys->SetCore(this);
  }

  System * Core::GetSystem( System::Type sysType )
  {
    return mSystems[sysType];
  }

  void Core::ClearEntities()
  {
    while (!mEntities.empty())
    {
      auto it = mEntities.begin();
      auto pair = *it;
      Entity::ID objID = pair.first;
      DeleteEntity(objID);
    }
    sID = Entity::InvalidId;
  }
#ifdef TACTWEAK

  Tweak & Core::GetTweak()
  {
    return mTweak;

  }

#endif
  Entity * Core::GetEntity( Entity::ID entityId )
  {
    Entity * gameObj = nullptr;
    auto it = mEntities.find(entityId);
    if (it != mEntities.end())
    {
      auto pair = *it;
      gameObj = pair.second;
    }
    return gameObj;
  }

  const Core::EntityContainer & Core::GetEntities()
  {
    return mEntities;
  }

  void Core::StopRunning()
  {
    mRunning = false;
  }

  void Core::PrintToStatus( const std::string & str )
  {
    mStatus << str << std::endl;
  }

  bool Core::Init()
  {
    for(auto pair : mSystems)
    {
      pair.second->Init();
    }
    
    return mInit = true;
  }

} // Tac

