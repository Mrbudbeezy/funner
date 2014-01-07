class DynamicPrimitiveEntityStorage;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct EntityLodDesc
{
  RendererOperationList&         operations;                         //�������� ����������
  DynamicPrimitiveEntityStorage& dynamic_primitive_storage;          //��������� ������������ ����������
  bool                           has_frame_dependent_operations;     //���� �� �������� ��������� �� �����
  bool                           has_frame_independent_operations;   //���� �� �������� �� ��������� �� �����
  bool                           has_entity_dependent_operations;    //���� �� �������� ��������� �� �������
  bool                           has_entity_independent_operations;  //���� �� �������� �� ��������� �� �������

  EntityLodDesc (RendererOperationList& in_operations, DynamicPrimitiveEntityStorage& in_storage)
    : operations (in_operations)
    , dynamic_primitive_storage (in_storage)
    , has_frame_dependent_operations ()
    , has_frame_independent_operations ()
    , has_entity_dependent_operations ()
    , has_entity_independent_operations ()
  {
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class EntityImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    EntityImpl  (const DeviceManagerPtr&, const TextureManagerPtr&, const PrimitiveManagerPtr&);
    ~EntityImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicTextureEntityStorage& DynamicTextureStorage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����-����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetShaderOptions (const common::PropertyMap&);
    const common::PropertyMap& ShaderOptions    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetUserData (void* data);
    void* UserData    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� (��� ���������)
///  �������������� ���������� �� ������� Entity::Transformation � ������ ���� ��� �� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetJointsCount         (size_t count);
    size_t             JointsCount            ();
    void               SetJointTransformation (size_t joint_index, const math::mat4f&);
    const math::mat4f& JointTransformation    (size_t joint_index);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LodsCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitivePtr Primitive           (size_t level_of_detail);
    const char*  PrimitiveName       (size_t level_of_detail);  
    void         SetPrimitive        (const PrimitivePtr&, size_t level_of_detail);
    void         SetPrimitive        (const char* name, size_t level_of_detail);
    void         ResetPrimitive      (size_t level_of_detail);
    bool         HasPrimitive        (size_t level_of_detail);
    void         ResetAllPrimitives  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� � ��������� ������� ��������� ������� ��� ������� ����������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetLodPoint (const math::vec3f&);
    const math::vec3f& LodPoint    ();
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void            SetScissor      (const RectArea& scissor);
    const RectArea& Scissor         ();
    void            SetScissorState (bool state);
    bool            ScissorState    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �� ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const EntityLodDesc& GetLod (size_t level_of_detail, bool find_nearest = false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
