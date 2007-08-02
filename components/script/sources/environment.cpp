#include <common/file.h>
#include <common/strlib.h>
#include <stl/string>
#include <stl/hash_map>
#include <xtl/function.h>

#include "shared.h"

using namespace common;
using namespace script::lua;

namespace
{

const char*  INVOKE_DISPATCH_NAME = "___recall"; //��� ������� ��������������� � lua
const char*  IMPL_NAME            = "___impl";   //��� ����������, ���������� ��������� �� ���������� lua
const size_t IMPL_ID               = 0xDEADBEEF;  //������������� ���������� lua

}

/*
    �������� ���������� ��������� Lua
*/

//����� ������������ ������
typedef stl::hash_map<stl::hash_key<const char*>, detail::Invoker*> InvokerMap;

namespace script
{

namespace lua
{

struct EnvironmentImpl
{
  size_t       id;           //������������� ����������
  lua_State*   state;        //��������� Lua
  InvokerMap   invokers;     //����������� ������
  stl::string  name;         //��� �����    
};

}

}

/*
    ��������������� �������
*/

namespace
{

//��������� �������
int invoke_dispatch (lua_State* state)
{
  lua_getglobal (state, IMPL_NAME);

  EnvironmentImpl* this_impl = reinterpret_cast<EnvironmentImpl*> (lua_touserdata (state, -1));
  
    //�������� ������������ lua-���������� '__impl'
  
  if (!this_impl || this_impl->id != IMPL_ID)
    Raise<RuntimeException> ("script::lua::invoke_dispatch", "Wrong '__impl' variable");

  const char* function_name = lua_tostring (state, 1);
  
    //�������� ������� ������� function_name

  if (!function_name)
    Raise<RuntimeException> ("script::lua::invoke_dispatch", "Null function name at call function '__recall'");

  InvokerMap::iterator iter = this_impl->invokers.find (function_name);

  if (iter == this_impl->invokers.end ())
    Raise<RuntimeException> ("script::lua::invoke_dispatch", "Attempt to call unregistered function '%s'", function_name);

    //���������� ����� �������

  return (*iter->second) (detail::Stack (state));
}

//������� ��������� ������ lua
int error_handler (lua_State* state)
{
  Raise<RuntimeException> ("script::lua::error_handler", "Lua internal error (lua calls 'lua_atpanic' function)");

  return 0;
}

//������� ���������� ��������� ������ ��� �������� �������� ����������������� ���� ������
int user_data_destroyer (lua_State* state)
{
  detail::IUserData* user_data = reinterpret_cast<detail::IUserData*> (luaL_checkudata (state, -1, USER_DATA_TAG));
  
  if (!user_data)
    return 0;

  stl::destroy (user_data);

  return 0;
}

}


Environment::Environment ()
  : impl (new EnvironmentImpl)
{
  try
  {
    impl->id = IMPL_ID;

      //������������� ��������� lua

    impl->state = lua_open ();

    if (!impl->state)
      Raise<ScriptException> ("script::lua::Environment::Environment", "Can't create lua state");

      //������!!!!!!!
    
    luaL_openlibs (impl->state);
    
      //����������� ������������ �������� ���������������� ����� ������    
      
    static const luaL_reg user_data_meta_table [] = {{"__gc", &user_data_destroyer}, {0,0}};    

    luaL_newmetatable (impl->state, USER_DATA_TAG);
    luaL_openlib      (impl->state, 0, user_data_meta_table, 0);
    
      //����������� ������� ��������� ������

    lua_atpanic (impl->state, &error_handler);

      //����������� ���������� �������
    
    lua_pushcfunction     (impl->state, &invoke_dispatch);
    lua_setglobal         (impl->state, INVOKE_DISPATCH_NAME);
    lua_pushlightuserdata (impl->state, impl); //������� ����� ����������� user data � ��������� ����!!!
    lua_setglobal         (impl->state, IMPL_NAME);  
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Environment::~Environment ()
{
  UnregisterAllFunctions ();

  lua_close (impl->state);

  delete impl;
}

/*
    ��� �����
*/

const char* Environment::Name () const
{
  return impl->name.c_str ();
}

void Environment::Rename (const char* new_name)
{
  if (!new_name)
    RaiseNullArgument ("script::lua::Environment::Rename", "new_name");

  impl->name = new_name;
}

/*
    ���� Lua-C
*/

detail::Stack Environment::Stack () const
{
  return impl->state;
}

/*
    ����������� �������
*/

void Environment::RegisterFunctionCore (const char* name, detail::Invoker* invoker, size_t arguments_count)
{
  try
  {
    if (!name)
      RaiseNullArgument ("script::lua::Environment::RegisterFunction", "name");

    if (!invoker)
      RaiseNullArgument ("script::lua::Environment::RegisterFunction", "invoker");
      
      //���������� ����������� � ����� ������������

    impl->invokers [name] = invoker;
    
      //��������� �������-��������, ���������� ��������� ��������� �������

    stl::string args;
    args.reserve (8 * arguments_count);

    for (size_t i = 0; i < arguments_count; i++)
    {
      char arg_buf [8];

      sprintf (&arg_buf[0], "arg%u", i);
      args += &arg_buf[0];
      if (i < arguments_count - 1)
        args += ", ";
    }

    stl::string generated_function ("\nfunction ");

    generated_function.reserve (64 + args.length () * 2 + strlen (name) * 2 + strlen (INVOKE_DISPATCH_NAME));
   
    generated_function += name;
    generated_function += " (";
    generated_function += args;
    generated_function += ")\n return ";
    generated_function += INVOKE_DISPATCH_NAME;
    generated_function += " (\"";
    generated_function += name;
    generated_function += "\"";
    
    if (arguments_count)
    {
      generated_function += ", ";
      generated_function += args;
    }
    
    generated_function += ")\n end";

    DoString (generated_function.c_str());
  }
  catch (...)
  {
    delete invoker;
    throw;
  }
}

void Environment::UnregisterFunction (const char* name) //no throw
{
  if (!name)
    return;
    
  InvokerMap::iterator iter = impl->invokers.find (name);
  
  if (iter == impl->invokers.end ())
    return;

  delete iter->second;

  impl->invokers.erase (iter);
}

void Environment::UnregisterAllFunctions () //no throw
{
  for (InvokerMap::iterator iter=impl->invokers.begin (); iter!=impl->invokers.end (); ++iter)
    delete iter->second;

  impl->invokers.clear ();
}

bool Environment::HasFunction (const char* name) const //no throw
{
  if (!name)
    return false;

  //�������!!!!!!!�������� ������� ������� � ��������� ��� (������� ����� �������, � ��������)
  return false;
}

/*
    ���������� ���������
*/

namespace
{

//������� ���������������� �� ���������
void default_log_handler (const char*)
{
}

}

bool Environment::DoString (const char* expression, const LogFunc& log)
{
  if (!expression)
    RaiseNullArgument ("script::lua::Environment::DoString", "expression");

      //������� dobuffer
  if (luaL_dostring (impl->state, expression))
  {
      //������� ����� ������
    return false;
  }

  return true;
}

bool Environment::DoString (const char* expression)
{
  return DoString (expression, &default_log_handler);
}

bool Environment::DoFile (const char* file_name, const LogFunc& log)
{
  if (!file_name)
    RaiseNullArgument ("script::lua::Environment::DoFile", "file_name");

    //�������� �����!!!

  InputFile in_file (file_name);
  size_t    file_size = in_file.Size ();
  char*     buffer    = (char*)::operator new (file_size + 1);

  try
  {
    in_file.Read (buffer, file_size);

    buffer [file_size] = '\0';

    bool result = DoString (buffer, log);

    ::operator delete (buffer);
    
    return result;
  }
  catch (...)
  {
    ::operator delete (buffer);
    throw;
  }
}

bool Environment::DoFile (const char* file_name)
{
  return DoFile (file_name, &default_log_handler);
}

/*
    ����� ������ lua
*/

void Environment::InvokeCore (size_t args_count, size_t results_count) const
{
  if (lua_pcall (impl->state, args_count, results_count, 0))
    Raise <Exception> ("script::lua::Environment::Invoke", "Error running function: '%s'", lua_tostring (impl->state, -1));
}
