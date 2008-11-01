#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::macosx;

/*
    �������� ���������� PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct PrimarySwapChain::Impl
{
  Log            log;                  //��������
  AdapterPtr     adapter;              //������� ������� ������
  SwapChainDesc  desc;                 //���������� ������� ������
  AGLPixelFormat pixel_format;         //������ ��������
  OutputManager  output_manager;       //�������� ��������� ������
  AGLContext     context;              //������������� ��������
  Output*        containing_output;    //�������, �� ������� ������������ ���������

///�����������
  Impl (const SwapChainDesc& in_desc, Adapter* in_adapter)
    : adapter (in_adapter), context (0)
  {
      //�������� ������������ ����������

    if (!in_desc.window_handle)
      throw xtl::make_null_argument_exception ("", "in_desc.window_handle");

      //����� �������, �� ������� ������������ ���������

    containing_output = output_manager.FindContainingOutput ((WindowRef)in_desc.window_handle);

      //����� ������� ��������

    static const size_t MAX_ATTRIBUTES_COUNT = 64;

    GLint attributes [MAX_ATTRIBUTES_COUNT], *attr = attributes;

    *attr++ = AGL_COMPLIANT;
    *attr++ = AGL_WINDOW;
    *attr++ = AGL_RGBA;

    if (in_desc.buffers_count > 1)
      *attr++ = AGL_DOUBLEBUFFER;

    *attr++ = AGL_PIXEL_SIZE;
    *attr++ = in_desc.frame_buffer.color_bits;
    *attr++ = AGL_ALPHA_SIZE;
    *attr++ = in_desc.frame_buffer.alpha_bits;
    *attr++ = AGL_DEPTH_SIZE;
    *attr++ = in_desc.frame_buffer.depth_bits;
    *attr++ = AGL_STENCIL_SIZE;
    *attr++ = in_desc.frame_buffer.stencil_bits;

    if (in_desc.fullscreen && containing_output)
    {
      *attr++ = AGL_FULLSCREEN;
      *attr++ = AGL_DISPLAY_MASK;
      *attr++ = CGDisplayIDToOpenGLDisplayMask (containing_output->GetDisplayID ());
    }

    if (in_desc.samples_count)
    {
      *attr++ = AGL_SAMPLE_BUFFERS_ARB;
      *attr++ = 1;
      *attr++ = AGL_SAMPLES_ARB;
      *attr++ = in_desc.samples_count;
      *attr++ = AGL_SUPERSAMPLE;
    }

    *attr++ = AGL_NONE;

    log.Printf ("...call aglChoosePixelFormat");

    pixel_format = aglChoosePixelFormat (0, 0, attributes);

    if (!pixel_format)
      raise_agl_error ("::aglChoosePixelFormat");

      //��������� ���������� ���������� ������� ��������

    try
    {
      memset (&desc, 0, sizeof (desc));

      ::Rect window_rect;

      check_window_manager_error (GetWindowBounds ((WindowRef)in_desc.window_handle, kWindowStructureRgn, &window_rect), "::GetWindowBounds");

      desc.frame_buffer.width        = window_rect.right - window_rect.left;
      desc.frame_buffer.height       = window_rect.bottom - window_rect.top;
      desc.frame_buffer.color_bits   = static_cast<size_t> (GetPixelFormatValue (AGL_PIXEL_SIZE));
      desc.frame_buffer.alpha_bits   = static_cast<size_t> (GetPixelFormatValue (AGL_ALPHA_SIZE));
      desc.frame_buffer.depth_bits   = static_cast<size_t> (GetPixelFormatValue (AGL_DEPTH_SIZE));
      desc.frame_buffer.stencil_bits = static_cast<size_t> (GetPixelFormatValue (AGL_STENCIL_SIZE));
      desc.samples_count             = static_cast<size_t> (GetPixelFormatValue (AGL_SAMPLES_ARB));
      desc.buffers_count             = GetPixelFormatValue (AGL_DOUBLEBUFFER) ? 2 : 1;
      desc.swap_method               = SwapMethod_Flip;
      desc.vsync                     = in_desc.vsync;
      desc.fullscreen                = GetPixelFormatValue (AGL_FULLSCREEN) != 0;
      desc.window_handle             = in_desc.window_handle;

      stl::string flags;

      if (desc.buffers_count > 1)
      {
        flags += ", SwapMethod=";
        flags += get_name (desc.swap_method);
        flags += ", DOUBLE_BUFFER";
      }

      if (desc.fullscreen)
      {
        if (flags.empty ()) flags += ", ";
        else                flags += " | ";

        flags += "FULLSCREEN";
      }

/*      if (desc.fullscreen)
      {
          //��������� ������ ���������� ������

        containing_output->GetCurrentMode (default_output_mode);

        OutputModeDesc fullscreen_mode;

        memset (&fullscreen_mode, 0, sizeof (fullscreen_mode));

        printf ("Width = %u, height = %u, color_bits = %u\n", desc.frame_buffer.width, desc.frame_buffer.height, desc.frame_buffer.color_bits);

        fullscreen_mode.width      = desc.frame_buffer.width;
        fullscreen_mode.height     = desc.frame_buffer.height;
        fullscreen_mode.color_bits = desc.frame_buffer.color_bits;

        containing_output->SetCurrentMode (fullscreen_mode);
      }*/

      log.Printf ("...choose %s pixel format (RGB/A: %u/%u, D/S: %u/%u, Samples: %u%s)", GetPixelFormatValue (AGL_ACCELERATED) ? "HW" : "SW",
        desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits, desc.frame_buffer.depth_bits,
        desc.frame_buffer.stencil_bits, desc.samples_count, flags.c_str ());

    }
    catch (...)
    {
      aglDestroyPixelFormat (pixel_format);
      throw;
    }
  }

///����������
  ~Impl ()
  {
    log.Printf ("...destroy pixel format");

/*    if (desc.fullscreen)
    {
        //�������������� ���������� ������ ���������� ������

      containing_output->SetCurrentMode (default_output_mode);
    }*/

    aglDestroyPixelFormat (pixel_format);

    log.Printf ("...release resources");
  }

  ///����� �������� ������� ������ � ��������� ������
  void Present ()
  {
    if (desc.buffers_count  < 2)
      return;

    if (!context)
      return;

    aglSwapBuffers (context);

    check_agl_error ("::aglSwapBuffers");
  }

///��������� �������� ���������
  GLint GetPixelFormatValue (GLint attribute)
  {
    try
    {
      GLint result = 0;

      if (!aglDescribePixelFormat (pixel_format, attribute, &result))
        raise_agl_error ("::aglDescribePixelFormat");

      return result;
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::Impl::GetPixelFormatValue(%04x)", attribute);
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

PrimarySwapChain::PrimarySwapChain (const SwapChainDesc& desc, Adapter* adapter)
{
  Log log;

  log.Printf ("...create primary swap chain (id=%u)", GetId ());

  try
  {
    impl = new Impl (desc, adapter);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::PrimarySwapChain");
    throw;
  }

  log.Printf ("...primary swap chain successfully created");
}

PrimarySwapChain::~PrimarySwapChain ()
{
  try
  {
    Log log;

    log.Printf ("Destroy primary swap chain (id=%u)...", GetId ());

    impl = 0;

    log.Printf ("...primary swap chain successfully destroyed");
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    ��������� ��������
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    ��������� �����������
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->output_manager.FindContainingOutput ((WindowRef)impl->desc.window_handle);
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PrimarySwapChain::Present ()
{
  try
  {
    impl->Present ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::Present");

    throw;
  }
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::macosx::PrimarySwapChain::SetFullscreenState",
                                             "Changing fullscreen state not supported");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return impl->desc.fullscreen;
}

/*
   ���������/��������� ���������
*/

void PrimarySwapChain::SetContext (AGLContext context)
{
  if (impl->context == context)
    return;

  impl->context = context;

  if (!context)
    return;

  try
  {
      //��������� vsync

    GLint swap_interval = impl->desc.vsync ? 1 : 0;

    if (!aglSetInteger (context, AGL_SWAP_INTERVAL, &swap_interval))
      raise_agl_error ("::aglSetInteger");

      //��������� fullscreen

    if (impl->desc.fullscreen)
    {
      if (!aglSetFullScreen (context, impl->desc.frame_buffer.width, impl->desc.frame_buffer.height, 0, 0))
        raise_agl_error ("::aglSetFullScreen");
    }
    else
    {
      if (!aglSetWindowRef (context, (WindowRef)impl->desc.window_handle))
        raise_agl_error ("::aglSetWindowRef");
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::SetContext");
    throw;
  }
}

AGLContext PrimarySwapChain::GetContext ()
{
  return impl->context;
}

AGLPixelFormat PrimarySwapChain::GetPixelFormat ()
{
  return impl->pixel_format;
}
