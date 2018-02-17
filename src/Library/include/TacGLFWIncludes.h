/*!
\author Nathan Park
\brief
*/
#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // no min/max macro
#include <windows.h> // hwnd
#include <commdlg.h> // openfilename

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include "TacGLIncludes.h" // include glew before gl
#include "GLFW/glfw3.h" // includes gl
#include <GLFW/glfw3native.h>

