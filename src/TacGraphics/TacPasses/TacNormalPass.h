
// structure based off KoRE rendering engine

#ifndef TAC_NORMAL_PASS_H_
#define TAC_NORMAL_PASS_H_

#include <string>
#include <vector>
#include "TacGraphics\TacShaderProgramPass.h"

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

namespace Tac
{
  TacDLLAPI void AddNormalPass(class GraphicsSystem * graphicsSys);
}
#endif