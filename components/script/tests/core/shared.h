#ifndef SCRIPTLIB_CORE_TESTS_SHARED_HEADER
#define SCRIPTLIB_CORE_TESTS_SHARED_HEADER

#include <stdio.h>
#include <stl/vector>
#include <stl/memory>
#include <xtl/shared_ptr.h>
#include <xtl/ref.h>
#include <common/strlib.h>
#include <script/bind.h>

using namespace script;

class MyStack: public IStack
{
  public:
    size_t Size () { return array.size (); }

    float       GetFloat   (size_t index) { return xtl::any_multicast<float> (array.at (index)); }
    int         GetInteger (size_t index) { return xtl::any_multicast<int> (array.at (index)); }
    void*       GetPointer (size_t index) { return xtl::any_multicast<void*> (array.at (index)); }
    const char* GetString  (size_t index) { return xtl::any_multicast<const char*> (array.at (index)); }
    const char* GetSymbol  (size_t index) { return MyStack::GetString (index); }
    xtl::any&   GetVariant (size_t index) { return array.at (index); }

    void Push (float value)        { array.push_back (xtl::make_ref_any (value)); }
    void Push (int value)          { array.push_back (xtl::make_ref_any (value)); }
    void Push (void* pointer)      { array.push_back (xtl::make_ref_any (pointer)); }
    void Push (const char* string) { array.push_back (xtl::make_ref_any (string)); }
    void Push (const xtl::any& a)  { array.push_back (a); }

    void PushSymbol (const char* string) { MyStack::Push (string); }

    void Pop (size_t arguments_count)
    {
      if (arguments_count > array.size ())
        arguments_count = array.size ();
      
      array.erase (array.end () - arguments_count, array.end ());
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
    
    Array array;
};

class MyInterpreter: public IInterpreter
{
  public:
    const char* Name () { return "MyInterpreter"; }
    IStack&     Stack () { return stack; }

    void DoCommands (const char*, const void*, size_t, const LogFunction&)
    {
      common::RaiseNotImplemented ("MyInterpreter::DoCommands");
    }

    bool HasFunction (const char*) { return true; }
    
    void Invoke (size_t arguments_count, size_t results_count)
    {
      if (arguments_count >= stack.Size ())
        throw StackException ("MyInterpreter::Invoke", "Stack underflow");
        
      const char* function_name = stack.GetSymbol (stack.Size () - arguments_count - 1);
      
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
    
  private:
    MyStack stack;
};

struct X: public xtl::dynamic_cast_root
{
  virtual const char* name () const { return "struct X"; }

  virtual const char* test () const { return "X::test"; }
};

struct Y: X
{
  const char* name () const { return "struct Y"; }

  const char* test () const { return "Y::test"; }
};

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

void to_string (stl::string& buffer, const X& value)
{
  buffer = value.name ();
}

void to_string (stl::string& buffer, const A& value)
{
  buffer = common::format ("A('%s')", value.name.c_str ());
}

#endif
