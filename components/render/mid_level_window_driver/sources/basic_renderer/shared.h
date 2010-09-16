#ifndef RENDER_MID_WINDOW_DRIVER_BASIC_RENDERER_SHARED_HEADER
#define RENDER_MID_WINDOW_DRIVER_BASIC_RENDERER_SHARED_HEADER

#include <stl/set>
#include <stl/string>

#include <xtl/any.h>
#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>
#include <xtl/trackable.h>

#include <common/parser.h>
#include <common/singleton.h>

#include <media/image.h>

#include <render/low_level/utils.h>

#include <render/mid_level/low_level_renderer.h>
#include <render/mid_level/window_driver.h>

#include <shared/basic_renderer.h>

namespace render
{

namespace mid_level
{

namespace window_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClearFrame: virtual public IClearFrame, public BasicFrame
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ClearFrame ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetFlags (size_t clear_flags);
    size_t GetFlags () { return clear_flags; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetColor (const math::vec4f& color);
    void GetColor (math::vec4f& color);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetDepthValue   (float depth_value);
    void          SetStencilIndex (unsigned char stencil_index);
    float         GetDepthValue   () { return clear_depth_value; }
    unsigned char GetStencilIndex () { return clear_stencil_index; }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawCore (render::low_level::IDevice*);

  private:
    size_t                     clear_flags;
    render::low_level::Color4f clear_color;
    float                      clear_depth_value;
    unsigned char              clear_stencil_index;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver  ();
    ~Driver ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      GetRenderersCount ();    
    const char* GetRendererName   (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderer* CreateRenderer (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ����������� ������� ���������� (��� ����������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterRenderer (const char* renderer_name, const common::ParseNode& configuration_node);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterWindow       (const char* renderer_name, syslib::Window& window, const common::ParseNode& configuration_node);
    void UnregisterWindow     (const char* renderer_name, syslib::Window& window);
    void UnregisterAllWindows (const char* renderer_name);
    void UnregisterAllWindows ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () {}
    void Release () {}

  private:
    class RendererEntry;
    
  private:
    typedef xtl::intrusive_ptr<RendererEntry> RendererEntryPtr;
    typedef stl::vector<RendererEntryPtr>     RendererEntries;

  private:
    RendererEntries renderer_entries;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class FrameBuffer: virtual public IFrameBuffer, public Object, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FrameBuffer  (low_level::IDevice& device, low_level::ISwapChain& in_swap_chain);
    ~FrameBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����� � ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderTarget* GetColorBuffer        ();
    IRenderTarget* GetDepthStencilBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Present () { swap_chain->Present (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� / �������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetSize           (size_t width, size_t height);
    void SetViewportOffset (int x, int y);

  private:
    typedef xtl::intrusive_ptr<RenderTarget>    RenderTargetPtr;
    typedef xtl::com_ptr<low_level::ISwapChain> SwapChainPtr;

  private:
    RenderTargetPtr color_buffer;         //����� �����
    RenderTargetPtr depth_stencil_buffer; //����� ������������� ���������
    SwapChainPtr    swap_chain;           //���� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������� ������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IRenderer2dDispatch: virtual public renderer2d::IRenderer
{
  public:
    virtual render::mid_level::renderer2d::IFrame* CreateFrame   () { return CreateFrame2d (); }
    virtual render::mid_level::renderer2d::IFrame* CreateFrame2d () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������� ������� ��������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ILowLevelRendererDispatch: virtual public ILowLevelRenderer
{
  public:
    virtual render::mid_level::ILowLevelFrame* CreateFrame () { return CreateLowLevelFrame (); }
    virtual render::mid_level::ILowLevelFrame* CreateLowLevelFrame () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RendererDispatch: virtual public IRenderer2dDispatch, virtual public ILowLevelRendererDispatch, public Object, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RendererDispatch  (low_level::IDevice& device);
    ~RendererDispatch ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddFrameBuffer    (FrameBuffer*);
    void RemoveFrameBuffer (FrameBuffer*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t        GetFrameBuffersCount ();
    IFrameBuffer* GetFrameBuffer       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderTarget* CreateDepthStencilBuffer (size_t width, size_t height);
    IRenderTarget* CreateRenderBuffer       (size_t width, size_t height);
    IClearFrame*   CreateClearFrame         ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� � ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddFrame (IFrame*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ / ������� ������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FramesCount      ();
    void   SetFramePosition (size_t position);
    size_t GetFramePosition ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� / ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawFrames   ();
    void CancelFrames ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachListener (IRendererListener*);
    void DetachListener (IRendererListener*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::mid_level::renderer2d::ITexture*   CreateTexture   (const media::Image& image);
    render::mid_level::renderer2d::ITexture*   CreateTexture   (const media::CompressedImage& image);
    render::mid_level::renderer2d::ITexture*   CreateTexture   (size_t width, size_t height, PixelFormat pixel_format);
    render::mid_level::renderer2d::IPrimitive* CreatePrimitive ();    
    render::mid_level::renderer2d::IFrame*     CreateFrame2d   ();        
    render::mid_level::ILowLevelFrame*         CreateLowLevelFrame ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FrameBufferCreateNotify  (IFrameBuffer* frame_buffer);
    void FrameBufferDestroyNotify (IFrameBuffer* frame_buffer);
    void FrameBufferUpdateNotify  (IFrameBuffer* frame_buffer);
    void FrameBufferResizeNotify  (IFrameBuffer* frame_buffer, size_t width, size_t height);

  private:
    typedef xtl::intrusive_ptr<BasicFrame>           FramePtr;
    typedef stl::list<FramePtr>                      FrameList;
    typedef stl::vector<FrameBuffer*>                FrameBufferArray;
    typedef xtl::com_ptr<render::low_level::IDevice> DevicePtr;  
    typedef stl::auto_ptr<Renderer2D>                Renderer2DPtr;
    typedef stl::auto_ptr<LowLevelRenderer>          LowLevelRendererPtr;
    typedef stl::set<IRendererListener*>             ListenerSet;

  private:
    DevicePtr           device;             //���������� ������
    FrameBufferArray    frame_buffers;      //������ ������� �����
    FrameList           frames;             //������ ������
    FrameList::iterator frame_position;     //������� ������� ������� ������
    size_t              frames_count;       //���������� ������
    Renderer2DPtr       renderer2d;         //2� ��������
    LowLevelRendererPtr low_level_renderer; //�������������� ������� ������������
    ListenerSet         listeners;          //��������� �������
};

}

}

}

#endif

