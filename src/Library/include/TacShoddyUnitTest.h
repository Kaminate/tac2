/*!

Goal:
  
  Make a unit test framework as fast as possible

*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once


#define TAC_UNIT_TEST

namespace Tac
{

#ifdef TAC_UNIT_TEST

//#define TAC_REGISTER_UNIT_TEST(fn)

  // you know what? the fastest way of making a unit test framework
  // is to not unit test
  // fuck this shit

#endif

} // Tac
