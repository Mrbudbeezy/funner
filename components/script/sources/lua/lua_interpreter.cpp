#include <common/exception.h>
#include <xtl/iterator.h>
#include <xtl/bind.h>
#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;
using namespace xtl;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

//��������� �������
int invoke_dispatch (lua_State* state)
{
  const char* invoker_name = "invoker_dispatch";

  try
  {     
      //��������� ��������� �� ����      

    const Invoker* invoker = reinterpret_cast<const Invoker*> (lua_touserdata (state, lua_upvalueindex (1)));

    invoker_name = lua_tostring (state, lua_upvalueindex (2));

    if (!invoker_name)
      invoker_name = "invoker_dispatch";

    if (!invoker)
      Raise<RuntimeException> ("script::lua::invoke_dispatch", "No upvalue associated with invoker");

      //�������� ���������� ���������� ����������

    if (invoker->ArgumentsCount () != lua_gettop (state))
      Raise<StackException> ("script::lua::invoke_dispatch", "Arguments count mismatch (expected %u, got %u)", 
                             invoker->ArgumentsCount (), lua_gettop (state));

      //����� �����

    (*invoker)(LuaStack (state));

    return invoker->ResultsCount ();    
  }
  catch (xtl::bad_any_cast& exception)
  {
    luaL_error (state, "%s: %s -> %s (at invoke '%s')", exception.what (), exception.source_type ().name (), exception.target_type ().name (),
                invoker_name);
  }
  catch (std::exception& exception)
  {
    luaL_error (state, "exception: %s (at invoke '%s')", exception.what (), invoker_name);
  }
  catch (...)
  {
    luaL_error (state, "%s (at invoke '%s')", lua_gettop (state) ? lua_tostring (state, -1) : "internal error", invoker_name);
  }
  
  return 0;
}

//������� ��������� ������ lua
int error_handler (lua_State* state)
{
  Raise<RuntimeException> ("script::lua::error_handler", "%s", lua_tostring (state, -1));

  return 0;
}

//������� ���������� ��������� ������ ��� �������� �������� ����������������� ���� ������
int user_data_destroyer (lua_State* state)
{
  xtl::any* variant = reinterpret_cast<xtl::any*> (luaL_checkudata (state, -1, USER_DATA_TAG));
  
  if (!variant)
    return 0;    

  stl::destroy (variant);  

  return 0;
}

void log_function (const char*)
{
}

}

/*
    ����������� / ����������
*/

LuaInterpreter::LuaInterpreter (const xtl::shared_ptr<InvokerRegistry>& in_registry)
  : registry (in_registry),
    ref_count (1), 
    on_register_invoker_connection (registry->RegisterHandler (InvokerRegistryEvent_OnRegisterInvoker,
      bind (&LuaInterpreter::RegisterInvoker, this, _2, _3)))
{
    //������������� ��������� lua

  state = lua_open ();

  if (!state)
    Raise <InterpreterException> ("script::lua::LuaInterpreter::LuaInterpreter", "Can't create lua state");
    
  try
  {
    luaL_openlibs (state);

      //����������� ������������ �������� ���������������� ����� ������    

    static const luaL_reg user_data_meta_table [] = {{"__gc", &user_data_destroyer}, {0,0}};

    luaL_newmetatable (state, USER_DATA_TAG);
    luaL_register     (state, 0, user_data_meta_table);

      //����������� ������� ��������� ������

    lua_atpanic (state, &error_handler);

      //������������� �����

    stack.SetState (state);

      //����������� ������������

    for (InvokerRegistry::Iterator i=registry->CreateIterator (); i; ++i)
      RegisterInvoker (registry->InvokerId (i), *i);
      
      //������� �����
      
    lua_settop (state, 0);
  }
  catch (...)
  {
    lua_close (state);
    throw;
  }
}

LuaInterpreter::~LuaInterpreter ()
{
  lua_close (state);
}

/*
    ��� ��������������
*/

const char* LuaInterpreter::Name ()
{
  return LUA_RELEASE;
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
  if (!name)
    return false;
      
  lua_getglobal (state, name);

  bool is_function = lua_isfunction (state, -1) != 0;

  lua_pop (state, 1);

  return is_function;
}

/*
    ���������� ������ ������������� ���
*/

void LuaInterpreter::DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
{
    //��������� ����� ������ � �������� ���

  if (luaL_loadbuffer (state, (const char*)buffer, buffer_size, buffer_name))
  {
    try
    {
      log (lua_tostring (state, -1));        
    }
    catch (...)
    {
      //��������� ��� ����������
    }

      //����������� ��������� �� ������ �� ����� 

    lua_pop (state, 1);

    return;
  }

  if (lua_pcall (state, 0, LUA_MULTRET, 0))
  {
    try
    {
      log (lua_tostring (state, -1));
    }
    catch (...)
    {
      //��������� ��� ����������
    }
    
      //����������� �� ����� ��������� �� ������
      
    lua_pop (state, 1);
  }
}

/*
    ����� ������� ���
*/

void LuaInterpreter::Invoke (size_t arguments_count, size_t results_count)
{
  if (lua_pcall (state, arguments_count, results_count, 0))
  {
      //��������� ��������� �� ������
    
    const char* reason    = lua_tostring (state, -1);
    stl::string error_msg = reason ? reason : "internal error";
    
      //����������� �� ����� ��������� �� ������

    lua_pop (state, 1);
    
      //���������� ����������

    Raise<RuntimeException> ("script::lua::LuaInterpreter::Invoke", "%s", error_msg.c_str ());    
  }
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

/*
   ����������� ������
*/

void LuaInterpreter::RegisterInvoker (const char* invoker_name, Invoker& invoker)
{
  if (!invoker_name)
    RaiseNullArgument ("script::lua::LuaInterpreter::RegisterInvoker", "invoker_name");
    
    //����������� �-closure
    
  lua_pushlightuserdata (state, &invoker);
  lua_pushstring        (state, invoker_name);
  lua_pushcclosure      (state, &invoke_dispatch, 2);
  lua_setglobal         (state, invoker_name);
}

namespace script
{

/*
    �������� �������������� lua
*/

xtl::com_ptr<IInterpreter> create_lua_interpreter (const xtl::shared_ptr<InvokerRegistry>& registry)
{
  if (!registry)
    common::RaiseNullArgument ("script::create_lua_interpreter", "registry");

  return xtl::com_ptr<IInterpreter> (new LuaInterpreter (registry), false);
}

}
