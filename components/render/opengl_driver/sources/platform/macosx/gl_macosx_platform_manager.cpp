#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::macosx;

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
      : opengl_library (new Library, false)
    {
      default_adapters.push_back (AdapterPtr (new Adapter, false));
    }

///�������� ��������
    IAdapter* CreateAdapter (const char* name, const char* path, const char* init_string)
    {
      throw xtl::format_not_supported_exception ("render::low_level::opengl::macosx::PlatformManagerImpl::CreateAdapter", "Custom adapters not supported");
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

        if (adapters_count != 1)
          throw xtl::format_not_supported_exception ("", "Multiple adapters not supported");

        Adapter* adapter = dynamic_cast<Adapter*> (*adapters);

        if (!adapter)
          throw xtl::format_not_supported_exception ("", "Only default adapter supported");

        log.Printf ("Create swap chain...");

          //�������� ������� ������

        return new PrimarySwapChain (desc, adapter);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::macosx::PlatformManagerImpl::CreateSwapChain");
        throw;
      }
    }

///�������� PBuffer'�
    ISwapChain* CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::macosx::PlatformManagerImpl::CreatePBuffer");
    }

///�������� ���������
    IContext* CreateContext (ISwapChain* swap_chain)
    {
      try
      {
        return new Context (swap_chain, opengl_library.get ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::macosx::PlatformManagerImpl::CreateContext");
        throw;
      }
    }

  private:
    typedef xtl::com_ptr<Library> LibraryPtr;
    typedef xtl::com_ptr<Adapter> AdapterPtr;
    typedef stl::list<AdapterPtr> AdapterList;

  private:
    Log         log;              //��������
    AdapterList default_adapters; //��������
    LibraryPtr  opengl_library;   //���������� ������� OpenGL
};

typedef common::Singleton<PlatformManagerImpl> PlatformManagerSingleton;

}

/*
    ������ ��� ���������� ���������
*/

IAdapter* PlatformManager::CreateAdapter (const char* name, const char* path, const char* init_string)
{
  return PlatformManagerSingleton::Instance ()->CreateAdapter (name, path, init_string);
}

void PlatformManager::EnumDefaultAdapters (const xtl::function<void (IAdapter*)>& handler)
{
  PlatformManagerSingleton::Instance ()->EnumDefaultAdapters (handler);
}

ISwapChain* PlatformManager::CreateSwapChain (size_t adapters_count, IAdapter** adapters, const SwapChainDesc& desc)
{
  return PlatformManagerSingleton::Instance ()->CreateSwapChain (adapters_count, adapters, desc);
}

ISwapChain* PlatformManager::CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
{
  return PlatformManagerSingleton::Instance ()->CreatePBuffer (source_chain, pbuffer_desc);
}

render::low_level::opengl::IContext* PlatformManager::CreateContext (ISwapChain* swap_chain)
{
  return PlatformManagerSingleton::Instance ()->CreateContext (swap_chain);
}
