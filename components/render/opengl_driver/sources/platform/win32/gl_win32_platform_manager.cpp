#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    ���������
*/

const size_t ADAPTERS_ARRAY_RESERVE_SIZE     = 4;   //������������� ���������� ��������� "�� ���������"
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
        //�������������� ����� ��� �������� ��������� "�� ���������"

      default_adapters.reserve (ADAPTERS_ARRAY_RESERVE_SIZE);

        //�������� �������� "�� ���������"

      LoadDefaultAdapter ("Default", "opengl32.dll");
      
        //�������� Direct3D ��������� (���� �� ����)

      stl::string acxtrnal_path;

      acxtrnal_path.fast_resize (GetWindowsDirectory (&acxtrnal_path [0], 0));      

      if (!GetWindowsDirectory (&acxtrnal_path [0], acxtrnal_path.size ()))
        raise_error ("GetWindowsDirectory");

      acxtrnal_path.pop_back ();

      acxtrnal_path += "/AppPatch/AcXtrnal.dll";

      LoadDefaultAdapter ("Direct3D wrapper", acxtrnal_path.c_str ());
    }
  
///�������� ��������
    IAdapter* CreateAdapter (const char* name, const char* path)
    {
      try
      {
        return new Adapter (name, path);
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
      for (AdapterArray::iterator iter=default_adapters.begin (), end=default_adapters.end (); iter!=end; ++iter)
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
            
          Adapter* adapter = dynamic_cast<Adapter*> (in_adapter);
          
          if (!adapter)
            throw xtl::format_exception<xtl::argument_exception> ("", "Invalid argument 'adapters[%u]'. Wrong type '%s'",
              i, typeid (*in_adapter).name ());

          adapter->EnumPixelFormats (dc_holder.window, dc_holder.dc, pixel_formats, wgl_extension_entries);
        }

          //����� �������� ����������� �������

        const PixelFormatDesc& pixel_format = ChoosePixelFormat (pixel_formats, desc);        

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
    IContext* CreateContext (ISwapChain* swap_chain, IContext* shared_context)
    {
      try
      {
        return new Context (swap_chain, shared_context);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::PlatformManagerImpl::CreateContext");
        throw;
      }
    }
    
  private:
///������� �������� �������� "�� ���������"
    void LoadDefaultAdapter (const char* name, const char* path)
    {
      try
      {
          //�������� ��������

        AdapterPtr adapter (new Adapter (name, path), false);

          //����������� ��������

        default_adapters.push_back (adapter);
      }
      catch (std::exception& exception)
      {
          //������� ����������������!!!
        printf ("Error at load default OpenGL adapter (name='%s', path='%s'): %s\n", name, path, exception.what ());

          //���������� ����������
      }
      catch (...)
      {
        //���������� ���� ����������
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
    typedef xtl::com_ptr<Adapter>   AdapterPtr;
    typedef stl::vector<AdapterPtr> AdapterArray;

  private:
    AdapterArray default_adapters; //�������� "�� ���������"
};

typedef common::Singleton<PlatformManagerImpl> PlatformManagerSingleton;

}

/*
    ������ ��� ���������� ���������
*/

IAdapter* PlatformManager::CreateAdapter (const char* name, const char* path)
{
  return PlatformManagerSingleton::Instance ().CreateAdapter (name, path);
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

render::low_level::opengl::IContext* PlatformManager::CreateContext (ISwapChain* swap_chain, render::low_level::opengl::IContext* shared_context)
{
  return PlatformManagerSingleton::Instance ().CreateContext (swap_chain, shared_context);
}
