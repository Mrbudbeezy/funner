#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    �������� ���������� ��������
*/

typedef WINGDIAPI HGLRC (WINAPI *CreateContextFn)       (HDC dc);
typedef WINGDIAPI BOOL  (WINAPI *DeleteContextFn)       (HGLRC context);
typedef WINGDIAPI int   (WINAPI *DescribePixelFormatFn) (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd);
typedef WINGDIAPI PROC  (WINAPI *GetProcAddressFn)      (LPCSTR name);
typedef WINGDIAPI BOOL  (WINAPI *MakeCurrentFn)         (HDC dc, HGLRC context);
typedef WINGDIAPI void  (WINAPI *SetCallbackProcsFn)    (void*); //???????????
typedef WINGDIAPI BOOL  (WINAPI *SetPixelFormatFn)      (HDC dc, int pixel_format, const PIXELFORMATDESCRIPTOR* pfd);
typedef WINGDIAPI BOOL  (WINAPI *SetPixelFormat1Fn)     (HDC hdc, int iPixelFormat);
typedef WINGDIAPI BOOL  (WINAPI *SwapBuffersFn)         (HDC dc);
typedef WINGDIAPI void* (WINAPI *SetContextFn)          (HDC dc, HGLRC context, void* callback);
typedef WINGDIAPI void  (WINAPI *ReleaseContextFn)      (HGLRC context);

/*
    ������� �������, ������������ ��� ������ OpenGL
*/

class WglEntries
{
  public:
///�����������
    WglEntries (syslib::DynamicLibrary& dll) : current_context (0), current_dc (0)
    {   
      if (dll.GetSymbol ("wglCreateContext"))
      {
        InitMcd (dll);
        return;
      }

      if (dll.GetSymbol ("DrvCreateContext"))
      {
        InitIcd (dll);
        return;
      }
    
      throw xtl::format_operation_exception ("render::low_level::opengl::windows::WglEntries::WglEntries",
        "Invalid library '%s'. Could not initialize MCD/ICD driver (neither wglCreateContext nor DrvCreateContext found)",
        dll.Name ());        
    }
    
///��������� �������� ��������� � �������� ���������� ������
    HGLRC GetCurrentContext () { return current_context; }
    HDC   GetCurrentDC      () { return current_dc; }
    
///���������� WGL
    HGLRC CreateContext  (HDC dc) { 
      
/*      if (fSetCallbackProcs)
      {
        fSetCallbackProcs (0);
      }*/
      
      HGLRC context = fCreateContext (dc);            
      
      return context; 
    }

    BOOL DeleteContext  (HGLRC context) { return fDeleteContext (context); }
    PROC GetProcAddress (LPCSTR name)   { return fGetProcAddress (name); }
    BOOL SwapBuffers    (HDC dc)        { return fSwapBuffers (dc); }

    BOOL SetPixelFormat (HDC dc, int pixel_format, const PIXELFORMATDESCRIPTOR* pfd)
    {
      if (fSetPixelFormat)
        return fSetPixelFormat (dc, pixel_format, pfd);

      return fSetPixelFormat1 (dc, pixel_format);
    }

    int DescribePixelFormat (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd)
    {
      return fDescribePixelFormat (dc, pixel_format, size, pfd);
    }

    BOOL MakeCurrent (HDC dc, HGLRC context)
    {
      if (fMakeCurrent)
      {
          //��������� ����� MCD
        
        if (!fMakeCurrent (dc, context))
          return FALSE;
        
        current_context = context;
        current_dc      = dc;

        return TRUE;
      }
      
        //��������� ����� ICD                

      if (current_context)
      {
        fReleaseContext (current_context);

        current_dc      = 0;
        current_context = 0;
      }
      
      if (dc || context)
      {
        if (!fSetContext (dc, context, 0))
          return FALSE;

        current_context = context;
        current_dc      = dc;
      }      

      return TRUE;
    }
    
  private:  
///������������� MCD
    void InitMcd (syslib::DynamicLibrary& dll)
    {
        //������������� ������� ����� �����

      get (dll, "wglCreateContext",       fCreateContext);
      get (dll, "wglDeleteContext",       fDeleteContext);
      get (dll, "wglDescribePixelFormat", fDescribePixelFormat);
      get (dll, "wglGetProcAddress",      fGetProcAddress);
      get (dll, "wglMakeCurrent",         fMakeCurrent);
      get (dll, "wglSetPixelFormat",      fSetPixelFormat);
      get (dll, "wglSwapBuffers",         fSwapBuffers);

      if (!xtl::xstricmp (dll.Name (), "opengl32.dll"))
      {
          //������� ������� ���������������

        fSetPixelFormat = &::SetPixelFormat;
      }
      
      fSetContext       = 0;
      fReleaseContext   = 0;
      fSetPixelFormat1  = 0;
      fSetCallbackProcs = 0;
    }
    
///������������� ICD
    void InitIcd (syslib::DynamicLibrary& dll)
    {
        //������������� ICD-��������

      get (dll, "DrvCreateContext",       fCreateContext);
      get (dll, "DrvDeleteContext",       fDeleteContext);
      get (dll, "DrvDescribePixelFormat", fDescribePixelFormat);
      get (dll, "DrvGetProcAddress",      fGetProcAddress);
      get (dll, "DrvReleaseContext",      fReleaseContext);
      get (dll, "DrvSetCallbackProcs",    fSetCallbackProcs);
      get (dll, "DrvSetContext",          fSetContext);
      get (dll, "DrvSetPixelFormat",      fSetPixelFormat1);
      get (dll, "DrvSwapBuffers",         fSwapBuffers);

      fMakeCurrent    = 0;
      fSetPixelFormat = 0;
    }

///���������� ���� � ��������� ������������ ��������
    template <class Fn> static void get (syslib::DynamicLibrary& dll, const char* symbol, Fn& fn)
    {
      fn = reinterpret_cast<Fn> (dll.GetSymbol (symbol));

      if (!fn)
        throw xtl::format_operation_exception ("render::low_level::opengl::windows::WglEntries::WglEntries",
          "Symbol '%s' not found in library '%s'", symbol, dll.Name ());
    }    

  private:
    CreateContextFn       fCreateContext;
    DeleteContextFn       fDeleteContext;
    DescribePixelFormatFn fDescribePixelFormat;
    GetProcAddressFn      fGetProcAddress;
    MakeCurrentFn         fMakeCurrent;
    SetCallbackProcsFn    fSetCallbackProcs;
    SetPixelFormatFn      fSetPixelFormat;
    SetPixelFormat1Fn     fSetPixelFormat1;
    SwapBuffersFn         fSwapBuffers;
    SetContextFn          fSetContext;
    ReleaseContextFn      fReleaseContext;
    HGLRC                 current_context;
    HDC                   current_dc;
};

/*
    �������� �������� OpenGL, ����������� ��� ���������� ������������� WGL
*/

class DummyContext
{
  public:
///�����������
    DummyContext (HWND parent_window, WglEntries& in_entries)
      : entries (in_entries),
        window (parent_window),
        dc (GetDC (window.GetHandle ())),
        context (0)
    {
      try
      {      
        if (!dc)
          raise_error ("GetDC");

        PIXELFORMATDESCRIPTOR pfd;

        static const size_t REQUIRE_FLAGS = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

        for (int pixel_format=1; entries.DescribePixelFormat (dc, pixel_format, sizeof pfd, &pfd); pixel_format++)
          if ((pfd.dwFlags & REQUIRE_FLAGS) == REQUIRE_FLAGS)
          {
            if (!entries.SetPixelFormat (dc, pixel_format, &pfd))
              raise_error ("wglSetPixelFormat");                           

            context = entries.CreateContext (dc);

            if (!context)
              raise_error ("wglCreateContext");                            

            if (!entries.MakeCurrent (dc, context))
              raise_error ("wglMakeCurrent");                            

            return;
          }

        throw xtl::format_operation_exception ("render::low_level::opengl::windows::DummyContext", "No pixel formats with OpenGL support");
      }
      catch (...)      
      {
        if (context) entries.DeleteContext (context);
        if (dc)      ReleaseDC (window.GetHandle (), dc);        

        throw;
      }              
    }

///����������
    ~DummyContext ()
    {      
      entries.MakeCurrent   (0, 0);
      entries.DeleteContext (context);
      ReleaseDC             (window.GetHandle (), dc);
    }

  private:
    WglEntries& entries; //����� ����� � wgl-�������
    DummyWindow window;  //�������� ����
    HDC         dc;      //�������� ���������� ���������
    HGLRC       context; //�������� OpenGL
};

/*
    �������� ������������ �������� ��������
*/

struct PixelFormatEnumContext
{  
  Adapter::PixelFormatArray& pixel_formats;         //������ �������� ��������
  HDC                        device_context;        //�������� ���������� ���������
  WglEntries&                wgl_entries;           //����� ����� wgl-�������
  WglExtensionEntries*       wgl_extension_entries; //������� WGL-����������  
  Adapter*                   adapter;               //�������
  int                        current_pixel_format;  //������� ����� ������� ��������
  bool                       has_pbuffer;           //������� WGL_ARB_pbuffer
  bool                       has_multisample;       //������� WGL_ARB_multisample
  
///�����������
  PixelFormatEnumContext
   (HWND                               window,
    HDC                                in_device_context,
    Adapter*                           in_adapter,
    WglEntries&                        in_entries,
    Adapter::PixelFormatArray&         in_pixel_formats,
    Adapter::WglExtensionEntriesArray& wgl_extension_entries_array)
      : pixel_formats (in_pixel_formats),
        device_context (in_device_context),
        wgl_entries (in_entries),
        adapter (in_adapter),
        current_pixel_format (0),
        wgl_extension_entries (0),
        has_pbuffer (false),
        has_multisample (false)
  {
      //������� �������� ����� ����� ��� ������������ �������� � �������������� ����������

    try
    {
        //�������� ��������� ���������        

      DummyContext context (window, wgl_entries);                  

        //������������� ����������

      wgl_extension_entries_array.push_back ();

      try
      {
        wgl_extension_entries = &wgl_extension_entries_array.back ();        

        wgl_extension_entries->Init (adapter->GetLibrary ());
        
          //�������� ������� ���������� WGL_ARB_pbuffer � WGL_ARB_multisample
          
        stl::string extensions = get_wgl_extensions_string (*wgl_extension_entries, device_context);
          
        has_pbuffer     = has_extension (extensions.c_str (), "WGL_ARB_pbuffer");
        has_multisample = has_extension (extensions.c_str (), "WGL_ARB_multisample");
      }
      catch (...)
      {
        wgl_extension_entries_array.pop_back ();
        throw;
      }
    }
    catch (...)
    {
      //!!!������� ����������������!!
      //���������� ���� ����������
    }
  }

///��������� �������� ������� ��������
  bool GetPixelFormatAttribute (int attribute_name, int& attribute_value)
  {
    return wgl_extension_entries->GetPixelFormatAttribivARB (device_context, current_pixel_format, 0, 1, &attribute_name, &attribute_value) != 0;
  }
  
///������������ ��������
  void EnumFormats ()
  {
    PixelFormatDesc out_desc;
    
      //������� ������������ � �������������� ��������� WGL_ARB_pixel_format
      
    if (wgl_extension_entries && wgl_extension_entries->GetPixelFormatAttribivARB)
    {
        //����������� ������������� ������� ��������      

      int name = WGL_NUMBER_PIXEL_FORMATS_ARB, max_pixel_format = 0;

      if (wgl_extension_entries->GetPixelFormatAttribivARB (device_context, 1, 0, 1, &name, &max_pixel_format))
      {
          //������������ ��������

        for (current_pixel_format=1; current_pixel_format<=max_pixel_format; ++current_pixel_format)
        {
          static int basic_names [] = {
            WGL_SUPPORT_OPENGL_ARB,
            WGL_DRAW_TO_WINDOW_ARB,
            WGL_PIXEL_TYPE_ARB,
            WGL_COLOR_BITS_ARB,
            WGL_ALPHA_BITS_ARB,
            WGL_DEPTH_BITS_ARB,
            WGL_STENCIL_BITS_ARB,
            WGL_DOUBLE_BUFFER_ARB,
            WGL_AUX_BUFFERS_ARB,
            WGL_STEREO_ARB,
            WGL_SWAP_METHOD_ARB,
            WGL_ACCELERATION_ARB,
          };

          static const int BASIC_NAMES_COUNT = sizeof (basic_names) / sizeof (GLenum);

          int value_buffer [BASIC_NAMES_COUNT], *iter = value_buffer;

          if (!wgl_extension_entries->GetPixelFormatAttribivARB (device_context, current_pixel_format, 0, BASIC_NAMES_COUNT, basic_names, value_buffer))
            continue;

          if (!*iter++) //������� ��� ��������� OpenGL ������������
            continue;

          if (!*iter++) //������� ��� ��������� ��������� � ���� ������������
            continue;

          if (*iter++ == PFD_TYPE_COLORINDEX) //���������� ������� ������������
            continue;

          out_desc.adapter                 = adapter;
          out_desc.wgl_extension_entries   = wgl_extension_entries;
          out_desc.pixel_format_index      = current_pixel_format;
          out_desc.color_bits              = *iter++;
          out_desc.alpha_bits              = *iter++;
          out_desc.depth_bits              = *iter++;
          out_desc.stencil_bits            = *iter++;
          out_desc.samples_count           = 0;      
          out_desc.buffers_count           = *iter++ ? 2 : 1;
          out_desc.aux_buffers             = *iter++;
          out_desc.support_draw_to_pbuffer = false;
          out_desc.support_stereo          = *iter++ != 0;
          
          switch (*iter++)
          {
            case WGL_SWAP_EXCHANGE_ARB:
              out_desc.swap_method = SwapMethod_Flip;
              break;
            case WGL_SWAP_COPY_ARB:
              out_desc.swap_method = SwapMethod_Copy;
              break;
            default:            
            case WGL_SWAP_UNDEFINED_ARB:
              out_desc.swap_method = SwapMethod_Discard;
              break;
          }

          switch (*iter++)
          {
            default:
            case WGL_NO_ACCELERATION_ARB:
              out_desc.acceleration = Acceleration_No;
              break;
            case WGL_GENERIC_ACCELERATION_ARB:
              out_desc.acceleration = Acceleration_MCD;
              break;
            case WGL_FULL_ACCELERATION_ARB:
              out_desc.acceleration = Acceleration_ICD;
              break;
          }

          int value;

          if (has_pbuffer && GetPixelFormatAttribute (WGL_DRAW_TO_PBUFFER_ARB, value))
          {
            out_desc.support_draw_to_pbuffer = value != 0;
          }
          
          if (has_multisample && GetPixelFormatAttribute (WGL_SAMPLES_ARB, value))
          {
            out_desc.samples_count = size_t (value);
          }
          
            //���������� ����������� � ������

          pixel_formats.push_back (out_desc);          
        }

        return;        
      }
    }
    
      //������������ ��� ������������� ����������

    PIXELFORMATDESCRIPTOR pfd;
    
      //����������� ������������� ������� ��������

    int max_pixel_format = wgl_entries.DescribePixelFormat (device_context, 1, sizeof pfd, &pfd);
    
    for (current_pixel_format=1; current_pixel_format<=max_pixel_format; ++current_pixel_format)
    {
      if (!wgl_entries.DescribePixelFormat (device_context, current_pixel_format, sizeof (pfd), &pfd))
        continue;

      if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL)) //������� ��� ��������� OpenGL ������������
        continue;

      if (pfd.iPixelType == PFD_TYPE_COLORINDEX) //���������� ������� ������������
        continue;
        
      if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW)) //������� ��� ��������� � ���� ������������
        continue;

      out_desc.adapter                 = adapter;
      out_desc.wgl_extension_entries   = wgl_extension_entries;
      out_desc.pixel_format_index      = current_pixel_format;
      out_desc.color_bits              = pfd.cColorBits;
      out_desc.alpha_bits              = pfd.cAlphaBits;
      out_desc.depth_bits              = pfd.cDepthBits;
      out_desc.stencil_bits            = pfd.cStencilBits;
      out_desc.samples_count           = 0;
      out_desc.buffers_count           = pfd.dwFlags & PFD_DOUBLEBUFFER ? 2 : 1;
      out_desc.aux_buffers             = pfd.cAuxBuffers;
      out_desc.support_draw_to_pbuffer = false;
      out_desc.support_stereo          = (pfd.dwFlags & PFD_STEREO) != 0;

      if      (pfd.dwFlags & PFD_SWAP_COPY)     out_desc.swap_method = SwapMethod_Copy;
      else if (pfd.dwFlags & PFD_SWAP_EXCHANGE) out_desc.swap_method = SwapMethod_Flip;
      else                                      out_desc.swap_method = SwapMethod_Discard;

      if (pfd.dwFlags & PFD_GENERIC_FORMAT)
      {
        out_desc.acceleration = Acceleration_No;
      }
      else
      {
        out_desc.acceleration = (pfd.dwFlags & PFD_GENERIC_ACCELERATED) ? Acceleration_MCD : Acceleration_ICD;
      }      

        //���������� ����������� � ������

      pixel_formats.push_back (out_desc);
    }
  }
};

}

struct Adapter::Impl: public ILibrary
{
  OutputManager          output_manager;   //�������� ��������� ������
  syslib::DynamicLibrary dll;              //������������ ���������� ������ �������� ���������
  WglEntries             entries;          //����� ����� wgl-�������
  stl::string            name;             //��� ��������
  stl::string            path;             //���� � ��������
  IContextListener*      context_listener; //��������� ������� ���������

///�����������
  Impl (const char* in_name, const char* in_dll_path)
    : dll (in_dll_path),
      entries (dll),
      name (in_name),
      path (in_dll_path),
      context_listener (0)
  {}

///��������� ������ ����� ����� OpenGL
  const void* GetProcAddress (const char* name, size_t search_flags)
  {
    try
    {
      static const char* METHOD_NAME = "render::low_level::opengl::windows::Adapter::Impl::GetProcAddress";

      if (!name)
      {
        if (search_flags & EntrySearch_NoThrow)
          return 0;

        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      }

      const void* address = 0;

      if (!address && (search_flags & EntrySearch_Context))
        address = entries.GetProcAddress (name);    
        
      if (address)
        return address;

      if (search_flags & EntrySearch_Library)
        address = dll.GetSymbol (name);

      if (address)
        return address;

      if (search_flags & EntrySearch_NoThrow)
        return 0;

      throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in library '%s'",
        name, dll.Name ());
    }
    catch (...)
    {
      if (search_flags & EntrySearch_NoThrow)
        return 0;

      throw;
    }
  }
};

/*
    ����������� / ����������
*/

Adapter::Adapter (const char* name, const char* dll_path)
{
  try
  {
      //�������� ������������ ����������    
    
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    if (!dll_path)
      throw xtl::make_null_argument_exception ("", "dll_path");
      
      //�������� ����������

    impl = new Impl (name, dll_path);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
}

/*
    ��� �������� / ���� � ������ / ��������
*/

const char* Adapter::GetName ()
{
  return impl->name.c_str ();
}

const char* Adapter::GetPath ()
{
  return impl->path.c_str ();
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::windows::Adapter::Adapter";
}

/*
    ������������ ��������� ��������� ������
*/

size_t Adapter::GetOutputsCount ()
{
  return impl->output_manager.GetOutputsCount ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  return impl->output_manager.GetOutput (index);
}

Output* Adapter::FindContainingOutput (HWND window)
{
  return impl->output_manager.FindContainingOutput (window);
}

/*
    ��������� ��������� ������� ���������
*/

void Adapter::SetContextListener (IContextListener* listener)
{
  impl->context_listener = listener;
}

IContextListener* Adapter::GetContextListener ()
{
  return impl->context_listener;
}

/*
    ��������� ���������� ���������� OpenGL
*/

ILibrary& Adapter::GetLibrary ()
{
  return *impl;
}

/*
    ������������ ��������� �������� ��������
*/

void Adapter::EnumPixelFormats
 (HWND                      window,
  HDC                       device_context,
  PixelFormatArray&         pixel_formats,
  WglExtensionEntriesArray& wgl_extension_entries_array)
{
  try
  {
      //���������� � ��������� ������ ���������

    if (impl->context_listener)
      impl->context_listener->OnLostCurrent ();

      //������������ ��������� ��������

    PixelFormatEnumContext (window, device_context, this, impl->entries, pixel_formats, wgl_extension_entries_array).EnumFormats ();
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ���������� ������� WGL
*/

HGLRC Adapter::CreateContext (HDC dc)
{
  HGLRC context = impl->entries.CreateContext (dc);
  
  if (!context)
    raise_error ("wglCreateContext");
    
  return context;
}

void Adapter::DeleteContext (HGLRC context)
{
  if (!impl->entries.DeleteContext (context))
    raise_error ("wglDeleteContext");
}

HGLRC Adapter::GetCurrentContext ()
{
  return impl->entries.GetCurrentContext ();
}

HDC Adapter::GetCurrentDC ()
{
  return impl->entries.GetCurrentDC ();
}

PROC Adapter::GetProcAddress (LPCSTR name)
{
  return impl->entries.GetProcAddress (name);
}

void Adapter::MakeCurrent (HDC device_context, HGLRC context)
{
  if (impl->context_listener)
    impl->context_listener->OnLostCurrent ();

  if (!impl->entries.MakeCurrent (device_context, context))
    raise_error ("wglMakeCurrent");

  if (device_context && context && impl->context_listener)
    impl->context_listener->OnSetCurrent ();
}

void Adapter::SetPixelFormat (HDC dc, int pixel_format)
{
  PIXELFORMATDESCRIPTOR pfd;

  if (!impl->entries.DescribePixelFormat (dc, pixel_format, sizeof pfd, &pfd))
    raise_error ("wglDescribePixelFormat");

  if (!impl->entries.SetPixelFormat (dc, pixel_format, &pfd))
    raise_error ("wglSetPixelFormat");
}

void Adapter::SwapBuffers (HDC dc)
{
    //����� �������

  if (!impl->entries.SwapBuffers (dc))
    raise_error ("wglSwapBuffers");
}
