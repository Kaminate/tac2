/*!
\author Nathan Park
*/
#include "TacResourceManager.h"
#include "TacErrorIf.h"
#include <windows.h>

namespace Tac
{
  ResourceManager::~ResourceManager()
  {
    Clear();
  }
  void ResourceManager::Clear()
  {
    for (Resource * todelete : mResources)
      delete todelete;
    mResources.clear();
  }

  void ResourceManager::AddResource( Resource*toAdd )
  {
    mResources.push_back(toAdd);
  }

  int ResourceManager::UpdateResources()
  {
    int numReloaded = 0;
    for (Resource*toUpdate : mResources)
    {
      if(toUpdate->IsOutOfDate())
      {
        std::string errors;
        do 
        {
          errors = toUpdate->TryReload();
          if(!errors.empty())
          {
            int value = MessageBox(
              nullptr,
              errors.c_str(),
              "Resource reload error",
              MB_RETRYCANCEL );
            switch (value)
            {
            case IDCANCEL:
              errors.clear();
              toUpdate->UpdateFileTimes();
              break;
            case IDRETRY: break;
            default: __debugbreak(); break;
            }
          }
        } while (!errors.empty());

        ++numReloaded;
      }
    }
    return numReloaded;
  }
} // Tac
