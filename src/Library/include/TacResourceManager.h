/*!
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacResource.h"
#include "TacSingleton.h"
#include <vector>

namespace Tac
{

  class ResourceManager : public Singleton<ResourceManager>
  {
    friend class Singleton<ResourceManager>;
  public:
    ~ResourceManager();

    void AddResource(Resource*toAdd); 

    // returns num reloaded
    int UpdateResources();
    void Clear();
    
  private:
    std::vector<Resource*> mResources; // owned
   
  };
} // Tac

