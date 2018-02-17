/*!
eauthor Nathan Park
\brief
*/

#ifndef __TAC_GLFW_
#define __TAC_GLFW_

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include <string>
#include <vector>

struct GLFWwindow;

namespace Tac
{
  class Window
  {
    static Window * gWindow;
  public:
    TacDLLAPI Window();
    TacDLLAPI static Window * GetInstance();
    TacDLLAPI ~Window();
    TacDLLAPI void Open(unsigned width, unsigned height, const char * title);
    TacDLLAPI void SwapBuffers();

    //TacDLLAPI GLFWwindow * operator ->();
    //TacDLLAPI operator GLFWwindow*();
    TacDLLAPI bool IsOpen() const;
    TacDLLAPI bool ShouldClose() const;
    TacDLLAPI int GetWidth() const;
    TacDLLAPI int GetHeight() const;
    TacDLLAPI float GetAspect() const;
    TacDLLAPI bool IsFocused() const;
    TacDLLAPI GLFWwindow * GetGLFWWindow() const;

    
    struct FileTypeThing
    {
      FileTypeThing(const std::string & name, const std::string & ext)
        : documentTypeName(name)
        , documentExtName(ext)
      {
      }
      std::string documentTypeName;
      std::string documentExtName;
    };
    // uses the windows dialog box
    TacDLLAPI bool OpenFile(std::string & filepath,
      const std::vector<FileTypeThing> & fileTHings);

    TacDLLAPI bool SaveFile(
      std::string & filepath, 
      const std::vector<FileTypeThing> & saveThings);


  private:
    GLFWwindow * mWindow;
  };

  class GLFW
  {
  public:
    TacDLLAPI GLFW();
    TacDLLAPI ~GLFW();
    TacDLLAPI void PollEvents();
  };
}

#endif