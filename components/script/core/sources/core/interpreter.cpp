#include "shared.h"

namespace script
{

/*
    ������, ����������� �� �������� IInterpreter
*/

xtl::trackable& get_trackable (IInterpreter& interpreter)
{
  return interpreter.GetTrackable ();
}

}
