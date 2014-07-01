///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderTargetDesc: public xtl::reference_counter
{
  RenderTargetPtr render_target; //���� ���������
  ViewportPtr     viewport;      //������� ������
  RectAreaPtr     scissor;       //������� ���������
  
  RenderTargetDesc (const RenderTargetPtr& in_render_target, const ViewportPtr& in_viewport, const RectAreaPtr& in_scissor)
    : render_target (in_render_target)
    , viewport (in_viewport)
    , scissor (in_scissor)
  {
  }  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetMapImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetMapImpl  ();
    ~RenderTargetMapImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const ViewportPtr& viewport);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const ViewportPtr& viewport, const RectAreaPtr& scissor);
    void RemoveRenderTarget     (const char* name);
    void RemoveAllRenderTargets ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetPtr     FindRenderTarget     (const char* name);
    ViewportPtr         FindViewport         (const char* name);
    RectAreaPtr         FindScissor          (const char* name);
    RenderTargetDescPtr FindRenderTargetDesc (const char* name);
    RenderTargetPtr     RenderTarget         (const char* name);
    ViewportPtr         Viewport             (const char* name);
    RectAreaPtr         Scissor              (const char* name);
    RenderTargetDescPtr RenderTargetDesc     (const char* name);    
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
