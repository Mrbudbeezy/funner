#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    �������� ��������� ��������� ������ ��������� OpenGL
*/

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

class OutputStageState
{
  public:  
      //������������
    OutputStageState () :
      frame_buffer (0),
      blend_state (0),
      on_destroy_frame_buffer (xtl::bind (&OutputStageState::SetFrameBuffer, this, (FrameBuffer*)0)),
      on_destroy_blend_state (xtl::bind (&OutputStageState::SetBlendState, this, (BlendState*)0))
       {}
    
    OutputStageState (const OutputStageState& state) :
      frame_buffer (state.frame_buffer),
      on_destroy_frame_buffer (xtl::bind (&OutputStageState::SetFrameBuffer, this, (FrameBuffer*)0)),
      on_destroy_blend_state (xtl::bind (&OutputStageState::SetBlendState, this, (BlendState*)0))
       {}
    
      //��������� �������� ������ �����
    void SetFrameBuffer (FrameBuffer* in_frame_buffer)
    {
      frame_buffer = in_frame_buffer;
      
      if (frame_buffer)
        frame_buffer->RegisterDestroyHandler (on_destroy_frame_buffer);
    }

      //��������� �������� ������ �����
    FrameBuffer* GetFrameBuffer () { return frame_buffer; }
    
      //��������� �������� ��������� ��������� ���������� ������
    void SetBlendState (BlendState* state)
    {
      blend_state = state;
      
      if (state)
        state->RegisterDestroyHandler (on_destroy_blend_state);
    }
    
      //��������� �������� ��������� ��������� ���������� ������
    BlendState* GetBlendState () { return blend_state; }

  private:  
    FrameBuffer*            frame_buffer;            //������� ����� �����
    BlendState*             blend_state;             //������� ��������� ��������� ���������� ������
    xtl::auto_slot<void ()> on_destroy_frame_buffer; //���������� ������� �������� ������ �����  
    xtl::auto_slot<void ()> on_destroy_blend_state;  //���������� ������� �������� ��������� ��������� ���������� ������
};

}

/*
    �������� ���������� ��������� ������ ��������� OpenGL
*/

struct OutputStage::Impl: public ContextObject
{
  OutputStageState          state;                //��������� ������
  xtl::com_ptr<ISwapChain>  default_swap_chain;   //������� ������ �� ���������
  xtl::com_ptr<FrameBuffer> default_frame_buffer; //����� ����� "�� ���������"
  xtl::com_ptr<IBlendState> default_blend_state;  //��������� ��������� ���������� ������ "�� ���������"
  
  Impl (ContextManager& in_context_manager, ISwapChain* swap_chain) :
    ContextObject (in_context_manager),
    default_swap_chain (swap_chain),
    default_frame_buffer (FrameBuffer::Create (GetContextManager (), swap_chain), false)
  {
    BlendDesc blend_desc;
    
    memset (&blend_desc, 0, sizeof (blend_desc));
    
    blend_desc.blend_enable                     = false;
    blend_desc.blend_color_operation            = BlendOperation_Add;
    blend_desc.blend_color_source_argument      = BlendArgument_One;
    blend_desc.blend_color_destination_argument = BlendArgument_Zero;
    blend_desc.blend_alpha_operation            = BlendOperation_Add;
    blend_desc.blend_alpha_source_argument      = BlendArgument_One;
    blend_desc.blend_alpha_destination_argument = BlendArgument_Zero;
    blend_desc.color_write_mask                 = ColorWriteFlag_All;

    default_blend_state = xtl::com_ptr<IBlendState> (new BlendState (GetContextManager (), blend_desc), false);

      //��������� ��������� "�� ���������"

    SetFrameBuffer (&*default_frame_buffer);
    SetBlendState (&*default_blend_state);
  }
    
    //��������� �������� ������ �����
  void SetFrameBuffer (IFrameBuffer* in_frame_buffer)
  {
    if (!in_frame_buffer)
      RaiseNullArgument ("render::low_level::opengl::OutputStage::SetFrameBuffer", "frame_buffer");
      
    FrameBuffer* frame_buffer = cast_object<FrameBuffer> (*this, in_frame_buffer, "render::low_level::opengl::OutputStage::SetFrameBuffer", "frame_buffer");

    state.SetFrameBuffer (frame_buffer);
  }

    //��������� ������ �����
  IFrameBuffer* GetFrameBuffer () { return state.GetFrameBuffer (); }
  
      //��������� �������� ��������� ��������� ���������� ������
  void SetBlendState (IBlendState* in_blend_state)
  {
    if (!in_blend_state)
      RaiseNullArgument ("render::low_level::opengl::OutputStage::SetBlendState", "blend_state");
      
    BlendState* blend_state = cast_object<BlendState> (*this, in_blend_state, "render::low_level::opengl::OutputStage::SetBlendState", "blend_state");
    
    state.SetBlendState (blend_state);
  }
    
    //��������� �������� ��������� ��������� ���������� ������
  IBlendState* GetBlendState () { return state.GetBlendState (); }
};

/*
    ����������� / ����������
*/

OutputStage::OutputStage (ContextManager& context_manager, ISwapChain* swap_chain)
  : impl (new Impl (context_manager, swap_chain))
{
}

OutputStage::~OutputStage ()
{
}

/*
    �������� ������� �����
*/

IFrameBuffer* OutputStage::CreateFrameBuffer (const FrameBufferDesc& desc)
{
  try
  {
    SwapChainDesc swap_chain_desc;

    impl->default_swap_chain->GetDesc (swap_chain_desc);

    swap_chain_desc.frame_buffer  = desc;

    xtl::com_ptr<ISwapChain> swap_chain (SwapChainManager::CreatePBuffer (&*impl->default_swap_chain, swap_chain_desc), false);

    return FrameBuffer::Create (impl->GetContextManager (), &*swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateFrameBuffer(const FrameBufferDesc&)");
    throw;
  }
}

IFrameBuffer* OutputStage::CreateFrameBuffer (ISwapChain* swap_chain)
{
  if (!swap_chain)
    RaiseNullArgument ("render::low_level::opengl::OutputStage::CreateFrameBuffer(ISwapChain*)", "swap_chain");

  try
  {
    return FrameBuffer::Create (impl->GetContextManager (), swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateFrameBuffer(ISwapChain*)");
    throw;    
  }
}

IFrameBuffer* OutputStage::CreateFrameBuffer (ITexture* render_target)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateFrameBuffer(ITexture*)");
  return 0;
}

/*
    �������� ��������� ��������� ������
*/

IBlendState* OutputStage::CreateBlendState (const BlendDesc& desc)
{
  try
  {
    return new BlendState (impl->GetContextManager (), desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateBlendState");

    throw;
  }
}

IDepthStencilState* OutputStage::CreateDepthStencilState (const DepthStencilDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateDepthStencilState");
  return 0;
}

/*
    ������ � ������� �����
*/

void OutputStage::SetFrameBuffer (IFrameBuffer* frame_buffer)
{
  impl->SetFrameBuffer (frame_buffer);
}

IFrameBuffer* OutputStage::GetFrameBuffer () const
{
  return impl->GetFrameBuffer ();
}

/*
    ��������� ��������� ���������� ������
*/

void OutputStage::SetBlendState (IBlendState* state)
{
  impl->SetBlendState (state);
}

IBlendState* OutputStage::GetBlendState () const
{
  return impl->GetBlendState ();
}

/*
    ��������� ��������� ������������� ���������
*/

void OutputStage::SetDepthStencil (IDepthStencilState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetDepthStencil");
}

void OutputStage::SetStencilReference (size_t reference)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetStencilReference");
}

IDepthStencilState* OutputStage::GetDepthStencilState () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilState");
  return 0;
}

size_t OutputStage::GetStencilReference () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilReference");
  return 0;
}
