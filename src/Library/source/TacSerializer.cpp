/*!
\author Nathan Park

\par Project: Tac Engine

\brief

*/
#include "TacSerializer.h"

namespace Tac
{
  /*
  static LoadCompFn gLoadCompFns[Component::Type::eCount];
  void RegisterLoadFn(Component::Type compType, LoadCompFn fn)
  {
    gLoadCompFns[compType] = fn;
  }
  LoadCompFn GetComponentLoadFn(Component::Type compType)
  {
    return gLoadCompFns[compType];
  }

  static SaveCompFn gSaveCompFns[Component::Type::eCount];
  void RegisterSaveFn(Component::Type compType, SaveCompFn fn)
  {
    gSaveCompFns[compType] = fn;
  }
  SaveCompFn GetComponentSaveFn(Component::Type compType)
  {
    return gSaveCompFns[compType];
  }
  */

  Saver::Saver( const std::string & filename ) 
    : mNumTabs(0)
    , mFile(filename)
    , mSpacesPerTab(2)
  {

  }

  bool Saver::Ok() const
  {
    return mFile.is_open();
  }

  void Saver::Tab()
  {
    ++mNumTabs;
  }

  void Saver::DeTab()
  {
    --mNumTabs;
  }

  //void Saver::CloseFile()
  //{
  //  mFile.close();
  //}


  Loader::Loader( const std::string & filename ) : mFile(filename)
  {

  }

  bool Loader::Ok() const
  {
    return mFile.is_open();
  }

} // Tac


