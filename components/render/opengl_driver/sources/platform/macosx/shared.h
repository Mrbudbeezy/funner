#ifndef RENDER_GL_DRIVER_MACOSX_SHARED_HEADER
#define RENDER_GL_DRIVER_MACOSX_SHARED_HEADER

#include <stl/auto_ptr.h>
#include <stl/list>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/utf_converter.h>

#include <syslib/dll.h>

#include <render/low_level/utils.h>

#include <shared/log.h>
#include <shared/object.h>
#include <shared/platform.h>

#include <IOGraphicsLib.h>

#include <CGDirectDisplay.h>
#include <CGDisplayConfiguration.h>

#include <agl.h>

namespace render
{

namespace low_level
{

namespace opengl
{

namespace macosx
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Output: virtual public IOutput, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Output  (CGDirectDisplayID display_id);
    ~Output ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CGDirectDisplayID GetDisplayID ();

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
    Output* FindContainingOutput (WindowRef window) const;

  private:
    OutputManager (const OutputManager&); //no impl
    OutputManager& operator = (const OutputManager&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Adapter: virtual public IAdapter, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Adapter  ();
    ~Adapter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� / �������� / ���� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetPath        ();
    const char* GetName        ();
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   GetOutputsCount ();
    IOutput* GetOutput       (size_t index);

  private:
    Adapter (const Adapter&); //no impl
    Adapter& operator = (const Adapter&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISwapChainImpl: virtual public ISwapChain
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void       SetContext (AGLContext context) = 0;
    virtual AGLContext GetContext () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual AGLPixelFormat GetPixelFormat () = 0;

  protected:
    virtual ~ISwapChainImpl () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimarySwapChain: virtual public ISwapChainImpl, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimarySwapChain  (const SwapChainDesc& sc_desc, Adapter* adapter);
    ~PrimarySwapChain ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (SwapChainDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* GetAdapter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������ � ������������ �������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IOutput* GetContainingOutput ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������� ������ � ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Present ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ ��������� full-screen mode
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFullscreenState (bool state);
    bool GetFullscreenState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ISwapChainImpl
///////////////////////////////////////////////////////////////////////////////////////////////////
    void           SetContext     (AGLContext context);
    AGLContext     GetContext     ();
    AGLPixelFormat GetPixelFormat ();


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
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Context: virtual public IContext, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Context  (ISwapChain* swap_chain, Library* library);
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

//�������� ������
void check_quartz_error         (CGError error_code, const char* source, const char* message);   //�������� ������ Quartz Framework
void check_window_manager_error (OSStatus error_code, const char* source);                       //��������� ���������� WindowManager Framework
void check_agl_error            (const char* source);                                            //�������� ������ Agl Framework
void raise_agl_error            (const char* source);                                            //��������� ���������� � ����������� �� ������


}

}

}

}

#endif
