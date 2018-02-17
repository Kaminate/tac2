/*!
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif


#pragma once

#define TACTWEAK

#ifdef TACTWEAK

// http://sourceforge.net/p/glfw/discussion/247562/thread/26ea1c55/
//#define GLFW_CDECL 
#include <AntTweakBar.h>
#include "TacSharedPtr.h"
#include "TacVector2.h"
#include <functional>


namespace Tac
{
  class Tweak;
  class TweakBar
  {
  private:
    TacDLLAPI TweakBar(const std::string & name); friend class Tweak;
  public:
    TacDLLAPI ~TweakBar();
    TacDLLAPI void ClearBar();
    TacDLLAPI void SetTweak(Tweak * tweak) { mTweak = tweak;}

    enum Type{ReadWrite, ReadOnly};

    TacDLLAPI void AddVar(
      const std::string & name, 
      const std::string & params, 
      TwType dataType, 
      void * data,
      Type varType = ReadWrite);


    typedef std::function<void(void*)> GetterFn;
    typedef std::function<void(const void*)> SetterFn;

    TacDLLAPI void AddVarCB(
      const std::string & name, 
      const std::string & params, 
      TwType datatype,
      GetterFn getter,
      SetterFn setter);

    TacDLLAPI void AddComment(
      const std::string & name,
      const std::string & comment);

    template<typename T>
    struct callbackHelper
    {
      typedef void(T::*MemFn)();
      callbackHelper(T * tClass, MemFn fn) 
        : mTClass(tClass), mFn(fn)
      {

      }
      T * mTClass;
      MemFn mFn;
      void CallFn()
      {
        (mTClass->*mFn)();
      }
      static void TW_CALL TweakCallback(void * data)
      {
        callbackHelper * helper = (callbackHelper * )data;
        helper->CallFn();
      }
    };

    template<typename T>
    void AddButton(const std::string & comment, T*thisT,void (T::*callbackFn)(), const std::string & group)
    {
      std::string name;
      for(char c : comment) if(!isspace(c)) name += c;
      AddButton(
        name,
        comment,
        callbackHelper<T>::TweakCallback,
        new callbackHelper<T>(thisT, callbackFn),
        group); 
    }

    TacDLLAPI void AddButton(
      const std::string & name,
      const std::string & description,
      TwButtonCallback onClick,
      void * data,
      const std::string & group="");

    TacDLLAPI void RemoveVar(const std::string & name);

    TacDLLAPI Tweak * GetTweak(){return mTweak;}
    TacDLLAPI void SetPosition( int x, int y );
    TacDLLAPI void GetPosition( int & x, int & y );
    TacDLLAPI void SetSize(int w, int h);

    TacDLLAPI void SetVisible(bool visible);
    TacDLLAPI void SetIconified(bool iconified);
    TacDLLAPI const std::string & GetName();
  private:
    TwBar * mBar;
    Tweak * mTweak;
    std::string mName;
    struct Accessor
    {
      GetterFn mGetter;
      SetterFn mSetter;
    };
    static const int MAX_ACCESSORS = 10;
    Accessor mAccessors[MAX_ACCESSORS];
    int numAccessors;
    Accessor * AddAccessor(GetterFn getter, SetterFn setter);
    static void TW_CALL gettercb(void * value, void * clientdata);
    static void TW_CALL settercb(const void * value, void * clientdata);

  };
  DefineSharedPtr(TweakBar);

  class Tweak
  {
  public:
    TacDLLAPI Tweak(unsigned w, unsigned h);
    TacDLLAPI ~Tweak();
    TacDLLAPI void Render();
    TacDLLAPI void Uninit();
    TacDLLAPI const TwType& GetTypeVector2() const;
    TacDLLAPI const TwType& GetTypeVector3() const;
    TacDLLAPI const TwType& GetTypeVector4() const;
    TacDLLAPI const TwType& GetTypeComponent() const;
    TacDLLAPI SPTweakBar Create(const std::string & name);

    static Tweak * gTweak;
  private:
    void RegisterVec2();
    TwType mTypeVector2;
    void RegisterVec3();
    TwType mTypeVector3;
    void RegisterVec4();
    TwType mTypeVector4;
    void RegesterCompoent();
    TwType mTypeComponent;
    int mWindowW, mWindowH;

  };


  void TW_CALL TweakErrorHandler(const char *errorMessage);
}
#endif
