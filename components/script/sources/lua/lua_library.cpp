#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

/*
    ������� ������������ lua
*/

namespace
{

//��������� �������
int unsafe_invoke_dispatch (lua_State* state)
{
    //��������� ��������� �� ����      

  const Invoker* invoker   = reinterpret_cast<const Invoker*> (lua_touserdata (state, lua_upvalueindex (1)));
  Interpreter* interpreter = reinterpret_cast<Interpreter*> (lua_touserdata (state, lua_upvalueindex (2)));

  if (!invoker || !interpreter)
    Raise<RuntimeException> ("script::lua::invoke_dispatch", "Bad invoker call (no up-values found)");

    //�������� ���������� ���������� ����������

  if ((int)invoker->ArgumentsCount () > lua_gettop (state))
    Raise<StackException> ("script::lua::invoke_dispatch", "Arguments count mismatch (expected %u, got %u)", 
                           invoker->ArgumentsCount (), lua_gettop (state));

    //����� �����

  (*invoker)(interpreter->Interpreter::Stack ());

  return invoker->ResultsCount ();
}

int invoke_dispatch (lua_State* state)
{
  return safe_call (state, &unsafe_invoke_dispatch);
}

//��������� ���� �� �����
int unsafe_variant_get_field (lua_State* state)
{
  if (!lua_getmetatable (state, 1))
    Raise<RuntimeException> ("script::lua::variant_get_field", "Bad '__index' call. Object isn't variant");

  lua_pushvalue (state, 2);
  lua_rawget    (state, -2);

  if (!lua_isnil (state, -1)) //���� � �������� ������ �������
    return 1;

    //�������� ����� ���� � ��������� get_

  lua_pop (state, 1); //������� ��������� ����������� ������
  lua_pushfstring (state, "get_%s", lua_tostring (state, 2));
  lua_rawget      (state, -2);

  if (lua_isnil (state, -1)) //�������� � ��������� ������ �� �������
    Raise<UndefinedFunctionCallException> ("script::lua::variant_get_field", "Field '%s' not found", lua_tostring (state, 2));

    //��������� ��������� ������ ����� � ����
  
  lua_pushvalue (state, 1);
  lua_call      (state, 1, 1);

  return 1;
}

int variant_get_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_get_field);
}

//��������� �������� ����
int unsafe_variant_set_field (lua_State* state)
{
  if (!lua_getmetatable (state, 1))
    Raise<RuntimeException> ("script::lua::variant_set_field", "Bad '__newindex' call. Object isn't variant");    

  lua_pushvalue (state, 2);
  lua_rawget    (state, -2);
  
  if (!lua_isnil (state, -1)) //���� ���������� ���� � ��������� ������
  {
    lua_pop       (state, 1);  //������� ��������� ����������� ������
    lua_pushvalue (state, 3);
    lua_rawset    (state, -3);
    
    return 1;
  }

    //�������� ����� ���� � ��������� �et_

  lua_pop         (state, 1); //������� ��������� ����������� ������
  lua_pushfstring (state, "set_%s", lua_tostring (state, 2));
  lua_rawget      (state, -2);

  if (lua_isnil (state, -1)) //�������� � ��������� ������ �� �������
    Raise<UndefinedFunctionCallException> ("script::lua::variant_set_field", "Field '%s' not found (or read-only)", lua_tostring (state, 2));

    //��������� ���������� ������ ����� � ����

  lua_pushvalue (state, 1);
  lua_pushvalue (state, 3);

  int top_index = lua_gettop (state) - 3;

  lua_call (state, 2, LUA_MULTRET);

  return lua_gettop (state) - top_index;
}

int variant_set_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_set_field);
}

}

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
    {"__gc",       &variant_destroy},
    {"__tostring", &variant_tostring},
    {"__index",    &variant_get_field},
    {"__newindex", &variant_set_field},
    {0, 0}
  };

  luaL_register (state, name, common_meta_table);
  lua_pushvalue (state, -1);
  lua_setfield  (state, LUA_REGISTRYINDEX, name); //����������� �����������
    
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
