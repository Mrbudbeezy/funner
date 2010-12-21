#ifndef RENDER_GL_DRIVER_GLX_SHARED_HEADER
#define RENDER_GL_DRIVER_GLX_SHARED_HEADER

#include <stl/algorithm>
#include <stl/auto_ptr.h>
#include <stl/hash_map>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/window.h>
#include <syslib/platform/x11.h>

#include <shared/log.h>
#include <shared/object.h>
#include <shared/platform.h>
#include <shared/property_list.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/xf86vmode.h>

namespace render
{

namespace low_level
{

namespace opengl
{

namespace glx
{

//forward declarations
class Adapter;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Output: virtual public IOutput, public Object
{
  public:
    typedef xtl::com_ptr<Output> Pointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    Output  (Display* display, int screen_number);
    ~Output ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    int GetScreenNumber ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetModesCount ();
    void   GetModeDesc   (size_t mode_index, OutputModeDesc& mode_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �����-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCurrentMode (const OutputModeDesc&);
    void GetCurrentMode (OutputModeDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetGammaRamp (const Color3f table [256]);
    void GetGammaRamp (Color3f table [256]);
    
  private:
    Output (const Output&); //no impl
    Output& operator = (const Output&); //no impl
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ��������������� ������ � ��������������� ������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputManager  ();
    ~OutputManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t  GetOutputsCount () const;
    Output* GetOutput       (size_t index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������, ���������� �������� � ����� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Output* FindContainingOutput (Window window) const;

  private:
    OutputManager (const OutputManager&); //no impl
    OutputManager& operator = (const OutputManager&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class Library: public ILibrary, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Library ();
    ~Library ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* GetProcAddress (const char* name, size_t search_flags);

  private:
    Library (const Library&); //no impl
    Library& operator = (const Library&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Adapter: virtual public IAdapter, public Object
{
  friend class Output;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Adapter  ();
    ~Adapter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� / ���� � ������ / ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName        ();
    const char* GetPath        ();
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   GetOutputsCount ();
    IOutput* GetOutput       (size_t index);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ILibrary& GetLibrary ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Output::Pointer GetOutput (Window window);

  private:
    Adapter (const Adapter&); //no impl
    Adapter& operator = (const Adapter&); //no impl    

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimarySwapChain: virtual public ISwapChain, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimarySwapChain  (Adapter* adapter, const SwapChainDesc& desc);
    ~PrimarySwapChain ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* GetAdapter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (SwapChainDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������ � ������������ �������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IOutput* GetContainingOutput ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ ��������� full-screen mode
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFullscreenState (bool state);
    bool GetFullscreenState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������� ������ � ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Present ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Context: virtual public IContext, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Context  (ISwapChain* swap_chain);
    ~Context ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeCurrent (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ������� ������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    ILibrary& GetLibrary ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachListener (IContextListener*);
    void DetachListener (IContextListener*);

  private:
    Context (const Context&); //no impl
    Context& operator = (const Context&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
void check_errors (const char* source);
void raise_error  (const char* source);

using syslib::x11::DisplayManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� X11
///////////////////////////////////////////////////////////////////////////////////////////////////
class DisplayLock
{
  public:
    DisplayLock () : display ((Display*)DisplayManager::DisplayHandle ())
    {
      XLockDisplay (display);    
    }
  
    DisplayLock (Display* in_display) : display (in_display)
    {
      XLockDisplay (display);
    }

    ~DisplayLock ()
    {
      XUnlockDisplay (display);
    }

  private:
    Display* display;
};


}

}

}

}

#endif
