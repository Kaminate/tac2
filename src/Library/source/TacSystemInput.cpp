/*!
\author Nathan Park
\brief
*/
#include "TacSystemInput.h"
#include "TacErrorIf.h"
#include "TacMacros.h"

#include <algorithm>
#include <iterator>

namespace Tac
{
  InputSystem * gInputSystem = nullptr;

  /////////////////////
  // InputMsgHandler //
  /////////////////////
  InputMsgHandler::InputMsgHandler( 
    Priority handlerPriority, 
    const std::string & name,
    std::function<Callback> callback ) 
    : mCallback(callback)
    , mPriority(handlerPriority)
    , mName(name)
  {

  }

  bool InputMsgHandler::HandleMessage( const InputMsg & msg ) const
  {
    return mCallback(msg);
  }

  Tac::InputMsgHandler::Priority InputMsgHandler::GetPriority() const
  {
    return mPriority;
  }

  const std::string & InputMsgHandler::GetName() const
  {
    return mName;
  }

  /////////////////
  // InputSystem //
  /////////////////

  InputSystem::InputSystem()
    : mMouseX(0)
    , mMouseY(0)
    , mLastMouseX(0)
    , mLastMouseY(0)
    , mLastMouseNeedsUpdate(true)
    //, mMouseScrollX(0)
    //, mMouseScrollY(0)
    //, mLastMouseScrollX(0)
    //, mLastMouseScrollY(0)
  {
    ErrorIf(gInputSystem, "There already exists an input sytsem");
    gInputSystem = this;

    for(int i = 0; i < GLFW_KEY_LAST; ++i)
    {
      mKeyStates[ i ] = GLFW_RELEASE;
    }

    for( int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i)
    {
      mMouseButtonStates[i] = GLFW_RELEASE;
    }
  }

  InputSystem::~InputSystem()
  {
    ErrorIf(!mHandlers.empty(), 
      "InputHandler not removed: %s",
      mHandlers.front()->GetName().c_str());


    ErrorIf(!gInputSystem, "There is no input system to destroy");
    gInputSystem = nullptr;
  }

  void InputSystem::Update( float dt )
  {
    if(mLastMouseNeedsUpdate)
    {
      mLastMouseX = mMouseX;
      mLastMouseY = mMouseY;
    }
    mLastMouseNeedsUpdate = true;

    UnusedParameter(dt);
  }

  void InputSystem::UpdateFakeTime( float dt )
  {
    UnusedParameter(dt);
  }

  void InputSystem::SendMsg( const InputMsg & msg )
  {
    switch (msg.mType)
    {
    case InputMsg::eKey:
      mKeyStates[msg.mKey.key] = msg.mKey.action;
      break;
    case InputMsg::eMouseClick:
      mMouseButtonStates[msg.mMouseClick.mButton]
      = msg.mMouseClick.mAction;
      break;
    case InputMsg::eMouseMove:
      mLastMouseX = mMouseX;
      mLastMouseY = mMouseY;
      mMouseX = (int)msg.mMouseMove.mX;
      mMouseY = (int)msg.mMouseMove.mY;
      mLastMouseNeedsUpdate = false;
      break;
    //case InputMsg::eMouseScroll:
    //  mLastMouseScrollX = mMouseScrollX;
    //  mLastMouseScrollY = mMouseScrollY;
    //  mMouseScrollX = msg.mMouseScroll.mOffsetX;
    //  mMouseScrollY = msg.mMouseScroll.mOffsetY;
    default:
      break;
    }


    for (auto handler : mHandlers)
    {
      if (handler->HandleMessage(msg))
        break;
    }
  }

  void InputSystem::AddHandler( std::shared_ptr<InputMsgHandler> h )
  {
    mHandlers.push_back(h);

    std::sort(mHandlers.begin(), mHandlers.end(), 
      [](std::shared_ptr<InputMsgHandler> lhs, std::shared_ptr<InputMsgHandler> rhs)
    {
      return lhs->GetPriority() > rhs->GetPriority();
    });
  }

  void InputSystem::RemHandler( std::shared_ptr<InputMsgHandler> h )
  {
    auto oldSize = mHandlers.size();

    auto it = std::remove_if(mHandlers.begin(), mHandlers.end(),
      [h](std::shared_ptr<InputMsgHandler>curHandler)
    {
      return curHandler.get() == h.get();
    });

    unsigned newSize = std::distance(mHandlers.begin(), it);
    mHandlers.resize(newSize);

    unsigned numRemoved = oldSize - newSize;

    ErrorIf(numRemoved == 0, "Handler not found");
    ErrorIf(numRemoved > 1, "Removed duplicate handlers");
  }

  int InputSystem::GetMouseX() const
  {
    return mMouseX;
  }

  int InputSystem::GetMouseY() const
  {
    return mMouseY;
  }
  
  int InputSystem::GetLastMouseX() const
  {
    return mLastMouseX;
  }

  int InputSystem::GetLastMouseY() const
  {
    return mLastMouseY;
  }

  bool InputSystem::IsKeyDown( int glfwKey ) const
  {
    return mKeyStates[ glfwKey ] == GLFW_PRESS ||
      mKeyStates[ glfwKey ] == GLFW_REPEAT;
  }

  bool InputSystem::IsMouseButtonDown( int glfwMouseButton ) const
  {
    return mMouseButtonStates[glfwMouseButton] == GLFW_PRESS;
  }

  //double InputSystem::GetMouseScrollX() const 
  //{
  //  return mMouseScrollX;
  //}
  //double InputSystem::GetLastMouseScrollX() const
  //{
  //  return mLastMouseScrollX;
  //}
  //double InputSystem::GetMouseScrollY() const
  //{
  //  return mMouseScrollY;
  //}
  //double InputSystem::GetLastMouseScrollY() const
  //{
  //  return mLastMouseScrollY;
  //}

  InputMsg::InputMsg( MouseClick click )
    : mType(eMouseClick)
    , mMouseClick(click)
  {
  }

  InputMsg::InputMsg( MouseMove move )
    : mType(eMouseMove)
    , mMouseMove(move)
  {
  }

  InputMsg::InputMsg( Key keyEvent )
    : mType(eKey)
    , mKey(keyEvent)
  {
  }

  InputMsg::InputMsg( MouseScroll scrollEvent )
    : mType(eMouseScroll)
    , mMouseScroll(scrollEvent)
  {
  }

} // Tac
