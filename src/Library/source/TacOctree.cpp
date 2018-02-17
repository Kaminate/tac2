
#include "TacOctree.h"
#include "TacErrorIf.h"

#include <iostream>
#include <queue>
#include <set>

namespace Tac
{
  Octree::Octree(
    int numLevels, 
    const Vector3 & pos, 
    const Vector3 & halfDim ) 
    : mRootCenter( pos )
    , mHalfDim( halfDim )
  {
    ErrorIf(numLevels < 1, "Must have at least one level");
    mLevels.resize(numLevels);
    mLevelNodeCounts.resize(numLevels, 0);
    for ( int i = 0; i < numLevels; ++i )
    {
      std::vector<OctreeNode> & curLevel = mLevels[ i ];
      int nodesForCurLevel = 1 << 3 * i; // 1, 8, 64, etc
      // NOTE( N8 ): why are we doing this? isn't it supposted to be sparse???
      curLevel.resize(nodesForCurLevel);
    }

    OctreeNode* root = GetRoot();
    root->used = true;
    root->childTileIndex = Allocate8Nodes(1);
    Initialize8Nodes(1, root->childTileIndex);
    mLevelNodeCounts[ 0 ] = 1;
  }

  // returns the index of the first node
  unsigned Octree::Allocate8Nodes(unsigned levelIndex)
  {
    unsigned & levelNodeCounts = mLevelNodeCounts[levelIndex];
    const unsigned childIndex = levelNodeCounts;
    levelNodeCounts += 8;

    return childIndex;
  }

  void Octree::Initialize8Nodes(
    unsigned levelIndex, unsigned firstNodeIndex)
  {
    OctreeNode * firstNode = &mLevels[levelIndex][firstNodeIndex];
    for(unsigned i = 0; i< 8; ++i)
    {
      OctreeNode * curNode = firstNode + i;
      curNode->childTileIndex = OctreeNode::NULL_CHILD;
      curNode->used = false;
    }
  }

  OctreeNode * Octree::GetRoot()
  {
    return &mLevels[0][0];
  }

  // @brief
  //   OpenGL Insights 22.5.4 Node Subdivision
  //   Call this with one thread per fragment
  // @indexOfUnusedNodes - level index of nodes to mark as used
  void Octree::MarkCellToAdd( Vector3 pos, int levelIndexOfUnusedNodes )
  {
    // traverse entire tree down to the level
    OctreeNode * curNode = GetRoot();
    Vector3 nodeCenter = mRootCenter;
    Vector3 nodeHalfDim = mHalfDim;
    int curLevel = 0;
    while (curLevel < levelIndexOfUnusedNodes )
    {
      // pre adjust halfDim
      nodeHalfDim /= 2;

      // find out what octant the child belongs to
      int octant = 0;
      for( int i = 0; i < 3; ++i )
      {
        int cmp = pos[i] > nodeCenter[i];
        octant |= ( 1 << i ) * cmp;
      }
      Vector3 dir = Direction(octant);
      nodeCenter.x += nodeHalfDim.x * dir.x;
      nodeCenter.y += nodeHalfDim.y * dir.y;
      nodeCenter.z += nodeHalfDim.z * dir.z;

      curNode = &mLevels[++curLevel][curNode->childTileIndex] + octant;
    }
    curNode->used = true;
  }

  bool Octree::ContainsPoint( const Vector3& pos )
  {
    Vector3 mini = mRootCenter - mHalfDim;
    if (pos.x < mini.x || pos.y < mini.y || pos.z < mini.z )
      return false;

    Vector3 maxi = mRootCenter + mHalfDim;
    if (pos.x > maxi.x || pos.y > maxi.y || pos.z > maxi.z)
      return false;

    return true;
  }

  void Octree::Print()const
  {
    std::queue<unsigned> nodeIndexes;
    nodeIndexes.push(0);
    // print info about root
    std::cout << "node" << std::endl;

    auto PrintOctet = 
      [&nodeIndexes, this](unsigned parentLevel, unsigned index)
    {
      const OctreeNode * octet = &mLevels[parentLevel + 1][0] + index;

      // need the parent position to get our position
      for( const OctreeNode * node = octet; node != octet + 8; ++node)
      {
        if(node ->used)
        {
          // todo: print info about node
          std::cout << "node ";

          // add children ( will do nothing for leaf nodes )
          if( node->childTileIndex != OctreeNode::NULL_CHILD )
          {
            nodeIndexes.push(node->childTileIndex);
          }
        }
      }
    };

    for(unsigned parentLevel = 0; 
      parentLevel < mLevels.size();
      ++parentLevel)
    {
      unsigned indexesInLevel = nodeIndexes.size();
      //std::cout 
        //<< "level " << parentLevel << ": " 
        //<< indexesInLevel << " nodes" << std::endl;;
      while(indexesInLevel--)
      {
        unsigned index = nodeIndexes.front();
        nodeIndexes.pop();

        PrintOctet(parentLevel, index);
      }
      std::cout << std::endl;
    }


  }

  void Octree::Clear()
  {
    for(unsigned level = 1; level < mLevels.size(); ++level)
    {
      for(OctreeNode& node : mLevels[level])
      {
        node.childTileIndex; // maybe unneeded?
        node.used = false;
      }
    }
    for(unsigned& count : mLevelNodeCounts)
    {
      count = 0;
    }
    // reset root
    mLevelNodeCounts[0] = 1;
    mLevels[0][0].used = true;
  }

  void Octree::AllocateMarkedNodes( unsigned levelIndexToMark )
  {
    std::vector<OctreeNode> & curLevel = mLevels[levelIndexToMark];

    unsigned nextLevel = levelIndexToMark + 1;
    for(OctreeNode & node : curLevel)
    {
      if(!node.used)
        continue;

      node.childTileIndex = Allocate8Nodes(nextLevel);
      // maybe pull this out
      Initialize8Nodes(nextLevel, node.childTileIndex); 
    }
  }

  const Vector3& Octree::GetPos() const
  {
    return mRootCenter;
  }

  const Vector3& Octree::GetHalfDim() const
  {
    return mHalfDim;
  }

  const OctreeNode* Octree::GetLevel( unsigned levelIndex )const
  {
    return &mLevels[levelIndex][0];
  }

  unsigned Octree::GetNumLevels() const
  {
    return mLevels.size();
  }

  unsigned Octree::GetDimension() const
  {
    // () for c4554
    return 1 << ( mLevels.size() - 1 );
  }

} // Tac
