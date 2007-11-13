#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;
using namespace xtl;

/*
    ��� ����������� ���� ������ �� ���������
*/

namespace script
{

namespace lua
{

const char* VARIANT_DEFAULT_TYPE_NAME = "__lua_variant_type";

}

}

/*
    ����������� / ����������
*/

Interpreter::Interpreter (const EnvironmentPointer& in_environment)
  : environment (in_environment),
    stack (state, *environment)
{
    //������������� ����������� ���������

  luaopen_base   (state);
  luaopen_string (state);
  luaopen_table  (state);    

    //����������� ������� ��������� ������

  lua_atpanic (state, &error_handler);
  
    //����������� ����������� �������� ����������������� ���� ������

  static const luaL_reg user_data_meta_table [] = {
    {"__gc", &destroy_object},
    {0,0}
  };
  
  luaL_newmetatable (state, VARIANT_DEFAULT_TYPE_NAME);
  luaI_openlib      (state, 0, user_data_meta_table, 0);

    //����������� ������������ ������� ��������/�������� ���������

  on_create_library_connection = environment->RegisterEventHandler (EnvironmentLibraryEvent_OnCreate,
    xtl::bind (&Interpreter::RegisterLibrary, this, _2, _3));

  on_remove_library_connection = environment->RegisterEventHandler (EnvironmentLibraryEvent_OnRemove,
    xtl::bind (&Interpreter::UnregisterLibrary, this, _2));

    //����������� ���������

  for (Environment::Iterator i=environment->CreateIterator (); i; ++i)
    RegisterLibrary (environment->LibraryId (i), *i);

    //������� �����

  lua_settop (state, 0);
}

Interpreter::~Interpreter ()
{
}

/*
    ��� ��������������
*/

const char* Interpreter::Name ()
{
  return LUA_RELEASE;
}

/*
    ���� ����������
*/

IStack& Interpreter::Stack ()
{
  return stack;
}

/*
    �������� ������� �������
*/

bool Interpreter::HasFunction (const char* name)
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

namespace
{

//������� ���������������� �� ���������
void log_function (const char*)
{
}

}

void Interpreter::DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
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

void Interpreter::Invoke (size_t arguments_count, size_t results_count)
{
  if (lua_pcall (state, arguments_count, results_count, 0))
  {
      //��������� ��������� �� ������
    
    const char* reason    = lua_tostring (state, -1);
    stl::string error_msg = reason ? reason : "internal error";

      //����������� �� ����� ��������� �� ������

    lua_pop (state, 1);

      //���������� ����������

    Raise<RuntimeException> ("script::lua::Interpreter::Invoke", "%s", error_msg.c_str ());    
  }
}

/*
    ������� ������
*/

void Interpreter::AddRef ()
{
  addref (this);
}

void Interpreter::Release ()
{
  release (this);
}
   
/*
    ����������� / �������� ���������
*/

void Interpreter::RegisterLibrary (const char* name, InvokerRegistry& registry)
{
  if (!name)
    RaiseNullArgument ("script::lua::RegisterLibrary", "name");

  libraries.insert_pair (name, LibraryPtr (new Library (*this, name, registry), false));
}

void Interpreter::UnregisterLibrary (const char* name)
{
  if (!name)
    return;
    
  libraries.erase (name);
}

namespace script
{

/*
    �������� �������������� lua
*/

xtl::com_ptr<IInterpreter> create_lua_interpreter (const xtl::shared_ptr<Environment>& environment)
{
  if (!environment)
    common::RaiseNullArgument ("script::create_lua_interpreter", "environment");

  return xtl::com_ptr<IInterpreter> (new Interpreter (environment), false);
}

}
