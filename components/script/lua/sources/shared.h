#ifndef SCRIPTLIB_LUA_SHARED_HEADER
#define SCRIPTLIB_LUA_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/any.h>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable.h>

#include <common/component.h>
#include <common/heap.h>
#include <common/strlib.h>

#include <script/bind.h>
#include <script/interpreter.h>
#include <script/environment.h>

#ifdef LUAJIT

#include <float.h>

extern "C" {
#include <luajit.h>
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef LUAJIT
}
#endif

extern "C" {
#include <ShinyManager.h>
}

namespace script
{

namespace lua
{

//forward declaration
class Interpreter;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������� ���� ������ "�� ���������"
///////////////////////////////////////////////////////////////////////////////////////////////////
extern const char* VARIANT_DEFAULT_TYPE_NAME;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ lua
///////////////////////////////////////////////////////////////////////////////////////////////////
class StateHolder
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    StateHolder  ();
    ~StateHolder ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    lua_State* State () const { return state; }

  private:
    StateHolder (const StateHolder&); //no impl
    StateHolder& operator = (const StateHolder&); //no impl

  protected:
    lua_State* state;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Stack: public IStack
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Stack (lua_State* state, lua::Interpreter& interpreter);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������, �������� ����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IInterpreter& Interpreter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    float       GetFloat   (size_t index);
    int         GetInteger (size_t index);
    bool        GetBoolean (size_t index);
    void*       GetPointer (size_t index);
    const char* GetString  (size_t index);
    ISymbol*    GetSymbol  (size_t index);
    xtl::any&   GetVariant (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Push       (float value);
    void Push       (int value);
    void Push       (bool value);
    void Push       (void* pointer);
    void Push       (const char* string);
    void Push       (const xtl::any& object);
    void PushSymbol (const char* string);
    void PushSymbol (ISymbol*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Pop (size_t arguments_count);    

  private:
    lua_State*        state;
    lua::Interpreter& interpreter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SymbolRegistry
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SymbolRegistry  (lua_State* state);
    ~SymbolRegistry ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    lua_State* State () const { return state; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISymbol* GetSymbol (int index);

  private:
    SymbolRegistry (const SymbolRegistry&); //no impl
    SymbolRegistry& operator = (const SymbolRegistry&); //no impl

    void RemoveSymbol (void* symbol_handle);    

  private:
    class Symbol;
    typedef stl::hash_map<void*, Symbol*> SymbolMap;

    friend class Symbol;

  private:
    lua_State* state;
    SymbolMap  symbols;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� Lua
///////////////////////////////////////////////////////////////////////////////////////////////////
class Library: public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Library  (Interpreter& interpreter, const char* name, const InvokerRegistry& registry);
    ~Library ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterInvoker   (const char* invoker_name, Invoker& invoker);
    void UnregisterInvoker (const char* invoker_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Destroy ();

  private:
    Library (const Library&); //no impl
    Library& operator = (const Library&); //no impl

  private:
    lua_State*           state;                            //��������� Lua
    Interpreter&         interpreter;                      //�������������
    InvokerRegistry      registry;                         //������ ������
    stl::string          table_name;                       //��� �������
    bool                 is_global;                        //�������� �� ���������� ����������
    xtl::auto_connection on_register_invoker_connection;   //���������� ����������� �����
    xtl::auto_connection on_unregister_invoker_connection; //���������� �������� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� Lua
///////////////////////////////////////////////////////////////////////////////////////////////////
class Interpreter: public IInterpreter, public StateHolder, public xtl::reference_counter, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Interpreter  (const script::Environment&);
    ~Interpreter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ �������� / ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    script::Environment& Environment ();
    lua::SymbolRegistry& SymbolRegistry ();
    IStack&              Stack ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasFunction (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ������������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Invoke (size_t arguments_count, size_t results_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������, ����������� �� �������� IInterpreter
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::trackable& GetTrackable () { return *this; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LuaHook (lua_Debug* ar);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterLibrary   (const char* name, InvokerRegistry& registry);
    void UnregisterLibrary (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        StartProfiling    ();
    void        StopProfiling     ();
    void        UpdateProfileInfo ();
    void        CleanProfileInfo ();
    const char* ProfileTreeState  (size_t max_lines);
    const char* ProfileFlatState  (size_t max_lines);

  private:
    //���������� ��� �������������� lua-�������
    struct LuaHookProfile
    {
      ShinyZone      zone;
      ShinyNodeCache cache;
      stl::string    name;

      LuaHookProfile ();
    };

  private:
    typedef xtl::intrusive_ptr<Library>                           LibraryPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, LibraryPtr> LibraryMap;
    typedef stl::hash_map<const void*, LuaHookProfile>            HookProfileMap;

  private:
    script::Environment   environment;                  //���������� ���������
    lua::SymbolRegistry   symbol_registry;              //������ ��������
    lua::Stack            stack;                        //���� ����������
    LibraryMap            libraries;                    //����� ���������
    HookProfileMap        hook_profiles;                //����� �������� ���-�������
    stl::string           profile_info_string;          //������ ��� �������� ���������� � ��������������
    xtl::auto_connection  on_create_library_connection; //���������� �� ������� �������� ����������
    xtl::auto_connection  on_remove_library_connection; //���������� �� ������� �������� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
int  variant_destroy  (lua_State*); //�������� ������� ����������� ���� ������
int  variant_tostring (lua_State*); //������ � ������ ��������� ������� ����������������� ���� ������
int  safe_call        (lua_State*, int (*f)(lua_State*)); //���������� ����� �����
void dump_stack       (lua_State*, stl::string& buffer); //������ ��������� �����
void check_stack      (lua_State* state, size_t count = 1); //�������� ����������� ��������� � ���� count ����������
void check_item       (lua_State* state, size_t index, int expected_type, const char* function_name); //�������� ������������ ���� ��������, ������������ �� �����
void raise_error      (lua_State* state, const char* source); //��������� ��������� �� ������

//��������� ������ � ������ ������
stl::string get_lua_position (lua_State* state);

//���������� ����� ������
template <class Fn>
int safe_call (lua_State* state)
{
  try
  {
    return Fn::Invoke (state);
  }
  catch (xtl::bad_any_cast& e)
  {
    luaL_error (state, "%s: %s -> %s%s", e.what (), e.source_type ().name (), e.target_type ().name (), get_lua_position (state).c_str ());
  }
  catch (std::exception& e)
  {
    luaL_error (state, "%s%s", e.what (), get_lua_position (state).c_str ());
  }
  catch (...)
  {
    luaL_error (state, "%s%s", lua_gettop (state) ? lua_tostring (state, -1) : "internal error", get_lua_position (state).c_str ());
  }

  return 0;
}

}

}

#endif
