#ifndef SCRIPTLIB_CORE_TESTS_SHARED_HEADER
#define SCRIPTLIB_CORE_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/vector>
#include <stl/memory>

#include <xtl/function.h>
#include <xtl/shared_ptr.h>
#include <xtl/ref.h>
#include <xtl/iterator.h>
#include <xtl/connection.h>
#include <xtl/implicit_cast.h>
#include <xtl/common_exceptions.h>
#include <xtl/lexical_cast.h>

#include <common/component.h>
#include <common/strlib.h>

#include <script/bind.h>
#include <script/environment.h>
#include <script/library_manager.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //this used in base member initializer list
#endif

using namespace script;

void to_string (stl::string& buffer, const xtl::any& a);

class MySymbol: public ISymbol
{
  public:
    MySymbol (const char* in_name) : name (in_name), ref_count (1) {}

    const char* Name () { return name.c_str (); }
    
    void AddRef () { ref_count++; }

    void Release ()
    {
      if (!--ref_count)
        delete this;
    }

  private:
    stl::string name;
    size_t      ref_count;
};

class MyStack: public IStack
{
  public:
    MyStack (IInterpreter* in_interpreter = 0) : interpreter (in_interpreter) {}
  
    size_t Size () { return array.size (); }
    
    IInterpreter& Interpreter ()
    {
      if (!interpreter)
        throw xtl::format_operation_exception ("MyStack::Interpreter", "Null interpreter");

      return *interpreter;
    }

    float       GetFloat   (size_t index) { return xtl::any_multicast<float> (array.at (index)); }
    int         GetInteger (size_t index) { return xtl::any_multicast<int> (array.at (index)); }
    bool        GetBoolean (size_t index) { return xtl::any_multicast<bool> (array.at (index)); }
    void*       GetPointer (size_t index) { return xtl::any_multicast<void*> (array.at (index)); }
    const char* GetString  (size_t index) { return xtl::any_multicast<const char*> (array.at (index)); }
    ISymbol*    GetSymbol  (size_t index) { return new MySymbol (GetString (index)); }
    xtl::any&   GetVariant (size_t index) { return array.at (index); }

    void Push (float value)        { array.push_back (xtl::any (value)); }
    void Push (int value)          { array.push_back (xtl::any (value)); }
    void Push (bool value)         { array.push_back (xtl::any (value)); }
    void Push (void* pointer)      { array.push_back (xtl::any (pointer)); }
    void Push (const char* string) { array.push_back (xtl::any (string)); }
    void Push (const xtl::any& a)  { array.push_back (a); }

    void PushSymbol (const char* string) { MyStack::Push (string); }
    void PushSymbol (ISymbol* symbol)    { MyStack::Push (symbol->Name ()); }

    void Pop (size_t arguments_count)
    {
      if (arguments_count > array.size ())
        arguments_count = array.size ();
      
      array.erase (array.end () - arguments_count, array.end ());
    }
    
    void Clear ()
    {
      array.clear ();
    }
    
    void Dump (size_t index)
    {
      try
      {
        stl::string buffer;      
      
        to_string (buffer, GetVariant (index));

        printf ("'%s'", buffer.c_str ());
      }
      catch (std::exception& exception)
      {
        printf ("dump failed; reason: %s", exception.what ());
      }    
    }
    
    void Dump ()
    {
      printf ("stack (size=%u):\n", Size ());

      for (size_t i=0; i<Size (); i++)
      {
        printf ("  item[%u]: ", i);
        Dump   (i);
        printf ("\n");
      }
    }    

  private:
    typedef stl::vector<xtl::any> Array;
    
    Array         array;
    IInterpreter* interpreter;
};

class MyInterpreter: public IInterpreter, public xtl::trackable
{
  public:
    MyInterpreter () : stack (this) {}

    const char* Name () { return "MyInterpreter"; }
    IStack&     Stack () { return stack; }

    void DoCommands (const char* name, const void* buffer, size_t buffer_size)
    {
      printf ("MyInterpreter::DoCommands(%s,'%s')\n", name, stl::string ((char*)buffer, buffer_size).c_str ());
    }

    bool HasFunction (const char*) { return true; }
    
    void Invoke (size_t arguments_count, size_t results_count)
    {
      if (arguments_count >= stack.Size ())
        throw xtl::format_exception<StackException> ("MyInterpreter::Invoke", "Stack underflow");
        
      const char* function_name = stack.GetString (stack.Size () - arguments_count - 1);
      
      printf ("invoke '%s'\n", function_name);
      
      for (size_t i=0; i<arguments_count; i++)
      {
        printf     ("  arg%u: ", i + 1);
        stack.Dump (stack.Size () - arguments_count + i);
        printf     ("\n");
      }
      
      stack.Pop (arguments_count + 1);
      
      for (size_t i=0; i<results_count; i++)
        stack.Push ("result");
    }

    void AddRef  () {}
    void Release () {}
    
    xtl::trackable& GetTrackable () { return *this; }
    
  private:
    MyStack stack;
};

struct X
{
  virtual const char* name () const { return "struct X"; }

  virtual const char* test () const { return "X::test"; }
};

struct Y: X
{
  const char* name () const { return "struct Y"; }

  const char* test () const { return "Y::test"; }
};

template class xtl::declcast<Y, X>;
template class xtl::declcast<X, Y, xtl::dynamic_caster>;

struct A
{
  A (const char* in_name) : name (in_name) {}

  xtl::shared_ptr<X> f (float& x, const char* y)
  {
    printf ("%s: f(%g,%s)\n", name.c_str (), x, y);
    
    x += 2.0f;
    name += '!';

    return xtl::shared_ptr<X> (new Y);
  }

  stl::string name;
};

void to_string (stl::string& buffer, const xtl::any& a)
{
  buffer.clear ();

/*  if (&a.type () == &typeid (int))
  {
    xtl::to_string (buffer, a.cast<int&> ());
    return;
  }

  if (&a.type () == &typeid (float))
  {
    xtl::to_string (buffer, a.cast<float&> ());
    return;
  }

  if (&a.type () == &typeid (stl::string))
  {
    xtl::to_string (buffer, a.cast<stl::string&> ());
    return;
  }*/
}

#endif
