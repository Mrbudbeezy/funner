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
      blend_state (0),
      on_destroy_blend_state (xtl::bind (&OutputStageState::SetBlendState, this, (BlendState*)0))
       {}
    
      //������������
    OutputStageState& operator = (const OutputStageState& state)
    {
      SetBlendState (state.GetBlendState ());

      return *this;
    }

      //��������� �������� ��������� ��������� ���������� ������
    void SetBlendState (BlendState* state)
    {
      blend_state = state;
      
      if (state)
        state->RegisterDestroyHandler (on_destroy_blend_state);
    }

      //��������� �������� ��������� ��������� ���������� ������
    BlendState* GetBlendState () const { return blend_state; }

  private:
    OutputStageState (const OutputStageState&); //no impl

  private:  
    BlendState*             blend_state;             //������� ��������� ��������� ���������� ������
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
  xtl::com_ptr<IBlendState> default_blend_state;  //��������� ��������� ���������� ������ "�� ���������"
  size_t                    context_id;           //TEST!!!

  Impl (ContextManager& context_manager, ISwapChain* swap_chain) :
    ContextObject (context_manager),
    default_swap_chain (swap_chain)
  {
      //TEST!!!!
      
    context_id = context_manager.CreateContext (swap_chain);
    
    context_manager.SetContext (context_id, swap_chain, swap_chain);
      
      //!!!!!!!!
        
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

    SetBlendState (&*default_blend_state);
  }    
  
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
    ��������� ����������� ������ ������� ������ �� ��������
*/

ITexture* OutputStage::GetBuffer (ISwapChain* swap_chain, size_t buffer_id)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetBuffer");
  return 0;
}

/*
    �������� �����������
*/

IView* OutputStage::CreateView (ITexture* texture, const ViewDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateView");
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
    ����� ������� �����������
*/

void OutputStage::SetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetRenderTargets");
}

IView* OutputStage::GetRenderTargetView () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetRenderTargetView");
  return 0;
}

IView* OutputStage::GetDepthStencilView () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilView");
  return 0;
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

/*
    ������� ������� ���������
*/

void OutputStage::ClearRenderTargetView (const Color4f& color)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::ClearRenderTargetView");
}

void OutputStage::ClearDepthStencilView (float depth, unsigned char stencil)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::ClearDepthStencilView");
}

void OutputStage::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::ClearViews");
}
