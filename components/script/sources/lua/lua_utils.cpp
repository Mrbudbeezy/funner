#include "shared.h"

using namespace common;
using namespace script::lua;

namespace script
{

namespace
{

//��������� ������ � ������ ������
stl::string get_lua_position (lua_State* state)
{
  stl::string buffer;  
  lua_Debug   debug;
  
  for (int level=0; ; level++)
  {
    if (!lua_getstack (state, level, &debug))
      break;
      
    lua_getinfo (state, "Sln", &debug);
   
    buffer += common::format ("\n    at lua-call %s(%d): function='%s'", debug.short_src, debug.currentline, debug.namewhat);
  }

  return buffer;
}

//���������� ����� ������
template <class Fn> int safe_call (lua_State* state, Fn f)
{
  try
  {
    return f (state);
  }
  catch (xtl::bad_any_cast& exception)
  {
    luaL_error (state, "%s: %s -> %s%s", exception.what (), exception.source_type ().name (), exception.target_type ().name (),
                get_lua_position (state).c_str ());
  }
  catch (std::exception& exception)
  {
    luaL_error (state, "exception: %s%s", exception.what (), get_lua_position (state).c_str ());
  }
  catch (...)
  {
    luaL_error (state, "%s%s", lua_gettop (state) ? lua_tostring (state, -1) : "internal error", get_lua_position (state).c_str ());
  }

  return 0;
}

//��������� �������
int unsafe_invoke_dispatch (lua_State* state)
{
    //��������� ��������� �� ����      

  const Invoker* invoker      = reinterpret_cast<const Invoker*> (lua_touserdata (state, lua_upvalueindex (1)));
  const char*    invoker_name = lua_tostring (state, lua_upvalueindex (2));

  if (!invoker_name)
    Raise<UndefinedFunctionCallException> ("script::lua::InvokerDispatch::operator ()", "Undefined function call (no up-values found)");

  if (!invoker)
    Raise<RuntimeException> ("script::lua::InvokerDispatch::operator ()", "Null invoker call (no up-values found)");

    //�������� ���������� ���������� ����������

  if (invoker->ArgumentsCount () != lua_gettop (state))
    Raise<StackException> ("script::lua::InvokerDispatch::operator ()", "Arguments count mismatch (expected %u, got %u)", 
                           invoker->ArgumentsCount (), lua_gettop (state));

    //����� �����
  
  (*invoker)(Stack (state));

  return invoker->ResultsCount ();
}

//�������, ���������� ��������� ������ ��� �������� �������� ����������������� ���� ������
int unsafe_destroy_object (lua_State* state)
{
  xtl::any* variant = reinterpret_cast<xtl::any*> (luaL_checkudata (state, -1, USER_DATA_TAG));
  
  if (!variant)
    return 0;    

  delete variant; //!!!!
//    stl::destroy (variant);  

  return 0;
}

}

namespace lua
{

int invoke_dispatch (lua_State* state)
{
  return safe_call (state, &unsafe_invoke_dispatch);
}

int destroy_object (lua_State* state)
{
  return safe_call (state, &unsafe_destroy_object);
}

}

}
