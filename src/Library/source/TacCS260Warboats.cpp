// Nathan Park
#include "TacCS260Warboats.h"
#include <string>
namespace Tac
{
  Warboats::Warboats(unsigned width , unsigned height )
    : mWidth(width), mHeight(height), mCurrentPlayerTurn(0)
  {

  }

  void Warboats::RenderForPlayer( unsigned index )
  {
    char toDraw = sError;
    Player & currPlayer = mPlayers[index];
    Player & opponent = mPlayers[1 - index];

    std::cout << "Opponent's ocean:" << std::endl;
    RenderOcean(opponent, currPlayer, false);
    std::cout << "Your ocean:" << std::endl;
    RenderOcean(currPlayer, opponent, true);
  }

  void Warboats::ListBoatsOfPlayer( unsigned index )
  {
    for (Boat & iBoat : mPlayers[index].mBoats)
    {
      std::cout << iBoat.mName;
      for (unsigned i = 0; i < 25 - iBoat.mName.length(); ++i)
        std::cout << " ";

      if (iBoat.Alive(mPlayers[1 - index].mShotsFired))
        std::cout << "[Alive]";
      else
        std::cout << "[Destroyed]";

      std::cout << std::endl;
    }
  }

  void Warboats::RenderOcean( 
    const Player & boatOwningPlayer,
    const Player & shotOwningPlayer,
    bool renderUnhitBoats )
  {
    char toDraw;
    for (unsigned r = mHeight - 1; r != (unsigned)-1; --r)
    {
      // render row indexes
      std::cout << r;
      for (int c = 0; c < mWidth; ++c)
      {
        Vector2 curPos((float)r, (float)c);
        bool boatHere = boatOwningPlayer.HasBoatAtPosition(curPos);
        if (shotOwningPlayer.HasShotAtPosition(curPos))
        {
          if (boatHere)
            toDraw = sHit;
          else
            toDraw = sMiss;
        }
        else if (boatHere)
          toDraw = renderUnhitBoats ? sBoat : sWater;
        else
          toDraw = sWater;
        std::cout << toDraw;
      }
      std::cout << std::endl;
    }

    // render column indexes
    std::cout << " ";
    for (int c = 0; c < mWidth; ++c)
      std::cout << c;
    std::cout << std::endl;
  }

  void Warboats::Update( Vector2 shot )
  {
    mPlayers[mCurrentPlayerTurn++].mShotsFired.push_back(shot);
    if (mCurrentPlayerTurn == mPlayers.size())
      mCurrentPlayerTurn = 0;
  }

  bool Warboats::PlayerWin( unsigned index ) const
  {
    const std::vector<Vector2> & shotsFired = mPlayers[index].mShotsFired;
    const Player & opponent = mPlayers[1 - index];
    for (const Boat & iBoat : opponent.mBoats)
    {
      if (iBoat.Alive(shotsFired))
        return false;
    }
    return true;
  }

  void Warboats::CreateSampleGame()
  {
    for (unsigned i = 0; i < 2; ++i)
    {
      Player p;
      p.PlaceBoatRandomValid(Boat::PatrolBoat, mWidth, mHeight);
      p.PlaceBoatRandomValid(Boat::AircraftCarrier, mWidth, mHeight);
      p.PlaceBoatRandomValid(Boat::Battleship, mWidth, mHeight);
      p.PlaceBoatRandomValid(Boat::Destroyer, mWidth, mHeight);
      p.PlaceBoatRandomValid(Boat::Submarine, mWidth, mHeight);
      mPlayers.push_back(p);
    }
    mCurrentPlayerTurn = rand() % 2;
  }

  Boat::Boat( Type BoatType, Vector2 topLeftPos, bool horizontal)
    : mTopLeftPosition(topLeftPos)
    , mHorizontal(horizontal)
  {
    switch (BoatType)
    {
    case Tac::Boat::AircraftCarrier:
      mName = "Aircraft Carrier";
      mLength = 5;
      break;
    case Tac::Boat::Battleship:
      mName = "Battleship";
      mLength = 4;
      break;
    case Tac::Boat::Submarine:
      mName = "Submarine";
      mLength = 3;
      break;
    case Tac::Boat::Destroyer:
      mName = "Destroyer";
      mLength = 3;
      break;
    case Tac::Boat::PatrolBoat:
      mName = "Patrol Boat";
      mLength = 2;
      break;
    default:
      break;
    }
  }

  Boat::Boat(
    const std::string & name, Vector2 topLeft, int len, bool horiz )
    : mName(name)
    , mTopLeftPosition(topLeft)
    , mLength(len)
    , mHorizontal(horiz)
  {

  }

  bool Boat::Alive( const std::vector<Vector2> & shotsFired ) const
  {
    for (int i = 0; i < mLength; ++i)
    {
      Vector2 boatPos = mTopLeftPosition;
      Vector2 dir = mHorizontal ? Vector2(0, 1) : Vector2(-1, 0);
      boatPos += dir * (float)i;

      bool hit = false;
      for (const Vector2 & shot : shotsFired)
      {
        if (shot == boatPos)
        {
          hit = true;
          break;
        }
      }
      if (hit == false)
        // this segment has not been hit by any shot, we are alive
          return true;
    }

    // every segment has been hit, we are dead
    return false;

  }

  bool Boat::Overlaps( const Boat & other ) const
  {
    for (int i = 0; i < mLength; ++i)
    {
      Vector2 dir = mHorizontal ? Vector2(0, 1) : Vector2(-1, 0);
      Vector2 boatPos = mTopLeftPosition + dir * (float)i;

      for (int j = 0; j < other.mLength; ++j)
      {
        Vector2 odir = other.mHorizontal ? Vector2(0, 1) : Vector2(-1, 0);
        Vector2 oboatPos = other.mTopLeftPosition + odir * (float)j;

        if (boatPos == oboatPos)
          return true;
      }
    }
    return false;
  }

  bool Player::HasShotAtPosition( const Vector2 & pos ) const
  {
    return std::find(mShotsFired.begin(), mShotsFired.end(), pos)
      != mShotsFired.end();
  }

  bool Player::HasBoatAtPosition( const Vector2 & pos ) const
  {
    for (const Boat & iBoat : mBoats)
    {
      for (int i = 0; i < iBoat.mLength; ++i)
      {
        Vector2 boatPos = iBoat.mTopLeftPosition;
        Vector2 dir = iBoat.mHorizontal ? Vector2(0, 1) : Vector2(-1, 0);
        boatPos += dir * (float)i;

        if (boatPos == pos)
        {
          return true;
        }
      }
    }
    return false;
  }

  void Player::PlaceBoatRandomValid(
    Boat::Type boatType, 
    unsigned w, 
    unsigned h)
  {
    const unsigned numTries = 100;
    for(unsigned i = 0; i < numTries; ++i)
    {
      int r = rand() % h;
      int c = rand() % w;
      bool horizontal = rand() % 2 == 0;
      Vector2 pos((float)r, (float)c);
      Boat warBoat(boatType, pos, horizontal);

      bool boatValid = true;

      for (int j = 0; j < warBoat.mLength; ++j)
      {
        Vector2 dir = horizontal ? Vector2(0, 1) : Vector2(-1, 0);
        Vector2 curPiecePos = pos + dir * (float)j;
        int curRow = (int)curPiecePos.x;
        int curCol = (int)curPiecePos.y;
        if (curRow == -1)
        {
          boatValid = false;
          break;
        }
        if (curCol == w)
        {
          boatValid = false;
          break;
        }

        for (const Boat & otherBoat : mBoats)
        {
          if (warBoat.Overlaps(otherBoat))
          {
            boatValid = false;
            break;
          }
        }

        if (!boatValid)
          break;
      }

      if (boatValid)
      {
        mBoats.push_back(warBoat);
        break;
      }
    }
    // if it loops 100 times without placing the boat, the player just
    // won't have the boat
  }

} // Tac
