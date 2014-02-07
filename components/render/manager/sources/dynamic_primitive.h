//implementation forwards
class RenderingContext;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum DynamicPrimitiveFlag
{
  DynamicPrimitiveFlag_FrameDependent  = 1, //������������ �������� ������� �� �����
  DynamicPrimitiveFlag_EntityDependent = 2, //������������ �������� ������� �� ��������

  DynamicPrimitiveFlag_Default = DynamicPrimitiveFlag_EntityDependent
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicPrimitive: public Object, public CacheSource, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~DynamicPrimitive () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateOnPrerender (FrameId frame_id, EntityImpl& entity);
    void UpdateOnRender    (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const manager::RendererPrimitiveGroup& RendererPrimitiveGroup ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������� ��������� �� ����� / �������� �� �������� ��������� �� EntityDrawParams
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsFrameDependent  ();
    bool IsEntityDependent ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitive (const manager::RendererPrimitiveGroup& group, size_t flags = DynamicPrimitiveFlag_Default);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateOnPrerenderCore (EntityImpl& entity) = 0;
    virtual void UpdateOnRenderCore    (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix) = 0;

  private:
    const manager::RendererPrimitiveGroup& group;
    bool                                   frame_dependent;
    bool                                   entity_dependent;
    FrameId                                cached_frame_id;
    EntityImpl*                            cached_entity;
    FrameImpl*                             cached_frame;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicPrimitiveEntityStorage: public CacheSource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitiveEntityStorage  (EntityImpl& entity);
    ~DynamicPrimitiveEntityStorage ();
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddPrimitive        (const DynamicPrimitivePtr&, void* source_tag = 0);
    void RemovePrimitive     (const DynamicPrimitivePtr&);
    void RemoveAllPrimitives ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� �� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitivePtr FindPrimitive (void* source_tag, bool touch);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ������� �������������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginUpdate ();
    void EndUpdate   ();

    struct UpdateScope: public xtl::noncopyable
    {
      UpdateScope (DynamicPrimitiveEntityStorage& in_storage) : storage (in_storage) { storage.BeginUpdate (); }
      ~UpdateScope () { storage.EndUpdate (); }

      DynamicPrimitiveEntityStorage& storage;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                         RendererPrimitiveGroupsCount ();
    RendererDynamicPrimitiveGroup* RendererPrimitiveGroups      ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    using CacheSource::UpdateCache;
    using CacheSource::ResetCache;
      
  private:
    void UpdateCacheCore ();
    void ResetCacheCore ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////

inline const RendererPrimitiveGroup& DynamicPrimitive::RendererPrimitiveGroup ()
{
  return group;
}

inline bool DynamicPrimitive::IsFrameDependent ()
{
  return frame_dependent;
}

inline bool DynamicPrimitive::IsEntityDependent ()
{
  return entity_dependent;
}

inline void DynamicPrimitive::UpdateOnPrerender (FrameId frame_id, EntityImpl& entity)
{
  if (frame_id == cached_frame_id)
    return;  

  cached_frame_id = frame_id;
  cached_entity   = &entity;

  try
  {
    UpdateOnPrerenderCore (entity);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitive::UpdateOnPrerender");
    throw;
  }
}

inline void DynamicPrimitive::UpdateOnRender (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix)
{
  if (&entity == cached_entity && &frame == cached_frame)
    return;

  cached_entity = &entity;
  cached_frame  = &frame;

  try
  {
    UpdateOnRenderCore (frame, entity, context, mvp_matrix);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitive::UpdateOnRender");
    throw;
  }  
}
