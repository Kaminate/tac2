#ifndef __TAC_OCTREE_H_
#define __TAC_OCTREE_H_

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif


#include "TacVector4.h"
#include "TacVector3.h"
#include "TacErrorIf.h"

#include <vector>

namespace Tac
{
  // OctreeNodes are grouped in tiles of 8
  struct OctreeNode
  {
    unsigned used; // 0 unused 1 used; (bitflags, but only 1 bit)
    unsigned childTileIndex; // -1 is nullptr
    unsigned padding[2];
    Vector4 color;
    static const unsigned NULL_CHILD = UINT_MAX;
  };

  static_assert(sizeof(OctreeNode) % 8 == 0, "OctreeNode must packed");

  class Octree
  {
  public:
    TacDLLAPI Octree( int numLevels, const Vector3 & pos, const Vector3 & halfDim );
    // returns the index of the first node
    TacDLLAPI bool ContainsPoint( const Vector3& pos);
    TacDLLAPI void Clear();
    TacDLLAPI void Print()const;
    TacDLLAPI unsigned GetNumLevels() const;
    TacDLLAPI unsigned GetDimension() const;
    TacDLLAPI Octree & operator =(const Octree & rhs); 

    static inline Vector3 Direction( unsigned childIndex )
    {
      // childIndex is zyx in binary
      // direction  is xyz
      //   this means we have to flip the order
      switch (childIndex)
      {                     //x   y   z
      case 0: return Vector3(-1, -1, -1); // 000
      case 1: return Vector3( 1, -1, -1); // 001
      case 2: return Vector3(-1,  1, -1); // 010
      case 3: return Vector3( 1,  1, -1); // 011
      case 4: return Vector3(-1, -1,  1); // 100
      case 5: return Vector3( 1, -1,  1); // 101
      case 6: return Vector3(-1,  1,  1); // 110
      case 7: return Vector3( 1,  1,  1); // 111
      default: 
        DebugError("Bad Index %u", childIndex);
        return Vector3(0,0,0);
      }
    }

    // @brief
    //   OpenGL Insights 22.5.4 Node Subdivision
    //   Call this with one thread per fragment
    // @indexOfUnusedNodes - level index of nodes to mark as used
    TacDLLAPI void MarkCellToAdd( 
      Vector3 pos,
      int levelIndexOfUnusedNodes );

    TacDLLAPI void AllocateMarkedNodes( unsigned levelIndexToMark );
    TacDLLAPI const Vector3&GetPos()const;
    TacDLLAPI const Vector3&GetHalfDim()const;
    TacDLLAPI const OctreeNode* GetLevel(unsigned levelIndex) const;
  private:

    OctreeNode * GetRoot();
    unsigned Allocate8Nodes(unsigned levelIndex);
    void Initialize8Nodes(unsigned levelIndex, unsigned firstNodeIndex);
    std::vector< std::vector< OctreeNode > > mLevels;
    std::vector<unsigned> mLevelNodeCounts;

    // = delete; is c++14 ( or vs2013 )
    // hmm this is gonna be a lot harder than I initially thought
    // ( I knew it was gonna be hard, but still underestimated )
    // 
    // I think what I have to do is make it pretty at EVERY STEP
    // and take FUCKING SMALL STEPS
    // 
    // I may want to consider using a NON-SPARSE octree
    // (read: big ass lower-resolution array)
    // and try to get in some nice bouncing lighting.
    //
    // I should start with a cornell box
    Vector3 mRootCenter;
    Vector3 mHalfDim;
  };

} // Tac

#endif
