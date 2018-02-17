// This file copy-pasted from
// http://dslweb.nwnexus.com/~ast/dload/guicon.htm

// It opens a console window, then redirects the stdout, stdin, and stderr
// to the colsole window

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#ifndef __GUICON_H__
#define __GUICON_H__

TacDLLAPI void RedirectIOToConsole();

#endif

