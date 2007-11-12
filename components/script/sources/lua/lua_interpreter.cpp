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

//������� ���������������� �� ���������
void log_function (const char*)
{
}

//������� ��������� ������ lua
int error_handler (lua_State* state)
{
  Raise<RuntimeException> ("script::lua::error_handler", "%s", lua_tostring (state, -1));

  return 0;
}

//������� ������ ������
void* reallocate (void* user_data, void* ptr, size_t old_size, size_t new_size)
{
  try
  {
    common::Heap& heap = *reinterpret_cast<common::Heap*> (user_data);

    if (!new_size)
    {
      heap.Deallocate (ptr);
      return 0;    
    }

    void* new_buffer = heap.Allocate (new_size);

    if (!new_buffer)
      return 0;    

    if (ptr)
    {
      memcpy (new_buffer, ptr, old_size < new_size ? old_size : new_size);

      heap.Deallocate (ptr);
    }

    return new_buffer;
  }
  catch (...)
  {
    //��������� ��� ����������
    return 0;
  }
}

}

/*
    ����������� / ����������
*/

Interpreter::Interpreter (const EnvironmentPointer& in_environment)
  : environment (in_environment),
    ref_count (1)
{
    //������������� ��������� lua

  state = lua_newstate (&reallocate, &MemoryManager::GetHeap ());

  if (!state)
    Raise <InterpreterException> ("script::lua::Interpreter::Interpreter", "Can't create lua state");

  try
  {
    luaL_openlibs (state);

      //����������� ������������ �������� ���������������� ����� ������    

    static const luaL_reg user_data_meta_table [] = {
      {"__gc", &destroy_object},
      {0,0}
    };
    
    luaL_newmetatable (state, USER_DATA_TAG);
    luaI_openlib      (state, 0, user_data_meta_table, 0); //must be 1

      //����������� ������� ��������� ������

    lua_atpanic (state, &error_handler);

      //������������� �����

    stack.SetState (state);
    
      //����������� ������������ ������� ��������/�������� ��������
      
    on_create_registry_connection = environment->RegisterEventHandler (EnvironmentEvent_OnCreateRegistry,
      xtl::bind (&Interpreter::RegisterInvokerRegistry, this, _2, _3));
      
    on_remove_registry_connection = environment->RegisterEventHandler (EnvironmentEvent_OnRemoveRegistry,
      xtl::bind (&Interpreter::UnregisterInvokerRegistry, this, _2));
      
      //����������� ������������ ������� ��������� ����� ��������

    for (Environment::Iterator i=environment->CreateIterator (); i; ++i)
      RegisterInvokerRegistry (environment->RegistryId (i), *i);

      //������� �����

    lua_settop (state, 0);
  }
  catch (...)
  {
    lua_close (state);
    throw;
  }
}

Interpreter::~Interpreter ()
{
    //�������� ����������
    
  for (MetatableMap::iterator i=metatables.begin (), end=metatables.end (); i!=end; ++i)
    delete i->second;
    
  metatables.clear ();
    
    //�������� lua

  lua_close (state);
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
  ref_count++;
}

void Interpreter::Release ()
{
  if (!--ref_count)
    delete this;
}

/*
   ����������� / �������� ������
*/

void Interpreter::RegisterInvoker (const char* invoker_name, Invoker& invoker)
{
  if (!invoker_name)
    RaiseNullArgument ("script::lua::Interpreter::RegisterInvoker", "invoker_name");
    
    //����������� �-closure
    
  lua_pushlightuserdata (state, &invoker);
  lua_pushstring        (state, invoker_name);  //??????
  lua_pushcclosure      (state, &invoke_dispatch, 2);
  lua_setglobal         (state, invoker_name);
}

void Interpreter::UnregisterInvoker (const char* invoker_name)
{
  if (!invoker_name)
    return;

  lua_pushnil    (state);
  lua_setglobal  (state, invoker_name);
}
    
/*
    ����������� / �������� ��������
*/

void Interpreter::RegisterInvokerRegistry (const char* registry_name, InvokerRegistry& registry)
{
  if (!registry_name)
    RaiseNullArgument ("script::lua::RegisterInvokerRegistry", "registry_name");     
    
    //��������: �������� �� �������������� ������ ����������

  if (!strcmp ("global", registry_name))
  {
    RegisterGlobalRegistry (registry);
  }
  else
  {    
      //����������� ����-�������
      
    Metatable* metatable = new Metatable (state, registry_name, registry);
    
    try
    {
      metatables.insert_pair (registry_name, metatable);
    }
    catch (...)
    {
      delete metatable;
      throw;
    }
  }
}

void Interpreter::UnregisterInvokerRegistry (const char* registry_name)
{
  if (!registry_name)
    return;
       
  if (!strcmp ("global", registry_name))
  {
    on_register_invoker_connection.disconnect ();
    on_unregister_invoker_connection.disconnect ();
  }
  else
  {
      //�������� �����������      

    MetatableMap::iterator iter = metatables.find (registry_name);

    if (iter == metatables.end ())
      return;

    delete iter->second;

    metatables.erase (iter);
  }
}

void Interpreter::RegisterGlobalRegistry (InvokerRegistry& registry)
{
    //����������� ������������ �������

  on_register_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnRegisterInvoker,
      bind (&Interpreter::RegisterInvoker, this, _2, _3));

  on_unregister_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnUnregisterInvoker,
      bind (&Interpreter::UnregisterInvoker, this, _2));
      
    //����������� ������
    
  for (InvokerRegistry::Iterator i=registry.CreateIterator (); i; ++i)
    RegisterInvoker (registry.InvokerId (i), *i);
}

/*
    ����� �����������
*/

Metatable* Interpreter::FindMetatable (const char* name) const
{
  if (!name)
    return 0;
    
  MetatableMap::const_iterator iter = metatables.find (name);
  
  if (iter == metatables.end ())
    return 0;
    
  return iter->second;
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
