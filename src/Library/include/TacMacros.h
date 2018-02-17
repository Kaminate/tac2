/*!
\author Nathan Park
\brief
*/

#ifndef __TAC_MACROS_H_
#define __TAC_MACROS_H_

#include <memory>

namespace Tac
{
#define Stringify(s) #s

#define StaticAssert(cond, msg) \
  typedef char staticassert##msg[(cond)?1:-1]
#define UnusedParameter(x) x

} // Tac

#endif
