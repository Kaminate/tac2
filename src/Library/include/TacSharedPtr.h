/*!
\author Nathan Park
\brief
*/

#ifndef __TAC_SHAREDPTR_H_
#define __TAC_SHAREDPTR_H_

#include <memory>
#include <fstream> // std::ifstream

namespace Tac
{
  // defines both shared and weak
#define DefineSharedPtr(T) \
  typedef std::shared_ptr<T>SP##T; \
  typedef std::weak_ptr<T>WP##T
  DefineSharedPtr(bool);
  DefineSharedPtr(float);
  DefineSharedPtr(int);
  DefineSharedPtr(unsigned);
  DefineSharedPtr(char);
  
  template <typename T>
  std::ifstream & operator >> (std::ifstream & ifs, std::shared_ptr<T> & t)
  {
    return ifs >> *t;
  }

  /*template <typename T>
  bool operator<(
    const std::shared_ptr<T> & lhs, 
    const std::shared_ptr<T> & rhs)
  {
    return lhs->get() < rhs->get();
  }*/

  
  template<typename T>
  struct LessSharedPtr
  {
    bool operator () (
      const std::shared_ptr<T> & lhs, 
      const std::shared_ptr<T> & rhs)
    {
      return lhs.get() < rhs.get();
    }
  };
  

} // Tac

#endif
