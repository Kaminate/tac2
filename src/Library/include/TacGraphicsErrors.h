/*!

\author Nathan Park
\par contributors:

\file GraphicsErrors.h
\par Project: Tac Engine
\par "All content c 2014 DigiPen (USA) Corporation, all rights reserved.

\brief
   Contains the general opengl error check, as well 
    as operation specific error messages

*/
#pragma once

namespace Tac
{
  void CheckOpenGLErrors();
  void CheckglMapBufferRangeErrors();
  void CheckFramebufferStatus();
}

