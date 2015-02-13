#ifndef RENDER_GL_DRIVER_WIN32_SHARED_HEADER
#define RENDER_GL_DRIVER_WIN32_SHARED_HEADER

#define _WIN32_WINNT 0x0501 //for ChangeDisplaySettingsEx

#include <stl/hash_map>
#include <stl/list>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/implicit_cast.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable_ptr.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <render/low_level/utils.h>

#include <shared/driver_output_manager.h>
#include <shared/gl.h>
#include <shared/log.h>
#include <shared/object.h>
#include <shared/property_list.h>
#include <shared/platform.h>

#include "wgl_entries.h"

#include <windows.h>

namespace render
{

namespace low_level
{

namespace opengl
{

namespace windows
{

//forward declarations
class Adapter;
class IAdapterLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Acceleration
{
  Acceleration_No,  //����������� ���������
  Acceleration_MCD, //��������� ��������� / ����������� ��������
  Acceleration_ICD, //������ ��������� ����� ���������� �������
  
  Acceleration_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PixelFormatDesc
{
  Adapter*                   adapter;                 //�������, �������������� ��������� ������
  const WglExtensionEntries* wgl_extension_entries;   //������� WGL-���������� (������ ���� ����������� � ������, ���������� PixelFormatDesc, ����� ���� 0)
  int                        pixel_format_index;      //������ ������� �������� � ������� �������� ��������
  Acceleration               acceleration;            //��� ��������� ������� ��������
  unsigned int               color_bits;              //���������� ��� �� ����
  unsigned int               alpha_bits;              //���������� ��� �� �����
  unsigned int               depth_bits;              //���������� ��� �� �������
  unsigned int               stencil_bits;            //���������� ��� �� ��������
  unsigned int               samples_count;           //���������� sample'�� (0=multisample off)
  unsigned int               buffers_count;           //���������� ������� � ������� ������ (0=default 2 buffers)
  SwapMethod                 swap_method;             //����� ������ ������� � ��������� �������
  unsigned int               aux_buffers;             //���������� ��������������� ������� ���������
  bool                       support_draw_to_pbuffer; //��������� ��������� � PBuffer
  bool                       support_stereo;          //��������� ������-����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IWindowListener
{
  public:
    virtual void OnDestroy           () = 0; //���������� ��� �������� ����
    virtual void OnDisplayModeChange () = 0; //���������� ��� ����� �����-������

  protected:
    virtual ~IWindowListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISwapChainImpl: virtual public ISwapChain
{
  public:
    virtual Adapter*                   GetAdapterImpl         () = 0; //��������� ���������� ��������
    virtual HDC                        GetDC                  () = 0; //�������� ���������� ������
    virtual int                        GetPixelFormat         () = 0; //������ �������� ���������� ������
    virtual bool                       HasVSync               () = 0; //���� �� ������������ �������������
    virtual const WglExtensionEntries& GetWglExtensionEntries () = 0; //��������� ������� WGL-����������

  protected:
    virtual ~ISwapChainImpl () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class DummyWindow
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DummyWindow  (HWND parent = 0, IWindowListener* listener = 0);
    ~DummyWindow ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    HWND GetHandle () const;

  private:
    DummyWindow (const DummyWindow&); //no impl
    DummyWindow& operator = (const DummyWindow&); //no impl        

  private:
    HWND window; //���������� ����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PixelFormatManager
{
  public:
    typedef int (WINAPI *DescribePixelFormatFn)(HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RedirectApiCalls (HMODULE module);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL        SetPixelFormat  (HDC dc, int pixel_format, DescribePixelFormatFn describe_pixel_format);
    static BOOL        CopyPixelFormat (HDC src_dc, HDC dst_dc);
    static BOOL WINAPI SetPixelFormat  (HDC dc, int pixel_format, PIXELFORMATDESCRIPTOR* pfd);
    static int  WINAPI GetPixelFormat  (HDC dc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static int WINAPI DescribePixelFormat (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� "�� ���������"
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetDefaultLibrary (IAdapterLibrary* library);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void ReleasePixelFormat (HDC dc);
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
    PrimarySwapChain  (const SwapChainDesc& sc_desc, const PixelFormatDesc& pf_desc);
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
///������ ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� IDeviceContext
///////////////////////////////////////////////////////////////////////////////////////////////////
    Adapter*                   GetAdapterImpl         (); //��������� ���������� ��������
    HDC                        GetDC                  (); //�������� ���������� ������
    int                        GetPixelFormat         (); //������ �������� ���������� ������
    bool                       HasVSync               (); //���� �� ������������ �������������
    const WglExtensionEntries& GetWglExtensionEntries (); //��������� ������� WGL-����������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ����������� �����-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void ()> EventHandler;

    xtl::connection RegisterDisplayModeChangeHandler (const EventHandler&);
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///PBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////
class PBuffer: virtual public ISwapChainImpl, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PBuffer  (PrimarySwapChain* primary_swap_chain, unsigned int width, unsigned int height);
    PBuffer  (PrimarySwapChain* swap_chain);
    ~PBuffer ();
    
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
///��������� ���������� IDeviceContext
///////////////////////////////////////////////////////////////////////////////////////////////////
    Adapter*                   GetAdapterImpl         (); //��������� ���������� ��������
    HDC                        GetDC                  (); //�������� ���������� ������
    int                        GetPixelFormat         (); //������ �������� ���������� ������
    bool                       HasVSync               (); //���� �� ������������ �������������
    const WglExtensionEntries& GetWglExtensionEntries (); //��������� ������� WGL-����������

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
///��������� ������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IContextLostListener
{
  public:
    virtual void OnLostCurrent () = 0;

  protected:
    virtual ~IContextLostListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class IAdapterLibrary: virtual public ILibrary
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual int  DescribePixelFormat (HDC dc, int pixel_format, unsigned int size, PIXELFORMATDESCRIPTOR* out_pfd) = 0;
    virtual void SetPixelFormat      (HDC dc, int pixel_format) = 0;    
    virtual void ReleasePixelFormat  (HDC dc) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� � �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual HGLRC CreateContext (HDC dc) = 0;
    virtual void  DeleteContext (HGLRC context) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void  MakeCurrent       (HDC dc, HGLRC context, IContextLostListener* = 0) = 0; //��������� �������� ���������
    virtual HGLRC GetCurrentContext () = 0; //��������� �������� ���������
    virtual HDC   GetCurrentDC      () = 0; //��������� �������� ���������� ������

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SwapBuffers (HDC dc) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;
};

typedef xtl::com_ptr<IAdapterLibrary> AdapterLibraryPtr;  

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class LibraryManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static AdapterLibraryPtr LoadLibrary (const char* name);
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
    Adapter  (const char* name, const char* dll_path, const char* init_string);
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
    size_t   GetOutputsCount      ();
    IOutput* GetOutput            (size_t index);
    Output*  FindContainingOutput (HWND window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef stl::vector<PixelFormatDesc>     PixelFormatArray;
    typedef stl::vector<WglExtensionEntries> WglExtensionEntriesArray;

    void EnumPixelFormats (HWND window, HDC device_context, PixelFormatArray& pixel_formats, WglExtensionEntriesArray& entries);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������� �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapterLibrary& GetLibrary ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

  private:
    Adapter (const Adapter&); //no impl
    Adapter& operator = (const Adapter&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ��������� ��������� ������ GetLastError ()
///////////////////////////////////////////////////////////////////////////////////////////////////
void raise_error (const char* source);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
stl::string get_wgl_extensions_string (const WglExtensionEntries& wgl_extension_entries, HDC device_context);
bool        has_extension             (const char* extensions_string, const char* extension);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����� ����� � ������� ICD-��������
///////////////////////////////////////////////////////////////////////////////////////////////////
int get_icd_table_index (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������� Dll
///////////////////////////////////////////////////////////////////////////////////////////////////
void redirect_dll_call (HMODULE module, const char* import_module_name, void* src_thunk, void* dst_thunk);

}

}

}

}

#endif
