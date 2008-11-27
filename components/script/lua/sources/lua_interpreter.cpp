#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;
using namespace xtl;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //this used in base member initializer list
#endif

namespace script
{

namespace lua
{

/*
    ��� ����������� ���� ������ �� ���������
*/

const char* VARIANT_DEFAULT_TYPE_NAME = "__lua_variant_type";

}

}

namespace
{

/*
    ���������
*/

const char* SHELL_LIBRARY_NAME = "Shell";

/*
    �������
*/

//������� ��������� ������ lua
int error_handler (lua_State* state)
{
  throw xtl::format_exception<RuntimeException> ("script::lua::error_handler", "%s", lua_tostring (state, -1));
}

}

/*
    ����������� / ����������
*/

Interpreter::Interpreter (const EnvironmentPointer& in_environment)
  : environment (in_environment),
    stack (state, *this)
{
    //������������� ����������� ���������

  luaopen_base   (state);
  luaopen_string (state);
  luaopen_table  (state);
  luaopen_math   (state);

    //����������� ������� ��������� ������

  lua_atpanic (state, &error_handler);
  
    //����������� ������������ ����������������� ���� ������

  static const luaL_reg user_data_meta_table [] = {
    {"__gc",       &variant_destroy},
    {"__tostring", &variant_tostring},
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
    
  RegisterShellLibrary ();

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
    ���������� ���������
*/

Environment& Interpreter::Environment ()
{
  return *environment;
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

//��������� ��������� �� ������
void raise_error (lua_State* state, const char* source)
{
  try
  {
    const char* reason = lua_tostring (state, -1);

    throw xtl::format_exception<RuntimeException> (source, "%s", reason ? reason : "internal error");
  }
  catch (...)
  {
    lua_pop (state, 1);
    throw;
  }
}

}

void Interpreter::DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size)
{
    //��������� ����� ������ � �������� ���

  if (luaL_loadbuffer (state, (const char*)buffer, buffer_size, buffer_name))
    raise_error (state, "script::Interpreter::DoCommands[compile]");

  if (lua_pcall (state, 0, LUA_MULTRET, 0))
    raise_error (state, "script::Interpreter::DoCommands[execute]");
}

/*
    ����� ������� ���
*/

void Interpreter::Invoke (size_t arguments_count, size_t results_count)
{
    //�������� ����� ������������ call???

  if (lua_pcall (state, arguments_count, results_count, 0))
    raise_error (state, "script::lua::Interpreter::Invoke");
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
    throw xtl::make_null_argument_exception ("script::lua::RegisterLibrary", "name");

  libraries.insert_pair (name, LibraryPtr (new Library (*this, name, registry), false));
}

void Interpreter::UnregisterLibrary (const char* name)
{
  if (!name)
    return;
    
  libraries.erase (name);
}

/*
    ����������� ���������� ������ ��������������
*/

void Interpreter::RegisterShellLibrary ()
{
//  shell_library.Register ("EventHandler", make_invoker ());

//  RegisterLibrary (SHELL_LIBRARY_NAME, shell_library);
}

namespace
{

/*
    �������� �������������� lua
*/

IInterpreter* create_lua_interpreter (const xtl::shared_ptr<Environment>& environment)
{
  if (!environment)
    throw xtl::make_null_argument_exception ("script::create_lua_interpreter", "environment");

  return new Interpreter (environment);
}

/*
   ��������� �������� �������������� lua
*/

class LuaInterpreterComponent
{
  public:
    //�������� ����������
    LuaInterpreterComponent () 
    {
      InterpreterManager::RegisterInterpreter ("lua", &create_lua_interpreter);
    }
};

extern "C"
{

ComponentRegistrator<LuaInterpreterComponent> LuaInterpreter ("script.interpreters.lua");

}


}
