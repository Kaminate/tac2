// Nathan Park
#include "TacCS260Common.h"
#include "TacErrorIf.h"
#include "TacMacros.h"
#include <string> //getline
namespace Tac
{
  void GetInput(std::string & input)
  {
    std::cout << "> ";
    std::getline(std::cin, input);
    std::cout << std::endl;
  }

  void Say(const std::string & msg)
  {
    std::cout << msg << std::endl;
  }

  bool UserAnswer(const std::string & question)
  {
    Say(question + "(y/n)");
    std::string answer;
    GetInput(answer);
    return !answer.empty() && answer[0] == 'y';
  }

  namespace Message
  {
    // returns Type::Count if invalid
    Type GetType(const std::string & message)
    {
      for (unsigned i = 0; i < eCount; ++i)
      {
        Type curType = (Type) i;
        if (GetName(curType) == message)
          return curType;
      }
      return eCount;
    }

    std::string GetName( Type messageType )
    {
      std::string name;
      switch (messageType)
      {
      case eChat:
        name = "Chat";
        break;
      case eAssignId:
        name = "AssignId";
        break;
      case eExit:
        name = "Exit";
        break;
      case eListUsers:
        name = "ListUsers";
        break;
      case eChallenge:
        name = "Challenge";
        break;
      case eGameStart:
        name = "GameStart";
        break;
      case eOpponentDisconnect:
        name = "OppDC";
        break;
      case eFire:
        name = "Fire";
        break;
      case eCount:
      default:
        ErrorIf(true, "Message type not registered");
        break;
      }
      ErrorIf(name.size() > sizeof(MessageName), "Msg name too long");

      return name;
    }
  }
} // Tac
