#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    ���������
*/

const size_t PIXEL_FORMAT_ARRAY_RESERVE_SIZE = 256; //������������� ���������� �������� ��������

/*
    ���������� ��������� ���������
*/

class PlatformManagerImpl
{
  public:
    typedef render::low_level::opengl::IContext IContext;
  
///�����������
    PlatformManagerImpl ()
    {
        //��������� ���� � ���������� Windows
        
      stl::string win_dir;      

      win_dir.fast_resize (GetWindowsDirectory (&win_dir [0], 0));      

      if (GetWindowsDirectory (&win_dir [0], win_dir.size ()))
      {      
        win_dir.pop_back ();

          //�������� MSOGL

        LoadDefaultAdapter ("MSOGL", "ogldrv", "bugs='GLBUG_swap_buffers_twice_call GLBUG_texture_no_subimage GLBUG_texture_no_mipmap'");

          //�������� Direct3D ��������� AcXtrnal

        LoadDefaultAdapter ("Direct3D wrapper", (win_dir + "\\AppPatch\\AcXtrnal").c_str (), "bugs='GLBUG_texture_no_subimage'");
      }      
      
        //�������� �������� "�� ���������"

      LoadDefaultAdapter ("OpenGL32", "opengl32");
    }

///�������� ��������
    Adapter* CreateAdapter (const char* name, const char* path, const char* init_string)
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");

        if (!path)
          throw xtl::make_null_argument_exception ("", "path");
          
        if (!init_string)
          init_string = "";

        log.Printf ("Create adapter '%s' (path='%s', init_string='%s')...", name, path, init_string);

        return new Adapter (name, path, init_string);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::PlatformManagerImpl::CreateAdapter");
        throw;
      }
    }

///������������ ��������� "�� ���������"
    void EnumDefaultAdapters (const xtl::function<void (IAdapter*)>& handler)
    {
      for (AdapterList::iterator iter=default_adapters.begin (), end=default_adapters.end (); iter!=end; ++iter)
        handler (get_pointer (*iter));
    }

///�������� ������� ������
    ISwapChain* CreateSwapChain (size_t adapters_count, IAdapter** adapters, const SwapChainDesc& desc)
    {
      try
      {
          //�������� ������������ ����������

        if (adapters_count && !adapters)
          throw xtl::make_null_argument_exception ("", "adapters");
          
        log.Printf ("Create swap chain...");

        struct DCHolder
        {
          HWND window;
          HDC  dc;
          
          DCHolder (const void* window_handle)
          {
            if (!window_handle)
              throw xtl::make_null_argument_exception ("", "swap_chain_desc.window_handle");                          
              
            window = (HWND)window_handle;
            dc     = GetDC (window);

            if (!dc)
              raise_error ("GetDC");
          }
          
          ~DCHolder ()
          {
            ReleaseDC (window, dc);
          }                    
        };

        DCHolder dc_holder (desc.window_handle);
        
          //������������ �������� �������� ��������

        Adapter::PixelFormatArray         pixel_formats;
        Adapter::WglExtensionEntriesArray wgl_extension_entries;

        pixel_formats.reserve (PIXEL_FORMAT_ARRAY_RESERVE_SIZE);
        wgl_extension_entries.reserve (adapters_count);

        for (size_t i=0; i<adapters_count; i++)
        {
          IAdapter* in_adapter = adapters [i];

          if (!in_adapter)
            throw xtl::format_exception<xtl::null_argument_exception> ("", "Null argument 'adapters[%u]'", i);
            
          Adapter* volatile adapter = dynamic_cast<Adapter*> (in_adapter);
          
          if (!adapter)
            throw xtl::format_exception<xtl::argument_exception> ("", "Invalid argument 'adapters[%u]'. Wrong type '%s'",
              i, typeid (*in_adapter).name ());

          try
          {
            log.Printf ("...enumerate pixel formats on adapter '%s'", adapter->GetName ());

            PixelFormatManager::SetDefaultLibrary (&adapter->GetLibrary ());

            adapter->EnumPixelFormats (dc_holder.window, dc_holder.dc, pixel_formats, wgl_extension_entries);
          }
          catch (std::exception& exception)
          {
            log.Printf ("%s\n    at enumerate pixel formats on adapter '%s'", exception.what (), adapter->GetName ());
          }
          catch (...)
          {
            log.Printf ("Unknown exception\n    at enumerate pixel formats on adapter '%s'", adapter->GetName ());
          }

          PixelFormatManager::SetDefaultLibrary (0);
        }

          //����� �������� ����������� �������          

        const PixelFormatDesc& pixel_format = ChoosePixelFormat (pixel_formats, desc);

          //���������������� ���������� �������

        const char* acceleration_string = "";
        
        switch (pixel_format.acceleration)
        {
          default:
          case Acceleration_No:
            acceleration_string = "SW";
            break;
          case Acceleration_MCD:
            acceleration_string = "MCD";
            break;
          case Acceleration_ICD:
            acceleration_string = "ICD";
            break;
        }

        stl::string flags;
        
        if (pixel_format.buffers_count > 1)
        {
          flags += ", SwapMethod=";
          flags += get_name (pixel_format.swap_method);
          flags += ", DOUBLE_BUFFER";
        }

        if (pixel_format.support_draw_to_pbuffer)
        {
          if (flags.empty ()) flags += ", ";
          else                flags += " | ";          
          
          flags += "PBUFFER";
        }

        if (pixel_format.support_stereo)
        {
          if (flags.empty ()) flags += ", ";
          else                flags += " | ";          

          flags += "STEREO";
        }

        log.Printf ("...choose %s pixel format #%u on adapter '%s' (RGB/A: %u/%u, D/S: %u/%u, Samples: %u%s)",
          acceleration_string, pixel_format.pixel_format_index, pixel_format.adapter->GetName (), pixel_format.color_bits,
          pixel_format.alpha_bits, pixel_format.depth_bits, pixel_format.stencil_bits, pixel_format.samples_count, flags.c_str ());

          //�������� ������� ������

        return new PrimarySwapChain (desc, pixel_format);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::PlatformManagerImpl::CreateSwapChain");
        throw;
      }
    }

///�������� PBuffer'�
    ISwapChain* CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
    {
      try
      {
        PrimarySwapChain* primary_swap_chain = dynamic_cast<PrimarySwapChain*> (source_chain);

        if (!primary_swap_chain)
          throw xtl::format_operation_exception ("", "Incompatible source chain, render::low_level::opengl::windows::PrimarySwapChain needed.");

        if (pbuffer_desc)
          return new PBuffer (primary_swap_chain, pbuffer_desc->frame_buffer.width, pbuffer_desc->frame_buffer.height);
        else
          return new PBuffer (primary_swap_chain);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::PlatformManagerImpl::CreatePBuffer");
        throw;
      }
    }

///�������� ���������
    IContext* CreateContext (ISwapChain* swap_chain)
    {
      try
      {
        return new Context (swap_chain);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::PlatformManagerImpl::CreateContext");
        throw;
      }
    }
    
  private:
///������� �������� �������� "�� ���������"
    void LoadDefaultAdapter (const char* name, const char* path, const char* init_string = "")
    {
      try
      {
          //�������� ��������

        AdapterPtr adapter (CreateAdapter (name, path, init_string), false);

          //����������� ��������

        default_adapters.push_front (adapter);
      }
      catch (std::exception& exception)
      {        
        log.Printf ("%s\n    at render::low_level::opengl::PlatformManager::LoadDefaultAdapter('%s', '%s')", exception.what (), name, path);
      }
      catch (...)
      {
        log.Printf ("Unknown exception\n    at render::low_level::opengl::PlatformManager::LoadDefaultAdapter('%s', '%s')", name, path);
      }
    }
    
///��������� ���� �������������� ����������� (0 - ������ ������ ����� ��������, 1 - ������ ������ ����� ��������)
    static int CompareFormatCounts (size_t source1, size_t source2, size_t require)
    {
      if (source1 == require)
        return 0;

      if (source2 == require)
        return 1;

      if (source1 < require)
      {
         if (source2 < require) return source1 < source2;
         else                   return 1;
      }
      else //source1 > require
      {
        if (source2 > require) return source1 > source2;
        else                   return 0;
      }
    }
    
///��������� ���� �������� (0 - ������ ������ ����� ��������, 1 - ������ ������ ����� ��������)
    static int CompareFormats (const PixelFormatDesc& fmt0, const PixelFormatDesc& fmt1, const SwapChainDesc& swap_chain_desc)
    {
        //�������������� �� ���� ���������

      if (fmt0.acceleration != fmt1.acceleration)
        return fmt0.acceleration < fmt1.acceleration;

        //�������������� �� ����������/������� ������� �����������
        
      if (fmt0.buffers_count != fmt1.buffers_count)
      {
        if (swap_chain_desc.buffers_count < 2)
        {
          if (fmt0.buffers_count == 1)
            return 0;

          if (fmt1.buffers_count == 1)
            return 1;          
        }
        else
        {
          if (fmt0.buffers_count >= 2 && fmt1.buffers_count < 2)
            return 0;

          if (fmt0.buffers_count < 2 && fmt1.buffers_count >= 2)
            return 1;
        }
      }

        //�������������� �� ���������� ����� �����
        
      if (fmt0.color_bits != fmt1.color_bits)
        return CompareFormatCounts (fmt0.color_bits, fmt1.color_bits, swap_chain_desc.frame_buffer.color_bits);
        
        //�������������� �� ���������� ����� �������

      if (fmt0.depth_bits != fmt1.depth_bits)
        return CompareFormatCounts (fmt0.depth_bits, fmt1.depth_bits, swap_chain_desc.frame_buffer.depth_bits);

        //�������������� �� ���������� ����� ���������

      if (fmt0.stencil_bits != fmt1.stencil_bits)
        return CompareFormatCounts (fmt0.stencil_bits, fmt1.stencil_bits, swap_chain_desc.frame_buffer.stencil_bits);

        //�������������� �� ���������� �������

      if (fmt0.samples_count != fmt1.samples_count)
        return CompareFormatCounts (fmt0.samples_count, fmt1.samples_count, swap_chain_desc.samples_count);

        //�������������� �� ���������� ����� �����

      if (fmt0.alpha_bits != fmt1.alpha_bits)
        return CompareFormatCounts (fmt0.alpha_bits, fmt1.alpha_bits, swap_chain_desc.frame_buffer.alpha_bits);        

        //�������������� �� ������ ������
        
      if (fmt0.swap_method != fmt1.swap_method)
      {
        if (fmt0.swap_method == swap_chain_desc.swap_method)
          return 0;
          
        if (fmt1.swap_method == swap_chain_desc.swap_method)
          return 1;
      }

        //�������������� �� ���������� ������� ������

      if (fmt0.buffers_count != fmt1.buffers_count)
        return CompareFormatCounts (fmt0.buffers_count, fmt1.buffers_count, swap_chain_desc.buffers_count);
        
        //�������������� �� ������� ��������� PBuffer
        
      if (fmt0.support_draw_to_pbuffer != fmt1.support_draw_to_pbuffer)
        return fmt1.support_draw_to_pbuffer;

        //�������������� �� ������� ������-������
        
      if (fmt0.support_stereo != fmt1.support_stereo)
        return fmt1.support_stereo;

        //��� ������ ������ ������ ������ ����� ��������

      return 1;
    }
    
///����� ������� ��������
    const PixelFormatDesc& ChoosePixelFormat (const Adapter::PixelFormatArray& pixel_formats, const SwapChainDesc& swap_chain_desc)
    {
        //���� �� ������� �� ������ ����������� ������� - �������� ������� ������ ����������                  

      if (pixel_formats.empty ())
        throw xtl::format_operation_exception ("render::low_level::opengl::windows::PlatformManagerImpl::ChoosePixelFormat", "No pixel formats found");

        //����� �������

      const PixelFormatDesc* best = &pixel_formats [0];
      
      for (Adapter::PixelFormatArray::const_iterator iter=pixel_formats.begin ()+1, end=pixel_formats.end (); iter!=end; ++iter)
        if (CompareFormats (*best, *iter, swap_chain_desc))
          best = &*iter;

      return *best;
    }

  private:
    typedef xtl::com_ptr<Adapter>  AdapterPtr;
    typedef stl::list<AdapterPtr> AdapterList;

  private:
    Log         log;              //��������
    AdapterList default_adapters; //�������� "�� ���������"
};

typedef common::Singleton<PlatformManagerImpl> PlatformManagerSingleton;

}

/*
    ������ ��� ���������� ���������
*/

IAdapter* PlatformManager::CreateAdapter (const char* name, const char* path, const char* init_string)
{
  return PlatformManagerSingleton::Instance ().CreateAdapter (name, path, init_string);
}

void PlatformManager::EnumDefaultAdapters (const xtl::function<void (IAdapter*)>& handler)
{
  PlatformManagerSingleton::Instance ().EnumDefaultAdapters (handler);
}

ISwapChain* PlatformManager::CreateSwapChain (size_t adapters_count, IAdapter** adapters, const SwapChainDesc& desc)
{
  return PlatformManagerSingleton::Instance ().CreateSwapChain (adapters_count, adapters, desc);
}

ISwapChain* PlatformManager::CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
{
  return PlatformManagerSingleton::Instance ().CreatePBuffer (source_chain, pbuffer_desc);
}

render::low_level::opengl::IContext* PlatformManager::CreateContext (ISwapChain* swap_chain)
{
  return PlatformManagerSingleton::Instance ().CreateContext (swap_chain);
}
