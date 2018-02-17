/*!
\author Nathan Park
\brief
*/

#ifndef __TAC_CALLBACK_H_
#define __TAC_CALLBACK_H_

#include <memory>
#include "TacSharedPtr.h"

namespace Tac
{
	class Callback
	{
	public:
		virtual void Execute() = 0;
	};
  DefineSharedPtr(Callback);
} // Tac

#endif
