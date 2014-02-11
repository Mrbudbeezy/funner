///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ������ ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicPrimitiveListImplBase: public Object, public CacheSource
{
  public:
    virtual ~DynamicPrimitiveListImplBase () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsEntityDependent ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::RendererPrimitive* RendererPrimitive ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        SetMaterial (const char* material) = 0;
    virtual const char* Material    () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual DynamicPrimitive* CreateDynamicPrimitiveInstanceCore () = 0;

  protected:
    DynamicPrimitiveListImplBase (bool is_entity_dependent);

    void SetRendererPrimitive (manager::RendererPrimitive* primitive);

  private:  
    bool                        entity_dependent;
    manager::RendererPrimitive* renderer_primitive;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ������ ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class DynamicPrimitiveListImpl: virtual public DynamicPrimitiveListImplBase
{
  public:
    typedef T Item;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Size () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Add    (size_t count, const Item* items) = 0;
    virtual void   Update (size_t first, size_t count, const Item* items) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Remove (size_t first, size_t count) = 0;
    virtual void Clear  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void   Reserve  (size_t count) = 0;
    virtual size_t Capacity () = 0;

  protected:
    DynamicPrimitiveListImpl (bool is_entity_dependent) : DynamicPrimitiveListImplBase (is_entity_dependent) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
LineListImpl*   create_standalone_line_list   (const MaterialManagerPtr&, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage);
SpriteListImpl* create_standalone_sprite_list (const MaterialManagerPtr&, SpriteMode mode, const math::vec3f& up, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage);
LineListImpl*   create_batching_line_list     (const BatchingManagerPtr&, const MaterialManagerPtr&);
SpriteListImpl* create_batching_sprite_list   (const BatchingManagerPtr&, const MaterialManagerPtr&, SpriteMode mode, const math::vec3f& up);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////

inline DynamicPrimitiveListImplBase::DynamicPrimitiveListImplBase (bool is_entity_dependent)
  : entity_dependent (is_entity_dependent)
{
}

inline bool DynamicPrimitiveListImplBase::IsEntityDependent ()
{
  return entity_dependent;
}

inline render::manager::RendererPrimitive* DynamicPrimitiveListImplBase::RendererPrimitive ()
{
  return renderer_primitive;
}

inline void DynamicPrimitiveListImplBase::SetRendererPrimitive (manager::RendererPrimitive* primitive)
{
  renderer_primitive = primitive;
}
