#ifndef RENDER_GL_DRIVER_SWAP_CHAIN_MANAGER_HEADER
#define RENDER_GL_DRIVER_SWAP_CHAIN_MANAGER_HEADER

#include <render/low_level/driver.h>
#include <shared/platform/output_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ISwapChain* CreateSwapChain (OutputManager& output_manager, const SwapChainDesc& swap_chain_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� PBuffer'�
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ISwapChain* CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc& pbuffer_desc);
    static ISwapChain* CreatePBuffer (ISwapChain* source_chain);
};

}

}

}

#endif
