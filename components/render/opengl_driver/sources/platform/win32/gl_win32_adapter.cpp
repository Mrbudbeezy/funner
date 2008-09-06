#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    �������� �������� OpenGL, ����������� ��� ���������� ������������� WGL
*/

class DummyContext
{
  public:
///�����������
    DummyContext (HWND parent_window, const AdapterLibraryPtr& in_library)
      : library (in_library),
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

        for (int pixel_format=1; library->DescribePixelFormat (dc, pixel_format, sizeof pfd, &pfd); pixel_format++)
          if ((pfd.dwFlags & REQUIRE_FLAGS) == REQUIRE_FLAGS)
          {
            library->SetPixelFormat (dc, pixel_format);

            context = library->CreateContext (dc);

            library->MakeCurrent (dc, context);

            return;
          }

        throw xtl::format_operation_exception ("render::low_level::opengl::windows::DummyContext", "No pixel formats with OpenGL support");
      }
      catch (...)      
      {
        if (context) library->DeleteContext (context);
        if (dc)
        {
          library->ReleasePixelFormat (dc);

          ReleaseDC (window.GetHandle (), dc);
        }

        throw;
      }              
    }

///����������
    ~DummyContext ()
    {
      library->MakeCurrent        (0, 0);
      library->DeleteContext      (context);
      library->ReleasePixelFormat (dc);
      ReleaseDC                   (window.GetHandle (), dc);
    }

  private:
    AdapterLibraryPtr library; //���������� ��������
    DummyWindow       window;  //�������� ����
    HDC               dc;      //�������� ���������� ���������
    HGLRC             context; //�������� OpenGL
};

}

/*
    �������� ���������� ��������
*/

struct Adapter::Impl
{
  Log               log;            //��������
  OutputManager     output_manager; //�������� ��������� ������
  AdapterLibraryPtr library;        //���������� ��������
  stl::string       name;           //��� ��������
  PropertyList      properties;     //�������� ��������

///������ ������ �������������
  Impl (const char* in_name, const char* in_dll_path, const char* init_string)
    : library (LibraryManager::LoadLibrary (in_dll_path)),
      name (in_name)
  {
      //������ ������ �������������

    stl::string bugs_string; //������ ����� ��������      

    common::parse_init_string (init_string, xtl::bind (&Impl::SetProperty, this, _1, _2, xtl::ref (bugs_string)));

      //����������� ��������� ����� ��������

    properties.AddProperty ("bugs", bugs_string.c_str ());
  }

///��������� ��������
  void SetProperty (const char* name, const char* value, stl::string& bugs_string)
  {
    if (!xtl::xstrcmp (name, "bugs"))
    {
        //���������� ������ ����� ��������
      
      stl::vector<stl::string> tokens = common::split (value);

      for (size_t i=0; i<tokens.size (); i++)
      {
        const char* bug_name = tokens [i].c_str ();
        
        if (!common::wcmatch ("GLBUG_*", bug_name))
        {
          if (!bugs_string.empty ())
            bugs_string += ' ';

          bugs_string += bug_name;
        }
      }
    }
  }
};

/*
    ����������� / ����������
*/

Adapter::Adapter (const char* name, const char* dll_path, const char* init_string)
{
  try
  {
      //�������� ������������ ����������    
    
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    if (!dll_path)
      throw xtl::make_null_argument_exception ("", "dll_path");
      
    if (!init_string)
      throw xtl::make_null_argument_exception ("", "init_string");

      //�������� ����������

    impl = new Impl (name, dll_path, init_string);

    impl->log.Printf ("...adapter successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
  Log log = impl->log;

  log.Printf ("Destroy adapter '%s' (path='%s')", impl->name.c_str (), impl->library->GetName ());

  impl = 0;

  log.Printf ("...adapter successfully destroyed");
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
  return impl->library->GetName ();
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
    ��������� ���������� ���������� OpenGL
*/

IAdapterLibrary& Adapter::GetLibrary ()
{
  return *impl->library;
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
    //��������� ������� ����������

  WglExtensionEntries* wgl_extension_entries = 0;      //������� WGL-����������        
  bool                 has_pbuffer           = false,  //������� WGL_ARB_pbuffer
                       has_multisample       = false;  //������� WGL_ARB_multisample        

    //������� �������� ����� ����� ��� ������������ �������� � �������������� ����������
    
  try
  {
      //�������� ��������� ���������        
      
    impl->log.Printf (".....create dummy context");

    DummyContext context (window, impl->library);

      //������������� ����������
      
    impl->log.Printf (".....initialize WGL extensions");

    wgl_extension_entries_array.push_back ();

    try
    {
      wgl_extension_entries = &wgl_extension_entries_array.back ();        

      wgl_extension_entries->Init (*impl->library);

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
  catch (std::exception& exception)
  {
    impl->log.Printf ("%s\n    at create dummy context on adapter '%s'", exception.what (), GetName ());
  }
  catch (...)
  {
    impl->log.Printf ("Unknown exception\n    at create dummy context on adapter '%s'", GetName ());
  }

  PixelFormatDesc out_desc;

    //������� ������������ � �������������� ��������� WGL_ARB_pixel_format

  if (wgl_extension_entries && wgl_extension_entries->GetPixelFormatAttribivARB)
  {
      //����������� ������������� ������� ��������      

    int name = WGL_NUMBER_PIXEL_FORMATS_ARB, max_pixel_format = 0;

    impl->log.Printf (".....enumerate using WGL_ARB_pixel_format");

    if (wgl_extension_entries->GetPixelFormatAttribivARB (device_context, 1, 0, 1, &name, &max_pixel_format))
    {
        //������������ ��������

      impl->log.Printf (".....%u formats found", max_pixel_format);

      for (int current_pixel_format=1; current_pixel_format<=max_pixel_format; ++current_pixel_format)
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

        out_desc.adapter                 = this;
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

        int name, value;

        if (has_pbuffer)
        {
          name = WGL_DRAW_TO_PBUFFER_ARB;
          
           if (wgl_extension_entries->GetPixelFormatAttribivARB (device_context, current_pixel_format, 0, 1, &name, &value))
             out_desc.support_draw_to_pbuffer = value != 0;
        }

        if (has_multisample)
        {
          name = WGL_SAMPLES_ARB;
          
          if (wgl_extension_entries->GetPixelFormatAttribivARB (device_context, current_pixel_format, 0, 1, &name, &value))
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
    
  impl->log.Printf (".....enumerate using DescribePixelFormat");

  int max_pixel_format = impl->library->DescribePixelFormat (device_context, 1, sizeof pfd, &pfd);  

  impl->log.Printf (".....%u formats found", max_pixel_format);

  for (int current_pixel_format=1; current_pixel_format<=max_pixel_format; ++current_pixel_format)
  {
    if (!impl->library->DescribePixelFormat (device_context, current_pixel_format, sizeof (pfd), &pfd))
      continue;

    if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL)) //������� ��� ��������� OpenGL ������������
      continue;

    if (pfd.iPixelType == PFD_TYPE_COLORINDEX) //���������� ������� ������������
      continue;
      
    if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW)) //������� ��� ��������� � ���� ������������
      continue;

    out_desc.adapter                 = this;
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

/*
    ������ ������� ��������
*/

IPropertyList* Adapter::GetProperties ()
{
  return &impl->properties;
}
