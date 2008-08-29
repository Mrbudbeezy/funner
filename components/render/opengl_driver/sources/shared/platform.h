#ifndef RENDER_GL_DRIVER_PLATFORM_HEADER
#define RENDER_GL_DRIVER_PLATFORM_HEADER

#include <render/low_level/driver.h>

#include <shared/library.h>
#include <shared/gl_entries.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IContextListener
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnSetCurrent  () {} //�������� ���� �������
    virtual void OnLostCurrent () {} //�������� �������� ���� �������

  protected:
    virtual ~IContextListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class IContext: virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void MakeCurrent (ISwapChain* swap_chain) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ������� ������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsCompatible (ISwapChain* swap_chain) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ILibrary& GetLibrary () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AttachListener (IContextListener*) = 0;
    virtual void DetachListener (IContextListener*) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PlatformManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IAdapter* CreateAdapter (const char* name, const char* path);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� "�� ���������"
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void EnumDefaultAdapters (const xtl::function<void (IAdapter*)>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������ � PBuffer-�
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ISwapChain* CreateSwapChain (size_t adapters_count, IAdapter** adapters, const SwapChainDesc& desc);
    static ISwapChain* CreatePBuffer   (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IContext* CreateContext (ISwapChain* swap_chain);
};

}

}

}

#endif
