/*!
\author Nathan Park
\brief
*/

#pragma once

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include <string>
#include <map>
#include <set>
#include <exception>
#include "TacMessage.h"
#include "TacTweak.h"
#include "TacSingleton.h"
#include "TacVector2.h"
#include "TacVector3.h"
#include "TacMacros.h"
#include "TacVector4.h"
#include "TacCallback.h"

namespace Tac
{
  namespace Asteroids
  {
    class Exception : public std::exception
    {
      std::string mMsg;
    public:
      TacDLLAPI Exception(const std::string & msg) : mMsg(msg) 
      {
        __debugbreak();
      }
      TacDLLAPI const char * what() const override
      {
        return mMsg.c_str();
      }
    };

    typedef int ID;
    extern std::string gConfigFilepath;


    // CANNOT HAVE MEMBER FUNS (in a union somehwere)
    class Object
    {
    public:
      enum Type {eShip, eAsteroid, eBullet, eCount};

      ID mOwner;
      Type mType;
      float mPosX;
      float mPosY;
      float mVelX;
      float mVelY;
    };
    bool Colliding (const Object & lhs, const Object & rhs);
    Vector4 GetColor(int x, int y, int z); // out of 255
    Vector4 GetColor(const Object & obj);
    float GetScale(Object::Type objType);
    DefineSharedPtr(Object);

    class OnDestroy
    {
    public:
      virtual void Execute(Object * obj) = 0;
    };
    DefineSharedPtr(OnDestroy);

    class Game
    {
    public:
      Game(SPOnDestroy onDestroyCallback, SPCallback gameOverCallback);
      void Update(float dt);
      void AttemptFire(ID playerID);

      void SteerMissiles(float dt);
      void Integrate(float dt);
      void AddPlayer(ID playerID);
      void RemPlayer(ID playerID);
      void SpawnAsteroid(Vector2 pos);
      void DeleteOutOfBoundBullets();
      Object * GetShip(ID playerID);
      unsigned GetNum(Object::Type objType);
      //private:
      SPOnDestroy mOnDestroyCallback;

      typedef std::map<Object*, SPObject> ObjectContainer;

      ObjectContainer mObjects;
      void WrapAllObjects();
      void Wrap(Object * obj);
      void DoCollisionThings();

      // waves
      void WaveLogic();
      unsigned mWavesToSpawn;
      unsigned mAsteroidsPerWave;
      unsigned mAdditionalAsteroidsPerWave;
      unsigned mWavesSpawned;
      SPCallback mGameOverCallback;

      float mElapsedTime;

      // shooting
      float mShootCooldown;
      std::map<ID, float> mPlayerTimeSinceLastFire;
      std::map<ID, Vector2> mLastNonZeroShipDir;

    };

    const int MAXOBJS = 100;
    struct ObjList
    {
      int mNumObjs;
      Object mObjs[MAXOBJS];
    };

#define FlagLEFT  (1<<0)
#define FlagRIGHT (1<<1)
#define FlagUP    (1<<2)
#define FlagDOWN  (1<<3)
#define FlagSPACE (1<<4)
    extern char gBuffer[65507];

    struct Inpput
    {
      unsigned mKeyFlags;
      char idAsChar;
    };

    void GetStateMachineTweakVars(
      unsigned & w, 
      unsigned & h, 
      unsigned & x,
      unsigned & y);
    

    namespace Message
    {
      enum Type
      {
        eAssignID,      // s-->c, int
        eAssignName,    // s-->c, string
        eChat,          // s<->c, string name, string msg
        eReadyToStart,  // s<--c, no data
        eGameStarted,   // s-->c, no data
        eGameEnded,     // s-->c, bool playerVictory
        eUDPInfo,       // s<--c, string ip, string port
        eCount
      };
      std::string tostring(Message::Type t);
      Type fromString(const std::string & str);
      class Handler
      {
      public:
        virtual void Handle(SPIncomingMessage m) = 0;
      };
      DefineSharedPtr(Handler);


      typedef std::map< Type, std::set<Handler*> > HandlerMap;
      void Handle(HandlerMap & map, SPIncomingMessage msg);


      class Adder
      {
        HandlerMap mHolder;
        HandlerMap & mMap;
      public:
        Adder(HandlerMap & map);
        void Add(Type t, Handler* h);
        ~Adder();
        Adder & operator = (const Adder & rhs); // not defined
      };
    };

    class SpaceNames
    {
      std::vector<std::string> mNames;
    public:
      SpaceNames();
      std::string GetName();
    };

  } // Asteroids
} // Tac
