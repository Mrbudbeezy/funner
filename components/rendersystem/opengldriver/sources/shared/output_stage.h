#ifndef RENDER_GL_DRIVER_OUTPUT_STAGE_HEADER
#define RENDER_GL_DRIVER_OUTPUT_STAGE_HEADER

#include <stl/auto_ptr.h>
#include <shared/context_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputStage
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputStage  (ContextManager&, ISwapChain*);
    ~OutputStage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* CreateTexture             (const TextureDesc&);    
    ITexture* CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index);
    ITexture* CreateDepthStencilTexture (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IView* CreateView (ITexture* texture, const ViewDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IBlendState*        CreateBlendState        (const BlendDesc&);
    IDepthStencilState* CreateDepthStencilState (const DepthStencilDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void    SetRenderTargets    (IView* render_target_view, IView* depth_stencil_view);
    IView*  GetRenderTargetView () const;
    IView*  GetDepthStencilView () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetBlendState (IBlendState*);
    IBlendState* GetBlendState () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void                SetDepthStencilState (IDepthStencilState* state);
    void                SetStencilReference  (size_t reference);
    IDepthStencilState* GetDepthStencilState () const;
    size_t              GetStencilReference  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ClearRenderTargetView (const Color4f& color);
    void ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil);
    void ClearViews            (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

  private:
    OutputStage (const OutputStage&); //no impl
    OutputStage& operator = (const OutputStage&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
