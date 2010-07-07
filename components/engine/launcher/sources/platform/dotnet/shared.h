#ifndef ENGINE_DOT_NET_WRAPPER_SHARED_HEADER
#define ENGINE_DOT_NET_WRAPPER_SHARED_HEADER

#using <mscorlib.dll>
#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>

#include <msclr/auto_gcroot.h>

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>

#include <engine/engine.h>

#include <windows.h> //for dll load
#undef CreateWindow

namespace engine
{

namespace dot_net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������, ����������� ��� ������������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BadAutoString: public xtl::message_exception_base
{
  BadAutoString (const char* source) : message_exception_base (source, "Can't translate string from System::String^ to const char*") {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ���������� CLR-����� � const char*
///////////////////////////////////////////////////////////////////////////////////////////////////
class AutoString
{
  public:
///������������
    AutoString () : str (0) { }

    AutoString (const System::String^ from)
    {
      try
      {
        str = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi (const_cast<System::String^> (from));
      }
      catch (...)
      {
        throw BadAutoString ("tools::ui::windows_forms::AutoString::AutoString");
      }
    }

///����������    
    ~AutoString ()
    {
      try
      {
        if (str)
          System::Runtime::InteropServices::Marshal::FreeHGlobal ((System::IntPtr)str);
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }

///������
    const char* Data ()
    {
      return str ? str : "";
    }
    
///�����
    void Swap (AutoString& s)
    {
      stl::swap (str, s.str);
    }

  private:
    char* str;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
inline const char* get_string (System::String^ from, AutoString& buffer)
{
  AutoString (from).Swap (buffer);
  
  return buffer.Data ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ����������, ��������������� �� .NET Framework
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DotNetException: public xtl::message_exception_base
{
  DotNetException (const char* source, System::Exception^ exception) :
    message_exception_base (source, AutoString (exception->ToString()).Data ()) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������, ����������� � ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IEventHandlerProxy
{
  public:
    virtual ~IEventHandlerProxy () {}
    
    virtual void Process (System::Object^, System::EventArgs^) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� � ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
System::EventHandler^ make_event_handler (stl::auto_ptr<IEventHandlerProxy>& proxy);

template <class Ptr, class MemFn> System::EventHandler^ make_event_handler (const Ptr& ptr, const MemFn& fn)
{
  struct EventHandlerProxy: public IEventHandlerProxy
  {
    Ptr   ptr;
    MemFn fn;
    
    EventHandlerProxy (const Ptr& in_ptr, const MemFn& in_fn) : ptr (in_ptr), fn (in_fn) {}
       
    void Process (System::Object^ sender, System::EventArgs^ args) { ((*ptr).*fn)(sender, args); }
  };
  
  stl::auto_ptr<IEventHandlerProxy> proxy (new EventHandlerProxy (ptr, fn));
  
  return make_event_handler (proxy);
}

}

}

#endif
