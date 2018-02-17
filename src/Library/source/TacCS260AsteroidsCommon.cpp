// Nathan Park
#include "TacCS260AsteroidsCommon.h"
#include "TacErrorIf.h"
#include <string> //getline
#include <algorithm>
#include <time.h>
#include <TacMathUtil.h>
#include "TacMatrix3.h"
namespace Tac
{
  namespace Asteroids
  {
    std::string gConfigFilepath("config.txt");

    char gBuffer[65507];


    class DeleteMap
    {
    public:
      DeleteMap()
      {
        mMap[Object::Type::eAsteroid][Object::Type::eAsteroid] = false;
        mMap[Object::Type::eAsteroid][Object::Type::eBullet] = true;
        mMap[Object::Type::eAsteroid][Object::Type::eShip] = true;
        mMap[Object::Type::eBullet][Object::Type::eAsteroid] = true;
        mMap[Object::Type::eBullet][Object::Type::eBullet] = false;
        mMap[Object::Type::eBullet][Object::Type::eShip] = false;
        mMap[Object::Type::eShip][Object::Type::eAsteroid] = true;
        mMap[Object::Type::eShip][Object::Type::eBullet] = false;
        mMap[Object::Type::eShip][Object::Type::eShip] = false;
      }
      bool Deletes(Object::Type lhs, Object::Type rhs)
      {
        return mMap[lhs][rhs];
      }
    private:
      bool mMap[Object::Type::eCount][Object::Type::eCount];
    };

    Game::Game(
      SPOnDestroy onDestroyCallback, 
      SPCallback gameOverCallback)
      : mOnDestroyCallback(onDestroyCallback)
      , mGameOverCallback(gameOverCallback)
      , mWavesToSpawn(4)
      , mAsteroidsPerWave(4)
      , mAdditionalAsteroidsPerWave(4)
      , mWavesSpawned(0)
      , mElapsedTime(0)
      , mShootCooldown(0.2f)
    {

    }

    void Game::Integrate(float dt)
    {
      for (std::pair<Object*const, SPObject> & pair: mObjects)
      {
        Object * curObj = pair.first;

        // damp only the ships
        if (curObj->mType == Object::Type::eShip)
        {
          float damping = 0.95f;
          curObj->mVelX *= damping;
          curObj->mVelY *= damping;
          Vector2 shipVel(curObj->mVelX, curObj->mVelY);
          if (shipVel.Length() > 0.01f)
            mLastNonZeroShipDir[curObj->mOwner] = shipVel;
        }
        curObj->mPosX += curObj->mVelX * dt;
        curObj->mPosY += curObj->mVelY * dt;
      }
    }

    void Game::Update( float dt )
    {
      for (std::pair<const ID, float> & pair : mPlayerTimeSinceLastFire)
      {
        float & timeSinceLastFire = pair.second;
        timeSinceLastFire += dt;
      }

      if (GetNum(Object::Type::eShip) == 0 && mElapsedTime > 3.0f)
        mGameOverCallback->Execute();

      WaveLogic();
      SteerMissiles(dt);
      Integrate(dt);
      DeleteOutOfBoundBullets();
      WrapAllObjects();
      DoCollisionThings();

      mElapsedTime += dt;
    }

    void Game::AddPlayer( ID playerID )
    {
      SPObject newObj(new Object());

      newObj->mOwner = playerID;

      newObj->mPosX = 0;
      newObj->mPosY = 0;

      newObj->mVelX = 0;
      newObj->mVelY = 0;

      newObj->mType = Object::Type::eShip;

      mObjects[newObj.get()] = newObj;

      mPlayerTimeSinceLastFire[playerID] = mShootCooldown;
      mLastNonZeroShipDir[playerID] = Vector2(0,1);
    }

    void Game::RemPlayer( ID playerID )
    {
      mPlayerTimeSinceLastFire.erase(playerID);
      mLastNonZeroShipDir.erase(playerID);

      // remove everything owned by the player (ship, bullets)
      std::vector<Object*> ownedByPlayer;
      for (auto pair : mObjects)
      {
        Object * obj = pair.first;
        if (obj->mOwner == playerID)
          ownedByPlayer.push_back(obj);
      }

      for (Object * obj : ownedByPlayer)
        mObjects.erase(obj);
    }

    void Game::Wrap( Object * obj )
    {
      float bound = 10;

      if (obj->mPosX > bound)
        obj->mPosX = -bound;

      if (obj->mPosY > bound)
        obj->mPosY = -bound;

      if (obj->mPosX < -bound)
        obj->mPosX = bound;

      if (obj->mPosY < -bound)
        obj->mPosY = bound;
    }

    void Game::WaveLogic()
    {
      if (GetNum(Object::Type::eAsteroid) == 0)
      {
        if (mWavesSpawned == mWavesToSpawn)
        {
          mGameOverCallback->Execute();
        }
        else
        {
          for (unsigned i = 0; i < mAsteroidsPerWave; ++i)
          {
            Vector2 points[4];
            points[0] = Vector2(10,10);
            points[1] = Vector2(10,-10);
            points[2] = Vector2(-10,-10);
            points[3] = Vector2(-10,10);

            unsigned index1 = rand() % 4;
            unsigned index2 = index1 + 1;

            if (index2 == 4)
              index2 = 0;

            Vector2 asteroidPos 
              = Lerp(points[index1], points[index2], RandFloat());

            SpawnAsteroid(asteroidPos);
          }
          mAsteroidsPerWave += mAdditionalAsteroidsPerWave;

          ++mWavesSpawned;
        }
      }

    }

    void Game::SpawnAsteroid( Vector2 pos )
    {
      SPObject newObj(new Object());

      newObj->mOwner = -1;

      newObj->mPosX = pos.x;
      newObj->mPosY = pos.y;

      float speed = 3;

      // how to get a random vector
      Vector2 vel(RandFloat(-1,1), RandFloat(-1,1));
      if (IsAboutZero(vel.x))
        vel.x += 0.1f; // prevent zero vector
      vel.Normalize();
      vel *= speed;

      newObj->mVelX = vel.x;
      newObj->mVelY = vel.y;

      newObj->mType = Object::Type::eAsteroid;

      mObjects[newObj.get()] = newObj;
    }

    unsigned Game::GetNum(Object::Type objType)
    {
      unsigned num=0;
      for (
        ObjectContainer::const_iterator it1 = mObjects.begin(); 
        it1 != mObjects.end(); ++it1)
      {
        const std::pair<Object*const, SPObject>& pair = *it1;
        Object * obj1 = pair.first;
        if (obj1->mType == objType)
          ++num;
      }
      return num;
    }

    void Game::AttemptFire( ID playerID )
    {
      // can't fire if there is no ship
      Object * playerShip = GetShip(playerID);
      if (!playerShip)
        return;

      float & timeSinceLastFire =
        mPlayerTimeSinceLastFire[playerID];

      if (timeSinceLastFire >= mShootCooldown)
      {
        timeSinceLastFire = 0;

        SPObject newObj(new Object());

        newObj->mOwner = playerID;

        Vector2 vel = mLastNonZeroShipDir[playerID];
        vel.Normalize();
        vel *= 15;

        newObj->mVelX = vel.x;
        newObj->mVelY = vel.y;
        newObj->mPosX = playerShip->mPosX;
        newObj->mPosY = playerShip->mPosY;

        newObj->mType = Object::Type::eBullet;

        mObjects[newObj.get()] = newObj;
      }

    }

    Object * Game::GetShip( ID playerID )
    {
      for (
        ObjectContainer::iterator it1 = mObjects.begin(); 
        it1 != mObjects.end(); ++it1)
      {
        const std::pair<Object*const, SPObject>& pair1 = *it1;
        Object * obj1 = pair1.first;
        if (obj1->mOwner == playerID
          && obj1->mType == Object::Type::eShip)
        {
          return obj1;
        }
      }
      return nullptr;
    }

    void Game::DeleteOutOfBoundBullets()
    {
      std::set<Object*> toDestroy;
      for (
        ObjectContainer::const_iterator it1 = mObjects.begin(); 
        it1 != mObjects.end(); ++it1)
      {
        const std::pair<Object*const, SPObject>& pair1 = *it1;
        Object * obj1 = pair1.first;

        if (obj1->mType != Object::Type::eBullet)
          continue;

        if (
          obj1->mPosX > 10
          || obj1->mPosX < -10
          || obj1->mPosY > 10
          || obj1->mPosY < -10)
        {
          toDestroy.insert(obj1);
        }
      }

      if (mOnDestroyCallback)
      {
        for (Object * obj : toDestroy)
        {
          mOnDestroyCallback->Execute(obj);
        }
      }

      for (Object * obj : toDestroy)
      {
        mObjects.erase(obj);
      }
    }

    void Game::WrapAllObjects()
    {
      for (
        ObjectContainer::const_iterator it1 = mObjects.begin(); 
        it1 != mObjects.end(); ++it1)
      {
        const std::pair<Object*const, SPObject>& pair1 = *it1;
        Object * obj1 = pair1.first;
        Wrap(obj1);
      }
    }

    void Game::DoCollisionThings()
    {

      std::set<Object*> toDestroy;
      for (
        ObjectContainer::const_iterator it1 = mObjects.begin(); 
        it1 != mObjects.end(); ++it1)
      {
        ObjectContainer::const_iterator it2 = it1;
        for (++it2; it2 != mObjects.end(); ++it2)
        {
          const std::pair<Object*const, SPObject>& pair1 = *it1;
          const std::pair<Object*const, SPObject>& pair2 = *it2;
          Object * obj1 = pair1.first;
          Object * obj2 = pair2.first;

          static DeleteMap sDeleteMap;

          if (sDeleteMap.Deletes(obj1->mType, obj2->mType) 
            && Colliding(*obj1, *obj2))
          {
            toDestroy.insert(obj1);
            toDestroy.insert(obj2);
          }
        }
      }

      if (mOnDestroyCallback)
      {
        for (Object * obj : toDestroy)
        {
          mOnDestroyCallback->Execute(obj);
        }
      }

      for (Object * obj : toDestroy)
      {
        mObjects.erase(obj);
      }
    }

    void Game::SteerMissiles( float dt )
    {
      static std::vector<Object*> bullets;
      static std::vector<Object*> asteroids;
      bullets.clear();
      asteroids.clear();

      // fill vectors
      for (
        ObjectContainer::const_iterator it1 = mObjects.begin(); 
        it1 != mObjects.end(); ++it1)
      {
        const std::pair<Object*const, SPObject>& pair1 = *it1;
        Object * obj1 = pair1.first;
        switch (obj1->mType)
        {
        case Object::Type::eBullet:
          bullets.push_back(obj1);
          break;
        case Object::Type::eAsteroid:
          asteroids.push_back(obj1);
          break;
        default:
          break;
        }
      }

      // steer each bullet
      for (Object * bullet : bullets)
      {
        Vector2 bulletPos(bullet->mPosX, bullet->mPosY);
        Vector2 bulletVel(bullet->mVelX, bullet->mVelY);

        // find target to steer towards
        Object * leastAngleAsteroid = nullptr;
        float leastAngleRads = 0; // quell warning
        bool bulletShouldRotateClockwise = true; // quell warning
        for (Object * asteroid : asteroids)
        {
          Vector2 asteroidPos(asteroid->mPosX, asteroid->mPosY);
          Vector2 bulletToAsteroid = asteroidPos - bulletPos;
          bool asteroidAheadofBullet 
            = bulletVel.Dot(bulletToAsteroid) > 0;
          if (!asteroidAheadofBullet)
            continue;

          float anglerad = bulletVel.AngleRad(bulletToAsteroid);
          if (!leastAngleAsteroid || anglerad < leastAngleRads)
          {
            leastAngleAsteroid = asteroid;
            leastAngleRads = anglerad;

            Vector2 rightVectorProbably(bulletVel.y, -bulletVel.x);
            bulletShouldRotateClockwise 
              = rightVectorProbably.Dot(bulletToAsteroid) > 0;

          }
        }
        // steer towards target
        if (leastAngleAsteroid)
        {
          
          float degreesPerSec = 60;
          float radsPerSec = ToRadians(degreesPerSec);

          float radsToRotateThisFrame = radsPerSec * dt;

          if (radsToRotateThisFrame > leastAngleRads)
            radsToRotateThisFrame = leastAngleRads;

          if (bulletShouldRotateClockwise)
          {
            radsToRotateThisFrame *= -1;
          }

          Vector3 bulletVel3(bulletVel, 0); 
          Vector3 rotatedBulletVel3 
            = Matrix3::RotRadZ(radsToRotateThisFrame)
            * bulletVel3;

          bullet->mVelX = rotatedBulletVel3.x;
          bullet->mVelY = rotatedBulletVel3.y;
        }
      }
    }

    bool Colliding (const Object & lhs, const Object & rhs)
    {
      float objHalfWidth = 0.5f;
      float lhsRad = objHalfWidth * GetScale(lhs.mType);
      float rhsRad = objHalfWidth * GetScale(rhs.mType);
      float totalRad = lhsRad + rhsRad;
      Vector2 lhsPos(lhs.mPosX, lhs.mPosY);
      Vector2 rhsPos(rhs.mPosX, rhs.mPosY);

      return lhsPos.DistSq(rhsPos) < (totalRad * totalRad);
    }

    namespace Message
    {
      std::string tostring( Type t )
      {
        unsigned unsignedType = (unsigned) t;
        return std::to_string(unsignedType);
      }

      Type fromString( const std::string & str )
      {
        for (unsigned i = 0; i < (unsigned) Type::eCount; ++i)
        {
          Type curType = (Message::Type) i;
          if (tostring(curType) == str)
            return curType;
        }
        return eCount;
      }

      void Handle(HandlerMap & map, SPIncomingMessage msg)
      {
        Type msgType = fromString(msg->mName);

        std::set<Handler*> & handlers = map[msgType];
        for (Handler * curHandler : handlers)
        {
          curHandler->Handle(msg);
        }
      }

      Adder::Adder( HandlerMap & map ) : mMap(map)
      {

      }

      void Adder::Add( Type t, Handler* h )
      {
        mHolder[t].insert(h);
        mMap[t].insert(h);
      }

      Adder::~Adder()
      {
        for (auto pair : mHolder)
        {
          Type curType = pair.first;
          std::set<Handler*> curSet = pair.second;
          for (Handler * h : curSet)
            mMap[curType].erase(h);
        }
      }

    } // message

    SpaceNames::SpaceNames()
    {
      mNames.push_back("Starla Moonseeker");
      mNames.push_back("Napalm Nova");
      mNames.push_back("Starcrusher Joe");
      mNames.push_back("Stella Sunset"); 
      mNames.push_back("Rosalind the Reaper");
      mNames.push_back("Space Witch Electra");
      mNames.push_back("Bounty Hunter Belinda");
      mNames.push_back("Lady Luna");
      mNames.push_back("Juliet of the Universe");
      mNames.push_back("Pandora the Explorer");
      mNames.push_back("Juno");
      mNames.push_back("Valkyrie Mira");
      mNames.push_back("Bioweapon Yamato");
      mNames.push_back("Orion Megaflare");
      mNames.push_back("Spaceman Nell");
      mNames.push_back("Future Soldier Yuri");
      mNames.push_back("Astrid Drake");
      mNames.push_back("Phoenix Firegaze");

      srand((unsigned)time(0));
      std::random_shuffle(mNames.begin(), mNames.end());
    }

    std::string SpaceNames::GetName()
    {
      if (mNames.empty())
      {
        static int i = 0;
        return "Astronaut " + std::to_string(++i);
      }
      else
      {
        std::string name = mNames.back();
        mNames.pop_back();
        return name;
      }
    }

    /*
    Vector4 GetUnitColor(ID playerID)
    {
    static std::vector<Vector4> colors;
    static bool once = true;
    if (once)
    {
    colors.push_back(GetColor(0,171,169)); //teal
    colors.push_back(GetColor(216,0,115)); //magenta
    colors.push_back(GetColor(250,104,0)); //orange
    colors.push_back(GetColor(118,96,138)); //mauve
    colors.push_back(GetColor(96,169,23)); //green
    colors.push_back(GetColor(0,80,239)); //cobalt
    colors.push_back(GetColor(227,200,0)); //yellow
    colors.push_back(GetColor(244,114,208)); //pink
    srand((unsigned)time(0));
    std::random_shuffle(colors.begin(), colors.end());
    }

    if (playerID < 8)
    return colors[playerID];
    else
    return GetColor(230,90,44);
    }
    */



    Tac::Vector4 GetColor( const Object & obj)
    {

      static std::vector<Vector4> colors;
      static bool once = true;
      if (once)
      {
        once = false;
        Vector4 teal    = GetColor(0,171,169);
        Vector4 magenta = GetColor(216,0,115);
        Vector4 orange  = GetColor(250,104,0);
        Vector4 green   = GetColor(96, 169, 23);
        Vector4 mauve   = GetColor(118,96,138);
        Vector4 cobalt  = GetColor(0,80,239);
        Vector4 yellow  = GetColor(227,200,0);
        Vector4 pink    = GetColor(244,114,208);

        Vector4 brown = GetColor(230,90,44);

        colors.push_back(teal);
        colors.push_back(magenta);
        colors.push_back(orange);
        colors.push_back(green);
        colors.push_back(mauve);
        colors.push_back(cobalt);
        colors.push_back(yellow);
        colors.push_back(pink);
        colors.push_back(brown);
      }

      Vector4 col;
      switch (obj.mType)
      {
      case Object::Type::eAsteroid:
        col = Vector4(0.2f, 0.2f, 0.2f, 1);
        break;
      case Object::Type::eShip: //fallthrough
      case Object::Type::eBullet:
        if (obj.mOwner < 8)
          col = colors[obj.mOwner];
        else
          col = colors[8];
      default:
        break;
      }

      return col;
    }

    Tac::Vector4 GetColor( int x, int y, int z)
    {
      return Vector4(
        x / 255.0f,
        y / 255.0f,
        z / 255.0f,
        1.0f);
    }

    float GetScale( Object::Type objType )
    {
      switch (objType)
      {
      case Tac::Asteroids::Object::eShip:
        return 1.0f;
        break;
      case Tac::Asteroids::Object::eAsteroid:
        return 2.0f;
        break;
      case Tac::Asteroids::Object::eBullet:
        return 0.5f;
        break;
      case Tac::Asteroids::Object::eCount:
      default:
        return 0;
        break;
      }

    }

    void GetStateMachineTweakVars( 
      unsigned & w, 
      unsigned & h, 
      unsigned & x, 
      unsigned & y )
    {
      w = 200;
      h = 200;
      x = 400;
      y = 100;
    }

  } // asteroids

} // namespace Tac
