#include "TacGLFW.h"

#include "TacGLFWIncludes.h"
#include "TacErrorIf.h"
#include "TacTweak.h"

//#include <ShObjIdl.h> //  idialogfileevents

#include "TacException.h"
#include "TacMacros.h"

#include "TacSystemInput.h"

namespace Tac
{
  void GLFWErrorCallback(int error, const char * desc)
  {
    DebugError("GLFW error %d %s\n", error, desc);
  }

  // GLFW

  GLFW::GLFW()
  {
    if (glfwInit() != GL_TRUE)
    {
      throw Exception("glfwinit failed");
    }
    glfwSetErrorCallback(GLFWErrorCallback);
  }

  GLFW::~GLFW()
  {
    glfwTerminate();
  }

  TacDLLAPI void GLFW::PollEvents()
  {
    glfwPollEvents();
  }

  // window

  Window * Window::GetInstance()
  {
    return gWindow;
  }

  Window * Window::gWindow = nullptr;
  Window::Window() 
    : mWindow(nullptr)
  {
    ErrorIf(gWindow, "window already exists");
    gWindow = this;
  }

  Window::~Window()
  {
    if (mWindow) 
    {
      glfwDestroyWindow(mWindow);
    }
    gWindow = nullptr;
  }

  void OnMouseButton(GLFWwindow *wind, int button, int action, int mods)
  {
    UnusedParameter(mods);
    UnusedParameter(wind);
#ifdef TACTWEAK
    if (TwEventMouseButtonGLFW(button, action))
    {
      return;
    }
#else
    UnusedParameter(button);
    UnusedParameter(action);
#endif
    InputMsg::MouseClick mouseData;
    mouseData.mAction = action;
    mouseData.mButton = button;
    mouseData.mMods = mods;
    gInputSystem->SendMsg(InputMsg(mouseData));
  }
  void OnCursorMove(GLFWwindow *wind, double x, double y)
  {
    UnusedParameter(wind);
#ifdef TACTWEAK
    if (TwEventMousePosGLFW((int)x, (int)y))
    {
      return;
    }
#else
#endif
    InputMsg::MouseMove mouseMove;
    mouseMove.mX = x;
    mouseMove.mY = y;
    gInputSystem->SendMsg(InputMsg(mouseMove));
  }

  // GL_TRUE if entered, GL_FALSE if exited
  void OnCursorEnter(GLFWwindow *wind, int entered)
  {
    UnusedParameter(wind);
    UnusedParameter(entered);
  }

  void OnScroll(GLFWwindow * wind, double xOffset, double yOffset)
  {
    // x or y?
#ifdef TACTWEAK
    TwEventMouseWheelGLFW((int)yOffset);
#else 
#endif
    UnusedParameter(wind);
    InputMsg::MouseScroll scroll;
    scroll.mOffsetX = xOffset;
    scroll.mOffsetY = yOffset;
    gInputSystem->SendMsg(InputMsg(scroll));
  }

  void OnKey(
    GLFWwindow * wind,
    int key, 
    int scancode, //sysyem-specific scancdoe
    int action, // GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
    int mods) // bit field descriing modifier keys
  {
    UnusedParameter(wind);
    // temp for 260
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //glfwSetWindowShouldClose(wind, GL_TRUE);
#ifdef TACTWEAK

    int handled = 0;

    // NOTE:
    // we sohuld be about to just use tweventkeyglfw and be down with,
    // but its not working for some reason, so heres my hack to fix it
    // kinda
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
      case GLFW_KEY_BACKSPACE:
        handled = TwKeyPressed(TW_KEY_BACKSPACE, TW_KMOD_NONE);
        break;
      case GLFW_KEY_RIGHT:
        handled = TwKeyPressed(TW_KEY_RIGHT, TW_KMOD_NONE);
        break;
      case GLFW_KEY_LEFT:
        handled = TwKeyPressed(TW_KEY_LEFT, TW_KMOD_NONE);
        break;
      case GLFW_KEY_UP:
        handled = TwKeyPressed(TW_KEY_UP, TW_KMOD_NONE);
        break;
      case GLFW_KEY_DOWN:
        handled = TwKeyPressed(TW_KEY_DOWN, TW_KMOD_NONE);
        break;
      case GLFW_KEY_ENTER:
        handled = TwKeyPressed(TW_KEY_RETURN, TW_KMOD_NONE);
        break;
      case GLFW_KEY_ESCAPE:
        handled = TwKeyPressed(TW_KEY_ESCAPE, TW_KMOD_NONE);
        break;
      default:
        break;
      }
    }
    if (!handled)
      TwEventKeyGLFW(key, action);
#endif
    InputMsg::Key keyEvent;
    keyEvent.action = action;
    keyEvent.key = key;
    keyEvent.mods = mods;
    keyEvent.scancode = scancode;
    gInputSystem->SendMsg(InputMsg(keyEvent));
  }

  void OnChar(GLFWwindow * wind, unsigned int codepoint)
  {
    UnusedParameter(wind);
#ifdef TACTWEAK

    TwEventCharGLFW(codepoint, GLFW_PRESS);
#else 
    UnusedParameter(codepoint);
#endif
  }

  void OnResize(GLFWwindow * wind, int w, int h)
  {
    UnusedParameter(wind);
#ifdef TACTWEAK

    TwWindowSize(w, h);
#else
    UnusedParameter(w);
    UnusedParameter(h);
#endif
    // todo: change camera aspect
  }

  void Window::Open( 
    unsigned width, 
    unsigned height, 
    const char * title )
  {
    ErrorIf(mWindow, "window already opened");
    mWindow = glfwCreateWindow(
      width, 
      height, 
      title, 
      nullptr, // glfwGetPrimaryMonitor(), <-- fullscreen, fux w/ res
      nullptr);

    if (mWindow) 
    {
      // create opengl context
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
      glfwMakeContextCurrent(mWindow);

      // directly redirect GLFW events to AntTweakBar
      glfwSetKeyCallback(mWindow,OnKey);
      //glfwSetKeyCallback(mWindow,(GLFWkeyfun)TwEventKeyGLFW);
      glfwSetCharCallback(mWindow, OnChar);
      //glfwSetCharCallback(mWindow, (GLFWcharfun)TwEventCharGLFW);
      glfwSetMouseButtonCallback(mWindow, OnMouseButton);
      glfwSetCursorPosCallback(mWindow, OnCursorMove);
      glfwSetCursorEnterCallback(mWindow,OnCursorEnter);
      glfwSetScrollCallback(mWindow,OnScroll);
      // send window size events to AntTweakBar
      glfwSetWindowSizeCallback(mWindow, OnResize);

      const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

      // center
      glfwSetWindowPos(
        mWindow, 
        (mode->width - width) / 2,
        (mode->height - height) / 2);
      // top left
      glfwSetWindowPos(mWindow,0,0);
      // top right
      glfwSetWindowPos(mWindow, mode->width - width - 50,50);
    }
  }

  void Window::SwapBuffers()
  {
    if(mWindow)
      glfwSwapBuffers(mWindow);
  }

  bool Window::IsOpen() const
  {
    return mWindow != nullptr;
  }

  bool Window::ShouldClose() const
  {
    return //mWindow && <-- no, let them call IsOpen()?
      glfwWindowShouldClose(mWindow) != 0;
  }

  int Window::GetWidth() const
  {
    int w; glfwGetWindowSize(mWindow, &w, nullptr);
    return w;
  }

  int Window::GetHeight() const
  {
    int h; glfwGetWindowSize(mWindow, nullptr, &h);
    return h;
  }

  std::string GetFilter(
    const std::vector<Window::FileTypeThing> & fileTHings)
  {
    std::string filter;
    std::vector<unsigned> nullIndexes;
    for (const Window::FileTypeThing & thign : fileTHings)
    {
      filter += thign.documentTypeName;
      nullIndexes.push_back(filter.size());
      filter += " ";
      filter += "*.";
      filter += thign.documentExtName; // <-- for all files, make this "*"
      nullIndexes.push_back(filter.size());
      filter += " ";
    }
    for(unsigned index : nullIndexes)
    {
      filter[index] = '\0';
    }
    return filter;
  }

  bool Window::OpenFile( std::string & filepath ,
    const std::vector<FileTypeThing> & fileTHings)
  {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name

    std::string filter = GetFilter(fileTHings);
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = glfwGetWin32Window(mWindow);;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter.c_str();//"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR ;

    // Display the Open dialog box. 

    if (GetOpenFileName(&ofn)==TRUE) 
    {
      filepath = ofn.lpstrFile;
      return true;
    }
    return false;
  }

  /*
  // Instance creation helper
  HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
  {
  *ppv = NULL;
  CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
  HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hr))
  {
  hr = pDialogEventHandler->QueryInterface(riid, ppv);
  pDialogEventHandler->Release();
  }
  return hr;
  }
  */

  bool Window::SaveFile(
    std::string & filepath,
    const std::vector<FileTypeThing> & saveThings)
  {
    char szFile[260];       // buffer for file name

    std::string filter = GetFilter(saveThings);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = glfwGetWin32Window(mWindow);
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr; // crashes if not null
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    // append the first filetype to the file if the user left the extension blank
    ofn.lpstrDefExt = saveThings[0].documentExtName.c_str(); 
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    if (GetSaveFileName(&ofn) == TRUE)
    {
      filepath = ofn.lpstrFile;
      return true;
    }
    return false;
  }

  float Window::GetAspect() const
  {
    float aspect = (float)GetWidth();
    aspect /= (float) GetHeight();
    return aspect;
  }

  bool Window::IsFocused() const
  {
    return glfwGetWindowAttrib(mWindow, GLFW_FOCUSED) != 0;
  }

  GLFWwindow * Window::GetGLFWWindow() const
  {
    return mWindow;
  }


} // Tac

