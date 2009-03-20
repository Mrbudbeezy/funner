#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;


namespace
{

/*
    ���������
*/

const char* INTERNAL_SYMBOL_PREFIX = "__internal_"; //������� ���������� ��� ��������

}

/*
    ������ - ���������� ����������� ������� ��� (�������, ����-������� � �.�.)
*/

class SymbolRegistry::Symbol: public ISymbol, public xtl::reference_counter
{
  public:
///�����������
    Symbol (SymbolRegistry* in_registry, void* in_symbol_handle, int index) 
      : registry (in_registry)
      , symbol_handle (in_symbol_handle)
      , internal_name (common::format ("%s%p", INTERNAL_SYMBOL_PREFIX, symbol_handle))
    {
      try
      {
        lua_State* state = registry->State ();
        
        check_stack (state, 2);

        lua_pushvalue (state, index);
        lua_setglobal (state, internal_name.c_str ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("script::lua::SymbolRegistry::Symbol::Symbol");
        throw;
      }
    }

///����������
    ~Symbol ()
    {            
      if (!registry)
        return;              
        
      try
      {
        lua_State* state = registry->State ();
        
        check_stack (state, 2);                        
        
        lua_pushnil   (state);
        lua_setglobal (state, internal_name.c_str ());
      }
      catch (...)
      {
        //���������� ���� ����������
      }
      
      registry->RemoveSymbol (symbol_handle);
    }

///��������� ����� �������
    const char* Name () { return internal_name.c_str (); }
    
///������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }        
    
///���������� �� �������� ����������� ���������
    void ResetState ()
    {
      registry = 0;
    }

  private:
    SymbolRegistry* registry;
    void*           symbol_handle;
    stl::string     internal_name;
};      

/*
    ����������� / ����������
*/

SymbolRegistry::SymbolRegistry (lua_State* in_state)
  : state (in_state)
{
}

SymbolRegistry::~SymbolRegistry ()
{
    //���������� ���� �������� �� �������� ����������� ���������
    
  for (SymbolMap::iterator iter=symbols.begin (), end=symbols.end (); iter!=end; ++iter)
    iter->second->ResetState ();
}

/*
    ��������� � �������� �������
*/

ISymbol* SymbolRegistry::GetSymbol (int index)
{
  try
  {
      //��������� �������������� �������

    void* symbol_handle = (void*)lua_topointer (state, index);

    if (!symbol_handle)
      throw xtl::format_exception<ArgumentException> ("", "Null symbol at index %u in stack", index);

      //����� ������� ����� ������������������

    SymbolMap::iterator iter = symbols.find (symbol_handle);

    if (iter != symbols.end ())
    {
        //������ ��� ���������������

      iter->second->AddRef ();

      return iter->second;
    }
    
      //����������� ������ �������

    xtl::com_ptr<Symbol> symbol (new Symbol (this, symbol_handle, index), false);

    symbols.insert_pair (symbol_handle, symbol.get ());

    symbol->AddRef ();

    return symbol.get ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::lua::SymbolRegistry::GetSymbol");
    throw;
  }
}

void SymbolRegistry::RemoveSymbol (void* symbol_handle)
{
  symbols.erase (symbol_handle);
}
