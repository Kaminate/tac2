/*
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once
#include "TacVector3.h"

namespace Tac
{
  struct Color
  {
    // add more if you want
    static const Vector3 red;
    static const Vector3 green;
    static const Vector3 blue;

    static const Vector3 yellow;
    static const Vector3 orange;
    static const Vector3 lime;
    static const Vector3 teal;
    static const Vector3 purple;

    static const Vector3 black;
    static const Vector3 white;

    static const Vector3 darkGrey;
    static const Vector3 mediumGrey;
    static const Vector3 lightGrey;
  };
}
