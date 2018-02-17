/*!
\author Nathan Park
\brief
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#ifndef __TAC_SYSTEM_INPUT_H
#define __TAC_SYSTEM_INPUT_H

#include "TacSystem.h"
#include <functional> // std function
#include <vector>
#include <memory>
#include "TacGLFWIncludes.h"

namespace Tac
{
  struct InputMsg
  {
    enum Type {eMouseClick, eMouseMove, eKey, eMouseScroll} mType;

    struct MouseScroll
    {
      double mOffsetX;
      double mOffsetY;
    };
    struct Key
    {
      int key;
      int scancode; //sysyem-specific scancdoe
      int action; // GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
      int mods; // bit field descriing modifier keys
    };
    struct MouseClick
    {
      //GLFW_MOUSE_BUTTON_1
      //GLFW_MOUSE_BUTTON_2
      //GLFW_MOUSE_BUTTON_3
      //GLFW_MOUSE_BUTTON_4
      //GLFW_MOUSE_BUTTON_5
      //GLFW_MOUSE_BUTTON_6
      //GLFW_MOUSE_BUTTON_7
      //GLFW_MOUSE_BUTTON_8
      //GLFW_MOUSE_BUTTON_LAST
      //GLFW_MOUSE_BUTTON_LEFT
      //GLFW_MOUSE_BUTTON_RIGHT
      //GLFW_MOUSE_BUTTON_MIDDLE
      int mButton;
      // GLFW_PRESS, 
      // GLFW_RELEASE,
      // GLFW_RELEASE
      int mAction; 

      // GLFW_MOD_SHIFT
      // GLFW_MOD_CONTROL
      // GLFW_MOD_ALT
      // GLFW_MOD_SUPER
      int mMods; // bitfield
    };
    struct MouseMove
    {
      double mX;
      double mY;
    };

    union
    {
      MouseClick mMouseClick;
      MouseMove mMouseMove;
      MouseScroll mMouseScroll;
      Key mKey;
    };

    TacDLLAPI InputMsg(MouseClick click);
    TacDLLAPI InputMsg(MouseMove move);
    TacDLLAPI InputMsg(Key keyEvent);
    TacDLLAPI InputMsg(MouseScroll scrollEvent);
  };

  class InputMsgHandler
  {
  public:

    enum Priority
    { // highest
      eEditor, 
      eEverythingElse
    }; //lowest

    // return true to not pass the message on
    // return false to pass the message to the next handler
    typedef bool Callback(const InputMsg &);

    TacDLLAPI InputMsgHandler(
      Priority handlerPriority, 
      const std::string & name,
      std::function<Callback> callback);

    TacDLLAPI Priority GetPriority() const;
    TacDLLAPI bool HandleMessage(const InputMsg & msg) const;
    TacDLLAPI const std::string & GetName() const;

  private:
    std::function<Callback> mCallback;
    std::string mName;
    Priority mPriority;
  };


  class InputSystem : public System
  {
  public:
    TacDLLAPI InputSystem();
    TacDLLAPI ~InputSystem();
    TacDLLAPI virtual void Update(float dt);
    TacDLLAPI virtual void UpdateFakeTime(float dt);

    TacDLLAPI void SendMsg(const InputMsg & msg);

    TacDLLAPI void AddHandler(std::shared_ptr<InputMsgHandler> h);
    TacDLLAPI void RemHandler(std::shared_ptr<InputMsgHandler> h);

    TacDLLAPI int GetMouseX() const;
    TacDLLAPI int GetMouseY() const;
    TacDLLAPI int GetLastMouseX() const;
    TacDLLAPI int GetLastMouseY() const;
    TacDLLAPI bool IsKeyDown(int glfwKey) const;
    TacDLLAPI bool IsMouseButtonDown(int glfwMouseButton) const;
    //TacDLLAPI double GetMouseScrollX() const;
    //TacDLLAPI double GetMouseScrollY() const;
    //TacDLLAPI double GetLastMouseScrollX() const;
    //TacDLLAPI double GetLastMouseScrollY() const;
  private:
    bool mLastMouseNeedsUpdate;
    int mMouseX;
    int mMouseY;
    int mLastMouseX;
    int mLastMouseY;
    //double mMouseScrollX;
    //double mMouseScrollY;
    //double mLastMouseScrollX;
    //double mLastMouseScrollY;
    std::vector< std::shared_ptr< InputMsgHandler > > mHandlers;

    // glfw_press, release or repeat
    int mKeyStates[GLFW_KEY_LAST]; 

    // glfw_press or release
    int mMouseButtonStates[GLFW_MOUSE_BUTTON_LAST]; 

  };

  // singleton because glfw input callbacks are merely function pointers
  // that do not hold state
  extern InputSystem * gInputSystem;

} // Tac

#endif
