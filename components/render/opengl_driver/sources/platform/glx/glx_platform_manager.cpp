#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    ���������
*/

const char* OPENGL_LIB_PATH = "libGL";

/*
    ���������� ��������� ��������� glx
*/

class PlatformManagerImpl
{
  public:
///�����������
    PlatformManagerImpl ()
    {
      try
      {
          //�������� �������� "�� ���������"

        LoadDefaultAdapter ("GLX", OPENGL_LIB_PATH);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::PlatformManagerImpl");
        throw;
      }      
    }

///�������� ��������
    IAdapter* CreateAdapter (const char* name, const char* path, const char* init_string)
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
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::CreateAdapter");
        throw;
      }
    }
    
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
        log.Printf ("%s\n    at render::low_level::opengl::glx::PlatformManager::LoadDefaultAdapter('%s', '%s')", exception.what (), name, path);
      }
      catch (...)
      {
        log.Printf ("Unknown exception\n    at render::low_level::opengl::glx::PlatformManager::LoadDefaultAdapter('%s', '%s')", name, path);
      }
    }

///������������ ��������� �� ���������
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

        log.Printf ("Create primary swap chain...");

          //�������� ������� ������

        return new PrimarySwapChain (adapter, desc);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::CreateSwapChain");
        throw;
      }      
    }

///�������� ������������ ������
    ISwapChain* CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PlatformManagerImpl::CreatePBuffer");
    }

///�������� ���������
    render::low_level::opengl::IContext* CreateContext (ISwapChain* swap_chain)
    {
      try
      {
        return new Context (swap_chain);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::CreateContext");
        throw;
      }
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
