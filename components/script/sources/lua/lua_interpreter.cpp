#include "shared.h"

using namespace script;
using namespace script::lua;

/*
    ����������� / ����������
*/

LuaInterpreter::LuaInterpreter (const InvokerRegistry& in_registry)
  : registry (in_registry), ref_count (1)
{
//  stack.SetState (environment.State ());
  
//  for (InvokerRegistry::Iterator i = ((InvokerRegistry)registry).CreateIterator (); i; ++i)
//    environment.RegisterFunction (registry.InvokerId (i), &LuaInterpreter::InvokeFunction);
}

LuaInterpreter::~LuaInterpreter ()
{
}

/*
    ��� ��������������
*/

const char* LuaInterpreter::Name ()
{
    //���������� �������� ������ ��� ������ �� ���

  return "LuaInterpreter";
}

/*
    ���� ����������
*/

IStack& LuaInterpreter::Stack ()
{
  return stack;
}

/*
    �������� ������� �������
*/

bool LuaInterpreter::HasFunction (const char* name)
{
  return false; //!!!!!
}

/*
    ���������� ������ ������������� ���
*/

void LuaInterpreter::DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
{
}

/*
    ����� ������� ���
*/

void LuaInterpreter::Invoke (size_t arguments_count, size_t results_count)
{
}

/*
    ������� ������
*/

void LuaInterpreter::AddRef ()
{
  ref_count++;
}

void LuaInterpreter::Release ()
{
  if (!--ref_count)
    delete this;
}

namespace script
{

/*
    �������� �������������� lua
*/

xtl::com_ptr<IInterpreter> create_lua_interpreter (const InvokerRegistry& registry)
{
  return xtl::com_ptr<IInterpreter> (new LuaInterpreter (registry), false);
}

}
