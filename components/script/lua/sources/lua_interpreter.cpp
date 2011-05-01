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

const char* VARIANT_DEFAULT_TYPE_NAME = "__lua_variant_type";   //��� ����������� ���� ������ �� ���������

}

}

namespace
{

const char* INTERPRETER_LUA_NAME = "__funner_interpreter"; //��� ����, ��������� ��������� �� �������������
const char* PROFILER_LIBRARY     = "Profiler";

/*
    �������
*/

//������� ��������� ������ lua
int error_handler (lua_State* state)
{
  throw xtl::format_exception<RuntimeException> ("script::lua::error_handler", "%s", lua_tostring (state, -1));
}

void lua_hook (lua_State *lua_state, lua_Debug *ar)
{
  lua_getglobal (lua_state, INTERPRETER_LUA_NAME);

  Interpreter* interpreter = (Interpreter*)lua_touserdata (lua_state, -1);

  lua_pop (lua_state, 1);

  interpreter->LuaHook (ar);
}

}

Interpreter::LuaHookProfile::LuaHookProfile ()
{
  ShinyZone_clear (&zone);
  cache = &_ShinyNode_dummy;
}

/*
    ����������� / ����������
*/

Interpreter::Interpreter (const script::Environment& in_environment)
  : environment (in_environment),
    symbol_registry (State ()),
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

#ifdef LUAJIT    
    //restore default float precision for luajit

  _clearfp   ();
  _control87 (_RC_NEAR + _PC_53 + _EM_INVALID + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW + _EM_INEXACT + _EM_DENORMAL, 0xfffff);
  
#endif

  luaL_openlib (state, 0, user_data_meta_table, 0);

    //����������� ������������ ������� ��������/�������� ���������

  on_create_library_connection = environment.RegisterEventHandler (EnvironmentLibraryEvent_OnCreate,
    xtl::bind (&Interpreter::RegisterLibrary, this, _2, _3));

  on_remove_library_connection = environment.RegisterEventHandler (EnvironmentLibraryEvent_OnRemove,
    xtl::bind (&Interpreter::UnregisterLibrary, this, _2));

    //����������� ���������

  for (Environment::LibraryIterator i=environment.CreateLibraryIterator (); i; ++i)
    RegisterLibrary (environment.LibraryId (i), *i);

    //������� �����

  lua_settop (state, 0);

    //����������� ������� ��������������

  stack.Push (this);
  lua_setglobal (state, INTERPRETER_LUA_NAME);

  InvokerRegistry profiler_lib;

  RegisterLibrary (PROFILER_LIBRARY, profiler_lib);

  profiler_lib.Register ("Start", make_invoker<void ()> (xtl::bind (&Interpreter::StartProfiling, this)));
  profiler_lib.Register ("Stop", make_invoker<void ()> (xtl::bind (&Interpreter::StopProfiling, this)));
  profiler_lib.Register ("Update", make_invoker<void ()> (xtl::bind (&Interpreter::UpdateProfileInfo, this)));
  profiler_lib.Register ("Clean", make_invoker<void ()> (xtl::bind (&Interpreter::CleanProfileInfo, this)));
  profiler_lib.Register ("TreeString", make_invoker (make_invoker<const char* (size_t)> (xtl::bind (&Interpreter::ProfileTreeState, this, _1)),
                                                     make_invoker<const char* ()> (xtl::bind (&Interpreter::ProfileTreeState, this, -1))));
  profiler_lib.Register ("FlatString", make_invoker (make_invoker<const char* (size_t)> (xtl::bind (&Interpreter::ProfileFlatState, this, _1)),
                                                     make_invoker<const char* ()> (xtl::bind (&Interpreter::ProfileFlatState, this, -1))));
}

Interpreter::~Interpreter ()
{
  StopProfiling ();
}

/*
    ��� ��������������
*/

const char* Interpreter::Name ()
{
  return LUA_RELEASE;
}

/*
    ���������� ��������� / ������ �������� / ���� ����������
*/

Environment& Interpreter::Environment ()
{
  return environment;
}

lua::SymbolRegistry& Interpreter::SymbolRegistry ()
{
  return symbol_registry;
}

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
   ��������������
*/

void Interpreter::LuaHook (lua_Debug* ar)
{
  // ignore tail call
  if (ar->i_ci == 0 || ar->event == LUA_HOOKTAILRET)
    return;

  // ignore nameless function
  lua_getinfo (state, "n", ar);

  if (!ar->name)
    return;

  if (ar->event == LUA_HOOKCALL)
  {
    lua_getinfo (state, "f", ar);

    const void* func = lua_topointer (state, -1);

    lua_pop (state, 1);

    LuaHookProfile& profile = hook_profiles [func];

    if (!profile.zone.name)
    {
      lua_getinfo (state, "S", ar);

      switch (ar->what [0])
      {
        case 'L': // "Lua"
          profile.name = common::format ("%s(%d):%s", ar->source, ar->linedefined, ar->name);
          break;
        case 'C': // "C"
          profile.name = "C:";
          profile.name += ar->name;
          break;
        default:
          profile.name = "<unknown>";
      }

      profile.zone.name = profile.name.c_str ();
    }

    ShinyManager_lookupAndBeginNode (&Shiny_instance, &profile.cache, &profile.zone);
  }
  else
    ShinyManager_endCurNode (&Shiny_instance);
}

void Interpreter::StartProfiling ()
{
  lua_sethook (state, lua_hook, LUA_MASKCALL | LUA_MASKRET, 0);
}

void Interpreter::StopProfiling ()
{
  lua_sethook (state, lua_hook, 0, 0);
}

void Interpreter::UpdateProfileInfo ()
{
  ShinyManager_update (&Shiny_instance);
}

void Interpreter::CleanProfileInfo ()
{
  ShinyManager_updateClean (&Shiny_instance);
}

const char* Interpreter::ProfileTreeState (size_t max_lines)
{
  const char* error = ShinyManager_getOutputErrorString (&Shiny_instance);

  if (error)
    return error;

  size_t nodes_count = stl::min (max_lines, (size_t)Shiny_instance.nodeCount);

  profile_info_string.resize (ShinyPrintNodesSize (Shiny_instance.nodeCount) - 1);

  ShinyPrintNodes (&profile_info_string[0], &Shiny_instance.rootNode);

  profile_info_string.resize (ShinyPrintNodesSize (nodes_count) - 1);

  return profile_info_string.c_str ();
}

const char* Interpreter::ProfileFlatState (size_t max_lines)
{
  const char* error = ShinyManager_getOutputErrorString (&Shiny_instance);

  if (error)
    return error;

  ShinyManager_sortZones (&Shiny_instance);

  size_t zones_count = stl::min (max_lines, (size_t)Shiny_instance.zoneCount);

  profile_info_string.resize (ShinyPrintZonesSize (Shiny_instance.zoneCount) - 1);

  ShinyPrintZones (&profile_info_string[0], &Shiny_instance.rootZone);

  profile_info_string.resize (ShinyPrintZonesSize (zones_count) - 1);

  return profile_info_string.c_str ();
}

namespace
{

/*
    �������� �������������� lua
*/

IInterpreter* create_lua_interpreter (const Environment& environment)
{
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
