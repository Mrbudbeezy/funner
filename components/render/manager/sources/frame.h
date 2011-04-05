///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class FrameImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FrameImpl  (const EffectManagerPtr&);
    ~FrameImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport, const RectAreaPtr& scissor);
    void RemoveRenderTarget     (const char* name);
    void RemoveAllRenderTargets ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetPtr FindRenderTarget (const char* name);
    RectAreaPtr     FindViewport     (const char* name);
    RectAreaPtr     FindScissor      (const char* name);
    RenderTargetPtr RenderTarget     (const char* name);
    RectAreaPtr     Viewport         (const char* name);
    RectAreaPtr     Scissor          (const char* name);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetScissorState (bool state);
    bool ScissorState    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetClearFlags (size_t clear_flags);
    size_t ClearFlags    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetClearColor (const math::vec4f& color);
    const math::vec4f& ClearColor    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetClearDepthValue   (float depth_value);
    void          SetClearStencilIndex (unsigned char stencil_index);
    float         ClearDepthValue      ();
    unsigned char ClearStencilIndex    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLocalTexture        (const char* name, const TexturePtr& texture);
    void RemoveLocalTexture     (const char* name);
    void RemoveAllLocalTextures ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TexturePtr FindLocalTexture (const char* name);
    TexturePtr LocalTexture     (const char* name);
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetEffect (const char* name);
    const char* Effect    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    EffectRendererPtr EffectRenderer ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t EntitiesCount     ();
    void   AddEntity         (const EntityPtr&);
    void   RemoveAllEntities ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���-���������� � ����-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FramesCount     ();
    void   AddFrame        (const FramePtr& frame);
    void   RemoveAllFrames ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
