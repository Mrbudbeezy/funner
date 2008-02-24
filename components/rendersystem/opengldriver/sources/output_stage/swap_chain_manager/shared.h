#ifndef RENDER_GL_DRIVER_OUTPUT_STAGE_SWAP_CHAIN_MANAGER_SHARED_HEADER
#define RENDER_GL_DRIVER_OUTPUT_STAGE_SWAP_CHAIN_MANAGER_SHARED_HEADER

#include "../shared.h"

#include <stl/list>

namespace render
{

namespace low_level
{

namespace opengl
{

//forward declarations
class SwapChainFrameBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainRenderBuffer: public RenderBuffer
{
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainRenderBuffer (const FrameBufferManager& manager, RenderTargetType target_type);
    SwapChainRenderBuffer (const FrameBufferManager& manager, const TextureDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFrameBuffer (size_t context_id, ISwapChain* swap_chain, GLenum buffer_type);

  private:
    FrameBufferManager frame_buffer_manager; //�������� ������� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainColorBuffer: public SwapChainRenderBuffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainColorBuffer (const FrameBufferManager& manager, ISwapChain* swap_chain, size_t buffer_index);
    SwapChainColorBuffer (const FrameBufferManager& manager, ISwapChain* swap_chain, const TextureDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ � ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* GetSwapChain   () const { return swap_chain.get (); }
    size_t      GetBufferIndex () const { return buffer_index; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    GLenum GetBufferType () const { return buffer_type; }

  private:
    void Bind ();

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

  private:
    SwapChainPtr swap_chain;   //������� ������
    size_t       buffer_index; //������ ������ ������ � ������� ������
    GLenum       buffer_type;  //��� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������-��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainDepthStencilBuffer: public SwapChainRenderBuffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainDepthStencilBuffer  (const FrameBufferManager& manager, ISwapChain* swap_chain);
    SwapChainDepthStencilBuffer  (const FrameBufferManager& manager, ISwapChain* swap_chain, const TextureDesc&);
    ~SwapChainDepthStencilBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetContextId () const { return context_id; }

  private:
    void Bind ();

  private:
    size_t context_id; //������������� ���������
};

typedef xtl::com_ptr<SwapChainColorBuffer>        ColorBufferPtr;
typedef xtl::com_ptr<SwapChainDepthStencilBuffer> DepthStencilBufferPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainFrameBufferManager: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainFrameBufferManager  (const FrameBufferManager& manager);
    ~SwapChainFrameBufferManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* CreateRenderBuffer       (const TextureDesc&);
    ITexture* CreateColorBuffer        (ISwapChain* swap_chain, size_t buffer_index);
    ITexture* CreateDepthStencilBuffer (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool          IsSupported       (View* color_view, View* depth_stencil_view);
           IFrameBuffer* CreateFrameBuffer (View* color_view, View* depth_stencil_view);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ColorBufferPtr        GetShadowBuffer  (SwapChainDepthStencilBuffer*);
    DepthStencilBufferPtr GetShadowBuffer  (SwapChainColorBuffer*);
    void                  GetShadowBuffers (ColorBufferPtr&, DepthStencilBufferPtr&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const FrameBufferManager& GetFrameBufferManager () const { return frame_buffer_manager; }
          FrameBufferManager& GetFrameBufferManager ()       { return frame_buffer_manager; }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ColorBufferPtr        CreateShadowColorBuffer        (SwapChainDepthStencilBuffer*);
    DepthStencilBufferPtr CreateShadowDepthStencilBuffer (SwapChainColorBuffer*);
    SwapChainFrameBuffer* CreateShadowFrameBuffer  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* GetDefaultSwapChain () const;

  private:
    typedef stl::list<SwapChainColorBuffer*>        ColorBufferList;
    typedef stl::list<SwapChainDepthStencilBuffer*> DepthStencilBufferList;

  private:
    FrameBufferManager     frame_buffer_manager;          //�������� ������� �����
    ColorBufferList        shadow_color_buffers;          //������ ������� ������� �����
    DepthStencilBufferList shadow_depth_stencil_buffers;  //������ ������� ������� ������������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ����� ��� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainFrameBuffer: public IFrameBuffer, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainFrameBuffer (SwapChainFrameBufferManager& manager, View* color_view, View* depth_stencil_view);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ��������� � ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void InvalidateRenderTargets (const Rect& update_rect);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateRenderTargets ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetColorView         (View*);
    void SetDepthStencilView  (View*);
    void FinishInitialization (SwapChainFrameBufferManager&);

  private:
    typedef xtl::com_ptr<IRenderTargetTexture> TexturePtr;

    struct RenderTarget
    {
      ViewDesc                view_desc;      //���������� �����������
      TexturePtr              target_texture; //������� ��������
      MipLevelDesc            mip_level_desc; //���������� �������� mip-������ ��������
      RenderTargetTextureDesc texture_desc;   //���������� ������� ��������
      
      RenderTarget ();
    };

  private:
    FrameBufferManager    frame_buffer_manager;                    //�������� ������� �����
    ColorBufferPtr        color_buffer;                            //����� �����
    DepthStencilBufferPtr depth_stencil_buffer;                    //����� ������������� ���������
    RenderTarget          render_targets [RenderTargetType_Num];   //���� ���������
    bool                  is_buffer_active [RenderTargetType_Num]; //���������� ������� ����� � ������������� ���������
    bool                  has_texture_targets;                     //����, ������������ ����������� ������� �������
    Rect                  dirty_rect;                              //������� ���������� ������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainNullFrameBuffer: public IFrameBuffer, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainNullFrameBuffer (const FrameBufferManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ��������� � ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void InvalidateRenderTargets (const Rect&) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateRenderTargets () {}
    
  private:
    FrameBufferManager frame_buffer_manager; //�������� ������� �����
};

}

}

}

#endif
