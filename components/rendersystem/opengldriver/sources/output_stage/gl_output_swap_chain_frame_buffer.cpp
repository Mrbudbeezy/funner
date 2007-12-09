#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����� ����� �� ������ ������� ������
*/

namespace
{

class SwapChainFrameBuffer: public FrameBuffer
{
  public:
      //�����������
    SwapChainFrameBuffer (const ContextManager& in_context_manager, ISwapChain* in_swap_chain) :
      swap_chain (in_swap_chain), context_manager (in_context_manager)
    {
      if (!in_swap_chain)
        RaiseNullArgument ("render::low_level::opengl::SwapChainFrameBuffer::SwapChainFrameBuffer", "swap_chain");
        
        //��� �������� ������������� � ����������
       
      Bind ();
    }
    
      //��������� ��������� ����������
    ContextManager& GetContextManager () { return context_manager; }

      //������������� ������ � ��������� OpenGL
    void Bind ()
    {
      context_manager.SetSwapChains (&*swap_chain, &*swap_chain);
      context_manager.MakeContextCurrent ();
    }
    
      //��������� �����������
    void GetDesc (FrameBufferDesc& out_desc)
    {
      SwapChainDesc swap_chain_desc;

      swap_chain->GetDesc (swap_chain_desc);

      out_desc = swap_chain_desc.frame_buffer;
    }
    
      //��������� �������� �����������
    IRenderTargetSurface* GetRenderTarget ()
    {
      RaiseNotImplemented ("render::low_level::opengl::SwapChainFrameBuffer::GetRenderTarget");
      return 0;
    }
    
      //��������� depth-stencil �����������
    IDepthStencilSurface* GetDepthStencil ()
    {
      RaiseNotImplemented ("render::low_level::opengl::SwapChainFrameBuffer::GetDepthStencil");
      return 0;
    }

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

  private:
    ContextManager  context_manager; //�������� ����������
    SwapChainPtr    swap_chain;      //������� ������
};

}

/*
    �������� SwapChainFrameBuffer
*/

FrameBuffer* FrameBuffer::Create (const ContextManager& context_manager, ISwapChain* swap_chain)
{
  return new SwapChainFrameBuffer (context_manager, swap_chain);
}

