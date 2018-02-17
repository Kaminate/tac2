/*!
\author Nathan Park
\brief
*/

#ifndef __TAC_CS260_WARBOATS_H_
#define __TAC_CS260_WARBOATS_H_

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacVector2.h"
#include <vector>

namespace Tac
{
  struct Boat
  {
    enum Type{AircraftCarrier, Battleship,Submarine,Destroyer,PatrolBoat};
    Boat(Type BoatType, Vector2 topLeftPos, bool horizontal);
    Boat(const std::string & name, Vector2 topLeft, int len, bool horiz);
    std::string mName;
    Vector2 mTopLeftPosition;
    int mLength;
    bool mHorizontal;
    bool Alive(const std::vector<Vector2> & shotsFired)const ;
    bool Overlaps(const Boat & other) const;
  };


  struct Player
  {
    std::vector<Boat> mBoats;
    std::vector<Vector2> mShotsFired; // [r, c]
    bool HasShotAtPosition(const Vector2 & pos)const;
    bool HasBoatAtPosition(const Vector2 & pos)const;
    void PlaceBoatRandomValid(Boat::Type boatType, unsigned w, unsigned h);
  };

  struct Warboats 
  {
    // rows
    // ^
    // |
    // |
    // +---> column
    Warboats(unsigned width = 10, unsigned height = 10);
    void Update(Vector2 shot);
    bool PlayerWin(unsigned index) const;

    void CreateSampleGame();

    int mWidth;
    int mHeight;
    std::vector<Player> mPlayers;
    int mCurrentPlayerTurn;
    void RenderForPlayer(unsigned index);
    void ListBoatsOfPlayer(unsigned index);
    void RenderOcean(
      const Player & boatOwningPlayer,
      const Player & shotOwningPlayer,
      bool renderUnhitBoats);

    static const char sHit = 'x';
    static const char sMiss = 'o';
    static const char sWater = ' ';
    static const char sBoat = '#';
    static const char sError = '?';
  };
} // Tac

#endif
