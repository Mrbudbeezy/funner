#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

/*
    ������������ / ����������
*/

Library::Library (Interpreter& in_interpreter, const char* name, InvokerRegistry& in_registry)
  : state (in_interpreter.State ()),
    interpreter (in_interpreter),
    registry (in_registry),
    table_name (name),
    is_global (table_name == "global")
{
    //����������� ������������ �������� ���������������� ����� ������

  static const luaL_reg common_meta_table [] = {
    {"__gc", &destroy_object},
    {"__tostring", &dump_to_string},
    {0, 0}
  };

  luaL_register (state, name, common_meta_table);
  lua_pushvalue (state, -1);
  lua_setfield  (state, LUA_REGISTRYINDEX, name); //����������� �����������

    //��������� ��������� ������� (���������� ���� __index ����������� ��������� �� ���� �����������)

  lua_pushstring (state, "__index");
  lua_pushvalue  (state, -2);
  lua_settable   (state, -3);
  lua_pop        (state, 1);    
    
  try
  {    
      //����������� ������

    for (InvokerRegistry::Iterator i=registry.CreateIterator (); i; ++i)
      RegisterInvoker (registry.InvokerId (i), *i);

      //����������� ������������ �������

    on_register_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnRegisterInvoker,
      xtl::bind (&Library::RegisterInvoker, this, _2, _3));

    on_unregister_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnUnregisterInvoker,
      xtl::bind (&Library::UnregisterInvoker, this, _2));
  }
  catch (...)
  {
    Destroy ();

    throw;    
  }
}

Library::~Library ()
{
  Destroy ();
}

void Library::Destroy ()
{
     //unregister all invokers

  lua_pushnil   (state);
  lua_pushvalue (state, -1);
  lua_setfield  (state, LUA_REGISTRYINDEX, table_name.c_str ());
  lua_setfield  (state, LUA_GLOBALSINDEX, table_name.c_str ());
}

/*
    �����������/�������� ������
*/

void Library::RegisterInvoker (const char* invoker_name, Invoker& invoker)
{
  if (!strcmp (invoker_name, "__index") || !strcmp (invoker_name, "__gc"))
    return; //����������� ������ � ���������� ����� ���������

  luaL_getmetatable     (state, table_name.c_str ());
  lua_pushlightuserdata (state, &invoker);
  lua_pushlightuserdata (state, &interpreter);
  lua_pushcclosure      (state, &invoke_dispatch, 2);
  
  if (is_global)
  {
    lua_pushvalue (state, -1);
    lua_setfield  (state, -3, invoker_name);
    lua_setglobal (state, invoker_name);
  }
  else
  {
    lua_setfield (state, -2, invoker_name);
  }

  lua_pop (state, 1);
}

void Library::UnregisterInvoker (const char* invoker_name)
{
  luaL_getmetatable (state, table_name.c_str ());
  lua_pushnil       (state);
  lua_setfield      (state, -2, invoker_name);

  if (is_global)
  {
    lua_pushnil   (state);
    lua_setglobal (state, invoker_name);
  }

  lua_pop (state, 1);
}
